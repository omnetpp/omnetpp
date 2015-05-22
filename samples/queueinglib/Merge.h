//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_MERGE_H
#define __QUEUEING_MERGE_H

#include "QueueingDefs.h"

namespace queueing {

/**
 * All messages received on any input gate will be sent out on the output gate
 */
class QUEUEING_API Merge : public cSimpleModule
{
    protected:
        virtual void handleMessage(cMessage *msg) override;
};

}; //namespace

#endif

