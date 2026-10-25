/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> EcoIntegralSimpson factory. </summary>
 * <description> Creates independent objects and releases partial construction on failure. </description>
 * <reference> docs/DESIGN.md </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"

#include "CEcoIntegralSimpson.h"
#include "CEcoIntegralSimpsonFactory.h"

extern CEcoIntegralSimpson_5BC4E3DF g_xCEcoIntegralSimpson_5BC4E3DF;

/* <summary> Query the factory interface. </summary>
 * <description> Returns an owned reference; clears out on failure. </description> */
static int16_t ECOCALLMETHOD CEcoIntegralSimpson_5BC4E3DFFactory_QueryInterface(IEcoComponentFactory* me, const UGUID* iid, voidptr_t* out) {
    if (out == 0) {
        return ERR_ECO_POINTER;
    }
    *out = 0;
    if (me == 0 || iid == 0) {
        return ERR_ECO_POINTER;
    }
    if (!IsEqualUGUID(iid, &IID_IEcoUnknown) && !IsEqualUGUID(iid, &IID_IEcoComponentFactory)) {
        return ERR_ECO_NOINTERFACE;
    }
    *out = me;
    me->pVTbl->AddRef(me);
    return ERR_ECO_SUCCESS;
}

/* <summary> Add a factory reference. </summary>
 * <description> Returns the updated reference count. </description> */
static uint32_t ECOCALLMETHOD CEcoIntegralSimpson_5BC4E3DFFactory_AddRef(IEcoComponentFactory* me) {
    CEcoIntegralSimpson_5BC4E3DFFactory* factory = (CEcoIntegralSimpson_5BC4E3DFFactory*)me;
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    factory->m_cRef++;
    return factory->m_cRef;
}

/* <summary> Release a factory reference. </summary>
 * <description> Static factory storage is retained. </description> */
static uint32_t ECOCALLMETHOD CEcoIntegralSimpson_5BC4E3DFFactory_Release(IEcoComponentFactory* me) {
    CEcoIntegralSimpson_5BC4E3DFFactory* factory = (CEcoIntegralSimpson_5BC4E3DFFactory*)me;
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    if (factory->m_cRef > 0) {
        factory->m_cRef--;
    }
    return factory->m_cRef;
}

/* <summary> Validate factory initialization. </summary>
 * <description> Requires me and system; parameters are unused. </description> */
static int16_t ECOCALLMETHOD CEcoIntegralSimpson_5BC4E3DFFactory_Init(IEcoComponentFactory* me, IEcoUnknown* system, voidptr_t parameters) {
    (void)parameters;
    if (me == 0 || system == 0) {
        return ERR_ECO_POINTER;
    }
    return ERR_ECO_SUCCESS;
}

/* <summary> Create an integration object. </summary>
 * <description> Returns an owned interface; cleans partial construction on failure. </description> */
static int16_t ECOCALLMETHOD CEcoIntegralSimpson_5BC4E3DFFactory_Alloc(IEcoComponentFactory* me, IEcoUnknown* system,
    IEcoUnknown* outer, const UGUID* iid, voidptr_t* out) {
    IEcoSystem1* sys = 0;
    IEcoInterfaceBus1* bus = 0;
    IEcoMemoryAllocator1* memory = 0;
    CEcoIntegralSimpson_5BC4E3DF* object = 0;
    int16_t status = ERR_ECO_POINTER;
    if (out == 0) {
        return ERR_ECO_POINTER;
    }
    *out = 0;
    if (me == 0 || system == 0 || iid == 0) {
        return ERR_ECO_POINTER;
    }
    if (outer != 0 && !IsEqualUGUID(iid, &IID_IEcoUnknown)) {
        return ERR_ECO_NOAGGREGATION;
    }
    status = system->pVTbl->QueryInterface(system, &GID_IEcoSystem, (voidptr_t*)&sys);
    if (status != 0 || sys == 0) {
        status = ERR_ECO_NOSYSTEM;
        goto Release;
    }
    status = sys->pVTbl->QueryInterface(sys, &IID_IEcoInterfaceBus1, (voidptr_t*)&bus);
    if (status != 0 || bus == 0) {
        status = ERR_ECO_NOBUS;
        goto Release;
    }
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryAllocator1, (voidptr_t*)&memory);
    if (status != 0 || memory == 0) {
        status = ERR_ECO_GET_MEMORY_ALLOCATOR;
        goto Release;
    }
    object = (CEcoIntegralSimpson_5BC4E3DF*)memory->pVTbl->Alloc(memory, (uint32_t)sizeof(CEcoIntegralSimpson_5BC4E3DF));
    if (object == 0) {
        status = ERR_ECO_OUTOFMEMORY;
        goto Release;
    }
    memory->pVTbl->Copy(memory, object, &g_xCEcoIntegralSimpson_5BC4E3DF, (uint32_t)sizeof(CEcoIntegralSimpson_5BC4E3DF));
    object->m_pIMem = memory;
    memory = 0;
    status = object->Create(object, system, outer);
    if (status == 0) {
        status = object->Init(object, system);
    }
    if (status == 0) {
        status = object->m_unknown.vtbl->QueryInterface((IEcoUnknown*)&object->m_unknown, iid, out);
    }
Release:
    if (object != 0) {
        object->m_unknown.vtbl->Release((IEcoUnknown*)&object->m_unknown);
    }
    if (memory != 0) {
        memory->pVTbl->Release(memory);
    }
    if (bus != 0) {
        bus->pVTbl->Release(bus);
    }
    if (sys != 0) {
        sys->pVTbl->Release(sys);
    }
    return status;
}
/* <summary> Get the factory name. </summary>
 * <description> All metadata getters return borrowed strings, or NULL for NULL me. </description> */
static char_t* ECOCALLMETHOD CEcoIntegralSimpson_5BC4E3DFFactory_get_Name(IEcoComponentFactory* me) {
    CEcoIntegralSimpson_5BC4E3DFFactory* factory = (CEcoIntegralSimpson_5BC4E3DFFactory*)me;
    if (me == 0) {
        return 0;
    }
    return factory->m_Name;
}

/* <summary> Get the factory version. </summary> <description> See get_Name for ownership rules. </description> */
static char_t* ECOCALLMETHOD CEcoIntegralSimpson_5BC4E3DFFactory_get_Version(IEcoComponentFactory* me) {
    CEcoIntegralSimpson_5BC4E3DFFactory* factory = (CEcoIntegralSimpson_5BC4E3DFFactory*)me;
    if (me == 0) {
        return 0;
    }
    return factory->m_Version;
}

/* <summary> Get the factory manufacturer. </summary> <description> See get_Name for ownership rules. </description> */
static char_t* ECOCALLMETHOD CEcoIntegralSimpson_5BC4E3DFFactory_get_Manufacturer(IEcoComponentFactory* me) {
    CEcoIntegralSimpson_5BC4E3DFFactory* factory = (CEcoIntegralSimpson_5BC4E3DFFactory*)me;
    if (me == 0) {
        return 0;
    }
    return factory->m_Manufacturer;
}

IEcoComponentFactoryVTbl g_xBA973067BA954E8FA50BDDE55BC4E3DFFactoryVTbl = {
    CEcoIntegralSimpson_5BC4E3DFFactory_QueryInterface,
    CEcoIntegralSimpson_5BC4E3DFFactory_AddRef,
    CEcoIntegralSimpson_5BC4E3DFFactory_Release,
    CEcoIntegralSimpson_5BC4E3DFFactory_Alloc,
    CEcoIntegralSimpson_5BC4E3DFFactory_Init,
    CEcoIntegralSimpson_5BC4E3DFFactory_get_Name,
    CEcoIntegralSimpson_5BC4E3DFFactory_get_Version,
    CEcoIntegralSimpson_5BC4E3DFFactory_get_Manufacturer
};


CEcoIntegralSimpson_5BC4E3DFFactory g_xBA973067BA954E8FA50BDDE55BC4E3DFFactory = {
    &g_xBA973067BA954E8FA50BDDE55BC4E3DFFactoryVTbl,
    0,
    "EcoIntegralSimpson\0",
    "1.0.0.0\0",
    "HSE NN\0"
};

#ifdef ECO_DLL
/* <summary> Get the dynamic factory. </summary>
 * <description> Returns the static factory object. </description> */
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr(void) {
    return (IEcoComponentFactory*)&g_xBA973067BA954E8FA50BDDE55BC4E3DFFactory;
}
#elif ECO_LIB
IEcoComponentFactory* GetIEcoComponentFactoryPtr_BA973067BA954E8FA50BDDE55BC4E3DF = (IEcoComponentFactory*)&g_xBA973067BA954E8FA50BDDE55BC4E3DFFactory;
#endif
