/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> EcoIntegralSimpson factory. </summary>
 * <description> Static factory layout and component metadata. </description>
 * <reference> docs/DESIGN.md </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#ifndef __C_ECOINTEGRALSIMPSON_FACTORY_H__
#define __C_ECOINTEGRALSIMPSON_FACTORY_H__

#include "IEcoSystem1.h"

typedef struct CEcoIntegralSimpson_5BC4E3DFFactory {

    IEcoComponentFactoryVTbl* m_pVTblICF;

    uint32_t m_cRef;

    char_t m_Name[64];
    char_t m_Version[16];
    char_t m_Manufacturer[64];

} CEcoIntegralSimpson_5BC4E3DFFactory;

#endif
