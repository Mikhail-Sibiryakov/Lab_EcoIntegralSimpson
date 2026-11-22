/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Simpson connection enumerator. </summary>
 * <description> A snapshot keeps every receiver alive independently of subscriptions. </description>
 * <reference> IEcoConnectionPointContainer.h </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#ifndef __C_ECO_INTEGRAL_SIMPSON_ENUM_CONNECTIONS_H__
#define __C_ECO_INTEGRAL_SIMPSON_ENUM_CONNECTIONS_H__
#include "IEcoEnumConnections.h"
#include "IEcoMemoryAllocator1.h"
struct SimpsonConnection;

/* <summary> Take a strong connection snapshot. </summary>
 * <description> The returned enumerator owns receiver references and starts at zero. </description> */
int16_t createCEcoIntegralSimpsonEnumConnections(IEcoMemoryAllocator1* memory,
    const struct SimpsonConnection* first, IEcoEnumConnections** out);
#endif
