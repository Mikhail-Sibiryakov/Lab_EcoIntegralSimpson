/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Eco.Lab2 outer component layout. </summary>
 * <description> Wizard CROOT structure with containment and aggregation outer enabled. </description>
 * <reference> Eco.AI.Promts/Templates/CROOT.h </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#ifndef C_ECO_LAB2_H
#define C_ECO_LAB2_H
#include "IEcoLab2.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
typedef struct CEcoLab2_47E653D2* CEcoLab2_47E653D2Ptr_t;
typedef struct Lab2XSlot {
    IEcoCalculatorXVTbl* vtbl;
    CEcoLab2_47E653D2Ptr_t owner;
} Lab2XSlot;
typedef struct Lab2YSlot {
    IEcoCalculatorYVTbl* vtbl;
    CEcoLab2_47E653D2Ptr_t owner;
} Lab2YSlot;
typedef struct CEcoLab2_47E653D2 {
    IEcoLab2VTbl* m_pVTblIEcoLab2;
    Lab2XSlot m_x;
    Lab2YSlot m_y;
    int16_t (ECOCALLMETHOD *Init)(CEcoLab2_47E653D2Ptr_t object, IEcoUnknown* system);
    int16_t (ECOCALLMETHOD *Create)(CEcoLab2_47E653D2Ptr_t object, IEcoUnknown* system, IEcoUnknown* outer);
    void (ECOCALLMETHOD *Delete)(CEcoLab2_47E653D2Ptr_t object);
    uint32_t m_cRef;
    IEcoMemoryAllocator1* m_pIMem;
    IEcoSystem1* m_pISys;
    /* Own contained C/E interfaces and private inner Unknown; never cache public M. */
    IEcoCalculatorX* m_pIX;
    IEcoCalculatorY* m_pIY;
    IEcoUnknown* m_pIUnkInner;
} CEcoLab2_47E653D2;
#endif
