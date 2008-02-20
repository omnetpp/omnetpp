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

class StressChannel : public cChannel
{
	protected:
		simtime_t transmittingUntil;

	public:
		StressChannel();

    protected:
	    virtual simtime_t transmissionFinishes() const { return transmittingUntil; }
    	virtual bool deliver(cMessage *msg, simtime_t t);
};

#endif
