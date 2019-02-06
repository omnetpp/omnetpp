//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __STRESSQUEUE_H
#define __STRESSQUEUE_H

#include <omnetpp.h>

using namespace omnetpp;

class StressQueue : public cSimpleModule
{
    protected:
        cMessage *timer;
        cQueue queue;

    public:
        StressQueue();
        virtual ~StressQueue();

    protected:
        void handleMessage(cMessage *msg);
};

#endif
