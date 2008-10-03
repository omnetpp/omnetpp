//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_SINK_H
#define __QUEUEING_SINK_H

#include "QueueingDefs.h"

namespace queueing {

/**
 * Consumes jobs; see NED file for more info.
 */
class Sink : public cSimpleModule
{
  private:
    cOutVector lifeTimeVector;
    cOutVector queueingTimeVector;
    cOutVector queueVector;
    cOutVector serviceTimeVector;
    cOutVector delayTimeVector;
    cOutVector delayVector;
    cOutVector generationVector;
    bool keepJobs;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

}; //namespace

#endif

