//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "stressdirect.h"
#include "stress_m.h"

Define_Module(StressDirect);

RandomModuleSelector::RandomModuleSelector()
{
    selectedModule = nullptr;
    numberOfVisitedModules = 0;
}

bool RandomModuleSelector::visit(cObject *object)
{
    cModule *module = dynamic_cast<cModule *>(object);

    if (module && module->hasGate("directIn")) {
        numberOfVisitedModules++;

        // this will result in a uniform distribution between modules
        if (module->uniform(0, 1) <= 1.0 / numberOfVisitedModules)
            selectedModule = module;
    }
    else if (module->hasSubmodules())
        module->forEachChild(this);
    return true;
}

cModule *StressDirect::getRandomModule()
{
    RandomModuleSelector selector;
    getSimulation()->getSystemModule()->forEachChild(&selector);
    return selector.selectedModule;
}

void StressDirect::handleMessage(cMessage *msg)
{
    EV << "Sending direct message: "  << msg << "\n";
    ;
    cModule *randomModule = getRandomModule();
    msg->setName("Direct");
    sendDirect(msg,
            par("propagationDelay").doubleValue(),
            par("transmissionDelay").doubleValue(),
            randomModule,
            "directIn");
}

