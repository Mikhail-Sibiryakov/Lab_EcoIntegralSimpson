/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> EcoIntegralSimpson object. </summary>
 * <description> Private object layout; owns references to the system and allocator. </description>
 * <reference> docs/DESIGN.md </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#ifndef __C_ECOINTEGRALSIMPSON_H__
#define __C_ECOINTEGRALSIMPSON_H__

#include "IEcoAdvancedMath.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CEcoIntegralSimpson_5BC4E3DF* CEcoIntegralSimpson_5BC4E3DFPtr_t;

typedef struct CEcoIntegralSimpson_5BC4E3DF {

    IEcoAdvancedMathVTbl* m_pVTblIEcoAdvancedMath;


    int16_t (ECOCALLMETHOD *Init)(/*in*/ CEcoIntegralSimpson_5BC4E3DFPtr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem);
    int16_t (ECOCALLMETHOD *Create)(/*in*/ CEcoIntegralSimpson_5BC4E3DFPtr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter);
    void (ECOCALLMETHOD *Delete)(/*in*/ CEcoIntegralSimpson_5BC4E3DFPtr_t pCMe);


    uint32_t m_cRef;

    IEcoMemoryAllocator1* m_pIMem;

    IEcoSystem1* m_pISys;


} CEcoIntegralSimpson_5BC4E3DF;

#endif
