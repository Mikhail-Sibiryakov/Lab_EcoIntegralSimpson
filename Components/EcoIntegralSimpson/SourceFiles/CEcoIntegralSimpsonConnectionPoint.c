/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Simpson connection point. </summary>
 * <description> Cookie-based subscriptions; acquired references retain the controlling object. </description>
 * <reference> IEcoConnectionPointContainer.h </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#include "CEcoIntegralSimpsonConnectionPoint.h"
#include "CEcoIntegralSimpsonEnumConnections.h"
#include "IEcoIntegralSimpsonEvents.h"

typedef struct CEcoIntegralSimpsonConnectionPoint {
    IEcoConnectionPointVTbl* vtbl;
    uint32_t references;
    IEcoMemoryAllocator1* memory;
    /* Both are borrowed. Each acquired CP reference retains the controller. */
    IEcoUnknown* controller;
    IEcoConnectionPointContainer* container;
    SimpsonConnection* connections;
    uint32_t nextCookie;
} CEcoIntegralSimpsonConnectionPoint;

/* <summary> Retain an acquired point reference. </summary>
 * <description> The owner's internal pointer creates no reference cycle. </description> */
static uint32_t ECOCALLMETHOD PointAddRef(IEcoConnectionPoint* me) {
    CEcoIntegralSimpsonConnectionPoint* point = (CEcoIntegralSimpsonConnectionPoint*)me;
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    point->references++;
    point->controller->pVTbl->AddRef(point->controller);
    return point->references;
}

/* <summary> Release an acquired point reference. </summary>
 * <description> Releasing the controller may delete this point; no later access is allowed. </description> */
static uint32_t ECOCALLMETHOD PointRelease(IEcoConnectionPoint* me) {
    CEcoIntegralSimpsonConnectionPoint* point = (CEcoIntegralSimpsonConnectionPoint*)me;
    uint32_t remaining = 0;
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    point->references--;
    remaining = point->references;
    point->controller->pVTbl->Release(point->controller);
    return remaining;
}

/* <summary> Query the point identity. </summary>
 * <description> Points are helper objects with their own Unknown identity. </description> */
static int16_t ECOCALLMETHOD PointQuery(IEcoConnectionPoint* me, const UGUID* iid, voidptr_t* out) {
    if (out == 0) {
        return ERR_ECO_POINTER;
    }
    *out = 0;
    if (me == 0 || iid == 0) {
        return ERR_ECO_POINTER;
    }
    if (!IsEqualUGUID(iid, &IID_IEcoUnknown) && !IsEqualUGUID(iid, &IID_IEcoConnectionPoint)) {
        return ERR_ECO_NOINTERFACE;
    }
    *out = me;
    PointAddRef(me);
    return ERR_ECO_SUCCESS;
}

/* <summary> Return the outgoing interface IID. </summary>
 * <description> Copies the full Eco GUID through the allocator service. </description> */
static int16_t ECOCALLMETHOD PointInterface(IEcoConnectionPoint* me, UGUID* iid) {
    CEcoIntegralSimpsonConnectionPoint* point = (CEcoIntegralSimpsonConnectionPoint*)me;
    if (me == 0 || iid == 0) {
        return ERR_ECO_POINTER;
    }
    point->memory->pVTbl->Copy(point->memory, iid, (voidptr_t)&IID_IEcoIntegralSimpsonEvents, (uint32_t)sizeof(UGUID));
    return ERR_ECO_SUCCESS;
}

/* <summary> Return an owned container reference. </summary>
 * <description> The container uses the existing controlling Unknown. </description> */
static int16_t ECOCALLMETHOD PointContainer(IEcoConnectionPoint* me, IEcoConnectionPointContainer** out) {
    CEcoIntegralSimpsonConnectionPoint* point = (CEcoIntegralSimpsonConnectionPoint*)me;
    if (out == 0) {
        return ERR_ECO_POINTER;
    }
    *out = 0;
    if (me == 0) {
        return ERR_ECO_POINTER;
    }
    *out = point->container;
    point->container->pVTbl->AddRef(point->container);
    return ERR_ECO_SUCCESS;
}

/* <summary> Subscribe a compatible receiver. </summary>
 * <description> Owns one receiver reference until Unadvise or owner destruction. </description> */
static int16_t ECOCALLMETHOD PointAdvise(IEcoConnectionPoint* me, IEcoUnknown* sink, uint32_t* cookie) {
    CEcoIntegralSimpsonConnectionPoint* point = (CEcoIntegralSimpsonConnectionPoint*)me;
    IEcoIntegralSimpsonEvents* events = 0;
    SimpsonConnection* connection = 0;
    SimpsonConnection** link = 0;
    int16_t status = ERR_ECO_POINTER;
    if (cookie == 0) {
        return ERR_ECO_POINTER;
    }
    *cookie = 0;
    if (me == 0 || sink == 0) {
        return ERR_ECO_POINTER;
    }
    status = sink->pVTbl->QueryInterface(sink, &IID_IEcoIntegralSimpsonEvents, (voidptr_t*)&events);
    if (status != ERR_ECO_SUCCESS || events == 0) {
        return ERR_ECO_NOINTERFACE;
    }
    events->pVTbl->Release(events);
    /* Zero is reserved; exhausted cookies are not silently reused. */
    if (point->nextCookie == 0) {
        return ERR_ECO_FAIL;
    }
    connection = (SimpsonConnection*)point->memory->pVTbl->Alloc(point->memory, (uint32_t)sizeof(SimpsonConnection));
    if (connection == 0) {
        return ERR_ECO_OUTOFMEMORY;
    }
    connection->sink = sink;
    connection->cookie = point->nextCookie++;
    connection->next = 0;
    sink->pVTbl->AddRef(sink);
    link = &point->connections;
    while (*link != 0) {
        link = &(*link)->next;
    }
    *link = connection;
    *cookie = connection->cookie;
    return ERR_ECO_SUCCESS;
}

/* <summary> Remove one subscription. </summary>
 * <description> Detaches before releasing the receiver so callbacks see the new list. </description> */
static int16_t ECOCALLMETHOD PointUnadvise(IEcoConnectionPoint* me, uint32_t cookie) {
    CEcoIntegralSimpsonConnectionPoint* point = (CEcoIntegralSimpsonConnectionPoint*)me;
    SimpsonConnection** link = 0;
    SimpsonConnection* connection = 0;
    IEcoUnknown* sink = 0;
    if (me == 0) {
        return ERR_ECO_POINTER;
    }
    link = &point->connections;
    while (*link != 0) {
        if ((*link)->cookie == cookie) {
            connection = *link;
            *link = connection->next;
            sink = connection->sink;
            point->memory->pVTbl->Free(point->memory, connection);
            sink->pVTbl->Release(sink);
            return ERR_ECO_SUCCESS;
        }
        link = &(*link)->next;
    }
    return ERR_ECO_INVALIDARG;
}

/* <summary> Enumerate a stable subscription snapshot. </summary>
 * <description> Later Advise/Unadvise calls do not change this enumeration. </description> */
static int16_t ECOCALLMETHOD PointConnections(IEcoConnectionPoint* me, IEcoEnumConnections** out) {
    CEcoIntegralSimpsonConnectionPoint* point = (CEcoIntegralSimpsonConnectionPoint*)me;
    if (out == 0) {
        return ERR_ECO_POINTER;
    }
    *out = 0;
    if (me == 0) {
        return ERR_ECO_POINTER;
    }
    return createCEcoIntegralSimpsonEnumConnections(point->memory, point->connections, out);
}

static IEcoConnectionPointVTbl g_PointVTbl = {
    PointQuery, PointAddRef, PointRelease, PointInterface, PointContainer,
    PointAdvise, PointUnadvise, PointConnections
};

/* <summary> Create the owner's point. </summary>
 * <description> Allocator and owner are borrowed; the owner outlives the internal point. </description> */
int16_t createCEcoIntegralSimpsonConnectionPoint(IEcoMemoryAllocator1* memory,
    IEcoUnknown* controller, IEcoConnectionPointContainer* container, IEcoConnectionPoint** out) {
    CEcoIntegralSimpsonConnectionPoint* point = 0;
    if (out == 0) {
        return ERR_ECO_POINTER;
    }
    *out = 0;
    if (memory == 0 || controller == 0 || container == 0) {
        return ERR_ECO_POINTER;
    }
    point = (CEcoIntegralSimpsonConnectionPoint*)memory->pVTbl->Alloc(memory, (uint32_t)sizeof(CEcoIntegralSimpsonConnectionPoint));
    if (point == 0) {
        return ERR_ECO_OUTOFMEMORY;
    }
    point->vtbl = &g_PointVTbl;
    point->references = 0;
    point->memory = memory;
    point->controller = controller;
    point->container = container;
    point->connections = 0;
    point->nextCookie = 1;
    *out = (IEcoConnectionPoint*)point;
    return ERR_ECO_SUCCESS;
}

/* <summary> Destroy all remaining subscriptions and the point. </summary>
 * <description> There are no acquired point references when the owner is destroyed. </description> */
void deleteCEcoIntegralSimpsonConnectionPoint(IEcoConnectionPoint* me) {
    CEcoIntegralSimpsonConnectionPoint* point = (CEcoIntegralSimpsonConnectionPoint*)me;
    SimpsonConnection* connection = 0;
    if (me == 0) {
        return;
    }
    while (point->connections != 0) {
        connection = point->connections;
        point->connections = connection->next;
        connection->sink->pVTbl->Release(connection->sink);
        point->memory->pVTbl->Free(point->memory, connection);
    }
    point->memory->pVTbl->Free(point->memory, point);
}
