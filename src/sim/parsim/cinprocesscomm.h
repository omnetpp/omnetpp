//=========================================================================
//  CINPROCESSCOMM.H - part of
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

#ifndef __OMNETPP_CINPROCESSCOMM_H
#define __OMNETPP_CINPROCESSCOMM_H


#include <cstdio>
#include <list>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "spinlock.h"
#include "omnetpp/simutil.h"
#include "omnetpp/opp_string.h"
#include "omnetpp/cparsimcomm.h"
#include "omnetpp/csoftowner.h"

namespace omnetpp {

class cMemCommBuffer;

/**
 * @brief Implementation of parsim communications layer for partitions
 * running in threads of a single OS process. It uses mutex-protected
 * std::queues as inboxes for each partition.
 *
 * @ingroup Parsim
 */
class SIM_API cInProcessCommunications : public cParsimCommunications
{
  protected:
    std::thread::id ownerThread;
    int numPartitions = -1;
    int myPartitionId = -1;
    bool sendPointers = true;
    cSimulation *simulation = nullptr; // (unused)
    cMemCommBuffer *spareBuffer = nullptr;

    std::vector<cInProcessCommunications*> partitions; // my peers

    struct ReceivedBuffer {int sourcePartitionId; int tag; cMemCommBuffer *buffer;};
    std::deque<ReceivedBuffer> receiveQueue;

    // use mutex or spinlock for receiveQueue accesses
    bool useSpinlock = true;

    // for the useSpinlock=false case
    std::mutex mutex;
    std::condition_variable queueNotEmpty;

    // for the useSpinlock=true case
    Spinlock spinlock;
    SpinConditionVariable spinConditionVariable;

  protected:
    // common impl. for receiveBlocking() and receiveNonblocking()
    bool receive(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId, bool blocking);
    bool doReceive(cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId, bool blocking);
    void extract(cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId, const ReceivedBuffer& item);
    void fillPartitionsArray(int simulationId, int numPartitions, int partitionId);

  public:
    /**
     * Constructor.
     */
    cInProcessCommunications();

    /**
     * Destructor.
     */
    virtual ~cInProcessCommunications();

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
     * Transferring messages by pointer is supported.
     */
    virtual bool supportsTransferringPointers() override {return sendPointers;}

    /**
     * Pack a message into the buffer.
     */
    virtual bool packMessage(cCommBuffer *buffer, cMessage *msg, int destPartitionId) override;

    /**
     * Unpacks a message from the buffer.
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


