/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Register the integration component. </summary>
 * <description> Laboratory 3: two sources and two shared event receivers. </description>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */

#include "Lab3TestInitialization.h"
#include "Lab2TestRegistration.h"
#include "IdEcoIntegralSimpson.h"

/* <summary> Register existing dependencies and the integrator. </summary>
 * <description> Factory pointers belong to initialization, not the event client. </description> */
int16_t Lab3InitializeComponents(IEcoInterfaceBus1* bus) {
    int16_t status = ERR_ECO_POINTER;
    if (bus == 0) {
        return status;
    }
    status = Lab2RegisterCalculators(bus);
    if (status == 0) {
        status = bus->pVTbl->RegisterComponent(bus, &CID_EcoIntegralSimpson,
            (IEcoUnknown*)GetIEcoComponentFactoryPtr_BA973067BA954E8FA50BDDE55BC4E3DF);
    }
    return status;
}
