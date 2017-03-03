//=========================================================================
//  CPROXYGATE.CC - part of
//
//                   OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, 2003
//          Dept. of Electrical and Computer Systems Engineering,
//          Monash University, Melbourne, Australia
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>
#include "omnetpp/cmodule.h"
#include "omnetpp/cmessage.h"
#include "cproxygate.h"
#include "cparsimpartition.h"

namespace omnetpp {

cProxyGate::cProxyGate() : cGate()
{
    partition = nullptr;
    remoteProcId = -1;
    remoteModuleId = -1;
    remoteGateId = -1;
    data = nullptr;
}

std::string cProxyGate::str() const
{
    std::stringstream out;
    out << "remote:(procId=" << remoteProcId << ",modId=" << remoteModuleId << ",gateId=" << remoteGateId << ") ";
    out << cGate::str();
    return out.str();
}

bool cProxyGate::deliver(cMessage *msg, simtime_t t)
{
    ASSERT(nextGate == nullptr);
    ASSERT(partition != nullptr);
    if (remoteProcId == -1)
        throw cRuntimeError(this, "Cannot deliver message '%s': Not connected to remote gate", msg->getName());

    msg->setArrivalTime(t);  // merge arrival time into message
    partition->processOutgoingMessage(msg, remoteProcId, remoteModuleId, remoteGateId, data);
    return false;  // meaning message should be deleted
}

void cProxyGate::setRemoteGate(short procId, int moduleId, int gateId)
{
    remoteProcId = procId;
    remoteModuleId = moduleId;
    remoteGateId = gateId;
}

}  // namespace omnetpp

