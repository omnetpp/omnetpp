//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2015 Andras Varga
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
}

bool StressChannel::deliver(cMessage *msg, simtime_t at)
{
    // drop if ongoing transmission
    if (getTransmissionFinishTime() > at) {
        EV << "Deleting message in channel due to ongoing transmission: " << msg << "\n";
        return false;
    }
    else {
        result_t result;
        cDatarateChannel::processMessage(msg, at, result);
        return result.discard;
    }
}

