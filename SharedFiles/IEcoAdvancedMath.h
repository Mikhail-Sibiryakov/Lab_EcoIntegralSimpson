/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> IEcoAdvancedMath. </summary>
 * <description> Public adaptive integration interface for three real types. </description>
 * <reference> docs/DESIGN.md </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */

#ifndef __I_ECOADVANCEDMATH_H__
#define __I_ECOADVANCEDMATH_H__

#include "IEcoBase1.h"

/* IEcoAdvancedMath IID = 667182A3-5F5C-4F90-9244-C763139ED855 */
#ifndef __IID_IEcoAdvancedMath
static const UGUID IID_IEcoAdvancedMath = {0x01, 0x10, {0x66, 0x71, 0x82, 0xA3, 0x5F, 0x5C, 0x4F, 0x90, 0x92, 0x44, 0xC7, 0x63, 0x13, 0x9E, 0xD8, 0x55}};
#endif 

typedef double_t (ECOCDECLMETHOD *EcoIntegrandD)(double_t x, voidptr_t context);
typedef float_t (ECOCDECLMETHOD *EcoIntegrandF)(float_t x, voidptr_t context);
typedef long double (ECOCDECLMETHOD *EcoIntegrandL)(long double x, voidptr_t context);

typedef struct IEcoAdvancedMath* IEcoAdvancedMathPtr_t;

typedef struct IEcoAdvancedMathVTbl {

    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoAdvancedMathPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoAdvancedMathPtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoAdvancedMathPtr_t me);

    /* Shared contract for integral, integralf and integrall:
     * Continuous real function with finite values and bounds.
     * Finite nonnegative epsabs/epsrel, at least one positive.
     * me, callback and separate result/error outputs are required.
     * Nonnegative error is an estimate, not a guaranteed bound.
     * On success: error <= max(epsabs, epsrel*abs(result)); failure preserves outputs.
     * Callback/context are borrowed during the call; NULL context is valid.
     */

    /* <summary> Integrate using double_t. </summary> <description> Shared contract above. </description> */
    int16_t (ECOCALLMETHOD *integral)(IEcoAdvancedMathPtr_t me, EcoIntegrandD function,
        voidptr_t context, double_t a, double_t b, double_t epsabs, double_t epsrel,
        double_t* result, double_t* error);

    /* <summary> Integrate using float_t. </summary> <description> Shared contract above. </description> */
    int16_t (ECOCALLMETHOD *integralf)(IEcoAdvancedMathPtr_t me, EcoIntegrandF function,
        voidptr_t context, float_t a, float_t b, float_t epsabs, float_t epsrel,
        float_t* result, float_t* error);

    /* <summary> Integrate using long double. </summary> <description> Shared contract above. </description> */
    int16_t (ECOCALLMETHOD *integrall)(IEcoAdvancedMathPtr_t me, EcoIntegrandL function,
        voidptr_t context, long double a, long double b, long double epsabs, long double epsrel,
        long double* result, long double* error);

} IEcoAdvancedMathVTbl, *IEcoAdvancedMathVTblPtr_t;

interface IEcoAdvancedMath {
    struct IEcoAdvancedMathVTbl *pVTbl;
} IEcoAdvancedMath;

#endif 

