//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "stresssource.h"
#include "stress_m.h"

Define_Module(StressSource);

StressSource::StressSource()
{
	timer = new cMessage();
}

StressSource::~StressSource()
{
	cancelAndDelete(timer);
}

void StressSource::initialize()
{
	// TEST: self message
	scheduleAt(par("serviceTime"), timer);
}

void StressSource::handleMessage(cMessage *msg)
{
	if (msg == timer) {
		send(new StressPacket(), "out", intrand(gateSize("out")));
		// TEST: reusing self message
		scheduleAt(simTime() + par("serviceTime"), timer);
	}
}
