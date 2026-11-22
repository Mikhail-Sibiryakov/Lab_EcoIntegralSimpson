/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Simpson point enumerator. </summary>
 * <description> Enumerates the single point and retains its owner through the point. </description>
 * <reference> IEcoConnectionPointContainer.h </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#ifndef __C_ECO_INTEGRAL_SIMPSON_ENUM_CONNECTION_POINTS_H__
#define __C_ECO_INTEGRAL_SIMPSON_ENUM_CONNECTION_POINTS_H__
#include "IEcoEnumConnectionPoints.h"
#include "IEcoMemoryAllocator1.h"

/* <summary> Enumerate the single point. </summary>
 * <description> Owns a point reference; position is zero or one for cloning. </description> */
int16_t createCEcoIntegralSimpsonEnumConnectionPoints(IEcoMemoryAllocator1* memory,
    IEcoConnectionPoint* point, uint32_t position, IEcoEnumConnectionPoints** out);
#endif
