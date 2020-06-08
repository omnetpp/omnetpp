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

#include "cobject.h"

namespace omnetpp {

class cCommBuffer;

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
    virtual void init(int numPartitions) = 0;

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
    virtual int getProcId() const = 0;

    /** @name Buffers, send, receive */
    //@{
    /**
     * Creates an empty buffer that can be used to send/receive data
     */
    virtual cCommBuffer *createCommBuffer() = 0;

    /**
     * Recycle communication buffer after use. May be used to implement pooling, etc.
     */
    virtual void recycleCommBuffer(cCommBuffer *buffer) = 0;

    /**
     * Sends packed data with given tag to destination.
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
    virtual bool receiveBlocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId) = 0;

    /**
     * Receives packed data with given tag from given destination.
     * Call is non-blocking -- it returns true if something has been
     * received, false otherwise.
     */
    virtual bool receiveNonblocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId) = 0;
    //@}
};

}  // namespace omnetpp


#endif

