/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> EcoIntegralSimpson identifiers. </summary>
 * <description> Wizard CID and entry points for static and dynamic layouts. </description>
 * <reference> docs/DESIGN.md </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#ifndef __ID_ECOINTEGRALSIMPSON_H__
#define __ID_ECOINTEGRALSIMPSON_H__

#include "IEcoBase1.h"
#include "IEcoAdvancedMath.h"

/* EcoIntegralSimpson CID = {BA973067-BA95-4E8F-A50B-DDE55BC4E3DF} */
#ifndef __CID_EcoIntegralSimpson
static const UGUID CID_EcoIntegralSimpson = {0x01, 0x10, {0xBA, 0x97, 0x30, 0x67, 0xBA, 0x95, 0x4E, 0x8F, 0xA5, 0x0B, 0xDD, 0xE5, 0x5B, 0xC4, 0xE3, 0xDF}};
#endif

#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr(void);
#elif ECO_LIB
extern IEcoComponentFactory* GetIEcoComponentFactoryPtr_BA973067BA954E8FA50BDDE55BC4E3DF;
#endif

#endif

