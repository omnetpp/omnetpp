//=========================================================================
//  CPARSIMPROTOCOLBASE.CC - part of
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

#include "omnetpp/cmessage.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cparsimcomm.h"
#include "omnetpp/ccommbuffer.h"
#include "omnetpp/csimplemodule.h" // SendOptions
#include "cparsimpartition.h"
#include "messagetags.h"
#include "cparsimprotocolbase.h"

namespace omnetpp {

SendOptions cParsimProtocolBase::unpackOptions(cCommBuffer *buffer)
{
    SendOptions options;
    buffer->unpack(options.sendDelay);
    buffer->unpack(options.propagationDelay_);
    buffer->unpack(options.duration_);
    buffer->unpack(options.transmissionId_);
    buffer->unpack(options.remainingDuration);
    return options;
}

void cParsimProtocolBase::packOptions(cCommBuffer *buffer, const SendOptions& options)
{
    buffer->pack(options.sendDelay);
    buffer->pack(options.propagationDelay_);
    buffer->pack(options.duration_);
    buffer->pack(options.transmissionId_);
    buffer->pack(options.remainingDuration);
}

void cParsimProtocolBase::processOutgoingMessage(cMessage *msg, const SendOptions& options, int destProcId, int destModuleId, int destGateId, void *)
{
    cCommBuffer *buffer = comm->createCommBuffer();

    buffer->pack(destModuleId);
    buffer->pack(destGateId);
    packOptions(buffer, options);
    buffer->packObject(msg);
    comm->send(buffer, TAG_CMESSAGE, destProcId);

    comm->recycleCommBuffer(buffer);
}

void cParsimProtocolBase::processReceivedBuffer(cCommBuffer *buffer, int tag, int sourceProcId)
{
    switch (tag) {
        case TAG_CMESSAGE: {
            int destModuleId;
            int destGateId;
            buffer->unpack(destModuleId);
            buffer->unpack(destGateId);
            SendOptions options = unpackOptions(buffer);
            cMessage *msg = (cMessage *)buffer->unpackObject();
            processReceivedMessage(msg, options, destModuleId, destGateId, sourceProcId);
            break;
        }

        default: {
            partition->processReceivedBuffer(buffer, tag, sourceProcId);
            break;
        }
    }
    buffer->assertBufferEmpty();
}

void cParsimProtocolBase::processReceivedMessage(cMessage *msg, const SendOptions& options, int destModuleId, int destGateId, int sourceProcId)
{
    partition->processReceivedMessage(msg, options, destModuleId, destGateId, sourceProcId);
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

