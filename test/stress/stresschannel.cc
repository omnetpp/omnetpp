//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "stresschannel.h"
#include "stress_m.h"

Register_Class(StressChannel);

StressChannel::StressChannel()
{
	transmittingUntil = 0;
}

bool StressChannel::deliver(cMessage *msg, simtime_t t)
{
    // drop if ongoing transmission
	if (transmittingUntil > t) {
		ev << "Deleting message in channel due to ongoing transmission: " << msg << "\n";;
		return false;
	}

    // datarate modeling
    simtime_t transmissionDelay = msg->length() / par("dataRate").doubleValue();
    t += transmissionDelay;
	transmittingUntil = t;

    // propagation delay modeling
    simtime_t delay = par("delay").doubleValue();
    t += delay;

	// TODO: this should be factored out to base class
    EVCB.messageSendHop(msg, getFromGate(), delay, transmissionDelay);

    // hand over msg to next gate
    return getFromGate()->getToGate()->deliver(msg, t);
}
