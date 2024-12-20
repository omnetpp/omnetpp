//=========================================================================
//  CNAMEDPIPECOMM.H - part of
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

#ifndef __OMNETPP_CNAMEDPIPECOMM_H
#define __OMNETPP_CNAMEDPIPECOMM_H


#include <cstdio>
#include <list>
#include "omnetpp/simutil.h"
#include "omnetpp/opp_string.h"
#include "omnetpp/cparsimcomm.h"
#include "omnetpp/platdep/platmisc.h"  // for <windows.h>

// decide platform
#if defined(_WIN32)
#define USE_WINDOWS_PIPES
#endif

namespace omnetpp {

#ifdef USE_WINDOWS_PIPES
typedef HANDLE PIPE;
#else
typedef int PIPE;
#endif

class cMemCommBuffer;

/**
 * @brief Implementation of the communications layer which uses named pipes.
 * Pipes are created at initialization time, and are used throughout
 * the whole simulation.
 *
 * @ingroup Parsim
 */
class SIM_API cNamedPipeCommunications : public cParsimCommunications
{
  protected:
    cSimulation *simulation = nullptr;
    int numPartitions = -1;
    int myPartitionId = -1;

    // pipes
    opp_string prefix;
    PIPE *rpipes = nullptr;
    PIPE *wpipes = nullptr;
    int maxFdPlus1 = -1;
    int rrBase = 0;

    // reordering buffer needed because of tag filtering support (filtTag)
    struct ReceivedBuffer {int receivedTag; int sourcePartitionId; cMemCommBuffer *buffer;};
    std::list<ReceivedBuffer> receivedBuffers;

  protected:
    // common impl. for receiveBlocking() and receiveNonblocking()
    bool receive(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId, bool blocking);
    bool doReceive(cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId, bool blocking);
    void extract(cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId, const ReceivedBuffer& item);

  public:
    /**
     * Constructor.
     */
    cNamedPipeCommunications();

    /**
     * Destructor.
     */
    virtual ~cNamedPipeCommunications();

    /** @name Redefined methods from cParsimCommunications */
    //@{
    /**
     * Init the library. Here we create and open the named pipes.
     */
    virtual void configure(cSimulation *simulation, cConfiguration *cfg, int numPartitions, int partitionId, const std::map<std::string,cValue>& extraData) override;

    /**
     * Shutdown the communications library. Closes and removes the named pipes.
     */
    virtual void shutdown() override;

    /**
     * Returns the associated simulation instance.
     */
    cSimulation *getSimulation() const override {return simulation;}

    /**
     * Returns total number of partitions.
     */
    virtual int getNumPartitions() const override;

    /**
     * Returns the id of this partition.
     */
    virtual int getPartitionId() const override;

    /**
     * Sending pointers is not supported.
     */
    virtual bool supportsTransferringPointers() override {return false;}

    /**
     * Serializes the message into the buffer.
     */
    virtual bool packMessage(cCommBuffer *buffer, cMessage *msg, int destPartitionId) override;

    /**
     * Deserializes the message from the buffer.
     */
    virtual cMessage *unpackMessage(cCommBuffer *buffer) override;

    /**
     * Creates an empty buffer of type cMemCommBuffer.
     */
    virtual cCommBuffer *createCommBuffer() override;

    /**
     * Recycle communication buffer after use.
     */
    virtual void recycleCommBuffer(cCommBuffer *buffer) override;

    /**
     * Sends packed data with given tag to destination.
     */
    virtual void send(cCommBuffer *buffer, int tag, int destination) override;

    /**
     * Receives packed data, and also returns tag and source partitionId.
     * Normally returns true; false is returned if blocking was interrupted by the user.
     */
    virtual bool receiveBlocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId) override;

    /**
     * Receives packed data, and also returns tag and source partitionId.
     * Call is non-blocking -- it returns true if something has been
     * received, false otherwise.
     */
    virtual bool receiveNonblocking(int filtTag, cCommBuffer *buffer,  int& receivedTag, int& sourcePartitionId) override;
    //@}
};

}  // namespace omnetpp


#endif


