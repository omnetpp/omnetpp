//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_SPLIT_H
#define __QUEUEING_SPLIT_H

#include "QueueingDefs.h"

namespace queueing {

/**
 * Forks a job. See the NED file for more info.
 */
class QUEUEING_API Split : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

}; // namespace

#endif
