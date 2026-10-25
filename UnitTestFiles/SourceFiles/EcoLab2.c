/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Laboratory 2 console UnitTest. </summary>
 * <description> Exercises real calculator and composite interfaces through Eco. </description>
 * <reference> docs/lab2/DESIGN.md </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#include "IEcoSystem1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IEcoFile1.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IdEcoCalculatorD.h"
#include "IdEcoCalculatorE.h"
#include "Lab2TestRegistration.h"
#include "IdEcoIntegralSimpson.h"
#include "IdEcoLab2.h"

static IEcoFile1* g_Output = 0;
static uint32_t g_Passed = 0;
static uint32_t g_Failed = 0;
static uint32_t g_Pairs = 0;
static int16_t g_OutputStatus = 0;

/* <summary> Write text using the Eco file service. </summary>
 * <description> Records partial or failed writes as an unsuccessful run. </description> */
static void Print(char_t* text) {
    uint32_t length = 0;
    uint32_t written = 0;
    while (text[length] != 0) {
        length++;
    }
    written = length;
    if (g_Output->pVTbl->Write(g_Output, text, &written) != 0 || written != length) {
        g_OutputStatus = ERR_ECO_FAIL;
    }
}

/* <summary> Print a decimal count without libc. </summary>
 * <description> The stack buffer covers uint32_t. </description> */
static void PrintCount(uint32_t value) {
    char_t digits[11];
    uint32_t position = 10;
    digits[position] = 0;
    do {
        position--;
        digits[position] = (char_t)('0' + value % 10);
        value /= 10;
    } while (value != 0);
    Print(&digits[position]);
}

/* <summary> Record an independently expected behavior. </summary>
 * <description> Every failure contributes to EcoMain's status. </description> */
static void Check(uint8_t condition, char_t* label) {
    if (condition != 0) {
        g_Passed++;
        Print("PASS ");
    }
    else {
        g_Failed++;
        Print("FAIL ");
    }
    Print(label);
    Print("\n");
}

/* <summary> Check public transitions and one identity per source interface. </summary>
 * <description> Object is the canonical Unknown returned by QueryComponent; results own references. </description> */
static void TestMatrix(IEcoUnknown* object, const UGUID** iids, char_t** names, uint32_t count) {
    IEcoUnknown* from = 0;
    IEcoUnknown* to = 0;
    IEcoUnknown* identity = 0;
    uint32_t i = 0;
    uint32_t j = 0;
    int16_t status = 0;
    for (i = 0; i < count; i++) {
        status = object->pVTbl->QueryInterface(object, iids[i], (voidptr_t*)&from);
        Check((uint8_t)(status == 0 && from != 0), "obtain source interface");
        if (from == 0) {
            continue;
        }
        status = from->pVTbl->QueryInterface(from, &IID_IEcoUnknown, (voidptr_t*)&identity);
        Check((uint8_t)(status == 0 && identity == object), "same object Unknown");
        if (identity != 0) {
            identity->pVTbl->Release(identity);
            identity = 0;
        }
        for (j = 0; j < count; j++) {
            Print("QI ");
            Print(names[i]);
            Print(" -> ");
            Print(names[j]);
            Print("\n");
            status = from->pVTbl->QueryInterface(from, iids[j], (voidptr_t*)&to);
            Check((uint8_t)(status == 0 && to != 0), "interface transition");
            g_Pairs++;
            if (to != 0) {
                to->pVTbl->Release(to);
                to = 0;
            }
        }
        from->pVTbl->Release(from);
        from = 0;
    }
}

/* <summary> Demonstrate the four operations of a composite component. </summary>
 * <description> Uses one ordinary example per method through public X and Y. </description> */
static void TestArithmetic(IEcoUnknown* object) {
    IEcoCalculatorX* x = 0;
    IEcoCalculatorY* y = 0;
    int16_t status = 0;
    status = object->pVTbl->QueryInterface(object, &IID_IEcoCalculatorX, (voidptr_t*)&x);
    Check((uint8_t)(status == 0 && x != 0), "obtain arithmetic X");
    if (x != 0) {
        Check((uint8_t)(x->pVTbl->Addition(x, 3, 5) == 8), "3 + 5 = 8");
        Check((uint8_t)(x->pVTbl->Subtraction(x, 3, 5) == -2), "3 - 5 = -2");
        x->pVTbl->Release(x);
    }
    status = object->pVTbl->QueryInterface(object, &IID_IEcoCalculatorY, (voidptr_t*)&y);
    Check((uint8_t)(status == 0 && y != 0), "obtain arithmetic Y");
    if (y != 0) {
        Check((uint8_t)(y->pVTbl->Multiplication(y, 3, 5) == 15), "3 * 5 = 15");
        Check((uint8_t)(y->pVTbl->Division(y, -9, 2) == -4), "-9 / 2 = -4");
        y->pVTbl->Release(y);
    }
}

/* <summary> Integrand for the aggregation example. </summary>
 * <description> The analytic integral of x squared on [0,1] is 1/3. </description> */
static double_t ECOCDECLMETHOD Square(double_t x, voidptr_t context) {
    (void)context;
    return x * x;
}

/* <summary> Call integration through the inherited public interface. </summary>
 * <description> Numerical regression for all three types remains in the original client. </description> */
static void TestIntegral(IEcoAdvancedMath* math) {
    double_t value = 0;
    double_t error = 0;
    int16_t status = 0;
    status = math->pVTbl->integral(math, Square, 0, 0, 1, 1e-10, 0, &value, &error);
    Check((uint8_t)(status == 0 && value > 0.3333333332 && value < 0.3333333335 && error >= 0 && error <= 1e-10), "integral of x squared on [0,1] = 1/3");
}

/* <summary> Demonstrate A and D through the standalone integrator. </summary>
 * <description> Creates the actual component with its normal Eco dependencies. </description> */
static void TestIntegrator(IEcoInterfaceBus1* bus) {
    IEcoAdvancedMath* math = 0;
    int16_t status = 0;
    Print("SCENARIO standalone integrator: A/D\n");
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoIntegralSimpson, 0, &IID_IEcoAdvancedMath, (voidptr_t*)&math);
    Check((uint8_t)(status == 0 && math != 0), "create standalone integrator");
    if (math != 0) {
        TestArithmetic((IEcoUnknown*)math);
        math->pVTbl->Release(math);
    }
}

/* <summary> Demonstrate the Calculate interface and its two common errors. </summary>
 * <description> Error calls preserve the previously stored result. </description> */
static void TestCalculate(IEcoLab2* lab) {
    int32_t value = 0;
    int16_t status = 0;
    status = lab->pVTbl->Calculate(lab, 1, 3, 5, &value);
    Check((uint8_t)(status == 0 && value == 8), "Calculate addition");
    status = lab->pVTbl->Calculate(lab, 2, 3, 5, &value);
    Check((uint8_t)(status == 0 && value == -2), "Calculate subtraction");
    status = lab->pVTbl->Calculate(lab, 3, 3, 5, &value);
    Check((uint8_t)(status == 0 && value == 15), "Calculate multiplication");
    status = lab->pVTbl->Calculate(lab, 4, -9, 2, &value);
    Check((uint8_t)(status == 0 && value == -4), "Calculate integer division");
    value = 99;
    status = lab->pVTbl->Calculate(lab, 0, 3, 5, &value);
    Check((uint8_t)(status == ERR_ECO_INVALIDARG && value == 99), "Calculate rejects unknown operation");
    status = lab->pVTbl->Calculate(lab, 4, 3, 0, &value);
    Check((uint8_t)(status == ERR_ECO_INVALIDARG && value == 99), "Calculate rejects division by zero");
}

/* <summary> Demonstrate C/E arithmetic and aggregation of the integrator. </summary>
 * <description> Obtains integration from the same outer EcoLab2 object. </description> */
static void TestLab2(IEcoInterfaceBus1* bus) {
    IEcoLab2* lab = 0;
    IEcoAdvancedMath* math = 0;
    int16_t status = 0;
    Print("SCENARIO EcoLab2: C/E, Calculate, integration\n");
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoLab2, 0, &IID_IEcoLab2, (voidptr_t*)&lab);
    Check((uint8_t)(status == 0 && lab != 0), "create EcoLab2");
    if (lab != 0) {
        TestArithmetic((IEcoUnknown*)lab);
        TestCalculate(lab);
        status = lab->pVTbl->QueryInterface(lab, &IID_IEcoAdvancedMath, (voidptr_t*)&math);
        Check((uint8_t)(status == 0 && math != 0), "obtain aggregated integration interface");
        if (math != 0) {
            TestIntegral(math);
            math->pVTbl->Release(math);
        }
        lab->pVTbl->Release(lab);
    }
}

/* <summary> Demonstrate B through D containment and E aggregation. </summary>
 * <description> Each real Y component supplies X with the reused addition method. </description> */
static void TestB(IEcoInterfaceBus1* bus) {
    const UGUID* cids[2];
    char_t* names[2] = {"D", "E"};
    IEcoCalculatorY* y = 0;
    IEcoCalculatorX* x = 0;
    uint32_t i = 0;
    int16_t status = 0;
    cids[0] = &CID_EcoCalculatorD;
    cids[1] = &CID_EcoCalculatorE;
    Print("SCENARIO B through D/E\n");
    for (i = 0; i < 2; i++) {
        Print("COMPONENT ");
        Print(names[i]);
        Print("\n");
        status = bus->pVTbl->QueryComponent(bus, cids[i], 0, &IID_IEcoCalculatorY, (voidptr_t*)&y);
        Check((uint8_t)(status == 0 && y != 0), "obtain calculator Y");
        if (y != 0) {
            status = y->pVTbl->QueryInterface(y, &IID_IEcoCalculatorX, (voidptr_t*)&x);
            Check((uint8_t)(status == 0 && x != 0), "obtain X from Y");
            if (x != 0) {
                Check((uint8_t)(x->pVTbl->Addition(x, 3, 5) == 8), "B addition through X = 8");
                x->pVTbl->Release(x);
                x = 0;
            }
            y->pVTbl->Release(y);
            y = 0;
        }
    }
}

/* <summary> Demonstrate every transition on the two own components. </summary>
 * <description> Uses canonical Unknown as the reference identity of each object. </description> */
static void TestInterfaces(IEcoInterfaceBus1* bus) {
    const UGUID* cids[2];
    const UGUID* iids[5];
    char_t* names[5] = {"Unknown", "AdvancedMath", "X", "Y", "Lab2"};
    IEcoUnknown* object = 0;
    uint32_t i = 0;
    int16_t status = 0;
    cids[0] = &CID_EcoIntegralSimpson;
    cids[1] = &CID_EcoLab2;
    iids[0] = &IID_IEcoUnknown;
    iids[1] = &IID_IEcoAdvancedMath;
    iids[2] = &IID_IEcoCalculatorX;
    iids[3] = &IID_IEcoCalculatorY;
    iids[4] = &IID_IEcoLab2;
    Print("SCENARIO public interface transitions\n");
    for (i = 0; i < 2; i++) {
        Print(i == 0 ? "COMPONENT EcoIntegralSimpson\n" : "COMPONENT EcoLab2\n");
        status = bus->pVTbl->QueryComponent(bus, cids[i], 0, &IID_IEcoUnknown, (voidptr_t*)&object);
        Check((uint8_t)(status == 0 && object != 0), "create own component through Unknown");
        if (object != 0) {
            TestMatrix(object, iids, names, i == 0 ? 4 : 5);
            object->pVTbl->Release(object);
            object = 0;
        }
    }
}

/* <summary> Keep EcoLab2 alive through its integration interface. </summary>
 * <description> Only the inherited interface remains after releasing the initial Lab2 reference. </description> */
static void TestLifetime(IEcoInterfaceBus1* bus) {
    IEcoLab2* lab = 0;
    IEcoLab2* again = 0;
    IEcoAdvancedMath* math = 0;
    uint32_t remaining = 0;
    int16_t status = 0;
    Print("SCENARIO aggregate lifetime\n");
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoLab2, 0, &IID_IEcoLab2, (voidptr_t*)&lab);
    Check((uint8_t)(status == 0 && lab != 0), "create lifetime example");
    if (lab != 0) {
        status = lab->pVTbl->QueryInterface(lab, &IID_IEcoAdvancedMath, (voidptr_t*)&math);
        Check((uint8_t)(status == 0 && math != 0), "retain integration interface");
        lab->pVTbl->Release(lab);
        lab = 0;
        if (math != 0) {
            TestIntegral(math);
            status = math->pVTbl->QueryInterface(math, &IID_IEcoLab2, (voidptr_t*)&again);
            Check((uint8_t)(status == 0 && again != 0), "recover EcoLab2 from remaining interface");
            if (again != 0) {
                again->pVTbl->Release(again);
            }
            remaining = math->pVTbl->Release(math);
            Check((uint8_t)(remaining == 0), "last Release returns zero references");
        }
    }
}

/* <summary> Run five laboratory demonstrations using real Eco services. </summary>
 * <description> Releases every acquired interface before returning the combined result. </description> */
int16_t EcoMain(IEcoUnknown* system) {
    IEcoSystem1* sys = 0;
    IEcoInterfaceBus1* bus = 0;
    IEcoFileManager1* files = 0;
    int16_t status = ERR_ECO_FAIL;
    if (system == 0) {
        return ERR_ECO_POINTER;
    }
    if (system->pVTbl->QueryInterface(system, &GID_IEcoSystem, (voidptr_t*)&sys) != 0) {
        goto Release;
    }
    if (sys->pVTbl->QueryInterface(sys, &IID_IEcoInterfaceBus1, (voidptr_t*)&bus) != 0) {
        goto Release;
    }
    if (bus->pVTbl->QueryComponent(bus, &CID_EcoFileSystemManagement1, 0, &IID_IEcoFileManager1, (voidptr_t*)&files) != 0) {
        goto Release;
    }
    g_Output = files->pVTbl->Create(files, "/dev/stdout");
    if (g_Output == 0) {
        goto Release;
    }
    Print("EcoLab2 UnitTest\n");
    Check((uint8_t)(Lab2RegisterCalculators(bus) == 0), "register A-E");
    Check((uint8_t)(bus->pVTbl->RegisterComponent(bus, &CID_EcoIntegralSimpson,
        (IEcoUnknown*)GetIEcoComponentFactoryPtr_BA973067BA954E8FA50BDDE55BC4E3DF) == 0), "register integrator");
    Check((uint8_t)(bus->pVTbl->RegisterComponent(bus, &CID_EcoLab2,
        (IEcoUnknown*)GetIEcoComponentFactoryPtr_7C9B7CECBC69410F9B283F6647E653D2) == 0), "register EcoLab2");
    TestIntegrator(bus);
    TestLab2(bus);
    TestB(bus);
    TestInterfaces(bus);
    TestLifetime(bus);
    Print("MATRIX PAIRS ");
    PrintCount(g_Pairs);
    Print("\nSUMMARY ");
    Print(g_Failed == 0 ? "PASS passed=" : "FAIL passed=");
    PrintCount(g_Passed);
    Print(" failed=");
    PrintCount(g_Failed);
    Print("\n");
    status = g_Failed == 0 && g_OutputStatus == 0 ? ERR_ECO_SUCCESS : ERR_ECO_FAIL;
Release:
    if (g_Output != 0) {
        g_Output->pVTbl->Close(g_Output);
        g_Output->pVTbl->Release(g_Output);
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
