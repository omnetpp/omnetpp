//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __SERVER_H
#define __SERVER_H

#include <omnetpp.h>
#include "SelectionStrategies.h"

namespace queueing {

class Job;
class SelectionStrategy;

/**
 * An interface that must be implemented by blocks that are capable
 * of processing a job. The block implementing IPassiveQueue is calling
 * these methods to communicate with the block to decide whether
 * a new job can be sent to a server
 */
class IServer
{
    public:
        virtual ~IServer() { };
        // indicate whether the service is currently idle (ie. no processing is done currently)
        virtual bool isIdle() = 0;
};

/**
 * The queue server. It cooperates with several Queues that which queue up
 * the jobs, and send them to Server on request.
 *
 * @see PassiveQueue
 */
class Server : public cSimpleModule, public IServer
{
    private:
        cWeightedStdDev scalarUtilizationStats;

        int numQueues;
        SelectionStrategy *selectionStrategy;

        simtime_t prevEventTimeStamp;
        Job *jobServiced;
        cMessage *endServiceMsg;

    public:
        Server();
        virtual ~Server();

    protected:
        virtual void initialize();
        virtual int numInitStages() const {return 2;}
        virtual void handleMessage(cMessage *msg);
        virtual void finish();

    public:
        virtual bool isIdle();
};

}; //namespace

#endif


