/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Console and statistics receiver. </summary>
 * <description> Laboratory 3: two sources and two shared event receivers. </description>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */

#ifndef ECO_LAB3_SINK_H
#define ECO_LAB3_SINK_H
#include "IEcoIntegralSimpsonEvents.h"
#include "IEcoMemoryAllocator1.h"
#include "IEcoFile1.h"

typedef struct Lab3Output {
    IEcoFile1* file;
    int16_t status;
} Lab3Output;

typedef struct Lab3Statistics {
    uint32_t starts;
    uint32_t refinements;
    uint32_t finishes;
    uint32_t lastStep;
    uint16_t precision;
    uint8_t running;
    int16_t status;
    long double value;
    long double error;
} Lab3Statistics;

typedef struct CEcoIntegralSimpsonSink {
    IEcoIntegralSimpsonEventsVTbl* pVTbl;
    uint32_t references;
    IEcoMemoryAllocator1* memory;
    /* Borrowed identities and output outlive every subscription. */
    IEcoUnknown* sources[2];
    Lab3Output* output;
    uint8_t journal;
    uint8_t invalidSequence;
    Lab3Statistics statistics[2];
} CEcoIntegralSimpsonSink;

int16_t Lab3CreateSink(IEcoMemoryAllocator1* memory, Lab3Output* output,
    IEcoUnknown** sources, uint8_t journal, CEcoIntegralSimpsonSink** result);
void Lab3Print(Lab3Output* output, char_t* text);
void Lab3PrintCount(Lab3Output* output, uint32_t value);
void Lab3PrintReal(Lab3Output* output, long double value);
#endif
