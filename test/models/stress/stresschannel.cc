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

cChannel::Result StressChannel::processMessage(cMessage *msg, const SendOptions& options, simtime_t t)
{
    // drop if ongoing transmission
    if (getTransmissionFinishTime() > t) {
        EV << "Deleting message in channel due to ongoing transmission: " << msg << "\n";
        Result result;
        result.discard = true;
        return result;
    }

    return cDatarateChannel::processMessage(msg, options, t);
}

