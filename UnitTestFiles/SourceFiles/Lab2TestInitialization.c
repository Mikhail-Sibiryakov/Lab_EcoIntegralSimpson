/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Initialize the static laboratory 2 application. </summary>
 * <description> Keeps component factory selection outside the test client. </description>
 * <reference> docs/lab2/DESIGN.md </reference>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */
#include "Lab2TestRegistration.h"
#include "IdEcoIntegralSimpson.h"
#include "IdEcoLab2.h"

/* <summary> Register the dependencies and both own component factories. </summary>
 * <description> Stops on failure; the interface bus owns successful registrations. </description> */
int16_t Lab2InitializeComponents(IEcoInterfaceBus1* bus) {
    int16_t status = ERR_ECO_POINTER;
    if (bus == 0) {
        return status;
    }
    status = Lab2RegisterCalculators(bus);
    if (status == 0) {
        status = bus->pVTbl->RegisterComponent(bus, &CID_EcoIntegralSimpson,
            (IEcoUnknown*)GetIEcoComponentFactoryPtr_BA973067BA954E8FA50BDDE55BC4E3DF);
    }
    if (status == 0) {
        status = bus->pVTbl->RegisterComponent(bus, &CID_EcoLab2,
            (IEcoUnknown*)GetIEcoComponentFactoryPtr_7C9B7CECBC69410F9B283F6647E653D2);
    }
    return status;
}
