//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "stressduplicate.h"
#include "stress_m.h"

Define_Module(StressDuplicate);

void StressDuplicate::handleMessage(cMessage *msg)
{
	send(msg, "out", intrand(gateSize("out")));

	// TEST: dup message
	for (int i = par("numberOfDuplicates").longValue(); i >= 0; i--)
		send(msg->dup(), "out", intrand(gateSize("out")));
}
