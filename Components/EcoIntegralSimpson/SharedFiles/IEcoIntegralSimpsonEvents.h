/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Adaptive Simpson outgoing events. </summary>
 * <description> Borrowed scalar snapshots; receivers do not control integration. </description>
 * <reference> EcoIntegralSimpsonEvents.idl </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#ifndef __I_ECO_INTEGRAL_SIMPSON_EVENTS_H__
#define __I_ECO_INTEGRAL_SIMPSON_EVENTS_H__
#include "IEcoBase1.h"

#define ECO_INTEGRAL_FLOAT 1
#define ECO_INTEGRAL_DOUBLE 2
#define ECO_INTEGRAL_LONG_DOUBLE 3

/* IEcoIntegralSimpsonEvents IID = 67EDF6B2-23EF-4CC6-BFD7-0EE619F222B6 */
#ifndef __IID_IEcoIntegralSimpsonEvents
static const UGUID IID_IEcoIntegralSimpsonEvents = {0x01, 0x10, {0x67, 0xED, 0xF6, 0xB2, 0x23, 0xEF, 0x4C, 0xC6, 0xBF, 0xD7, 0x0E, 0xE6, 0x19, 0xF2, 0x22, 0xB6}};
#endif

typedef struct IEcoIntegralSimpsonEvents* IEcoIntegralSimpsonEventsPtr_t;

/* source is a borrowed controlling Unknown, valid during the callback.
 * Calls are synchronous; the demonstration uses sequential integrations.
 * Numeric values are widened to long double without changing the algorithm type.
 * Start follows argument validation. Refined follows recomputation of Q/E.
 * Finished numeric fields are valid only when status == ERR_ECO_SUCCESS.
 * Handler return codes do not alter the mathematical method's result.
 */
typedef struct IEcoIntegralSimpsonEventsVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(IEcoIntegralSimpsonEventsPtr_t me, const UGUID* iid, voidptr_t* out);
    uint32_t (ECOCALLMETHOD *AddRef)(IEcoIntegralSimpsonEventsPtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(IEcoIntegralSimpsonEventsPtr_t me);
    int16_t (ECOCALLMETHOD *OnIntegrationStarted)(IEcoIntegralSimpsonEventsPtr_t me, IEcoUnknown* source,
        uint16_t precision, long double a, long double b, long double epsabs, long double epsrel);
    int16_t (ECOCALLMETHOD *OnIntervalRefined)(IEcoIntegralSimpsonEventsPtr_t me, IEcoUnknown* source,
        uint32_t step, uint32_t intervals, long double left, long double middle, long double right,
        long double value, long double error);
    int16_t (ECOCALLMETHOD *OnIntegrationFinished)(IEcoIntegralSimpsonEventsPtr_t me, IEcoUnknown* source,
        int16_t status, long double value, long double error);
} IEcoIntegralSimpsonEventsVTbl;

interface IEcoIntegralSimpsonEvents {
    struct IEcoIntegralSimpsonEventsVTbl* pVTbl;
} IEcoIntegralSimpsonEvents;
#endif
