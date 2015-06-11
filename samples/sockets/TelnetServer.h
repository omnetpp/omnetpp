//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __TELNETSERVER_H
#define __TELNETSERVER_H

#include "QueueBase.h"

/**
 * Simple model of a Telnet server.
 */
class TelnetServer : public QueueBase
{
  protected:
    virtual simtime_t startService(cMessage *msg) override;
    virtual void endService(cMessage *msg) override;
    virtual std::string processChars(const char *chars);
};

#endif

