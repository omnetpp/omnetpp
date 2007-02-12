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
	ev << "TEST: Cancelling and deleting self message: "  << timer << "\n";;
	cancelAndDelete(timer);
}

void StressSource::initialize()
{
	ev << "TEST: Sending self message for the first time: "  << timer << "\n";;
	scheduleAt(par("serviceTime"), timer);
}

void StressSource::handleMessage(cMessage *msg)
{
	if (msg == timer)
		msg = new StressPacket();
	else {
		ev << "TEST: Cancelling self message: " << timer << "\n";;
		cancelEvent(timer);
	}

	send(msg, "out", intrand(gateSize("out")));

	ev << "TEST: Reusing self message: " << timer << "\n";
	scheduleAt(simTime() + par("serviceTime"), timer);
}
