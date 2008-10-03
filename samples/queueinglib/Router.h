//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __ROUTER_H
#define __ROUTER_H

#include <omnetpp.h>

namespace queueing {

// routing algorithms
enum {
     ALG_RANDOM,
     ALG_ROUND_ROBIN,
     ALG_MIN_QUEUE_LENGTH,
     ALG_MIN_DELAY,
     ALG_MIN_SERVICE_TIME
};

/**
 * Sends the messages to different outputs depending on a set algorithm.
 */
class Router : public cSimpleModule
{
    private:
        int routingAlgorithm;  // the algorithm we are using for routing
        int rrCounter;         // msgCounter for round robin routing
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

}; //namespace

#endif
