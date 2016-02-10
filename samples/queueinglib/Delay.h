//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_DELAY_H
#define __QUEUEING_DELAY_H

#include "QueueingDefs.h"

namespace queueing {

/**
 * Delays the incoming messages
 */
class QUEUEING_API Delay : public cSimpleModule
{
    private:
		simsignal_t delayedJobsSignal;
        int currentlyStored;
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void refreshDisplay() const override;
};

}; //namespace

#endif

