//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <stdio.h>
#include "Fork.h"
#include "Job.h"

namespace queueing {

Define_Module(Fork);

void Fork::initialize()
{
    changeMsgNames = par("changeMsgNames");
}

void Fork::handleMessage(cMessage *msg)
{
    Job *job = check_and_cast<Job *>(msg);

    // increment the generation counter
    job->setGeneration(job->getGeneration()+1);

    // if there are more than one output gates, send a duplicate on each output gate (index>0)
    for (int i = 1; i < gateSize("out"); ++i)
    {
        cMessage *dupMsg = msg->dup();
        if (changeMsgNames)
        {
            char buff[80];
            sprintf(buff, "%.60s.%d", msg->getName(), i);
            dupMsg->setName(buff);
        }
        send(dupMsg, "out", i);
    }

    // send out the original message on out[0]
    if (changeMsgNames)
    {
        char buff[80];
        sprintf(buff, "%.60s.0", msg->getName());
        msg->setName(buff);
    }
    send(msg, "out", 0);
}

}; //namespace

