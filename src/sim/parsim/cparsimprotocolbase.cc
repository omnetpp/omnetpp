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

bool cParsimProtocolBase::processOutgoingMessage(cMessage *msg, const SendOptions& options, int destPartitionId, int destModuleId, int destGateId, void *)
{
    cCommBuffer *buffer = comm->createCommBuffer();

    buffer->pack(destModuleId);
    buffer->pack(destGateId);
    packOptions(buffer, options);
    buffer->packObject(msg);
    comm->send(buffer, TAG_CMESSAGE, destPartitionId);

    comm->recycleCommBuffer(buffer);
    return false; // caller should delete msg
}

void cParsimProtocolBase::processReceivedBuffer(cCommBuffer *buffer, int tag, int sourcePartitionId)
{
    switch (tag) {
        case TAG_CMESSAGE: {
            int destModuleId;
            int destGateId;
            buffer->unpack(destModuleId);
            buffer->unpack(destGateId);
            SendOptions options = unpackOptions(buffer);
            cMessage *msg = (cMessage *)buffer->unpackObject();
            processReceivedMessage(msg, options, destModuleId, destGateId, sourcePartitionId);
            break;
        }

        default: {
            partition->processReceivedBuffer(buffer, tag, sourcePartitionId);
            break;
        }
    }
    buffer->assertBufferEmpty();
}

void cParsimProtocolBase::processReceivedMessage(cMessage *msg, const SendOptions& options, int destModuleId, int destGateId, int sourcePartitionId)
{
    partition->processReceivedMessage(msg, options, destModuleId, destGateId, sourcePartitionId);
}

void cParsimProtocolBase::receiveNonblocking()
{
    int tag, sourcePartitionId;
    cCommBuffer *buffer = comm->createCommBuffer();
    while (comm->receiveNonblocking(PARSIM_ANY_TAG, buffer, tag, sourcePartitionId))
        processReceivedBuffer(buffer, tag, sourcePartitionId);
    comm->recycleCommBuffer(buffer);
}

bool cParsimProtocolBase::receiveBlocking()
{
    cCommBuffer *buffer = comm->createCommBuffer();

    int tag, sourcePartitionId;
    if (!comm->receiveBlocking(PARSIM_ANY_TAG, buffer, tag, sourcePartitionId)) {
        comm->recycleCommBuffer(buffer);
        return false;
    }

    processReceivedBuffer(buffer, tag, sourcePartitionId);
    while (comm->receiveNonblocking(PARSIM_ANY_TAG, buffer, tag, sourcePartitionId))
        processReceivedBuffer(buffer, tag, sourcePartitionId);

    comm->recycleCommBuffer(buffer);
    return true;
}

}  // namespace omnetpp

