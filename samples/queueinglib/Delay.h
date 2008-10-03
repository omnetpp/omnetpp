//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
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
class Delay : public cSimpleModule
{
    private:
        cOutVector sizeVector;
        int currentlyStored;
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

}; //namespace

#endif

