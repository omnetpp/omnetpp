//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_SOURCE_H
#define __QUEUEING_SOURCE_H

#include "QueueingDefs.h"

namespace queueing {

/**
 * Generates packets; see NED file for more info.
 */
class QUEUEING_API Source : public cSimpleModule
{
    private:
        cMessage *selfMsg;
        simtime_t startTime;       // when the module sends out the first job
        simtime_t stopTime;        // when the module stops the job generation (-1 means no limit)
        int numJobs;               // number of jobs to be generated (-1 means no limit)
        int jobCounter;            // number of jobs generated since start
        const char *jobName;       // the name of the generated job (if "" the job name should be
                                   // generated from the module name)

    protected:
        virtual ~Source();
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
};

}; //namespace

#endif
