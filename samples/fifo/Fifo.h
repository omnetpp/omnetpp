//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#ifndef __FIFO_H
#define __FIFO_H

#include "AbstractFifo.h"

namespace fifo {

/**
 * Single-server queue with given service time.
 */
class PacketFifo : public AbstractFifo
{
  protected:
    virtual simtime_t startService(cMessage *msg);
    virtual void endService(cMessage *msg);
};

/**
 * Single-server queue with service time based on message length.
 */
class BitFifo : public AbstractFifo
{
  protected:
    virtual simtime_t startService(cMessage *msg);
    virtual void endService(cMessage *msg);
};

}; //namespace

#endif
