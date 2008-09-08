//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __STRESSCHANNEL_H
#define __STRESSCHANNEL_H

#include <omnetpp.h>

class StressChannel : public cDatarateChannel
{
	public:
		StressChannel();

    protected:
        virtual bool deliver(cMessage *msg, simtime_t at);
};

#endif
