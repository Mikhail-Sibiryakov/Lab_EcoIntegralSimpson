/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Simpson point enumerator. </summary>
 * <description> One point, an independent cursor, and complete SDK enumeration methods. </description>
 * <reference> IEcoConnectionPointContainer.h </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#include "CEcoIntegralSimpsonEnumConnectionPoints.h"

typedef struct CEcoIntegralSimpsonEnumConnectionPoints {
    IEcoEnumConnectionPointsVTbl* vtbl;
    uint32_t references;
    IEcoMemoryAllocator1* memory;
    IEcoConnectionPoint* point;
    uint32_t position;
} CEcoIntegralSimpsonEnumConnectionPoints;

/* <summary> Retain the enumerator. </summary>
 * <description> Its single point reference is independent of this count. </description> */
static uint32_t ECOCALLMETHOD PointsAddRef(IEcoEnumConnectionPoints* me) {
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    return ++((CEcoIntegralSimpsonEnumConnectionPoints*)me)->references;
}

/* <summary> Query the enumeration identity. </summary>
 * <description> Returns an owned Unknown or EnumConnectionPoints reference. </description> */
static int16_t ECOCALLMETHOD PointsQuery(IEcoEnumConnectionPoints* me, const UGUID* iid, voidptr_t* out) {
    if (out == 0) {
        return ERR_ECO_POINTER;
    }
    *out = 0;
    if (me == 0 || iid == 0) {
        return ERR_ECO_POINTER;
    }
    if (!IsEqualUGUID(iid, &IID_IEcoUnknown) && !IsEqualUGUID(iid, &IID_IEcoEnumConnectionPoints)) {
        return ERR_ECO_NOINTERFACE;
    }
    *out = me;
    PointsAddRef(me);
    return ERR_ECO_SUCCESS;
}

/* <summary> Release the enumerator and its point. </summary>
 * <description> The allocator remains alive while freeing the object. </description> */
static uint32_t ECOCALLMETHOD PointsRelease(IEcoEnumConnectionPoints* me) {
    CEcoIntegralSimpsonEnumConnectionPoints* enumerator = (CEcoIntegralSimpsonEnumConnectionPoints*)me;
    IEcoMemoryAllocator1* memory = 0;
    uint32_t remaining = 0;
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    remaining = --enumerator->references;
    if (remaining == 0) {
        memory = enumerator->memory;
        enumerator->point->pVTbl->Release(enumerator->point);
        memory->pVTbl->Free(memory, enumerator);
        memory->pVTbl->Release(memory);
    }
    return remaining;
}

/* <summary> Fetch the available point. </summary>
 * <description> Returns zero for a full request, one for a partial request. </description> */
static int16_t ECOCALLMETHOD PointsNext(IEcoEnumConnectionPoints* me, uint32_t count,
    IEcoConnectionPoint** out, uint32_t* fetched) {
    CEcoIntegralSimpsonEnumConnectionPoints* enumerator = (CEcoIntegralSimpsonEnumConnectionPoints*)me;
    uint32_t actual = 0;
    if (fetched != 0) {
        *fetched = 0;
    }
    if (me == 0 || out == 0 || (count != 1 && fetched == 0)) {
        return ERR_ECO_POINTER;
    }
    if (count != 0 && enumerator->position == 0) {
        *out = enumerator->point;
        enumerator->point->pVTbl->AddRef(enumerator->point);
        enumerator->position = 1;
        actual = 1;
    }
    if (fetched != 0) {
        *fetched = actual;
    }
    return actual == count ? ERR_ECO_SUCCESS : 1;
}

/* <summary> Skip requested points. </summary>
 * <description> Stops at the end and reports a partial request with one. </description> */
static int16_t ECOCALLMETHOD PointsSkip(IEcoEnumConnectionPoints* me, uint32_t count) {
    CEcoIntegralSimpsonEnumConnectionPoints* enumerator = (CEcoIntegralSimpsonEnumConnectionPoints*)me;
    uint32_t remaining = 0;
    if (me == 0) {
        return ERR_ECO_POINTER;
    }
    remaining = 1-enumerator->position;
    if (count <= remaining) {
        enumerator->position += count;
        return ERR_ECO_SUCCESS;
    }
    enumerator->position = 1;
    return 1;
}

/* <summary> Restart point enumeration. </summary>
 * <description> Keeps the same point and ownership. </description> */
static int16_t ECOCALLMETHOD PointsReset(IEcoEnumConnectionPoints* me) {
    if (me == 0) {
        return ERR_ECO_POINTER;
    }
    ((CEcoIntegralSimpsonEnumConnectionPoints*)me)->position = 0;
    return ERR_ECO_SUCCESS;
}

/* <summary> Copy the enumeration cursor. </summary>
 * <description> The clone owns a separate reference to the same point. </description> */
static int16_t ECOCALLMETHOD PointsClone(IEcoEnumConnectionPoints* me, IEcoEnumConnectionPoints** out) {
    CEcoIntegralSimpsonEnumConnectionPoints* enumerator = (CEcoIntegralSimpsonEnumConnectionPoints*)me;
    if (out == 0) {
        return ERR_ECO_POINTER;
    }
    *out = 0;
    if (me == 0) {
        return ERR_ECO_POINTER;
    }
    return createCEcoIntegralSimpsonEnumConnectionPoints(enumerator->memory,
        enumerator->point, enumerator->position, out);
}

static IEcoEnumConnectionPointsVTbl g_PointsVTbl = {
    PointsQuery, PointsAddRef, PointsRelease, PointsNext, PointsSkip, PointsReset, PointsClone
};

/* <summary> Create the single-point enumerator. </summary>
 * <description> Owns allocator and point references until its final Release. </description> */
int16_t createCEcoIntegralSimpsonEnumConnectionPoints(IEcoMemoryAllocator1* memory,
    IEcoConnectionPoint* point, uint32_t position, IEcoEnumConnectionPoints** out) {
    CEcoIntegralSimpsonEnumConnectionPoints* enumerator = 0;
    if (out == 0) {
        return ERR_ECO_POINTER;
    }
    *out = 0;
    if (memory == 0 || point == 0) {
        return ERR_ECO_POINTER;
    }
    if (position > 1) {
        return ERR_ECO_INVALIDARG;
    }
    enumerator = (CEcoIntegralSimpsonEnumConnectionPoints*)memory->pVTbl->Alloc(memory,
        (uint32_t)sizeof(CEcoIntegralSimpsonEnumConnectionPoints));
    if (enumerator == 0) {
        return ERR_ECO_OUTOFMEMORY;
    }
    enumerator->vtbl = &g_PointsVTbl;
    enumerator->references = 1;
    enumerator->memory = memory;
    enumerator->point = point;
    enumerator->position = position;
    memory->pVTbl->AddRef(memory);
    point->pVTbl->AddRef(point);
    *out = (IEcoEnumConnectionPoints*)enumerator;
    return ERR_ECO_SUCCESS;
}
