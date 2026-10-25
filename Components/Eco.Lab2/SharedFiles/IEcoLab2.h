/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Eco.Lab2 arithmetic facade. </summary>
 * <description> New public interface; legacy X/Y and AdvancedMath remain unchanged. </description>
 * <reference> Eco.AI.Promts/Templates/IROOT.h; docs/lab2/DESIGN.md </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#ifndef I_ECO_LAB2_H
#define I_ECO_LAB2_H
#include "IEcoBase1.h"
/* IEcoLab2 IID = {F0DFF109-2767-4C68-87E1-A3228E910B28} */
static const UGUID IID_IEcoLab2 = {0x01, 0x10, {0xF0, 0xDF, 0xF1, 0x09, 0x27, 0x67, 0x4C, 0x68, 0x87, 0xE1, 0xA3, 0x22, 0x8E, 0x91, 0x0B, 0x28}};
typedef struct IEcoLab2* IEcoLab2Ptr_t;
typedef struct IEcoLab2VTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(IEcoLab2Ptr_t me, const UGUID* iid, voidptr_t* out);
    uint32_t (ECOCALLMETHOD *AddRef)(IEcoLab2Ptr_t me);
    uint32_t (ECOCALLMETHOD *Release)(IEcoLab2Ptr_t me);
    /* Operations 1/2/3/4: addition/subtraction/multiplication/division.
     * me/result required. Invalid operation, unrepresentable legacy result or
     * zero divisor: ERR_ECO_INVALIDARG. Result changes only on success. */
    int16_t (ECOCALLMETHOD *Calculate)(IEcoLab2Ptr_t me, uint16_t operation, int16_t a, int16_t b, int32_t* result);
} IEcoLab2VTbl;
typedef struct IEcoLab2 {
    IEcoLab2VTbl* pVTbl;
} IEcoLab2;
#endif
