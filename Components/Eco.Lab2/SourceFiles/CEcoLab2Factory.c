/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> EcoLab2 factory. </summary>
 * <description> Creates independent objects and releases partial construction on failure. </description>
 * <reference> docs/lab2/DESIGN.md; Eco.AI.Promts/Templates/CROOTFactory.c </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"

#include "CEcoLab2.h"
#include "CEcoLab2Factory.h"

extern CEcoLab2_47E653D2 g_xCEcoLab2_47E653D2;

/* <summary> Query the factory interface. </summary>
 * <description> Returns an owned reference; clears out on failure. </description> */
static int16_t ECOCALLMETHOD CEcoLab2_47E653D2Factory_QueryInterface(IEcoComponentFactory* me, const UGUID* iid, voidptr_t* out) {
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
static uint32_t ECOCALLMETHOD CEcoLab2_47E653D2Factory_AddRef(IEcoComponentFactory* me) {
    CEcoLab2_47E653D2Factory* factory = (CEcoLab2_47E653D2Factory*)me;
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    factory->m_cRef++;
    return factory->m_cRef;
}

/* <summary> Release a factory reference. </summary>
 * <description> Static factory storage is retained. </description> */
static uint32_t ECOCALLMETHOD CEcoLab2_47E653D2Factory_Release(IEcoComponentFactory* me) {
    CEcoLab2_47E653D2Factory* factory = (CEcoLab2_47E653D2Factory*)me;
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
static int16_t ECOCALLMETHOD CEcoLab2_47E653D2Factory_Init(IEcoComponentFactory* me, IEcoUnknown* system, voidptr_t parameters) {
    (void)parameters;
    if (me == 0 || system == 0) {
        return ERR_ECO_POINTER;
    }
    return ERR_ECO_SUCCESS;
}

/* <summary> Create an outer laboratory component. </summary>
 * <description> Returns an owned interface; cleans partial construction on failure. </description> */
static int16_t ECOCALLMETHOD CEcoLab2_47E653D2Factory_Alloc(IEcoComponentFactory* me, IEcoUnknown* system,
    IEcoUnknown* outer, const UGUID* iid, voidptr_t* out) {
    IEcoSystem1* sys = 0;
    IEcoInterfaceBus1* bus = 0;
    IEcoMemoryAllocator1* memory = 0;
    CEcoLab2_47E653D2* object = 0;
    int16_t status = ERR_ECO_POINTER;
    if (out == 0) {
        return ERR_ECO_POINTER;
    }
    *out = 0;
    if (me == 0 || system == 0 || iid == 0) {
        return ERR_ECO_POINTER;
    }
    if (outer != 0) {
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
    object = (CEcoLab2_47E653D2*)memory->pVTbl->Alloc(memory, (uint32_t)sizeof(CEcoLab2_47E653D2));
    if (object == 0) {
        status = ERR_ECO_OUTOFMEMORY;
        goto Release;
    }
    memory->pVTbl->Copy(memory, object, &g_xCEcoLab2_47E653D2, (uint32_t)sizeof(CEcoLab2_47E653D2));
    object->m_pIMem = memory;
    memory = 0;
    status = object->Create(object, system, outer);
    if (status == 0) {
        status = object->Init(object, system);
    }
    if (status == 0) {
        status = object->m_pVTblIEcoLab2->QueryInterface((IEcoLab2*)object, iid, out);
    }
Release:
    if (object != 0) {
        object->m_pVTblIEcoLab2->Release((IEcoLab2*)object);
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
static char_t* ECOCALLMETHOD CEcoLab2_47E653D2Factory_get_Name(IEcoComponentFactory* me) {
    CEcoLab2_47E653D2Factory* factory = (CEcoLab2_47E653D2Factory*)me;
    if (me == 0) {
        return 0;
    }
    return factory->m_Name;
}

/* <summary> Get the factory version. </summary> <description> See get_Name for ownership rules. </description> */
static char_t* ECOCALLMETHOD CEcoLab2_47E653D2Factory_get_Version(IEcoComponentFactory* me) {
    CEcoLab2_47E653D2Factory* factory = (CEcoLab2_47E653D2Factory*)me;
    if (me == 0) {
        return 0;
    }
    return factory->m_Version;
}

/* <summary> Get the factory manufacturer. </summary> <description> See get_Name for ownership rules. </description> */
static char_t* ECOCALLMETHOD CEcoLab2_47E653D2Factory_get_Manufacturer(IEcoComponentFactory* me) {
    CEcoLab2_47E653D2Factory* factory = (CEcoLab2_47E653D2Factory*)me;
    if (me == 0) {
        return 0;
    }
    return factory->m_Manufacturer;
}

IEcoComponentFactoryVTbl g_x7C9B7CECBC69410F9B283F6647E653D2FactoryVTbl = {
    CEcoLab2_47E653D2Factory_QueryInterface,
    CEcoLab2_47E653D2Factory_AddRef,
    CEcoLab2_47E653D2Factory_Release,
    CEcoLab2_47E653D2Factory_Alloc,
    CEcoLab2_47E653D2Factory_Init,
    CEcoLab2_47E653D2Factory_get_Name,
    CEcoLab2_47E653D2Factory_get_Version,
    CEcoLab2_47E653D2Factory_get_Manufacturer
};


CEcoLab2_47E653D2Factory g_x7C9B7CECBC69410F9B283F6647E653D2Factory = {
    &g_x7C9B7CECBC69410F9B283F6647E653D2FactoryVTbl,
    0,
    "Eco.Lab2\0",
    "1.0.0.0\0",
    "HSE NN\0"
};

#ifdef ECO_DLL
/* <summary> Get the dynamic factory. </summary>
 * <description> Returns the static factory object. </description> */
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr(void) {
    return (IEcoComponentFactory*)&g_x7C9B7CECBC69410F9B283F6647E653D2Factory;
}
#elif ECO_LIB
IEcoComponentFactory* GetIEcoComponentFactoryPtr_7C9B7CECBC69410F9B283F6647E653D2 = (IEcoComponentFactory*)&g_x7C9B7CECBC69410F9B283F6647E653D2Factory;
#endif
