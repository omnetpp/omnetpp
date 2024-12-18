//=========================================================================
//  CPARSIMCOMM.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __OMNETPP_CPARSIMCOMM_H
#define __OMNETPP_CPARSIMCOMM_H

#include <map>
#include "cobject.h"
#include "cvalue.h"

namespace omnetpp {

class cCommBuffer;
class cConfiguration;
class cSimulation;
class cMessage;

#define PARSIM_ANY_TAG  -1


/**
 * @brief Basic communications layer for parallel simulation. It provides an
 * abstraction layer above MPI, PVM, shared-memory communications, etc.
 *
 * This is an interface: functionality is added by specific subclasses
 * for MPI, PVM, etc.
 *
 * @ingroup ParsimBrief
 * @ingroup Parsim
 */
class SIM_API cParsimCommunications : public cObject
{
  public:
    /**
     * Virtual destructor.
     */
    virtual ~cParsimCommunications() {}

    /** @name Lifecycle of communications library */
    //@{
    /**
     * Initialize the communications library.
     */
    virtual void configure(cSimulation *simulation, cConfiguration *cfg, int numPartitions=-1, int partitionId=-1, const std::map<std::string,cValue>& extraData={}) = 0;

    /**
     * Shutdown the communications library.
     */
    virtual void shutdown() = 0;
    //@}

    //@{
    /**
     * Returns total number of partitions.
     */
    virtual int getNumPartitions() const = 0;

    /**
     * Returns id of this partition, an integer in the range 0..getNumPartitions()-1.
     */
    virtual int getPartitionId() const = 0;

    /** @name Buffers, send, receive */
    //@{
    /**
     * Whether messages need to be serialized to be transferred, or sending the pointer also works.
     */
    virtual bool supportsTransferringPointers() = 0;

    /**
     * Packs a message object into the buffer. Returns a "keepit" flag, which
     * will be used as the value returned from cProxyGate::deliver();
     * see cGate::deliver() for more info.
     */
    virtual bool packMessage(cCommBuffer *buffer, cMessage *msg, int destPartitionId) = 0;

    /**
     * Unpacks a message object from the buffer.
     */
    virtual cMessage *unpackMessage(cCommBuffer *buffer) = 0;

    /**
     * Creates an empty buffer that can be used to send/receive data
     */
    virtual cCommBuffer *createCommBuffer() = 0;

    /**
     * Recycle communication buffer after use. May be used to implement pooling, etc.
     */
    virtual void recycleCommBuffer(cCommBuffer *buffer) = 0;

    /**
     * Sends packed data with given tag to destination. IMPORTANT: send() is not
     * guaranteed to preserve the buffer, i.e. it may be a destructive operation!
     * Some subclasses may move the content out of the buffer, or replace it with
     * arbitrary content.
     */
    virtual void send(cCommBuffer *buffer, int tag, int destination) = 0;

    /**
     * Broadcasts packed data with given tag to all partitions. This default
     * implementation just simply sends the data to every other partition.
     */
    virtual void broadcast(cCommBuffer *buffer, int tag);

    /**
     * Receives packed data with given tag from given destination.
     * Normally returns true; false is returned if blocking was interrupted by the user.
     */
    virtual bool receiveBlocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId) = 0;

    /**
     * Receives packed data with given tag from given destination.
     * Call is non-blocking -- it returns true if something has been
     * received, false otherwise.
     */
    virtual bool receiveNonblocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId) = 0;
    //@}
};

}  // namespace omnetpp


#endif

