//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_SERVER_H
#define __QUEUEING_SERVER_H

#include "IServer.h"

namespace queueing {

class Job;
class SelectionStrategy;

/**
 * The queue server. It cooperates with several Queues that which queue up
 * the jobs, and send them to Server on request.
 *
 * @see PassiveQueue
 */
class QUEUEING_API Server : public cSimpleModule, public IServer
{
    private:
        simsignal_t busySignal;

        SelectionStrategy *selectionStrategy;

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


