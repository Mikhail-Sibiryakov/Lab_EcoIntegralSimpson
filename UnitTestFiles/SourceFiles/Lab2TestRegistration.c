/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Register educational dependencies. </summary>
 * <description> Uses only public SharedFiles and real factories. </description>
 * <reference> docs/lab2/REFERENCES.md </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#include "Lab2TestRegistration.h"
#include "IdEcoCalculatorA.h"
#include "IdEcoCalculatorB.h"
#include "IdEcoCalculatorCCompat.h"
#include "IdEcoCalculatorD.h"
#include "IdEcoCalculatorE.h"

/* <summary> Register all A-E factories. </summary>
 * <description> Stops on failure; registration ownership belongs to the bus. </description> */
int16_t Lab2RegisterCalculators(IEcoInterfaceBus1* bus) {
    int16_t status = ERR_ECO_POINTER;
    if (bus == 0) {
        return status;
    }
    status = bus->pVTbl->RegisterComponent(bus, &CID_EcoCalculatorA, (IEcoUnknown*)GetIEcoComponentFactoryPtr_4828F6552E4540E78121EBD220DC360E);
    if (status == 0) {
        status = bus->pVTbl->RegisterComponent(bus, &CID_EcoCalculatorB, (IEcoUnknown*)GetIEcoComponentFactoryPtr_AE202E543CE54550899603BD70C62565);
    }
    if (status == 0) {
        status = bus->pVTbl->RegisterComponent(bus, &CID_EcoCalculatorC_Lab2, (IEcoUnknown*)GetIEcoComponentFactoryPtr_A77A8179F52342DCA168E9A2FB42C673);
    }
    if (status == 0) {
        status = bus->pVTbl->RegisterComponent(bus, &CID_EcoCalculatorD, (IEcoUnknown*)GetIEcoComponentFactoryPtr_3A8E44677E82475CB4A3719ED8397E61);
    }
    if (status == 0) {
        status = bus->pVTbl->RegisterComponent(bus, &CID_EcoCalculatorE, (IEcoUnknown*)GetIEcoComponentFactoryPtr_872FEF1DE3314B87AD44D1E7C232C2F0);
    }
    return status;
}
