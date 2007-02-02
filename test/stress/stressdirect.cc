//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2005 Andras Varga
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
	selectedModule = NULL;
	numberOfVisitedModules = 0;
}

void RandomModuleSelector::visit(cObject *object)
{
	cSimpleModule *simpleModule = dynamic_cast<cSimpleModule *>(object);
	cCompoundModule *compoundModule = dynamic_cast<cCompoundModule *>(object);
	
	if (simpleModule && simpleModule->hasGate("directIn")) {
		numberOfVisitedModules++;

		// this will result in a uniform distribution between modules		
		if (uniform(0, 1) <= 1.0 / numberOfVisitedModules)
			selectedModule = simpleModule;
	}
	else if (compoundModule)
		compoundModule->forEachChild(this);
}

cModule* StressDirect::getRandomModule()
{
	RandomModuleSelector selector;
	simulation.systemModule()->forEachChild(&selector);
	return selector.selectedModule;
}

void StressDirect::handleMessage(cMessage *msg)
{
	// TEST: send direct
	cModule *randomModule = getRandomModule();
	sendDirect(msg,
	           par("propagationDelay").doubleValue(),
	           par("transmissionDelay").doubleValue(), 
			   randomModule, 
			   "directIn");
}
