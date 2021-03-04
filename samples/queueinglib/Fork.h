//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
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
class QUEUEING_API Fork : public cSimpleModule
{
  protected:
    virtual void handleMessage(cMessage *msg) override;
    virtual ~Fork();
};

}; // namespace

#endif
