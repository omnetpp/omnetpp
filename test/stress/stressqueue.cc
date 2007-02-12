//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "stressqueue.h"
#include "stress_m.h"

Define_Module(StressQueue);

StressQueue::StressQueue()
{
	timer = new cMessage();
}

StressQueue::~StressQueue()
{
	ev << "TEST: Cancelling and deleting self message: "  << timer << "\n";;
	cancelAndDelete(timer);
}

void StressQueue::handleMessage(cMessage *msg)
{
	if (msg == timer) {
		msg = (cMessage*)queue.pop();
		ev << "TEST: Sending out queued message: "  << msg << "\n";;
		send(msg, "out", intrand(gateSize("out")));

        if (!queue.empty())
			scheduleAt(simTime() + par("serviceTime"), timer);
	}
	else {
		ev << "TEST: Queuing message: "  << msg << "\n";;
		queue.insert(msg);

		if (!timer->isScheduled())
			scheduleAt(simTime() + par("serviceTime"), timer);
	}
}
