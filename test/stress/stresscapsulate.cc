//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "stresscapsulate.h"
#include "stress_m.h"

Define_Module(StressCapsulate);

void StressCapsulate::handleMessage(cMessage *msg)
{
	if (msg->encapsulatedMsg() && uniform(0, 1) < par("decapsulateProbability").doubleValue()) {
		ev << "TEST: Decapsulating message: "  << msg << "\n";;
		cMessage *decapsulatedMsg = msg->decapsulate();
		delete msg;
		msg = decapsulatedMsg;
	}
	
	if (uniform(0, 1) < par("encapsulateProbability").doubleValue()) {
		ev << "TEST: Encapsulating message: "  << msg << "\n";;
		StressPacket *encapsulatedMsg = new StressPacket();
	   	encapsulatedMsg->encapsulate(msg);
	   	msg = encapsulatedMsg;
	}
	
	send(msg, "out", intrand(gateSize("out")));
}
