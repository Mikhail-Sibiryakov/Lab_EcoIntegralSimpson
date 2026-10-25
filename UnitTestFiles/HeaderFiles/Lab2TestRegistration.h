/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Public-API helpers for laboratory 2 tests. </summary>
 * <description> Registers the actual educational calculator factories. </description>
 * <reference> docs/lab2/DESIGN.md </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#ifndef LAB2_TEST_REGISTRATION_H
#define LAB2_TEST_REGISTRATION_H
#include "IEcoInterfaceBus1.h"
int16_t Lab2RegisterCalculators(IEcoInterfaceBus1* bus);
#endif
