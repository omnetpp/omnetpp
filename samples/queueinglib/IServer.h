//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_ISERVER_H
#define __QUEUEING_ISERVER_H

#include "QueueingDefs.h"

namespace queueing {

/**
 * An interface that must be implemented by blocks that are capable
 * of processing a job. The block implementing IPassiveQueue is calling
 * these methods to communicate with the block to decide whether
 * a new job can be sent to a server
 */
class QUEUEING_API IServer
{
    public:
        virtual ~IServer() {}
        // indicates whether the service is currently idle (ie. no processing is pending or in progress currently)
        virtual bool isIdle() = 0;
        // notify the server that we want to allocate it for a job. after calling this we MUST send a job via the server's input gate
        virtual void allocate() = 0;
};

}; //namespace

#endif

