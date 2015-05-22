//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#ifndef __FIFO_H
#define __FIFO_H

#include "AbstractFifo.h"

namespace fifo {

/**
 * Single-server queue with a given service time.
 */
class Fifo : public AbstractFifo
{
  protected:
    virtual simtime_t startService(cMessage *msg) override;
    virtual void endService(cMessage *msg) override;
};

}; //namespace

#endif
