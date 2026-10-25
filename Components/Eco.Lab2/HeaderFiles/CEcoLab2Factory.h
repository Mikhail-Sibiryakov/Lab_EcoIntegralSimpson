/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> EcoLab2 factory. </summary>
 * <description> Static factory layout and component metadata. </description>
 * <reference> docs/lab2/DESIGN.md; Eco.AI.Promts/Templates/CROOTFactory.c </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#ifndef __C_ECOLAB2_FACTORY_H__
#define __C_ECOLAB2_FACTORY_H__

#include "IEcoSystem1.h"

typedef struct CEcoLab2_47E653D2Factory {

    IEcoComponentFactoryVTbl* m_pVTblICF;

    uint32_t m_cRef;

    char_t m_Name[64];
    char_t m_Version[16];
    char_t m_Manufacturer[64];

} CEcoLab2_47E653D2Factory;

#endif
