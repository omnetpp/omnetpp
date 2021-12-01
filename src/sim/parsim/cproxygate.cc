//=========================================================================
//  CPROXYGATE.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
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
}

std::string cProxyGate::str() const
{
    std::stringstream out;
    out << "remote:(procId=" << remoteProcId << ",modId=" << remoteModuleId << ",gateId=" << remoteGateId << ") ";
    out << cGate::str();
    return out.str();
}

bool cProxyGate::deliver(cMessage *msg, const SendOptions& options, simtime_t t)
{
    ASSERT(nextGate == nullptr);
    ASSERT(partition != nullptr);
    if (remoteProcId == -1)
        throw cRuntimeError(this, "Cannot deliver message '%s': Not connected to remote gate", msg->getName());

    msg->setArrivalTime(t);  // merge arrival time into message
    partition->processOutgoingMessage(msg, options, remoteProcId, remoteModuleId, remoteGateId, data);
    return false;  // meaning message should be deleted
}

void cProxyGate::setRemoteGate(short procId, int moduleId, int gateId)
{
    remoteProcId = procId;
    remoteModuleId = moduleId;
    remoteGateId = gateId;
}

}  // namespace omnetpp

