//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __STRESSCHANNEL_H
#define __STRESSCHANNEL_H

#include <omnetpp.h>

using namespace omnetpp;

class StressChannel : public cDatarateChannel
{
    public:
        StressChannel();

    protected:
        virtual Result processMessage(cMessage *msg, const SendOptions& options, simtime_t t) override;
};

#endif
