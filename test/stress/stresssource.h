//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __STRESSSOURCE_H
#define __STRESSSOURCE_H

#include <omnetpp.h>

using namespace omnetpp;

class StressSource : public cSimpleModule
{
    protected:
        cMessage *timer;

    public:
        StressSource();
        virtual ~StressSource();

    protected:
        void initialize();
        void handleMessage(cMessage *msg);
        void sendOut(cMessage *msg);
        cMessage *generateMessage();
};

#endif
