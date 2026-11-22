/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Simpson subscription enumerator. </summary>
 * <description> Strong snapshots and independent cursors implement the complete SDK API. </description>
 * <reference> IEcoConnectionPointContainer.h </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#include "CEcoIntegralSimpsonEnumConnections.h"
#include "CEcoIntegralSimpsonConnectionPoint.h"

typedef struct CEcoIntegralSimpsonEnumConnections {
    IEcoEnumConnectionsVTbl* vtbl;
    uint32_t references;
    IEcoMemoryAllocator1* memory;
    uint32_t count;
    uint32_t position;
    EcoConnectionData* entries;
} CEcoIntegralSimpsonEnumConnections;

/* <summary> Retain the enumerator. </summary>
 * <description> Snapshot receiver references remain owned until the final Release. </description> */
static uint32_t ECOCALLMETHOD ConnectionsAddRef(IEcoEnumConnections* me) {
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    return ++((CEcoIntegralSimpsonEnumConnections*)me)->references;
}

/* <summary> Query enumeration interfaces. </summary>
 * <description> Both supported IIDs have this helper object's identity. </description> */
static int16_t ECOCALLMETHOD ConnectionsQuery(IEcoEnumConnections* me, const UGUID* iid, voidptr_t* out) {
    if (out == 0) {
        return ERR_ECO_POINTER;
    }
    *out = 0;
    if (me == 0 || iid == 0) {
        return ERR_ECO_POINTER;
    }
    if (!IsEqualUGUID(iid, &IID_IEcoUnknown) && !IsEqualUGUID(iid, &IID_IEcoEnumConnections)) {
        return ERR_ECO_NOINTERFACE;
    }
    *out = me;
    ConnectionsAddRef(me);
    return ERR_ECO_SUCCESS;
}

/* <summary> Release the snapshot and enumerator. </summary>
 * <description> Every saved sink reference is paired with one Release. </description> */
static uint32_t ECOCALLMETHOD ConnectionsRelease(IEcoEnumConnections* me) {
    CEcoIntegralSimpsonEnumConnections* enumerator = (CEcoIntegralSimpsonEnumConnections*)me;
    IEcoMemoryAllocator1* memory = 0;
    uint32_t remaining = 0;
    uint32_t i = 0;
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    remaining = --enumerator->references;
    if (remaining == 0) {
        memory = enumerator->memory;
        for (i = 0; i < enumerator->count; i++) {
            enumerator->entries[i].pUnk->pVTbl->Release(enumerator->entries[i].pUnk);
        }
        memory->pVTbl->Free(memory, enumerator);
        memory->pVTbl->Release(memory);
    }
    return remaining;
}

/* <summary> Fetch connection records. </summary>
 * <description> Returned pUnk references belong to the caller; one means a partial request. </description> */
static int16_t ECOCALLMETHOD ConnectionsNext(IEcoEnumConnections* me, uint32_t count,
    EcoConnectionData* out, uint32_t* fetched) {
    CEcoIntegralSimpsonEnumConnections* enumerator = (CEcoIntegralSimpsonEnumConnections*)me;
    uint32_t actual = 0;
    if (fetched != 0) {
        *fetched = 0;
    }
    if (me == 0 || out == 0 || (count != 1 && fetched == 0)) {
        return ERR_ECO_POINTER;
    }
    while (actual < count && enumerator->position < enumerator->count) {
        out[actual].pUnk = enumerator->entries[enumerator->position].pUnk;
        out[actual].cCookie = enumerator->entries[enumerator->position].cCookie;
        out[actual].pUnk->pVTbl->AddRef(out[actual].pUnk);
        enumerator->position++;
        actual++;
    }
    if (fetched != 0) {
        *fetched = actual;
    }
    return actual == count ? ERR_ECO_SUCCESS : 1;
}

/* <summary> Skip records without returning references. </summary>
 * <description> Bounded subtraction avoids overflowing the cursor. </description> */
static int16_t ECOCALLMETHOD ConnectionsSkip(IEcoEnumConnections* me, uint32_t count) {
    CEcoIntegralSimpsonEnumConnections* enumerator = (CEcoIntegralSimpsonEnumConnections*)me;
    if (me == 0) {
        return ERR_ECO_POINTER;
    }
    if (count <= enumerator->count-enumerator->position) {
        enumerator->position += count;
        return ERR_ECO_SUCCESS;
    }
    enumerator->position = enumerator->count;
    return 1;
}

/* <summary> Restart the same snapshot. </summary>
 * <description> Does not refresh subscriptions. </description> */
static int16_t ECOCALLMETHOD ConnectionsReset(IEcoEnumConnections* me) {
    if (me == 0) {
        return ERR_ECO_POINTER;
    }
    ((CEcoIntegralSimpsonEnumConnections*)me)->position = 0;
    return ERR_ECO_SUCCESS;
}

static IEcoEnumConnectionsVTbl g_ConnectionsVTbl;

/* <summary> Allocate an empty snapshot of known size. </summary>
 * <description> Entries follow the aligned object in one allocation; callers fill all entries. </description> */
static int16_t AllocateConnections(IEcoMemoryAllocator1* memory, uint32_t count,
    CEcoIntegralSimpsonEnumConnections** out) {
    CEcoIntegralSimpsonEnumConnections* enumerator = 0;
    uint32_t size = 0;
    if (count > ((uint32_t)-1-(uint32_t)sizeof(CEcoIntegralSimpsonEnumConnections))/(uint32_t)sizeof(EcoConnectionData)) {
        return ERR_ECO_OUTOFMEMORY;
    }
    size = (uint32_t)sizeof(CEcoIntegralSimpsonEnumConnections)+count*(uint32_t)sizeof(EcoConnectionData);
    enumerator = (CEcoIntegralSimpsonEnumConnections*)memory->pVTbl->Alloc(memory, size);
    if (enumerator == 0) {
        return ERR_ECO_OUTOFMEMORY;
    }
    enumerator->vtbl = &g_ConnectionsVTbl;
    enumerator->references = 1;
    enumerator->memory = memory;
    enumerator->count = count;
    enumerator->position = 0;
    enumerator->entries = (EcoConnectionData*)(enumerator+1);
    memory->pVTbl->AddRef(memory);
    *out = enumerator;
    return ERR_ECO_SUCCESS;
}

/* <summary> Copy snapshot and cursor. </summary>
 * <description> The clone owns its references independently of the original. </description> */
static int16_t ECOCALLMETHOD ConnectionsClone(IEcoEnumConnections* me, IEcoEnumConnections** out) {
    CEcoIntegralSimpsonEnumConnections* enumerator = (CEcoIntegralSimpsonEnumConnections*)me;
    CEcoIntegralSimpsonEnumConnections* clone = 0;
    uint32_t i = 0;
    int16_t status = ERR_ECO_POINTER;
    if (out == 0) {
        return ERR_ECO_POINTER;
    }
    *out = 0;
    if (me == 0) {
        return ERR_ECO_POINTER;
    }
    status = AllocateConnections(enumerator->memory, enumerator->count, &clone);
    if (status != ERR_ECO_SUCCESS) {
        return status;
    }
    for (i = 0; i < enumerator->count; i++) {
        clone->entries[i].pUnk = enumerator->entries[i].pUnk;
        clone->entries[i].cCookie = enumerator->entries[i].cCookie;
        clone->entries[i].pUnk->pVTbl->AddRef(clone->entries[i].pUnk);
    }
    clone->position = enumerator->position;
    *out = (IEcoEnumConnections*)clone;
    return ERR_ECO_SUCCESS;
}

static IEcoEnumConnectionsVTbl g_ConnectionsVTbl = {
    ConnectionsQuery, ConnectionsAddRef, ConnectionsRelease, ConnectionsNext,
    ConnectionsSkip, ConnectionsReset, ConnectionsClone
};

/* <summary> Take a subscription snapshot. </summary>
 * <description> Strong receiver references protect Unadvise during synchronous dispatch. </description> */
int16_t createCEcoIntegralSimpsonEnumConnections(IEcoMemoryAllocator1* memory,
    const SimpsonConnection* first, IEcoEnumConnections** out) {
    const SimpsonConnection* connection = first;
    CEcoIntegralSimpsonEnumConnections* enumerator = 0;
    uint32_t count = 0;
    uint32_t i = 0;
    int16_t status = ERR_ECO_POINTER;
    if (out == 0) {
        return ERR_ECO_POINTER;
    }
    *out = 0;
    if (memory == 0) {
        return ERR_ECO_POINTER;
    }
    while (connection != 0) {
        if (count == (uint32_t)-1) {
            return ERR_ECO_OUTOFMEMORY;
        }
        count++;
        connection = connection->next;
    }
    status = AllocateConnections(memory, count, &enumerator);
    if (status != ERR_ECO_SUCCESS) {
        return status;
    }
    connection = first;
    for (i = 0; i < count; i++) {
        enumerator->entries[i].pUnk = connection->sink;
        enumerator->entries[i].cCookie = connection->cookie;
        connection->sink->pVTbl->AddRef(connection->sink);
        connection = connection->next;
    }
    *out = (IEcoEnumConnections*)enumerator;
    return ERR_ECO_SUCCESS;
}
