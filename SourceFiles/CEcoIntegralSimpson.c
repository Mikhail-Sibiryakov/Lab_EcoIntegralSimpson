/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> EcoIntegralSimpson object. </summary>
 * <description> ACOM lifetime and adaptive Simpson methods for three real types. </description>
 * <reference> docs/DESIGN.md </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "CEcoIntegralSimpson.h"
/* <summary> Query an object interface. </summary>
 * <description> Returns an owned reference; clears out on failure. </description> */
static int16_t ECOCALLMETHOD CEcoIntegralSimpson_5BC4E3DF_QueryInterface(IEcoAdvancedMathPtr_t me, const UGUID* iid, voidptr_t* out) {
    if (out == 0) {
        return ERR_ECO_POINTER;
    }
    *out = 0;
    if (me == 0 || iid == 0) {
        return ERR_ECO_POINTER;
    }
    if (!IsEqualUGUID(iid, &IID_IEcoAdvancedMath) && !IsEqualUGUID(iid, &IID_IEcoUnknown)) {
        return ERR_ECO_NOINTERFACE;
    }
    *out = me;
    me->pVTbl->AddRef(me);
    return ERR_ECO_SUCCESS;
}

/* <summary> Add an object reference. </summary>
 * <description> Returns the updated reference count. </description> */
static uint32_t ECOCALLMETHOD CEcoIntegralSimpson_5BC4E3DF_AddRef(IEcoAdvancedMathPtr_t me) {
    CEcoIntegralSimpson_5BC4E3DF* object = (CEcoIntegralSimpson_5BC4E3DF*)me;
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    object->m_cRef++;
    return object->m_cRef;
}

/* <summary> Release an object reference. </summary>
 * <description> Deletes the object at zero; returns the remaining count. </description> */
static uint32_t ECOCALLMETHOD CEcoIntegralSimpson_5BC4E3DF_Release(IEcoAdvancedMathPtr_t me) {
    CEcoIntegralSimpson_5BC4E3DF* object = (CEcoIntegralSimpson_5BC4E3DF*)me;
    uint32_t remaining = 0;
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    object->m_cRef--;
    remaining = object->m_cRef;
    if (remaining == 0) {
        object->Delete(object);
    }
    return remaining;
}

/* <summary> Compute the absolute value. </summary>
 * <description> Arithmetic only; no libc calls. </description> */
static double_t CEcoIntegralSimpson_5BC4E3DF_D_Abs(double_t value) {
    return value < (double_t)0 ? -value : value;
}

/* <summary> Check whether a value is finite. </summary>
 * <description> Self-comparison rejects NaN; bounds reject infinities. </description> */
static uint8_t CEcoIntegralSimpson_5BC4E3DF_D_Finite(double_t value) {
    return (uint8_t)(value == value && value <= (double_t)__DBL_MAX__ && value >= -(double_t)__DBL_MAX__);
}

/* <summary> Compute the midpoint. </summary>
 * <description> Halves opposite-sign bounds before adding to avoid overflow. </description> */
static double_t CEcoIntegralSimpson_5BC4E3DF_D_Midpoint(double_t a, double_t b) {
    if (a < (double_t)0 && b > (double_t)0) {
        return a/(double_t)2 + b/(double_t)2;
    }
    return a + (b-a)/(double_t)2;
}

/* <summary> Evaluate the callback. </summary>
 * <description> Rejects NaN and infinite results with ERR_ECO_FAIL. </description> */
static int16_t CEcoIntegralSimpson_5BC4E3DF_D_Evaluate(EcoIntegrandD function, voidptr_t context, double_t x, double_t* value) {
    *value = function(x, context);
    return CEcoIntegralSimpson_5BC4E3DF_D_Finite(*value) ? ERR_ECO_SUCCESS : ERR_ECO_FAIL;
}

/* <summary> Evaluate the local Simpson rule. </summary>
 * <description> Returns Q2 from two halves; E=max(abs(Q2-Q1)/15, 4*epsilon*A),
 * where A is the quadrature of |f|. E is an estimate, not a guaranteed bound. </description> */
static int16_t CEcoIntegralSimpson_5BC4E3DF_D_Rule(EcoIntegrandD function, voidptr_t context, double_t a, double_t b,
    double_t* q, double_t* error) {
    double_t values[5];
    double_t points[5];
    double_t width = b-a;
    double_t coarse = (double_t)0;
    double_t fine = (double_t)0;
    double_t magnitude = (double_t)0;
    double_t roundoff = (double_t)0;
    uint32_t i = 0;
    int16_t status = ERR_ECO_SUCCESS;
    if (!CEcoIntegralSimpson_5BC4E3DF_D_Finite(width)) {
        return ERR_ECO_FAIL;
    }
    points[0] = a;
    points[2] = CEcoIntegralSimpson_5BC4E3DF_D_Midpoint(a,b);
    points[4] = b;
    points[1] = CEcoIntegralSimpson_5BC4E3DF_D_Midpoint(a,points[2]);
    points[3] = CEcoIntegralSimpson_5BC4E3DF_D_Midpoint(points[2],b);
    for (i = 0; i < 5; i++) {
        status = CEcoIntegralSimpson_5BC4E3DF_D_Evaluate(function, context, points[i], &values[i]);
        if (status != ERR_ECO_SUCCESS) {
            return status;
        }
    }
    coarse = (width/(double_t)6)*(values[0]+(double_t)4*values[2]+values[4]);
    fine = (width/(double_t)12)*(values[0]+(double_t)4*values[1]+(double_t)2*values[2]+(double_t)4*values[3]+values[4]);
    magnitude = (width/(double_t)12)*(CEcoIntegralSimpson_5BC4E3DF_D_Abs(values[0])+(double_t)4*CEcoIntegralSimpson_5BC4E3DF_D_Abs(values[1])+
        (double_t)2*CEcoIntegralSimpson_5BC4E3DF_D_Abs(values[2])+(double_t)4*CEcoIntegralSimpson_5BC4E3DF_D_Abs(values[3])+CEcoIntegralSimpson_5BC4E3DF_D_Abs(values[4]));
    *q = fine;
    *error = CEcoIntegralSimpson_5BC4E3DF_D_Abs(fine-coarse)/(double_t)15;
    roundoff = ((double_t)4*(double_t)__DBL_EPSILON__)*magnitude;
    if (roundoff > *error) {
        *error = roundoff;
    }
    if (!CEcoIntegralSimpson_5BC4E3DF_D_Finite(coarse) || !CEcoIntegralSimpson_5BC4E3DF_D_Finite(*q) || !CEcoIntegralSimpson_5BC4E3DF_D_Finite(*error) || !CEcoIntegralSimpson_5BC4E3DF_D_Finite(magnitude)) {
        return ERR_ECO_FAIL;
    }
    return ERR_ECO_SUCCESS;
}

typedef struct CEcoIntegralSimpson_5BC4E3DF_D_Interval {
    double_t a;
    double_t b;
    double_t q;
    double_t error;
} CEcoIntegralSimpson_5BC4E3DF_D_Interval;

/* <summary> Integrate adaptively using double_t. </summary>
 * <description> Refines the worst interval, up to 4096. Holds an object reference during
 * callback calls; writes both outputs only on success. </description> */
static int16_t ECOCALLMETHOD CEcoIntegralSimpson_5BC4E3DF_integral(IEcoAdvancedMathPtr_t me, EcoIntegrandD function,
    voidptr_t context, double_t a, double_t b, double_t epsabs, double_t epsrel,
    double_t* result, double_t* error) {
    CEcoIntegralSimpson_5BC4E3DF* object = (CEcoIntegralSimpson_5BC4E3DF*)me;
    CEcoIntegralSimpson_5BC4E3DF_D_Interval* intervals = 0;
    double_t q = (double_t)0;
    double_t estimate = (double_t)0;
    double_t threshold = (double_t)0;
    double_t relative = (double_t)0;
    double_t midpoint = (double_t)0;
    double_t left = (double_t)0;
    double_t right = (double_t)0;
    double_t swap = (double_t)0;
    uint32_t count = 1;
    uint32_t worst = 0;
    uint32_t i = 0;
    uint8_t reverse = 0;
    int16_t status = ERR_ECO_FAIL;
    if (me == 0 || function == 0 || result == 0 || error == 0) {
        return ERR_ECO_POINTER;
    }
    if (!CEcoIntegralSimpson_5BC4E3DF_D_Finite(a) || !CEcoIntegralSimpson_5BC4E3DF_D_Finite(b) || !CEcoIntegralSimpson_5BC4E3DF_D_Finite(epsabs) || !CEcoIntegralSimpson_5BC4E3DF_D_Finite(epsrel) ||
        epsabs < (double_t)0 || epsrel < (double_t)0 || (epsabs == (double_t)0 && epsrel == (double_t)0)) {
        return ERR_ECO_INVALIDARG;
    }
    if (a == b) {
        *result = (double_t)0;
        *error = (double_t)0;
        return ERR_ECO_SUCCESS;
    }
    if (a > b) {
        swap = a;
        a = b;
        b = swap;
        reverse = 1;
    }
    me->pVTbl->AddRef(me);
    intervals = (CEcoIntegralSimpson_5BC4E3DF_D_Interval*)object->m_pIMem->pVTbl->Alloc(object->m_pIMem,
        (uint32_t)(4096*sizeof(CEcoIntegralSimpson_5BC4E3DF_D_Interval)));
    if (intervals == 0) {
        status = ERR_ECO_OUTOFMEMORY;
        goto Release;
    }
    intervals[0].a = a;
    intervals[0].b = b;
    status = CEcoIntegralSimpson_5BC4E3DF_D_Rule(function, context, a, b, &intervals[0].q, &intervals[0].error);
    while (status == ERR_ECO_SUCCESS) {
        q = (double_t)0;
        estimate = (double_t)0;
        worst = 0;
        for (i = 0; i < count; i++) {
            q += intervals[i].q;
            estimate += intervals[i].error;
            if (intervals[i].error > intervals[worst].error) {
                worst = i;
            }
        }
        if (!CEcoIntegralSimpson_5BC4E3DF_D_Finite(q) || !CEcoIntegralSimpson_5BC4E3DF_D_Finite(estimate)) {
            status = ERR_ECO_FAIL;
            break;
        }
        relative = epsrel*CEcoIntegralSimpson_5BC4E3DF_D_Abs(q);
        if (!CEcoIntegralSimpson_5BC4E3DF_D_Finite(relative)) {
            status = ERR_ECO_FAIL;
            break;
        }
        threshold = epsabs > relative ? epsabs : relative;
        if (estimate <= threshold) {
            break;
        }
        if (count >= 4096) {
            status = ERR_ECO_FAIL;
            break;
        }
        left = intervals[worst].a;
        right = intervals[worst].b;
        midpoint = CEcoIntegralSimpson_5BC4E3DF_D_Midpoint(left, right);
        if (midpoint <= left || midpoint >= right) {
            status = ERR_ECO_FAIL;
            break;
        }
        status = CEcoIntegralSimpson_5BC4E3DF_D_Rule(function, context, left, midpoint, &intervals[worst].q, &intervals[worst].error);
        if (status != ERR_ECO_SUCCESS) {
            break;
        }
        status = CEcoIntegralSimpson_5BC4E3DF_D_Rule(function, context, midpoint, right, &intervals[count].q, &intervals[count].error);
        if (status != ERR_ECO_SUCCESS) {
            break;
        }
        intervals[worst].b = midpoint;
        intervals[count].a = midpoint;
        intervals[count].b = right;
        count++;
    }
    if (status == ERR_ECO_SUCCESS) {
        *result = reverse ? -q : q;
        *error = estimate;
    }
Release:
    if (intervals != 0) {
        object->m_pIMem->pVTbl->Free(object->m_pIMem, intervals);
    }
    me->pVTbl->Release(me);
    return status;
}

/* <summary> Float absolute value. </summary> <description> See D_Abs. </description> */
static float_t CEcoIntegralSimpson_5BC4E3DF_F_Abs(float_t value) {
    return value < (float_t)0 ? -value : value;
}

/* <summary> Check a float_t value. </summary> <description> See D_Finite. </description> */
static uint8_t CEcoIntegralSimpson_5BC4E3DF_F_Finite(float_t value) {
    return (uint8_t)(value == value && value <= (float_t)__FLT_MAX__ && value >= -(float_t)__FLT_MAX__);
}

/* <summary> Float midpoint. </summary> <description> See D_Midpoint for overflow protection. </description> */
static float_t CEcoIntegralSimpson_5BC4E3DF_F_Midpoint(float_t a, float_t b) {
    if (a < (float_t)0 && b > (float_t)0) {
        return a/(float_t)2 + b/(float_t)2;
    }
    return a + (b-a)/(float_t)2;
}

/* <summary> Evaluate a float_t callback. </summary> <description> See D_Evaluate for failure rules. </description> */
static int16_t CEcoIntegralSimpson_5BC4E3DF_F_Evaluate(EcoIntegrandF function, voidptr_t context, float_t x, float_t* value) {
    *value = function(x, context);
    return CEcoIntegralSimpson_5BC4E3DF_F_Finite(*value) ? ERR_ECO_SUCCESS : ERR_ECO_FAIL;
}

/* <summary> Float Simpson rule. </summary> <description> See D_Rule for the error estimate. </description> */
static int16_t CEcoIntegralSimpson_5BC4E3DF_F_Rule(EcoIntegrandF function, voidptr_t context, float_t a, float_t b,
    float_t* q, float_t* error) {
    float_t values[5];
    float_t points[5];
    float_t width = b-a;
    float_t coarse = (float_t)0;
    float_t fine = (float_t)0;
    float_t magnitude = (float_t)0;
    float_t roundoff = (float_t)0;
    uint32_t i = 0;
    int16_t status = ERR_ECO_SUCCESS;
    if (!CEcoIntegralSimpson_5BC4E3DF_F_Finite(width)) {
        return ERR_ECO_FAIL;
    }
    points[0] = a;
    points[2] = CEcoIntegralSimpson_5BC4E3DF_F_Midpoint(a,b);
    points[4] = b;
    points[1] = CEcoIntegralSimpson_5BC4E3DF_F_Midpoint(a,points[2]);
    points[3] = CEcoIntegralSimpson_5BC4E3DF_F_Midpoint(points[2],b);
    for (i = 0; i < 5; i++) {
        status = CEcoIntegralSimpson_5BC4E3DF_F_Evaluate(function, context, points[i], &values[i]);
        if (status != ERR_ECO_SUCCESS) {
            return status;
        }
    }
    coarse = (width/(float_t)6)*(values[0]+(float_t)4*values[2]+values[4]);
    fine = (width/(float_t)12)*(values[0]+(float_t)4*values[1]+(float_t)2*values[2]+(float_t)4*values[3]+values[4]);
    magnitude = (width/(float_t)12)*(CEcoIntegralSimpson_5BC4E3DF_F_Abs(values[0])+(float_t)4*CEcoIntegralSimpson_5BC4E3DF_F_Abs(values[1])+
        (float_t)2*CEcoIntegralSimpson_5BC4E3DF_F_Abs(values[2])+(float_t)4*CEcoIntegralSimpson_5BC4E3DF_F_Abs(values[3])+CEcoIntegralSimpson_5BC4E3DF_F_Abs(values[4]));
    *q = fine;
    *error = CEcoIntegralSimpson_5BC4E3DF_F_Abs(fine-coarse)/(float_t)15;
    roundoff = ((float_t)4*(float_t)__FLT_EPSILON__)*magnitude;
    if (roundoff > *error) {
        *error = roundoff;
    }
    if (!CEcoIntegralSimpson_5BC4E3DF_F_Finite(coarse) || !CEcoIntegralSimpson_5BC4E3DF_F_Finite(*q) || !CEcoIntegralSimpson_5BC4E3DF_F_Finite(*error) || !CEcoIntegralSimpson_5BC4E3DF_F_Finite(magnitude)) {
        return ERR_ECO_FAIL;
    }
    return ERR_ECO_SUCCESS;
}

typedef struct CEcoIntegralSimpson_5BC4E3DF_F_Interval {
    float_t a;
    float_t b;
    float_t q;
    float_t error;
} CEcoIntegralSimpson_5BC4E3DF_F_Interval;

/* <summary> Integrate using float_t. </summary> <description> See integral for lifetime and output rules. </description> */
static int16_t ECOCALLMETHOD CEcoIntegralSimpson_5BC4E3DF_integralf(IEcoAdvancedMathPtr_t me, EcoIntegrandF function,
    voidptr_t context, float_t a, float_t b, float_t epsabs, float_t epsrel,
    float_t* result, float_t* error) {
    CEcoIntegralSimpson_5BC4E3DF* object = (CEcoIntegralSimpson_5BC4E3DF*)me;
    CEcoIntegralSimpson_5BC4E3DF_F_Interval* intervals = 0;
    float_t q = (float_t)0;
    float_t estimate = (float_t)0;
    float_t threshold = (float_t)0;
    float_t relative = (float_t)0;
    float_t midpoint = (float_t)0;
    float_t left = (float_t)0;
    float_t right = (float_t)0;
    float_t swap = (float_t)0;
    uint32_t count = 1;
    uint32_t worst = 0;
    uint32_t i = 0;
    uint8_t reverse = 0;
    int16_t status = ERR_ECO_FAIL;
    if (me == 0 || function == 0 || result == 0 || error == 0) {
        return ERR_ECO_POINTER;
    }
    if (!CEcoIntegralSimpson_5BC4E3DF_F_Finite(a) || !CEcoIntegralSimpson_5BC4E3DF_F_Finite(b) || !CEcoIntegralSimpson_5BC4E3DF_F_Finite(epsabs) || !CEcoIntegralSimpson_5BC4E3DF_F_Finite(epsrel) ||
        epsabs < (float_t)0 || epsrel < (float_t)0 || (epsabs == (float_t)0 && epsrel == (float_t)0)) {
        return ERR_ECO_INVALIDARG;
    }
    if (a == b) {
        *result = (float_t)0;
        *error = (float_t)0;
        return ERR_ECO_SUCCESS;
    }
    if (a > b) {
        swap = a;
        a = b;
        b = swap;
        reverse = 1;
    }
    me->pVTbl->AddRef(me);
    intervals = (CEcoIntegralSimpson_5BC4E3DF_F_Interval*)object->m_pIMem->pVTbl->Alloc(object->m_pIMem,
        (uint32_t)(4096*sizeof(CEcoIntegralSimpson_5BC4E3DF_F_Interval)));
    if (intervals == 0) {
        status = ERR_ECO_OUTOFMEMORY;
        goto Release;
    }
    intervals[0].a = a;
    intervals[0].b = b;
    status = CEcoIntegralSimpson_5BC4E3DF_F_Rule(function, context, a, b, &intervals[0].q, &intervals[0].error);
    while (status == ERR_ECO_SUCCESS) {
        q = (float_t)0;
        estimate = (float_t)0;
        worst = 0;
        for (i = 0; i < count; i++) {
            q += intervals[i].q;
            estimate += intervals[i].error;
            if (intervals[i].error > intervals[worst].error) {
                worst = i;
            }
        }
        if (!CEcoIntegralSimpson_5BC4E3DF_F_Finite(q) || !CEcoIntegralSimpson_5BC4E3DF_F_Finite(estimate)) {
            status = ERR_ECO_FAIL;
            break;
        }
        relative = epsrel*CEcoIntegralSimpson_5BC4E3DF_F_Abs(q);
        if (!CEcoIntegralSimpson_5BC4E3DF_F_Finite(relative)) {
            status = ERR_ECO_FAIL;
            break;
        }
        threshold = epsabs > relative ? epsabs : relative;
        if (estimate <= threshold) {
            break;
        }
        if (count >= 4096) {
            status = ERR_ECO_FAIL;
            break;
        }
        left = intervals[worst].a;
        right = intervals[worst].b;
        midpoint = CEcoIntegralSimpson_5BC4E3DF_F_Midpoint(left, right);
        if (midpoint <= left || midpoint >= right) {
            status = ERR_ECO_FAIL;
            break;
        }
        status = CEcoIntegralSimpson_5BC4E3DF_F_Rule(function, context, left, midpoint, &intervals[worst].q, &intervals[worst].error);
        if (status != ERR_ECO_SUCCESS) {
            break;
        }
        status = CEcoIntegralSimpson_5BC4E3DF_F_Rule(function, context, midpoint, right, &intervals[count].q, &intervals[count].error);
        if (status != ERR_ECO_SUCCESS) {
            break;
        }
        intervals[worst].b = midpoint;
        intervals[count].a = midpoint;
        intervals[count].b = right;
        count++;
    }
    if (status == ERR_ECO_SUCCESS) {
        *result = reverse ? -q : q;
        *error = estimate;
    }
Release:
    if (intervals != 0) {
        object->m_pIMem->pVTbl->Free(object->m_pIMem, intervals);
    }
    me->pVTbl->Release(me);
    return status;
}

/* <summary> Long double absolute value. </summary> <description> See D_Abs. </description> */
static long double CEcoIntegralSimpson_5BC4E3DF_L_Abs(long double value) {
    return value < (long double)0 ? -value : value;
}

/* <summary> Check a long double value. </summary> <description> See D_Finite. </description> */
static uint8_t CEcoIntegralSimpson_5BC4E3DF_L_Finite(long double value) {
    return (uint8_t)(value == value && value <= (long double)__LDBL_MAX__ && value >= -(long double)__LDBL_MAX__);
}

/* <summary> Long double midpoint. </summary> <description> See D_Midpoint for overflow protection. </description> */
static long double CEcoIntegralSimpson_5BC4E3DF_L_Midpoint(long double a, long double b) {
    if (a < (long double)0 && b > (long double)0) {
        return a/(long double)2 + b/(long double)2;
    }
    return a + (b-a)/(long double)2;
}

/* <summary> Evaluate a long double callback. </summary> <description> See D_Evaluate for failure rules. </description> */
static int16_t CEcoIntegralSimpson_5BC4E3DF_L_Evaluate(EcoIntegrandL function, voidptr_t context, long double x, long double* value) {
    *value = function(x, context);
    return CEcoIntegralSimpson_5BC4E3DF_L_Finite(*value) ? ERR_ECO_SUCCESS : ERR_ECO_FAIL;
}

/* <summary> Long double Simpson rule. </summary> <description> See D_Rule for the error estimate. </description> */
static int16_t CEcoIntegralSimpson_5BC4E3DF_L_Rule(EcoIntegrandL function, voidptr_t context, long double a, long double b,
    long double* q, long double* error) {
    long double values[5];
    long double points[5];
    long double width = b-a;
    long double coarse = (long double)0;
    long double fine = (long double)0;
    long double magnitude = (long double)0;
    long double roundoff = (long double)0;
    uint32_t i = 0;
    int16_t status = ERR_ECO_SUCCESS;
    if (!CEcoIntegralSimpson_5BC4E3DF_L_Finite(width)) {
        return ERR_ECO_FAIL;
    }
    points[0] = a;
    points[2] = CEcoIntegralSimpson_5BC4E3DF_L_Midpoint(a,b);
    points[4] = b;
    points[1] = CEcoIntegralSimpson_5BC4E3DF_L_Midpoint(a,points[2]);
    points[3] = CEcoIntegralSimpson_5BC4E3DF_L_Midpoint(points[2],b);
    for (i = 0; i < 5; i++) {
        status = CEcoIntegralSimpson_5BC4E3DF_L_Evaluate(function, context, points[i], &values[i]);
        if (status != ERR_ECO_SUCCESS) {
            return status;
        }
    }
    coarse = (width/(long double)6)*(values[0]+(long double)4*values[2]+values[4]);
    fine = (width/(long double)12)*(values[0]+(long double)4*values[1]+(long double)2*values[2]+(long double)4*values[3]+values[4]);
    magnitude = (width/(long double)12)*(CEcoIntegralSimpson_5BC4E3DF_L_Abs(values[0])+(long double)4*CEcoIntegralSimpson_5BC4E3DF_L_Abs(values[1])+
        (long double)2*CEcoIntegralSimpson_5BC4E3DF_L_Abs(values[2])+(long double)4*CEcoIntegralSimpson_5BC4E3DF_L_Abs(values[3])+CEcoIntegralSimpson_5BC4E3DF_L_Abs(values[4]));
    *q = fine;
    *error = CEcoIntegralSimpson_5BC4E3DF_L_Abs(fine-coarse)/(long double)15;
    roundoff = ((long double)4*(long double)__LDBL_EPSILON__)*magnitude;
    if (roundoff > *error) {
        *error = roundoff;
    }
    if (!CEcoIntegralSimpson_5BC4E3DF_L_Finite(coarse) || !CEcoIntegralSimpson_5BC4E3DF_L_Finite(*q) || !CEcoIntegralSimpson_5BC4E3DF_L_Finite(*error) || !CEcoIntegralSimpson_5BC4E3DF_L_Finite(magnitude)) {
        return ERR_ECO_FAIL;
    }
    return ERR_ECO_SUCCESS;
}

typedef struct CEcoIntegralSimpson_5BC4E3DF_L_Interval {
    long double a;
    long double b;
    long double q;
    long double error;
} CEcoIntegralSimpson_5BC4E3DF_L_Interval;

/* <summary> Integrate using long double. </summary> <description> See integral for lifetime and output rules. </description> */
static int16_t ECOCALLMETHOD CEcoIntegralSimpson_5BC4E3DF_integrall(IEcoAdvancedMathPtr_t me, EcoIntegrandL function,
    voidptr_t context, long double a, long double b, long double epsabs, long double epsrel,
    long double* result, long double* error) {
    CEcoIntegralSimpson_5BC4E3DF* object = (CEcoIntegralSimpson_5BC4E3DF*)me;
    CEcoIntegralSimpson_5BC4E3DF_L_Interval* intervals = 0;
    long double q = (long double)0;
    long double estimate = (long double)0;
    long double threshold = (long double)0;
    long double relative = (long double)0;
    long double midpoint = (long double)0;
    long double left = (long double)0;
    long double right = (long double)0;
    long double swap = (long double)0;
    uint32_t count = 1;
    uint32_t worst = 0;
    uint32_t i = 0;
    uint8_t reverse = 0;
    int16_t status = ERR_ECO_FAIL;
    if (me == 0 || function == 0 || result == 0 || error == 0) {
        return ERR_ECO_POINTER;
    }
    if (!CEcoIntegralSimpson_5BC4E3DF_L_Finite(a) || !CEcoIntegralSimpson_5BC4E3DF_L_Finite(b) || !CEcoIntegralSimpson_5BC4E3DF_L_Finite(epsabs) || !CEcoIntegralSimpson_5BC4E3DF_L_Finite(epsrel) ||
        epsabs < (long double)0 || epsrel < (long double)0 || (epsabs == (long double)0 && epsrel == (long double)0)) {
        return ERR_ECO_INVALIDARG;
    }
    if (a == b) {
        *result = (long double)0;
        *error = (long double)0;
        return ERR_ECO_SUCCESS;
    }
    if (a > b) {
        swap = a;
        a = b;
        b = swap;
        reverse = 1;
    }
    me->pVTbl->AddRef(me);
    intervals = (CEcoIntegralSimpson_5BC4E3DF_L_Interval*)object->m_pIMem->pVTbl->Alloc(object->m_pIMem,
        (uint32_t)(4096*sizeof(CEcoIntegralSimpson_5BC4E3DF_L_Interval)));
    if (intervals == 0) {
        status = ERR_ECO_OUTOFMEMORY;
        goto Release;
    }
    intervals[0].a = a;
    intervals[0].b = b;
    status = CEcoIntegralSimpson_5BC4E3DF_L_Rule(function, context, a, b, &intervals[0].q, &intervals[0].error);
    while (status == ERR_ECO_SUCCESS) {
        q = (long double)0;
        estimate = (long double)0;
        worst = 0;
        for (i = 0; i < count; i++) {
            q += intervals[i].q;
            estimate += intervals[i].error;
            if (intervals[i].error > intervals[worst].error) {
                worst = i;
            }
        }
        if (!CEcoIntegralSimpson_5BC4E3DF_L_Finite(q) || !CEcoIntegralSimpson_5BC4E3DF_L_Finite(estimate)) {
            status = ERR_ECO_FAIL;
            break;
        }
        relative = epsrel*CEcoIntegralSimpson_5BC4E3DF_L_Abs(q);
        if (!CEcoIntegralSimpson_5BC4E3DF_L_Finite(relative)) {
            status = ERR_ECO_FAIL;
            break;
        }
        threshold = epsabs > relative ? epsabs : relative;
        if (estimate <= threshold) {
            break;
        }
        if (count >= 4096) {
            status = ERR_ECO_FAIL;
            break;
        }
        left = intervals[worst].a;
        right = intervals[worst].b;
        midpoint = CEcoIntegralSimpson_5BC4E3DF_L_Midpoint(left, right);
        if (midpoint <= left || midpoint >= right) {
            status = ERR_ECO_FAIL;
            break;
        }
        status = CEcoIntegralSimpson_5BC4E3DF_L_Rule(function, context, left, midpoint, &intervals[worst].q, &intervals[worst].error);
        if (status != ERR_ECO_SUCCESS) {
            break;
        }
        status = CEcoIntegralSimpson_5BC4E3DF_L_Rule(function, context, midpoint, right, &intervals[count].q, &intervals[count].error);
        if (status != ERR_ECO_SUCCESS) {
            break;
        }
        intervals[worst].b = midpoint;
        intervals[count].a = midpoint;
        intervals[count].b = right;
        count++;
    }
    if (status == ERR_ECO_SUCCESS) {
        *result = reverse ? -q : q;
        *error = estimate;
    }
Release:
    if (intervals != 0) {
        object->m_pIMem->pVTbl->Free(object->m_pIMem, intervals);
    }
    me->pVTbl->Release(me);
    return status;
}




/* <summary> Acquire the system interface. </summary>
 * <description> The object owns the returned system reference. </description> */
static int16_t ECOCALLMETHOD initCEcoIntegralSimpson_5BC4E3DF(CEcoIntegralSimpson_5BC4E3DFPtr_t object, IEcoUnknownPtr_t system) {
    if (object == 0 || system == 0) {
        return ERR_ECO_POINTER;
    }
    return system->pVTbl->QueryInterface(system, &GID_IEcoSystem, (voidptr_t*)&object->m_pISys);
}

/* <summary> Validate stand-alone creation. </summary>
 * <description> Rejects aggregation with ERR_ECO_NOAGGREGATION. </description> */
static int16_t ECOCALLMETHOD createCEcoIntegralSimpson_5BC4E3DF(CEcoIntegralSimpson_5BC4E3DFPtr_t object, IEcoUnknownPtr_t system, IEcoUnknownPtr_t outer) {
    if (object == 0 || system == 0) {
        return ERR_ECO_POINTER;
    }
    if (outer != 0) {
        return ERR_ECO_NOAGGREGATION;
    }
    return ERR_ECO_SUCCESS;
}

/* <summary> Destroy the object. </summary>
 * <description> Free object memory before releasing its allocator. </description> */
static void ECOCALLMETHOD deleteCEcoIntegralSimpson_5BC4E3DF(CEcoIntegralSimpson_5BC4E3DFPtr_t object) {
    IEcoMemoryAllocator1* memory = 0;
    if (object == 0) {
        return;
    }
    memory = object->m_pIMem;
    if (object->m_pISys != 0) {
        object->m_pISys->pVTbl->Release(object->m_pISys);
    }
    memory->pVTbl->Free(memory, object);
    memory->pVTbl->Release(memory);
}

IEcoAdvancedMathVTbl g_x667182A35F5C4F909244C763139ED855VTbl_5BC4E3DF = {
    CEcoIntegralSimpson_5BC4E3DF_QueryInterface,
    CEcoIntegralSimpson_5BC4E3DF_AddRef,
    CEcoIntegralSimpson_5BC4E3DF_Release,
    CEcoIntegralSimpson_5BC4E3DF_integral,
    CEcoIntegralSimpson_5BC4E3DF_integralf,
    CEcoIntegralSimpson_5BC4E3DF_integrall
};



CEcoIntegralSimpson_5BC4E3DF g_xCEcoIntegralSimpson_5BC4E3DF = {
    &g_x667182A35F5C4F909244C763139ED855VTbl_5BC4E3DF,
   
    initCEcoIntegralSimpson_5BC4E3DF,
    createCEcoIntegralSimpson_5BC4E3DF,
    deleteCEcoIntegralSimpson_5BC4E3DF,
    1, /* m_cRef */
    0,
    0
};
