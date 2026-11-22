/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Simpson connection point. </summary>
 * <description> Private connection list and owner-bound point construction. </description>
 * <reference> IEcoConnectionPointContainer.h </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#ifndef __C_ECO_INTEGRAL_SIMPSON_CONNECTION_POINT_H__
#define __C_ECO_INTEGRAL_SIMPSON_CONNECTION_POINT_H__
#include "IEcoConnectionPointContainer.h"
#include "IEcoMemoryAllocator1.h"

typedef struct SimpsonConnection {
    IEcoUnknown* sink;
    uint32_t cookie;
    struct SimpsonConnection* next;
} SimpsonConnection;

/* <summary> Create the owner's point. </summary>
 * <description> The returned internal pointer does not retain its owner. </description> */
int16_t createCEcoIntegralSimpsonConnectionPoint(IEcoMemoryAllocator1* memory,
    IEcoUnknown* controller, IEcoConnectionPointContainer* container, IEcoConnectionPoint** out);

/* <summary> Destroy the owner's point. </summary>
 * <description> Called by the owner after all acquired point references are released. </description> */
void deleteCEcoIntegralSimpsonConnectionPoint(IEcoConnectionPoint* point);
#endif
