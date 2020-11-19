//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_IPASSIVEQUEUE_H
#define __QUEUEING_IPASSIVEQUEUE_H

#include "QueueingDefs.h"

namespace queueing {

/**
 * The following interface must be implemented by a queue which can't process
 * jobs on its own. A server process uses these methods to query for new jobs
 * once it becomes idle.
 */
class QUEUEING_API IPassiveQueue
{
    public:
        virtual ~IPassiveQueue() {}
        // the current length of the queue
        virtual int length() = 0;
        // requests the queue to send out the next job on its "gateIndex" gate.
        virtual void request(int gateIndex) = 0;
};

}; //namespace

#endif

