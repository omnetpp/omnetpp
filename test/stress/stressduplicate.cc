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
	for (int i = par("numberOfDuplicates"); i >= 0; i--) {
		ev << "Sending out duplicated message: "  << msg << "\n";;
		send(msg->dup(), "out", intrand(gateSize("out")));
	}

	ev << "Sending out original message: "  << msg << "\n";;
	send(msg, "out", intrand(gateSize("out")));
}
