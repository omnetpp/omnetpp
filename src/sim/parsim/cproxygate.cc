//=========================================================================
//  CPROXYGATE.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2005 Andras Varga
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cproxygate.h"
#include "cparsimpartition.h"
#include "cmodule.h"
#include "cmessage.h"


cProxyGate::cProxyGate(const char *name, char type) : cGate(name, type)
{
    partition = NULL;
    remoteProcId = -1;
    remoteModuleId = -1;
    remoteGateId = -1;
    data = NULL;
}

cProxyGate::cProxyGate(const cProxyGate& gate) : cGate(NULL, 0)
{
    setName(gate.name());
    operator=(gate);
}

cProxyGate& cProxyGate::operator=(const cProxyGate& gate)
{
    if (this == &gate)
        return *this;

    cGate::operator = (gate);
    partition = gate.partition;
    remoteProcId = gate.remoteProcId;
    remoteModuleId = gate.remoteModuleId;
    remoteGateId = gate.remoteGateId;
    return *this;
}

std::string cProxyGate::info() const
{
    std::stringstream out;
    out << "remote:(procId=" << remoteProcId << ",modId=" << remoteModuleId << ",gateId=" << remoteGateId << ") ",
    out << cGate::info();
    return out.str();
}

bool cProxyGate::deliver(cMessage *msg, simtime_t t)
{
    ASSERT(togatep==NULL);
    ASSERT(partition!=NULL);
    if (remoteProcId==-1)
        throw new cRuntimeError(this, "cannot deliver message '%s': not connected to remote gate", msg->name());

    msg->setArrivalTime(t);  // merge arrival time into message
    partition->processOutgoingMessage(msg, remoteProcId, remoteModuleId, remoteGateId, data);
    return false;  // meaning message should be deleted
}

void cProxyGate::setRemoteGate(int procId, int moduleId, int gateId)
{
    remoteProcId = procId;
    remoteModuleId = moduleId;
    remoteGateId = gateId;
}


