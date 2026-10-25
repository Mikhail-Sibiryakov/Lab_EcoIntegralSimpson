/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Eco.Lab2 component identity. </summary>
 * <description> Public factory declaration for static and dynamic linkage. </description>
 * <reference> Eco.AI.Promts/Templates/IdROOT.h </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#ifndef ID_ECO_LAB2_H
#define ID_ECO_LAB2_H
#include "IEcoLab2.h"
/* EcoLab2 CID = {7C9B7CEC-BC69-410F-9B28-3F6647E653D2} */
static const UGUID CID_EcoLab2 = {0x01, 0x10, {0x7C, 0x9B, 0x7C, 0xEC, 0xBC, 0x69, 0x41, 0x0F, 0x9B, 0x28, 0x3F, 0x66, 0x47, 0xE6, 0x53, 0xD2}};
#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr(void);
#elif ECO_LIB
extern IEcoComponentFactory* GetIEcoComponentFactoryPtr_7C9B7CECBC69410F9B283F6647E653D2;
#endif
#endif
