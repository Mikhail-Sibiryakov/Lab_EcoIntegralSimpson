/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> EcoIntegralSimpson console UnitTest. </summary>
 * <description> Checks the Simpson component and all three numeric methods through Eco. </description>
 * <reference> Eco.AI.Promts/Templates/ROOT.c; docs/DESIGN.md </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */

#include "IEcoSystem1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IEcoFile1.h"
#include "IdEcoIntegralSimpson.h"
#include "IEcoAdvancedMath.h"

static uint32_t g_Passed = 0;
static uint32_t g_Failed = 0;
static int16_t g_OutputStatus = ERR_ECO_SUCCESS;

/* <summary> Write text through Eco. </summary>
 * <description> A failed or partial write makes the UnitTest fail. </description> */
static void Print(IEcoFile1* file, char_t* text) {
    uint32_t length = 0;
    uint32_t size = 0;
    int16_t status = ERR_ECO_SUCCESS;
    while (text[length] != 0) {
        length++;
    }
    size = length;
    status = file->pVTbl->Write(file, text, &size);
    if (status != ERR_ECO_SUCCESS || size != length) {
        g_OutputStatus = ERR_ECO_FAIL;
    }
}

/* <summary> Record a test result. </summary>
 * <description> Any failed check makes EcoMain fail. </description> */
static void Check(IEcoFile1* file, uint8_t condition, char_t* name) {
    if (condition != 0) {
        g_Passed++;
        Print(file, "PASS ");
    }
    else {
        g_Failed++;
        Print(file, "FAIL ");
    }
    Print(file, name);
    Print(file, "\n");
}

/* <summary> Format a finite number. </summary>
 * <description> Scientific notation with 22 significant digits. </description> */
static void EmitNumber(IEcoFile1* file, long double number) {
    char_t buffer[48];
    uint32_t position = 0;
    uint32_t digit = 0;
    uint32_t i = 0;
    int32_t exponent = 0;
    uint32_t magnitude = 0;
    if (number < 0.0L) {
        buffer[position++] = '-';
        number = -number;
    }
    if (number != 0.0L) {
        while (number >= 10.0L) {
            number /= 10.0L;
            exponent++;
        }
        while (number < 1.0L) {
            number *= 10.0L;
            exponent--;
        }
    }
    for (i = 0; i < 22; i++) {
        digit = (uint32_t)number;
        if (digit > 9) {
            digit = 9;
        }
        buffer[position++] = (char_t)('0' + digit);
        if (i == 0) {
            buffer[position++] = '.';
        }
        number = (number - (long double)digit)*10.0L;
        if (number < 0.0L) {
            number = 0.0L;
        }
    }
    buffer[position++] = 'e';
    buffer[position++] = exponent < 0 ? '-' : '+';
    magnitude = (uint32_t)(exponent < 0 ? -exponent : exponent);
    buffer[position++] = (char_t)('0' + (magnitude/1000)%10);
    buffer[position++] = (char_t)('0' + (magnitude/100)%10);
    buffer[position++] = (char_t)('0' + (magnitude/10)%10);
    buffer[position++] = (char_t)('0' + magnitude%10);
    buffer[position] = 0;
    Print(file, buffer);
}

/* <summary> Write a decimal counter. </summary>
 * <description> Formats digits in a local buffer. </description> */
static void PrintUInt(IEcoFile1* file, uint32_t number) {
    char_t digits[11];
    char_t result[11];
    uint32_t count = 0;
    uint32_t i = 0;
    do {
        digits[count++] = (char_t)('0' + number%10);
        number /= 10;
    }
    while (number != 0);
    for (i = 0; i < count; i++) {
        result[i] = digits[count-1-i];
    }
    result[count] = 0;
    Print(file, result);
}

/* <summary> Write a comparison record. </summary>
 * <description> RESULT;algorithm;type;case;a;b;epsabs;epsrel;Q;E;calls. </description> */
static void EmitResult(IEcoFile1* file, char_t* algorithm, char_t* type, char_t* name,
    long double a, long double b, long double epsabs, long double epsrel,
    long double q, long double error, uint32_t calls) {
    Print(file, "RESULT;");
    Print(file, algorithm);
    Print(file, ";");
    Print(file, type);
    Print(file, ";");
    Print(file, name);
    Print(file, ";");
    EmitNumber(file, a);
    Print(file, ";");
    EmitNumber(file, b);
    Print(file, ";");
    EmitNumber(file, epsabs);
    Print(file, ";");
    EmitNumber(file, epsrel);
    Print(file, ";");
    EmitNumber(file, q);
    Print(file, ";");
    EmitNumber(file, error);
    Print(file, ";");
    PrintUInt(file, calls);
    Print(file, "\n");
}

typedef struct TestD_Context {
    uint32_t calls;
    uint32_t mode;
    double_t scale;
} TestD_Context;

/* <summary> Evaluate the selected test function. </summary>
 * <description> Context selects the function and scale and counts evaluations. </description> */
static double_t ECOCDECLMETHOD TestD_Function(double_t x, voidptr_t context) {
    TestD_Context* data = (TestD_Context*)context;
    double_t value = (double_t)1;
    uint32_t i = 0;
    if (data == 0) {
        return (double_t)1;
    }
    data->calls++;
    if (data->mode == 1) {
        value = x*x;
    }
    else if (data->mode == 2) {
        for (i = 0; i < 14; i++) {
            value *= x;
        }
    }
    else if (data->mode == 3) {
        value = x - (double_t)0.375;
        value = (double_t)1 / ((double_t)1 + (double_t)400*value*value);
    }
    else if (data->mode == 4) {
        value = x - (double_t)0.5;
    }
    else if (data->mode == 5) {
        return __builtin_nan("");
    }
    else if (data->mode == 6) {
        return __builtin_inf();
    }
    return value*data->scale;
}

/* <summary> Test integration using double_t. </summary>
 * <description> Checks known integrals, error handling and subdivision limits. </description> */
static void TestD_Test(IEcoFile1* file, IEcoAdvancedMath* component, char_t* algorithm, uint32_t initialCalls) {
    TestD_Context context;
    double_t q = (double_t)123;
    double_t error = (double_t)456;
    double_t expected = (double_t)0;
    double_t difference = (double_t)0;
    double_t tolerance = (double_t)1e-11;
    double_t a = (double_t)0;
    double_t b = (double_t)1;
    double_t absTolerance = (double_t)1e-11;
    double_t relTolerance = (double_t)1e-11;
    int16_t status = ERR_ECO_SUCCESS;
    uint32_t n = 0;
    char_t* names[10];
    names[0] = "constant";
    names[1] = "quadratic";
    names[2] = "power14";
    names[3] = "peak";
    names[4] = "odd";
    names[5] = "negative";
    names[6] = "small";
    names[7] = "large";
    names[8] = "absolute_only";
    names[9] = "relative_only";
    Print(file, "TYPE double\n");
    for (n = 0; n < 10; n++) {
        context.calls = 0;
        context.mode = n < 5 ? n : 1;
        context.scale = (double_t)1;
        a = (double_t)0;
        b = (double_t)1;
        absTolerance = tolerance;
        relTolerance = tolerance;
        expected = (double_t)1;
        if (n == 1 || n >= 5) {
            expected = (double_t)1/(double_t)3;
        }
        if (n == 2) {
            expected = (double_t)1/(double_t)15;
        }
        if (n == 3) {
            expected = (double_t)0.1464605567790440950649885874003950335360722615884534686L;
        }
        if (n == 4) {
            expected = (double_t)0;
        }
        if (n == 5) {
            context.scale = (double_t)-1;
            expected = -expected;
        }
        if (n == 6) {
            context.scale = (double_t)0.0001;
            expected *= context.scale;
        }
        if (n == 7) {
            context.scale = (double_t)100;
            expected *= context.scale;
        }
        if (n == 8) {
            absTolerance = tolerance;
            relTolerance = (double_t)0;
        }
        if (n == 9) {
            absTolerance = (double_t)0;
            relTolerance = tolerance;
        }
        status = component->pVTbl->integral(component, TestD_Function, &context, a, b,
            absTolerance, relTolerance, &q, &error);
        difference = q - expected;
        if (difference < (double_t)0) {
            difference = -difference;
        }
        Check(file, (uint8_t)(status == 0 && difference <= (double_t)8*tolerance*((double_t)1+(expected < (double_t)0 ? -expected : expected))), names[n]);
        Check(file, (uint8_t)(status == 0 && error >= (double_t)0 && (error <= absTolerance || error <= relTolerance*(q < (double_t)0 ? -q : q))), "returned global error criterion");
        if (n == 3) {
            Check(file, (uint8_t)(status == 0 && context.calls > initialCalls), "adaptive refinement on a peak");
        }
        if (status == 0) {
            EmitResult(file, algorithm, "double", names[n], (long double)a, (long double)b,
                (long double)absTolerance, (long double)relTolerance, (long double)q, (long double)error, context.calls);
        }
    }
    context.calls = 0;
    context.mode = 1;
    context.scale = (double_t)1;
    status = component->pVTbl->integral(component, TestD_Function, &context, (double_t)1, (double_t)0, tolerance, tolerance, &q, &error);
    difference = q + (double_t)1/(double_t)3;
    if (difference < (double_t)0) {
        difference = -difference;
    }
    Check(file, (uint8_t)(status == 0 && difference < (double_t)8*tolerance && error >= (double_t)0), "reverse interval");
    context.calls = 0;
    status = component->pVTbl->integral(component, TestD_Function, &context, (double_t)2, (double_t)2, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == 0 && q == (double_t)0 && error == (double_t)0 && context.calls == 0), "equal bounds without callback");
    status = component->pVTbl->integral(component, TestD_Function, 0, (double_t)0, (double_t)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == 0 && q == (double_t)1), "optional NULL context");
    context.mode = 0;
    context.scale = (double_t)0;
    status = component->pVTbl->integral(component, TestD_Function, &context, (double_t)0, (double_t)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == 0 && q == (double_t)0), "zero function value is valid");
    context.scale = (double_t)1;
    q = (double_t)123;
    error = (double_t)456;
    status = component->pVTbl->integral(0, TestD_Function, &context, (double_t)0, (double_t)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_POINTER && q == (double_t)123 && error == (double_t)456), "NULL me preserves outputs");
    status = component->pVTbl->integral(component, 0, &context, (double_t)0, (double_t)0, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_POINTER && q == (double_t)123 && error == (double_t)456), "NULL callback even for equal bounds");
    status = component->pVTbl->integral(component, TestD_Function, &context, (double_t)0, (double_t)1, tolerance, tolerance, 0, &error);
    Check(file, (uint8_t)(status == ERR_ECO_POINTER && error == (double_t)456), "NULL result");
    status = component->pVTbl->integral(component, TestD_Function, &context, (double_t)0, (double_t)1, tolerance, tolerance, &q, 0);
    Check(file, (uint8_t)(status == ERR_ECO_POINTER && q == (double_t)123), "NULL error estimate");
    context.calls = 0;
    status = component->pVTbl->integral(component, TestD_Function, &context, (double_t)0, (double_t)0, (double_t)0, (double_t)0, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (double_t)123 && error == (double_t)456 && context.calls == 0), "both tolerances zero even for equal bounds");
    status = component->pVTbl->integral(component, TestD_Function, &context, (double_t)0, (double_t)1, (double_t)-1, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (double_t)123 && error == (double_t)456), "negative absolute tolerance");
    status = component->pVTbl->integral(component, TestD_Function, &context, (double_t)0, (double_t)1, tolerance, (double_t)-1, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (double_t)123 && error == (double_t)456), "negative relative tolerance");
    status = component->pVTbl->integral(component, TestD_Function, &context, __builtin_nan(""), (double_t)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (double_t)123 && error == (double_t)456), "NaN bound");
    status = component->pVTbl->integral(component, TestD_Function, &context, (double_t)0, __builtin_inf(), tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (double_t)123 && error == (double_t)456), "infinite bound");
    status = component->pVTbl->integral(component, TestD_Function, &context, (double_t)0, (double_t)1, __builtin_nan(""), tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (double_t)123 && error == (double_t)456), "NaN tolerance");
    status = component->pVTbl->integral(component, TestD_Function, &context, (double_t)0, (double_t)1, tolerance, __builtin_inf(), &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (double_t)123 && error == (double_t)456), "infinite tolerance");
    context.mode = 5;
    status = component->pVTbl->integral(component, TestD_Function, &context, (double_t)0, (double_t)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_FAIL && q == (double_t)123 && error == (double_t)456), "NaN callback");
    context.mode = 6;
    status = component->pVTbl->integral(component, TestD_Function, &context, (double_t)0, (double_t)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_FAIL && q == (double_t)123 && error == (double_t)456), "infinite callback");
    context.mode = 3;
    context.calls = 0;
    status = component->pVTbl->integral(component, TestD_Function, &context, (double_t)0, (double_t)1, (double_t)1e-40L, (double_t)0, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_FAIL && q == (double_t)123 && error == (double_t)456), "unattainable precision preserves outputs");
    Check(file, (uint8_t)(context.calls <= initialCalls*(2*4096-1)), "bounded number of evaluations");
    Print(file, "LIMIT;");
    Print(file, algorithm);
    Print(file, ";double;");
    PrintUInt(file, context.calls);
    Print(file, "\n");
    context.mode = 0;
    status = component->pVTbl->integral(component, TestD_Function, &context, (double_t)1, (double_t)1+(double_t)__DBL_EPSILON__, (double_t)1e-40L, (double_t)0, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_FAIL && q == (double_t)123 && error == (double_t)456), "rounding prevents further subdivision");
    context.mode = 1;
    status = component->pVTbl->integral(component, TestD_Function, &context, (double_t)0, (double_t)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == 0), "valid call after failures");
}

typedef struct TestF_Context {
    uint32_t calls;
    uint32_t mode;
    float_t scale;
} TestF_Context;

/* <summary> Evaluate the selected float function. </summary> <description> See TestD_Function. </description> */
static float_t ECOCDECLMETHOD TestF_Function(float_t x, voidptr_t context) {
    TestF_Context* data = (TestF_Context*)context;
    float_t value = (float_t)1;
    uint32_t i = 0;
    if (data == 0) {
        return (float_t)1;
    }
    data->calls++;
    if (data->mode == 1) {
        value = x*x;
    }
    else if (data->mode == 2) {
        for (i = 0; i < 14; i++) {
            value *= x;
        }
    }
    else if (data->mode == 3) {
        value = x - (float_t)0.375;
        value = (float_t)1 / ((float_t)1 + (float_t)400*value*value);
    }
    else if (data->mode == 4) {
        value = x - (float_t)0.5;
    }
    else if (data->mode == 5) {
        return __builtin_nanf("");
    }
    else if (data->mode == 6) {
        return __builtin_inff();
    }
    return value*data->scale;
}

/* <summary> Test integration using float_t. </summary> <description> Same cases as TestD_Test. </description> */
static void TestF_Test(IEcoFile1* file, IEcoAdvancedMath* component, char_t* algorithm, uint32_t initialCalls) {
    TestF_Context context;
    float_t q = (float_t)123;
    float_t error = (float_t)456;
    float_t expected = (float_t)0;
    float_t difference = (float_t)0;
    float_t tolerance = (float_t)2e-6;
    float_t a = (float_t)0;
    float_t b = (float_t)1;
    float_t absTolerance = (float_t)2e-6;
    float_t relTolerance = (float_t)2e-6;
    int16_t status = ERR_ECO_SUCCESS;
    uint32_t n = 0;
    char_t* names[10];
    names[0] = "constant";
    names[1] = "quadratic";
    names[2] = "power14";
    names[3] = "peak";
    names[4] = "odd";
    names[5] = "negative";
    names[6] = "small";
    names[7] = "large";
    names[8] = "absolute_only";
    names[9] = "relative_only";
    Print(file, "TYPE float\n");
    for (n = 0; n < 10; n++) {
        context.calls = 0;
        context.mode = n < 5 ? n : 1;
        context.scale = (float_t)1;
        a = (float_t)0;
        b = (float_t)1;
        absTolerance = tolerance;
        relTolerance = tolerance;
        expected = (float_t)1;
        if (n == 1 || n >= 5) {
            expected = (float_t)1/(float_t)3;
        }
        if (n == 2) {
            expected = (float_t)1/(float_t)15;
        }
        if (n == 3) {
            expected = (float_t)0.1464605567790440950649885874003950335360722615884534686L;
        }
        if (n == 4) {
            expected = (float_t)0;
        }
        if (n == 5) {
            context.scale = (float_t)-1;
            expected = -expected;
        }
        if (n == 6) {
            context.scale = (float_t)0.0001;
            expected *= context.scale;
        }
        if (n == 7) {
            context.scale = (float_t)100;
            expected *= context.scale;
        }
        if (n == 8) {
            absTolerance = tolerance;
            relTolerance = (float_t)0;
        }
        if (n == 9) {
            absTolerance = (float_t)0;
            relTolerance = tolerance;
        }
        status = component->pVTbl->integralf(component, TestF_Function, &context, a, b,
            absTolerance, relTolerance, &q, &error);
        difference = q - expected;
        if (difference < (float_t)0) {
            difference = -difference;
        }
        Check(file, (uint8_t)(status == 0 && difference <= (float_t)8*tolerance*((float_t)1+(expected < (float_t)0 ? -expected : expected))), names[n]);
        Check(file, (uint8_t)(status == 0 && error >= (float_t)0 && (error <= absTolerance || error <= relTolerance*(q < (float_t)0 ? -q : q))), "returned global error criterion");
        if (n == 3) {
            Check(file, (uint8_t)(status == 0 && context.calls > initialCalls), "adaptive refinement on a peak");
        }
        if (status == 0) {
            EmitResult(file, algorithm, "float", names[n], (long double)a, (long double)b,
                (long double)absTolerance, (long double)relTolerance, (long double)q, (long double)error, context.calls);
        }
    }
    context.calls = 0;
    context.mode = 1;
    context.scale = (float_t)1;
    status = component->pVTbl->integralf(component, TestF_Function, &context, (float_t)1, (float_t)0, tolerance, tolerance, &q, &error);
    difference = q + (float_t)1/(float_t)3;
    if (difference < (float_t)0) {
        difference = -difference;
    }
    Check(file, (uint8_t)(status == 0 && difference < (float_t)8*tolerance && error >= (float_t)0), "reverse interval");
    context.calls = 0;
    status = component->pVTbl->integralf(component, TestF_Function, &context, (float_t)2, (float_t)2, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == 0 && q == (float_t)0 && error == (float_t)0 && context.calls == 0), "equal bounds without callback");
    status = component->pVTbl->integralf(component, TestF_Function, 0, (float_t)0, (float_t)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == 0 && q == (float_t)1), "optional NULL context");
    context.mode = 0;
    context.scale = (float_t)0;
    status = component->pVTbl->integralf(component, TestF_Function, &context, (float_t)0, (float_t)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == 0 && q == (float_t)0), "zero function value is valid");
    context.scale = (float_t)1;
    q = (float_t)123;
    error = (float_t)456;
    status = component->pVTbl->integralf(0, TestF_Function, &context, (float_t)0, (float_t)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_POINTER && q == (float_t)123 && error == (float_t)456), "NULL me preserves outputs");
    status = component->pVTbl->integralf(component, 0, &context, (float_t)0, (float_t)0, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_POINTER && q == (float_t)123 && error == (float_t)456), "NULL callback even for equal bounds");
    status = component->pVTbl->integralf(component, TestF_Function, &context, (float_t)0, (float_t)1, tolerance, tolerance, 0, &error);
    Check(file, (uint8_t)(status == ERR_ECO_POINTER && error == (float_t)456), "NULL result");
    status = component->pVTbl->integralf(component, TestF_Function, &context, (float_t)0, (float_t)1, tolerance, tolerance, &q, 0);
    Check(file, (uint8_t)(status == ERR_ECO_POINTER && q == (float_t)123), "NULL error estimate");
    context.calls = 0;
    status = component->pVTbl->integralf(component, TestF_Function, &context, (float_t)0, (float_t)0, (float_t)0, (float_t)0, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (float_t)123 && error == (float_t)456 && context.calls == 0), "both tolerances zero even for equal bounds");
    status = component->pVTbl->integralf(component, TestF_Function, &context, (float_t)0, (float_t)1, (float_t)-1, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (float_t)123 && error == (float_t)456), "negative absolute tolerance");
    status = component->pVTbl->integralf(component, TestF_Function, &context, (float_t)0, (float_t)1, tolerance, (float_t)-1, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (float_t)123 && error == (float_t)456), "negative relative tolerance");
    status = component->pVTbl->integralf(component, TestF_Function, &context, __builtin_nanf(""), (float_t)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (float_t)123 && error == (float_t)456), "NaN bound");
    status = component->pVTbl->integralf(component, TestF_Function, &context, (float_t)0, __builtin_inff(), tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (float_t)123 && error == (float_t)456), "infinite bound");
    status = component->pVTbl->integralf(component, TestF_Function, &context, (float_t)0, (float_t)1, __builtin_nanf(""), tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (float_t)123 && error == (float_t)456), "NaN tolerance");
    status = component->pVTbl->integralf(component, TestF_Function, &context, (float_t)0, (float_t)1, tolerance, __builtin_inff(), &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (float_t)123 && error == (float_t)456), "infinite tolerance");
    context.mode = 5;
    status = component->pVTbl->integralf(component, TestF_Function, &context, (float_t)0, (float_t)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_FAIL && q == (float_t)123 && error == (float_t)456), "NaN callback");
    context.mode = 6;
    status = component->pVTbl->integralf(component, TestF_Function, &context, (float_t)0, (float_t)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_FAIL && q == (float_t)123 && error == (float_t)456), "infinite callback");
    context.mode = 3;
    context.calls = 0;
    status = component->pVTbl->integralf(component, TestF_Function, &context, (float_t)0, (float_t)1, (float_t)1e-40L, (float_t)0, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_FAIL && q == (float_t)123 && error == (float_t)456), "unattainable precision preserves outputs");
    Check(file, (uint8_t)(context.calls <= initialCalls*(2*4096-1)), "bounded number of evaluations");
    Print(file, "LIMIT;");
    Print(file, algorithm);
    Print(file, ";float;");
    PrintUInt(file, context.calls);
    Print(file, "\n");
    context.mode = 0;
    status = component->pVTbl->integralf(component, TestF_Function, &context, (float_t)1, (float_t)1+(float_t)__FLT_EPSILON__, (float_t)1e-40L, (float_t)0, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_FAIL && q == (float_t)123 && error == (float_t)456), "rounding prevents further subdivision");
    context.mode = 1;
    status = component->pVTbl->integralf(component, TestF_Function, &context, (float_t)0, (float_t)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == 0), "valid call after failures");
}

typedef struct TestL_Context {
    uint32_t calls;
    uint32_t mode;
    long double scale;
} TestL_Context;

/* <summary> Evaluate the selected long double function. </summary> <description> See TestD_Function. </description> */
static long double ECOCDECLMETHOD TestL_Function(long double x, voidptr_t context) {
    TestL_Context* data = (TestL_Context*)context;
    long double value = (long double)1;
    uint32_t i = 0;
    if (data == 0) {
        return (long double)1;
    }
    data->calls++;
    if (data->mode == 1) {
        value = x*x;
    }
    else if (data->mode == 2) {
        for (i = 0; i < 14; i++) {
            value *= x;
        }
    }
    else if (data->mode == 3) {
        value = x - (long double)0.375;
        value = (long double)1 / ((long double)1 + (long double)400*value*value);
    }
    else if (data->mode == 4) {
        value = x - (long double)0.5;
    }
    else if (data->mode == 5) {
        return __builtin_nanl("");
    }
    else if (data->mode == 6) {
        return __builtin_infl();
    }
    return value*data->scale;
}

/* <summary> Test integration using long double. </summary> <description> Same cases as TestD_Test. </description> */
static void TestL_Test(IEcoFile1* file, IEcoAdvancedMath* component, char_t* algorithm, uint32_t initialCalls) {
    TestL_Context context;
    long double q = (long double)123;
    long double error = (long double)456;
    long double expected = (long double)0;
    long double difference = (long double)0;
    long double tolerance = (long double)2e-13L;
    long double a = (long double)0;
    long double b = (long double)1;
    long double absTolerance = (long double)2e-13L;
    long double relTolerance = (long double)2e-13L;
    int16_t status = ERR_ECO_SUCCESS;
    uint32_t n = 0;
    char_t* names[10];
    names[0] = "constant";
    names[1] = "quadratic";
    names[2] = "power14";
    names[3] = "peak";
    names[4] = "odd";
    names[5] = "negative";
    names[6] = "small";
    names[7] = "large";
    names[8] = "absolute_only";
    names[9] = "relative_only";
    Print(file, "TYPE longdouble\n");
    for (n = 0; n < 10; n++) {
        context.calls = 0;
        context.mode = n < 5 ? n : 1;
        context.scale = (long double)1;
        a = (long double)0;
        b = (long double)1;
        absTolerance = tolerance;
        relTolerance = tolerance;
        expected = (long double)1;
        if (n == 1 || n >= 5) {
            expected = (long double)1/(long double)3;
        }
        if (n == 2) {
            expected = (long double)1/(long double)15;
        }
        if (n == 3) {
            expected = (long double)0.1464605567790440950649885874003950335360722615884534686L;
        }
        if (n == 4) {
            expected = (long double)0;
        }
        if (n == 5) {
            context.scale = (long double)-1;
            expected = -expected;
        }
        if (n == 6) {
            context.scale = (long double)0.0001;
            expected *= context.scale;
        }
        if (n == 7) {
            context.scale = (long double)100;
            expected *= context.scale;
        }
        if (n == 8) {
            absTolerance = tolerance;
            relTolerance = (long double)0;
        }
        if (n == 9) {
            absTolerance = (long double)0;
            relTolerance = tolerance;
        }
        status = component->pVTbl->integrall(component, TestL_Function, &context, a, b,
            absTolerance, relTolerance, &q, &error);
        difference = q - expected;
        if (difference < (long double)0) {
            difference = -difference;
        }
        Check(file, (uint8_t)(status == 0 && difference <= (long double)8*tolerance*((long double)1+(expected < (long double)0 ? -expected : expected))), names[n]);
        Check(file, (uint8_t)(status == 0 && error >= (long double)0 && (error <= absTolerance || error <= relTolerance*(q < (long double)0 ? -q : q))), "returned global error criterion");
        if (n == 3) {
            Check(file, (uint8_t)(status == 0 && context.calls > initialCalls), "adaptive refinement on a peak");
        }
        if (status == 0) {
            EmitResult(file, algorithm, "longdouble", names[n], (long double)a, (long double)b,
                (long double)absTolerance, (long double)relTolerance, (long double)q, (long double)error, context.calls);
        }
    }
    context.calls = 0;
    context.mode = 1;
    context.scale = (long double)1;
    status = component->pVTbl->integrall(component, TestL_Function, &context, (long double)1, (long double)0, tolerance, tolerance, &q, &error);
    difference = q + (long double)1/(long double)3;
    if (difference < (long double)0) {
        difference = -difference;
    }
    Check(file, (uint8_t)(status == 0 && difference < (long double)8*tolerance && error >= (long double)0), "reverse interval");
    context.calls = 0;
    status = component->pVTbl->integrall(component, TestL_Function, &context, (long double)2, (long double)2, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == 0 && q == (long double)0 && error == (long double)0 && context.calls == 0), "equal bounds without callback");
    status = component->pVTbl->integrall(component, TestL_Function, 0, (long double)0, (long double)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == 0 && q == (long double)1), "optional NULL context");
    context.mode = 0;
    context.scale = (long double)0;
    status = component->pVTbl->integrall(component, TestL_Function, &context, (long double)0, (long double)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == 0 && q == (long double)0), "zero function value is valid");
    context.scale = (long double)1;
    q = (long double)123;
    error = (long double)456;
    status = component->pVTbl->integrall(0, TestL_Function, &context, (long double)0, (long double)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_POINTER && q == (long double)123 && error == (long double)456), "NULL me preserves outputs");
    status = component->pVTbl->integrall(component, 0, &context, (long double)0, (long double)0, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_POINTER && q == (long double)123 && error == (long double)456), "NULL callback even for equal bounds");
    status = component->pVTbl->integrall(component, TestL_Function, &context, (long double)0, (long double)1, tolerance, tolerance, 0, &error);
    Check(file, (uint8_t)(status == ERR_ECO_POINTER && error == (long double)456), "NULL result");
    status = component->pVTbl->integrall(component, TestL_Function, &context, (long double)0, (long double)1, tolerance, tolerance, &q, 0);
    Check(file, (uint8_t)(status == ERR_ECO_POINTER && q == (long double)123), "NULL error estimate");
    context.calls = 0;
    status = component->pVTbl->integrall(component, TestL_Function, &context, (long double)0, (long double)0, (long double)0, (long double)0, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (long double)123 && error == (long double)456 && context.calls == 0), "both tolerances zero even for equal bounds");
    status = component->pVTbl->integrall(component, TestL_Function, &context, (long double)0, (long double)1, (long double)-1, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (long double)123 && error == (long double)456), "negative absolute tolerance");
    status = component->pVTbl->integrall(component, TestL_Function, &context, (long double)0, (long double)1, tolerance, (long double)-1, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (long double)123 && error == (long double)456), "negative relative tolerance");
    status = component->pVTbl->integrall(component, TestL_Function, &context, __builtin_nanl(""), (long double)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (long double)123 && error == (long double)456), "NaN bound");
    status = component->pVTbl->integrall(component, TestL_Function, &context, (long double)0, __builtin_infl(), tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (long double)123 && error == (long double)456), "infinite bound");
    status = component->pVTbl->integrall(component, TestL_Function, &context, (long double)0, (long double)1, __builtin_nanl(""), tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (long double)123 && error == (long double)456), "NaN tolerance");
    status = component->pVTbl->integrall(component, TestL_Function, &context, (long double)0, (long double)1, tolerance, __builtin_infl(), &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_INVALIDARG && q == (long double)123 && error == (long double)456), "infinite tolerance");
    context.mode = 5;
    status = component->pVTbl->integrall(component, TestL_Function, &context, (long double)0, (long double)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_FAIL && q == (long double)123 && error == (long double)456), "NaN callback");
    context.mode = 6;
    status = component->pVTbl->integrall(component, TestL_Function, &context, (long double)0, (long double)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_FAIL && q == (long double)123 && error == (long double)456), "infinite callback");
    context.mode = 3;
    context.calls = 0;
    status = component->pVTbl->integrall(component, TestL_Function, &context, (long double)0, (long double)1, (long double)1e-40L, (long double)0, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_FAIL && q == (long double)123 && error == (long double)456), "unattainable precision preserves outputs");
    Check(file, (uint8_t)(context.calls <= initialCalls*(2*4096-1)), "bounded number of evaluations");
    Print(file, "LIMIT;");
    Print(file, algorithm);
    Print(file, ";longdouble;");
    PrintUInt(file, context.calls);
    Print(file, "\n");
    context.mode = 0;
    status = component->pVTbl->integrall(component, TestL_Function, &context, (long double)1, (long double)1+(long double)__LDBL_EPSILON__, (long double)1e-40L, (long double)0, &q, &error);
    Check(file, (uint8_t)(status == ERR_ECO_FAIL && q == (long double)123 && error == (long double)456), "rounding prevents further subdivision");
    context.mode = 1;
    status = component->pVTbl->integrall(component, TestL_Function, &context, (long double)0, (long double)1, tolerance, tolerance, &q, &error);
    Check(file, (uint8_t)(status == 0), "valid call after failures");
}

/* <summary> Check ACOM identity. </summary>
 * <description> Releases references returned by QueryInterface. </description> */
static void TestIdentity(IEcoFile1* file, IEcoAdvancedMath* component) {
    IEcoUnknown* unknown1 = 0;
    IEcoUnknown* unknown2 = 0;
    IEcoAdvancedMath* again = 0;
    voidptr_t unsupported = component;
    int16_t status = ERR_ECO_SUCCESS;
    const UGUID missing = {0x01, 0x10, {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}};
    status = component->pVTbl->QueryInterface(component, &IID_IEcoUnknown, (voidptr_t*)&unknown1);
    Check(file, (uint8_t)(status == ERR_ECO_SUCCESS && unknown1 != 0), "QueryInterface Unknown");
    status = component->pVTbl->QueryInterface(component, &IID_IEcoAdvancedMath, (voidptr_t*)&again);
    Check(file, (uint8_t)(status == ERR_ECO_SUCCESS && again == component), "QueryInterface reflexivity");
    if (unknown1 != 0 && again != 0) {
        status = again->pVTbl->QueryInterface(again, &IID_IEcoUnknown, (voidptr_t*)&unknown2);
        Check(file, (uint8_t)(status == ERR_ECO_SUCCESS && unknown2 == unknown1), "one Unknown identity");
    }
    status = component->pVTbl->QueryInterface(component, &missing, &unsupported);
    Check(file, (uint8_t)(status == ERR_ECO_NOINTERFACE && unsupported == 0), "unsupported IID clears out");
    status = component->pVTbl->QueryInterface(component, 0, &unsupported);
    Check(file, (uint8_t)(status == ERR_ECO_POINTER && unsupported == 0), "NULL IID");
    status = component->pVTbl->QueryInterface(component, &IID_IEcoUnknown, 0);
    Check(file, (uint8_t)(status == ERR_ECO_POINTER), "NULL interface out");
    if (unknown2 != 0) {
        unknown2->pVTbl->Release(unknown2);
    }
    if (again != 0) {
        again->pVTbl->Release(again);
    }
    if (unknown1 != 0) {
        unknown1->pVTbl->Release(unknown1);
    }
}

/* <summary> Run the console UnitTest. </summary>
 * <description> Registers the Simpson factory; any failed check makes EcoMain fail. </description> */
int16_t EcoMain(IEcoUnknown* system) {
    IEcoSystem1* sys = 0;
    IEcoInterfaceBus1* bus = 0;
    IEcoFileManager1* files = 0;
    IEcoFile1* output = 0;
    IEcoAdvancedMath* component = 0;
    IEcoAdvancedMath* second = 0;
    int16_t status = ERR_ECO_POINTER;
    int16_t result = ERR_ECO_FAIL;
    if (system == 0) {
        return ERR_ECO_POINTER;
    }
    status = system->pVTbl->QueryInterface(system, &GID_IEcoSystem, (voidptr_t*)&sys);
    if (status != 0 || sys == 0) {
        goto Release;
    }
    status = sys->pVTbl->QueryInterface(sys, &IID_IEcoInterfaceBus1, (voidptr_t*)&bus);
    if (status != 0 || bus == 0) {
        goto Release;
    }
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoFileSystemManagement1, 0, &IID_IEcoFileManager1, (voidptr_t*)&files);
    if (status != 0 || files == 0) {
        goto Release;
    }
    output = files->pVTbl->Create(files, "/dev/stdout");
    if (output == 0) {
        goto Release;
    }
    Print(output, "EcoIntegralSimpson UnitTest\n");
    status = bus->pVTbl->RegisterComponent(bus, &CID_EcoIntegralSimpson,
        (IEcoUnknown*)GetIEcoComponentFactoryPtr_BA973067BA954E8FA50BDDE55BC4E3DF);
    Check(output, (uint8_t)(status == 0), "register Simpson factory");
    Print(output, "ALGORITHM Simpson\n");
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoIntegralSimpson, 0,
        &IID_IEcoAdvancedMath, (voidptr_t*)&component);
    Check(output, (uint8_t)(status == 0 && component != 0), "create integration component");
    if (component != 0) {
        TestIdentity(output, component);
        TestD_Test(output, component, "Simpson", 5);
        TestF_Test(output, component, "Simpson", 5);
        TestL_Test(output, component, "Simpson", 5);
        status = bus->pVTbl->QueryComponent(bus, &CID_EcoIntegralSimpson, 0,
            &IID_IEcoAdvancedMath, (voidptr_t*)&second);
        Check(output, (uint8_t)(status == 0 && second != 0 && second != component), "independent instances");
        if (second != 0) {
            second->pVTbl->Release(second);
            second = 0;
        }
        component->pVTbl->Release(component);
        component = 0;
    }
    Print(output, g_Failed == 0 ? "SUMMARY PASS passed=" : "SUMMARY FAIL passed=");
    PrintUInt(output, g_Passed);
    Print(output, " failed=");
    PrintUInt(output, g_Failed);
    Print(output, "\n");
    if (g_Failed == 0 && g_OutputStatus == 0) {
        result = ERR_ECO_SUCCESS;
    }
Release:
    if (second != 0) {
        second->pVTbl->Release(second);
    }
    if (component != 0) {
        component->pVTbl->Release(component);
    }
    if (output != 0) {
        output->pVTbl->Close(output);
        output->pVTbl->Release(output);
    }
    if (files != 0) {
        files->pVTbl->Release(files);
    }
    if (bus != 0) {
        bus->pVTbl->Release(bus);
    }
    if (sys != 0) {
        sys->pVTbl->Release(sys);
    }
    return result;
}
