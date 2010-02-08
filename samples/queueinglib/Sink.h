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
class QUEUEING_API Sink : public cSimpleModule
{
  private:
	simsignal_t lifeTimeSignal;
	simsignal_t totalQueueingTimeSignal;
	simsignal_t queuesVisitedSignal;
	simsignal_t totalServiceTimeSignal;
	simsignal_t totalDelayTimeSignal;
	simsignal_t delaysVisitedSignal;
	simsignal_t generationSignal;
    bool keepJobs;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

}; //namespace

#endif

