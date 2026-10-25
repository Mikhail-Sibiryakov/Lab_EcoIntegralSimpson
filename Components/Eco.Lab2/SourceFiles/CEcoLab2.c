/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Eco.Lab2 aggregation and containment. </summary>
 * <description> C/X and E/Y wrappers with directly exposed aggregated AdvancedMath. </description>
 * <reference> Eco.AI.Promts/Templates/CROOT.c; docs/lab2/DESIGN.md </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#include "CEcoLab2.h"
#include "IEcoInterfaceBus1.h"
#include "IdEcoCalculatorCCompat.h"
#include "IdEcoCalculatorE.h"
#include "IdEcoIntegralSimpson.h"

/* <summary> Query the aggregate's public interface set. </summary>
 * <description> Outer X/Y have priority; only AdvancedMath is obtained through inner ND. </description> */
static int16_t ECOCALLMETHOD Lab2Query(IEcoLab2* me, const UGUID* iid, voidptr_t* out) {
    CEcoLab2_47E653D2* object = (CEcoLab2_47E653D2*)me;
    if (out == 0) {
        return ERR_ECO_POINTER;
    }
    *out = 0;
    if (me == 0 || iid == 0) {
        return ERR_ECO_POINTER;
    }
    if (IsEqualUGUID(iid, &IID_IEcoUnknown) || IsEqualUGUID(iid, &IID_IEcoLab2)) {
        *out = me;
    }
    else if (IsEqualUGUID(iid, &IID_IEcoCalculatorX)) {
        *out = &object->m_x;
    }
    else if (IsEqualUGUID(iid, &IID_IEcoCalculatorY)) {
        *out = &object->m_y;
    }
    else if (IsEqualUGUID(iid, &IID_IEcoAdvancedMath) && object->m_pIUnkInner != 0) {
        return object->m_pIUnkInner->pVTbl->QueryInterface(object->m_pIUnkInner, iid, out);
    }
    else {
        return ERR_ECO_NOINTERFACE;
    }
    object->m_cRef++;
    return ERR_ECO_SUCCESS;
}

/* <summary> Retain the whole aggregate. </summary>
 * <description> Public inner AdvancedMath delegates here as well. </description> */
static uint32_t ECOCALLMETHOD Lab2AddRef(IEcoLab2* me) {
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    return ++((CEcoLab2_47E653D2*)me)->m_cRef;
}

/* <summary> Release the whole aggregate. </summary>
 * <description> Final release destroys inner private ownership and contained objects. </description> */
static uint32_t ECOCALLMETHOD Lab2Release(IEcoLab2* me) {
    CEcoLab2_47E653D2* object = (CEcoLab2_47E653D2*)me;
    uint32_t remaining = 0;
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    remaining = --object->m_cRef;
    if (remaining == 0) {
        object->Delete(object);
    }
    return remaining;
}

/* <summary> Dispatch an arithmetic operation with a status/out contract. </summary>
 * <description> Validates legacy narrow results before calling the original calculators. </description> */
static int16_t ECOCALLMETHOD Lab2Calculate(IEcoLab2* me, uint16_t operation, int16_t a, int16_t b, int32_t* result) {
    CEcoLab2_47E653D2* object = (CEcoLab2_47E653D2*)me;
    int32_t difference = 0;
    int32_t value = 0;
    if (me == 0 || result == 0) {
        return ERR_ECO_POINTER;
    }
    if (operation == 1) {
        value = object->m_pIX->pVTbl->Addition(object->m_pIX, a, b);
    }
    else if (operation == 2) {
        difference = (int32_t)a - (int32_t)b;
        if (difference < -32768 || difference > 32767) {
            return ERR_ECO_INVALIDARG;
        }
        value = object->m_pIX->pVTbl->Subtraction(object->m_pIX, a, b);
    }
    else if (operation == 3) {
        value = object->m_pIY->pVTbl->Multiplication(object->m_pIY, a, b);
    }
    else if (operation == 4) {
        if (b == 0 || (a == -32768 && b == -1)) {
            return ERR_ECO_INVALIDARG;
        }
        value = object->m_pIY->pVTbl->Division(object->m_pIY, a, b);
    }
    else {
        return ERR_ECO_INVALIDARG;
    }
    *result = value;
    return ERR_ECO_SUCCESS;
}

/* <summary> Query through the X wrapper. </summary>
 * <description> All public paths share the outer interface set. </description> */
static int16_t ECOCALLMETHOD Lab2XQuery(IEcoCalculatorX* me, const UGUID* iid, voidptr_t* out) {
    if (me == 0) {
        if (out != 0) {
            *out = 0;
        }
        return ERR_ECO_POINTER;
    }
    return Lab2Query((IEcoLab2*)((Lab2XSlot*)me)->owner, iid, out);
}

/* <summary> Retain the aggregate through X. </summary>
 * <description> Contained C's count stays private. </description> */
static uint32_t ECOCALLMETHOD Lab2XAddRef(IEcoCalculatorX* me) {
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    return Lab2AddRef((IEcoLab2*)((Lab2XSlot*)me)->owner);
}

/* <summary> Release the aggregate through X. </summary>
 * <description> May delete the entire composition. </description> */
static uint32_t ECOCALLMETHOD Lab2XRelease(IEcoCalculatorX* me) {
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    return Lab2Release((IEcoLab2*)((Lab2XSlot*)me)->owner);
}

/* <summary> Add through included C. </summary>
 * <description> Preserves int32_t return values. </description> */
static int32_t ECOCALLMETHOD Lab2Addition(IEcoCalculatorX* me, int16_t a, int16_t b) {
    CEcoLab2_47E653D2* object = 0;
    if (me == 0) {
        return ERR_ECO_POINTER;
    }
    object = ((Lab2XSlot*)me)->owner;
    return object->m_pIX->pVTbl->Addition(object->m_pIX, a, b);
}

/* <summary> Subtract through included C. </summary>
 * <description> Keeps the original X contract. </description> */
static int16_t ECOCALLMETHOD Lab2Subtraction(IEcoCalculatorX* me, int16_t a, int16_t b) {
    CEcoLab2_47E653D2* object = 0;
    if (me == 0) {
        return ERR_ECO_POINTER;
    }
    object = ((Lab2XSlot*)me)->owner;
    return object->m_pIX->pVTbl->Subtraction(object->m_pIX, a, b);
}

/* <summary> Query through the Y wrapper. </summary>
 * <description> Does not leak E's independent identity. </description> */
static int16_t ECOCALLMETHOD Lab2YQuery(IEcoCalculatorY* me, const UGUID* iid, voidptr_t* out) {
    if (me == 0) {
        if (out != 0) {
            *out = 0;
        }
        return ERR_ECO_POINTER;
    }
    return Lab2Query((IEcoLab2*)((Lab2YSlot*)me)->owner, iid, out);
}

/* <summary> Retain the aggregate through Y. </summary>
 * <description> Uses the same count as Lab2 and AdvancedMath. </description> */
static uint32_t ECOCALLMETHOD Lab2YAddRef(IEcoCalculatorY* me) {
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    return Lab2AddRef((IEcoLab2*)((Lab2YSlot*)me)->owner);
}

/* <summary> Release the aggregate through Y. </summary>
 * <description> Final release also destroys E and its B. </description> */
static uint32_t ECOCALLMETHOD Lab2YRelease(IEcoCalculatorY* me) {
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    return Lab2Release((IEcoLab2*)((Lab2YSlot*)me)->owner);
}

/* <summary> Multiply through included E. </summary>
 * <description> Uses E's own Y implementation. </description> */
static int32_t ECOCALLMETHOD Lab2Multiplication(IEcoCalculatorY* me, int16_t a, int16_t b) {
    CEcoLab2_47E653D2* object = 0;
    if (me == 0) {
        return ERR_ECO_POINTER;
    }
    object = ((Lab2YSlot*)me)->owner;
    return object->m_pIY->pVTbl->Multiplication(object->m_pIY, a, b);
}

/* <summary> Divide through included E. </summary>
 * <description> Preserves Y's integer return contract. </description> */
static int16_t ECOCALLMETHOD Lab2Division(IEcoCalculatorY* me, int16_t a, int16_t b) {
    CEcoLab2_47E653D2* object = 0;
    if (me == 0) {
        return ERR_ECO_POINTER;
    }
    object = ((Lab2YSlot*)me)->owner;
    return object->m_pIY->pVTbl->Division(object->m_pIY, a, b);
}

/* <summary> Initialize the fixed composition. </summary>
 * <description> All partial resources belong to the object and are cleaned by Delete. </description> */
static int16_t ECOCALLMETHOD Lab2Init(CEcoLab2_47E653D2Ptr_t object, IEcoUnknown* system) {
    IEcoInterfaceBus1* bus = 0;
    int16_t status = ERR_ECO_POINTER;
    if (object == 0 || system == 0) {
        return status;
    }
    status = system->pVTbl->QueryInterface(system, &GID_IEcoSystem, (voidptr_t*)&object->m_pISys);
    if (status != 0) {
        return status;
    }
    status = object->m_pISys->pVTbl->QueryInterface(object->m_pISys, &IID_IEcoInterfaceBus1, (voidptr_t*)&bus);
    if (status != 0 || bus == 0) {
        return ERR_ECO_NOBUS;
    }
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoCalculatorC_Lab2, 0, &IID_IEcoCalculatorX, (voidptr_t*)&object->m_pIX);
    if (status == 0 && object->m_pIX != 0) {
        status = bus->pVTbl->QueryComponent(bus, &CID_EcoCalculatorE, 0, &IID_IEcoCalculatorY, (voidptr_t*)&object->m_pIY);
    }
    if (status == 0 && object->m_pIY != 0) {
        status = bus->pVTbl->QueryComponent(bus, &CID_EcoIntegralSimpson, (IEcoUnknown*)object, &IID_IEcoUnknown, (voidptr_t*)&object->m_pIUnkInner);
    }
    bus->pVTbl->Release(bus);
    if (status == 0 && (object->m_pIX == 0 || object->m_pIY == 0 || object->m_pIUnkInner == 0)) {
        return ERR_ECO_FAIL;
    }
    return status;
}

/* <summary> Set interface owners before initialization. </summary>
 * <description> This outer component does not itself support external aggregation. </description> */
static int16_t ECOCALLMETHOD Lab2Create(CEcoLab2_47E653D2Ptr_t object, IEcoUnknown* system, IEcoUnknown* outer) {
    if (object == 0 || system == 0) {
        return ERR_ECO_POINTER;
    }
    if (outer != 0) {
        return ERR_ECO_NOAGGREGATION;
    }
    object->m_x.owner = object;
    object->m_y.owner = object;
    return ERR_ECO_SUCCESS;
}

/* <summary> Destroy the composition from its single controlling owner. </summary>
 * <description> Only inner ND is released, preventing recursive public releases. </description> */
static void ECOCALLMETHOD Lab2Delete(CEcoLab2_47E653D2Ptr_t object) {
    IEcoMemoryAllocator1* memory = object->m_pIMem;
    if (object->m_pIUnkInner != 0) {
        object->m_pIUnkInner->pVTbl->Release(object->m_pIUnkInner);
    }
    if (object->m_pIY != 0) {
        object->m_pIY->pVTbl->Release(object->m_pIY);
    }
    if (object->m_pIX != 0) {
        object->m_pIX->pVTbl->Release(object->m_pIX);
    }
    if (object->m_pISys != 0) {
        object->m_pISys->pVTbl->Release(object->m_pISys);
    }
    memory->pVTbl->Free(memory, object);
    memory->pVTbl->Release(memory);
}

static IEcoLab2VTbl g_Lab2 = {Lab2Query, Lab2AddRef, Lab2Release, Lab2Calculate};
static IEcoCalculatorXVTbl g_Lab2X = {Lab2XQuery, Lab2XAddRef, Lab2XRelease, Lab2Addition, Lab2Subtraction};
static IEcoCalculatorYVTbl g_Lab2Y = {Lab2YQuery, Lab2YAddRef, Lab2YRelease, Lab2Multiplication, Lab2Division};

CEcoLab2_47E653D2 g_xCEcoLab2_47E653D2 = {
    &g_Lab2,
    {&g_Lab2X, 0},
    {&g_Lab2Y, 0},
    Lab2Init,
    Lab2Create,
    Lab2Delete,
    1,
    0,
    0,
    0,
    0,
    0
};
