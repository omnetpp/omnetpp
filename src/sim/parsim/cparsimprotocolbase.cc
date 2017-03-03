//=========================================================================
//  CPARSIMPROTOCOLBASE.CC - part of
//
//                  OMNeT++/OMNEST
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

#include "omnetpp/cmessage.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cparsimcomm.h"
#include "omnetpp/ccommbuffer.h"
#include "cparsimpartition.h"
#include "messagetags.h"
#include "cparsimprotocolbase.h"

namespace omnetpp {

cParsimProtocolBase::cParsimProtocolBase() : cParsimSynchronizer()
{
}

cParsimProtocolBase::~cParsimProtocolBase()
{
}

void cParsimProtocolBase::processOutgoingMessage(cMessage *msg, int destProcId, int destModuleId, int destGateId, void *)
{
    cCommBuffer *buffer = comm->createCommBuffer();

    buffer->pack(destModuleId);
    buffer->pack(destGateId);
    buffer->packObject(msg);
    comm->send(buffer, TAG_CMESSAGE, destProcId);

    comm->recycleCommBuffer(buffer);
}

void cParsimProtocolBase::processReceivedBuffer(cCommBuffer *buffer, int tag, int sourceProcId)
{
    int destModuleId;
    int destGateId;
    cMessage *msg;

    switch (tag) {
        case TAG_CMESSAGE:
            buffer->unpack(destModuleId);
            buffer->unpack(destGateId);
            msg = (cMessage *)buffer->unpackObject();
            processReceivedMessage(msg, destModuleId, destGateId, sourceProcId);
            break;

        default:
            partition->processReceivedBuffer(buffer, tag, sourceProcId);
            break;
    }
    buffer->assertBufferEmpty();
}

void cParsimProtocolBase::processReceivedMessage(cMessage *msg, int destModuleId, int destGateId, int sourceProcId)
{
    partition->processReceivedMessage(msg, destModuleId, destGateId, sourceProcId);
}

void cParsimProtocolBase::receiveNonblocking()
{
    int tag, sourceProcId;
    cCommBuffer *buffer = comm->createCommBuffer();
    while (comm->receiveNonblocking(PARSIM_ANY_TAG, buffer, tag, sourceProcId))
        processReceivedBuffer(buffer, tag, sourceProcId);
    comm->recycleCommBuffer(buffer);
}

bool cParsimProtocolBase::receiveBlocking()
{
    cCommBuffer *buffer = comm->createCommBuffer();

    int tag, sourceProcId;
    if (!comm->receiveBlocking(PARSIM_ANY_TAG, buffer, tag, sourceProcId)) {
        comm->recycleCommBuffer(buffer);
        return false;
    }

    processReceivedBuffer(buffer, tag, sourceProcId);
    while (comm->receiveNonblocking(PARSIM_ANY_TAG, buffer, tag, sourceProcId))
        processReceivedBuffer(buffer, tag, sourceProcId);

    comm->recycleCommBuffer(buffer);
    return true;
}

}  // namespace omnetpp

