/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Direct two-source event application. </summary>
 * <description> Laboratory 3: two sources and two shared event receivers. </description>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */

#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoIntegralSimpson.h"
#include "IEcoAdvancedMath.h"
#include "IEcoConnectionPointContainer.h"
#include "IEcoConnectionPoint.h"
#include "CEcoIntegralSimpsonSink.h"
#include "Lab3TestInitialization.h"

/* <summary> Record one complete scenario. </summary>
 * <description> Setup failures and output errors also fail EcoMain. </description> */
static void Check(Lab3Output* output, uint8_t condition, char_t* label,
    uint32_t* passed, uint32_t* failed) {
    if (condition) {
        (*passed)++;
        Lab3Print(output, "PASS ");
    }
    else {
        (*failed)++;
        Lab3Print(output, "FAIL ");
    }
    Lab3Print(output, label);
    Lab3Print(output, "\n");
}

/* <summary> A smooth peak requiring adaptive interval refinement. </summary>
 * <description> The same function is used with two different tolerances. </description> */
static double_t ECOCDECLMETHOD Peak(double_t x, voidptr_t context) {
    double_t offset = x - 0.375;
    (void)context;
    return 1.0 / (1.0 + 400.0 * offset * offset);
}

/* <summary> Float version of the same demonstration function. </summary>
 * <description> Computation remains in float_t. </description> */
static float_t ECOCDECLMETHOD PeakF(float_t x, voidptr_t context) {
    float_t offset = x - 0.375F;
    (void)context;
    return 1.0F / (1.0F + 400.0F * offset * offset);
}

/* <summary> Long-double version of the same demonstration function. </summary>
 * <description> Neither the integrand nor event payload narrows to double. </description> */
static long double ECOCDECLMETHOD PeakL(long double x, voidptr_t context) {
    long double offset = x - 0.375L;
    (void)context;
    return 1.0L / (1.0L + 400.0L * offset * offset);
}

/* <summary> Produce a nonfinite function value. </summary>
 * <description> The runtime addition needs no library call. </description> */
static double_t ECOCDECLMETHOD Nonfinite(double_t x, voidptr_t context) {
    double_t value = __DBL_MAX__;
    (void)x;
    (void)context;
    return value + value;
}

/* <summary> Compare the finish event with the actual numerical outputs. </summary>
 * <description> Refined events and source identities are checked by both receivers. </description> */
static uint8_t Matches(Lab3Statistics* stats, int16_t status, long double value, long double error) {
    return (uint8_t)(status == 0 && stats->status == 0 && !stats->running &&
        stats->value == value && stats->error == error);
}

/* <summary> Run a compact public-interface demonstration and release all subscriptions. </summary>
 * <description> Two source objects share one journal and one statistics receiver. </description> */
int16_t ECOCALLMETHOD EcoMain(IEcoUnknown* system) {
    IEcoSystem1* sys = 0;
    IEcoInterfaceBus1* bus = 0;
    IEcoFileManager1* files = 0;
    IEcoMemoryAllocator1* memory = 0;
    IEcoAdvancedMath* math[2] = {0, 0};
    IEcoUnknown* sources[2] = {0, 0};
    IEcoConnectionPoint* points[2] = {0, 0};
    IEcoConnectionPointContainer* container = 0;
    CEcoIntegralSimpsonSink* sinks[2] = {0, 0};
    uint32_t cookies[2][2] = {{0, 0}, {0, 0}};
    Lab3Output output = {0, 0};
    Lab3Statistics* stats = 0;
    uint32_t passed = 0;
    uint32_t failed = 0;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t before[2];
    uint32_t refinements = 0;
    uint32_t starts = 0;
    uint32_t finishes = 0;
    uint8_t balanced = 1;
    int16_t status = ERR_ECO_FAIL;
    double_t value = 0;
    double_t error = 0;
    float_t valueF = 0;
    float_t errorF = 0;
    long double valueL = 0;
    long double errorL = 0;
    if (system == 0) {
        return ERR_ECO_POINTER;
    }
    status = system->pVTbl->QueryInterface(system, &GID_IEcoSystem, (voidptr_t*)&sys);
    if (status != 0 || sys == 0) {
        goto Cleanup;
    }
    status = sys->pVTbl->QueryInterface(sys, &IID_IEcoInterfaceBus1, (voidptr_t*)&bus);
    if (status != 0 || bus == 0) {
        goto Cleanup;
    }
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoFileSystemManagement1, 0,
        &IID_IEcoFileManager1, (voidptr_t*)&files);
    if (status != 0 || files == 0) {
        goto Cleanup;
    }
    output.file = files->pVTbl->Create(files, "/dev/stdout");
    if (output.file == 0) {
        status = ERR_ECO_FAIL;
        goto Cleanup;
    }
    Lab3Print(&output, "EcoLab3: two integrators, two shared receivers\n");
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoMemoryManager1, 0,
        &IID_IEcoMemoryAllocator1, (voidptr_t*)&memory);
    if (status != 0 || memory == 0) {
        goto SetupFailed;
    }
    status = Lab3InitializeComponents(bus);
    if (status != 0) {
        goto SetupFailed;
    }
    for (i = 0; i < 2; i++) {
        status = bus->pVTbl->QueryComponent(bus, &CID_EcoIntegralSimpson, 0,
            &IID_IEcoAdvancedMath, (voidptr_t*)&math[i]);
        if (status != 0 || math[i] == 0) {
            goto SetupFailed;
        }
        status = math[i]->pVTbl->QueryInterface(math[i], &IID_IEcoUnknown, (voidptr_t*)&sources[i]);
        if (status != 0 || sources[i] == 0) {
            goto SetupFailed;
        }
        status = math[i]->pVTbl->QueryInterface(math[i], &IID_IEcoConnectionPointContainer,
            (voidptr_t*)&container);
        if (status != 0 || container == 0) {
            goto SetupFailed;
        }
        status = container->pVTbl->FindConnectionPoint(container, &IID_IEcoIntegralSimpsonEvents, &points[i]);
        container->pVTbl->Release(container);
        container = 0;
        if (status != 0 || points[i] == 0) {
            goto SetupFailed;
        }
    }
    Check(&output, (uint8_t)(sources[0] != sources[1]), "two independent event sources", &passed, &failed);
    for (j = 0; j < 2; j++) {
        status = Lab3CreateSink(memory, &output, sources, (uint8_t)(j == 0), &sinks[j]);
        if (status != 0) {
            goto SetupFailed;
        }
        for (i = 0; i < 2; i++) {
            status = points[i]->pVTbl->Advise(points[i], (IEcoUnknown*)sinks[j], &cookies[i][j]);
            if (status != 0 || cookies[i][j] == 0) {
                goto SetupFailed;
            }
        }
    }
    Check(&output, (uint8_t)(sinks[0]->references == 3 && sinks[1]->references == 3),
        "four subscriptions retain both receivers", &passed, &failed);
    for (i = 0; i < 2; i++) {
        status = math[i]->pVTbl->integral(math[i], Peak, 0, 0.0, 1.0,
            i == 0 ? 1e-4 : 1e-8, 0.0, &value, &error);
        stats = &sinks[1]->statistics[i];
        Check(&output, (uint8_t)(Matches(stats, status, (long double)value, (long double)error) &&
            stats->starts == 1 && stats->finishes == 1 && stats->refinements > 0 &&
            stats->precision == ECO_INTEGRAL_DOUBLE),
            i == 0 ? "source 1: requested error 1e-4" : "source 2: requested error 1e-8", &passed, &failed);
        Lab3Print(&output, "initial source ");
        Lab3PrintCount(&output, i + 1);
        Lab3Print(&output, " refinements=");
        Lab3PrintCount(&output, stats->refinements);
        Lab3Print(&output, " Q=");
        Lab3PrintReal(&output, stats->value);
        Lab3Print(&output, " E=");
        Lab3PrintReal(&output, stats->error);
        Lab3Print(&output, "\n");
    }
    Check(&output, (uint8_t)(sinks[0]->statistics[0].refinements == sinks[1]->statistics[0].refinements &&
        sinks[0]->statistics[1].refinements == sinks[1]->statistics[1].refinements &&
        !sinks[0]->invalidSequence && !sinks[1]->invalidSequence),
        "both receivers distinguish the sources and event order", &passed, &failed);
    before[0] = sinks[0]->statistics[0].starts;
    before[1] = sinks[0]->statistics[1].starts;
    status = points[0]->pVTbl->Unadvise(points[0], cookies[0][0]);
    if (status != 0) {
        goto SetupFailed;
    }
    cookies[0][0] = 0;
    for (i = 0; i < 2; i++) {
        status = math[i]->pVTbl->integral(math[i], Peak, 0, 1.0, 0.0, 1e-4, 0.0, &value, &error);
        if (!Matches(&sinks[1]->statistics[i], status, (long double)value, (long double)error)) {
            balanced = 0;
        }
    }
    Check(&output, (uint8_t)(balanced && sinks[0]->statistics[0].starts == before[0] &&
        sinks[0]->statistics[1].starts == before[1] + 1 &&
        sinks[1]->statistics[0].starts == 2 && sinks[1]->statistics[1].starts == 2),
        "unsubscribe affects only journal/source 1; reversed bounds work", &passed, &failed);
    status = math[0]->pVTbl->integralf(math[0], PeakF, 0, 0.0F, 1.0F, 1e-4F, 0.0F, &valueF, &errorF);
    stats = &sinks[1]->statistics[0];
    Check(&output, (uint8_t)(Matches(stats, status, (long double)valueF, (long double)errorF) &&
        stats->precision == ECO_INTEGRAL_FLOAT), "float events preserve typed outputs", &passed, &failed);
    status = math[0]->pVTbl->integrall(math[0], PeakL, 0, 0.0L, 1.0L, 1e-10L, 0.0L, &valueL, &errorL);
    Check(&output, (uint8_t)(Matches(stats, status, valueL, errorL) &&
        stats->precision == ECO_INTEGRAL_LONG_DOUBLE), "long double events preserve typed outputs", &passed, &failed);
    stats = &sinks[1]->statistics[1];
    refinements = stats->refinements;
    starts = stats->starts;
    finishes = stats->finishes;
    status = math[1]->pVTbl->integral(math[1], Nonfinite, 0, 0.5, 0.5, 1e-8, 0.0, &value, &error);
    Check(&output, (uint8_t)(Matches(stats, status, 0.0L, 0.0L) && value == 0 && error == 0 &&
        stats->starts == starts + 1 && stats->finishes == finishes + 1 && stats->refinements == refinements),
        "equal bounds: start and finish without evaluation or refinement", &passed, &failed);
    value = 123.0;
    error = 456.0;
    starts = stats->starts;
    finishes = stats->finishes;
    status = math[1]->pVTbl->integral(math[1], Nonfinite, 0, 0.0, 1.0, 1e-8, 0.0, &value, &error);
    Check(&output, (uint8_t)(status != 0 && stats->status == status &&
        stats->starts == starts + 1 && stats->finishes == finishes + 1 &&
        value == 123.0 && error == 456.0 && !stats->running),
        "numerical failure: finish reports error and outputs stay unchanged", &passed, &failed);
    Check(&output, (uint8_t)(!sinks[0]->invalidSequence && !sinks[1]->invalidSequence),
        "all observed runs have a valid event sequence", &passed, &failed);
    goto Cleanup;
SetupFailed:
    Check(&output, 0, "application setup or subscription", &passed, &failed);
Cleanup:
    if (container != 0) {
        container->pVTbl->Release(container);
    }
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (cookies[i][j] != 0) {
                if (points[i]->pVTbl->Unadvise(points[i], cookies[i][j]) != 0) {
                    balanced = 0;
                }
            }
        }
    }
    if (sinks[0] != 0 && sinks[1] != 0) {
        Check(&output, (uint8_t)(balanced && sinks[0]->references == 1 && sinks[1]->references == 1),
            "unsubscribe releases the retained receiver references", &passed, &failed);
    }
    for (j = 0; j < 2; j++) {
        if (sinks[j] != 0) {
            sinks[j]->pVTbl->Release((IEcoIntegralSimpsonEvents*)sinks[j]);
        }
    }
    for (i = 0; i < 2; i++) {
        if (points[i] != 0) {
            points[i]->pVTbl->Release(points[i]);
        }
        if (sources[i] != 0) {
            sources[i]->pVTbl->Release(sources[i]);
        }
        if (math[i] != 0) {
            math[i]->pVTbl->Release(math[i]);
        }
    }
    if (memory != 0) {
        memory->pVTbl->Release(memory);
    }
    if (output.file != 0) {
        Lab3Print(&output, "Lab3 summary: ");
        Lab3PrintCount(&output, passed);
        Lab3Print(&output, " PASS, ");
        Lab3PrintCount(&output, failed);
        Lab3Print(&output, " FAIL\n");
        status = (failed == 0 && output.status == 0 && passed != 0) ? 0 : ERR_ECO_FAIL;
        output.file->pVTbl->Close(output.file);
        output.file->pVTbl->Release(output.file);
    }
    if (files != 0) {
        files->pVTbl->Release(files);
    }
    if (bus != 0) {
        bus->pVTbl->Release(bus);
    }
    if (sys != 0) {
        sys->pVTbl->Release(sys);
    }
    return status;
}
