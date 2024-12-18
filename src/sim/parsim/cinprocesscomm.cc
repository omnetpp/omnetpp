//=========================================================================
//  CINPROCESSCOMM.CC - part of
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

#include <thread>
#include <algorithm>
#include "cinprocesscomm.h"
#include "omnetpp/cexception.h"
#include "omnetpp/clog.h"
#include "omnetpp/globals.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cpacket.h"
#include "cmemcommbuffer.h"

namespace omnetpp {

Register_Class(cInProcessCommunications);

Register_GlobalConfigOption(CFGID_PARSIM_INPROCESS_COMM_SEND_POINTERS, "parsim-inprocess-comm-send-pointers", CFG_BOOL, "true", "With `parallel-simulation=true` and `parsim-communications-class=omnetpp::cInProcessCommunications`: specifies whether messages are sent by sending pointers to the messages (default) or by serializing the messages.");
Register_GlobalConfigOption(CFGID_PARSIM_INPROCESS_COMM_USE_SPINLOCK, "parsim-inprocess-comm-use-spinlock", CFG_BOOL, "false", "With `parallel-simulation=true` and `parsim-communications-class=omnetpp::cInProcessCommunications`: specifies whether a spinlock or a mutex is used to synchronize access to the message queues. Using spinlocks instead of mutexes avoids context switches, potentially reducing communication latency and improving performance, at the cost of consuming more CPU cycles due to busy waiting.");

//TODO this should NOT be global, as it's possible to run several parsim runs in parallel (if there's enough cores)
static std::vector<cInProcessCommunications*> partitions;
static std::mutex partitionsMutex;
static std::condition_variable partitionsConditionVariable;

cInProcessCommunications::cInProcessCommunications()
{
}

cInProcessCommunications::~cInProcessCommunications()
{
    for (auto item : receiveQueue)
        delete item.buffer;
    delete spareBuffer;
}

void cInProcessCommunications::configure(cSimulation *sim, cConfiguration *cfg, int np, int partitionId, const std::map<std::string,cValue>& extraData)
{
    ownerThread = std::this_thread::get_id();
    simulation = sim;
    numPartitions = np;
    myPartitionId = partitionId;
    if (numPartitions == -1)
        throw cRuntimeError("%s: Number of partitions not specified", getClassName());
    if (myPartitionId == -1)
        throw cRuntimeError("%s: partitionId not specified", getClassName());
    if (numPartitions < 1 || myPartitionId < 0 || myPartitionId >= numPartitions)
        throw cRuntimeError("%s: Invalid value for the number of partitions (%d) or partitionId (%d)", getClassName(), np, partitionId);

    sendPointers = cfg->getAsBool(CFGID_PARSIM_INPROCESS_COMM_SEND_POINTERS, true);
    useSpinlock = cfg->getAsBool(CFGID_PARSIM_INPROCESS_COMM_USE_SPINLOCK, true);

    EV << "cInProcessCommunications: started as process " << myPartitionId << " out of " << numPartitions << ".\n";

    // register this partition in partitions[]
    std::unique_lock<std::mutex> lock(partitionsMutex);
    partitions.resize(np);
    partitions[partitionId] = this;
    partitionsConditionVariable.notify_all();

    // wait until all partitions register themselves in partitions[]
    partitionsConditionVariable.wait(lock, [&] {
        return std::all_of(partitions.begin(), partitions.end(), [](auto p) { return p != nullptr; });
    });
}

void cInProcessCommunications::shutdown()
{
}

int cInProcessCommunications::getNumPartitions() const
{
    return numPartitions;
}

int cInProcessCommunications::getPartitionId() const
{
    return myPartitionId;
}

bool cInProcessCommunications::packMessage(cCommBuffer *buffer, cMessage *msg, int destPartitionId)
{
    if (sendPointers) {
        // unshare to eliminate race conditions
        msg->unshare();

        intptr_t ptr = (intptr_t)msg;
        buffer->pack(ptr);

        // hand ownership to the destination partition
        // note: no locking needed, because only the owner pointer in the message object changes
        partitions[destPartitionId]->take(msg);
        return true;
    }
    else {
        buffer->packObject(msg);
        return false;
    }
}

cMessage *cInProcessCommunications::unpackMessage(cCommBuffer *buffer)
{
    cMessage *msg;
    if (sendPointers) {
        intptr_t ptr;
        buffer->unpack(ptr);
        msg = (cMessage *)ptr;
        drop(msg); //TODO or implement yieldOwnership() to do nothing
    }
    else {
        msg = (cMessage *)buffer->unpackObject();
    }
    return msg;
}

cCommBuffer *cInProcessCommunications::createCommBuffer()
{
    if (spareBuffer) {
        cMemCommBuffer *result = spareBuffer;
        result->reset();
        spareBuffer = nullptr;
        return result;
    }
    else {
        return new cMemCommBuffer();
    }
}

void cInProcessCommunications::recycleCommBuffer(cCommBuffer *buffer)
{
    if (spareBuffer)
        delete buffer;
    else
        spareBuffer = static_cast<cMemCommBuffer *>(buffer);
}

void cInProcessCommunications::send(cCommBuffer *buffer, int tag, int destination)
{
    ASSERT(destination >= 0 && destination < numPartitions && destination != myPartitionId);
    cMemCommBuffer *sendBuffer = new cMemCommBuffer(std::move(*static_cast<cMemCommBuffer *>(buffer)));
    sendBuffer->checkThreadAccess();
    sendBuffer->setOwnerThread(partitions[destination]->ownerThread);

    if (useSpinlock) {
        std::lock_guard<Spinlock> lock(partitions[destination]->spinlock);
        partitions[destination]->receiveQueue.push_back({myPartitionId, tag, sendBuffer});
    }
    else {
        std::lock_guard<std::mutex> lock(partitions[destination]->mutex);
        partitions[destination]->receiveQueue.push_back({myPartitionId, tag, sendBuffer});
        partitions[destination]->queueNotEmpty.notify_one();
    }
}

inline void cInProcessCommunications::extract(cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId, const ReceivedBuffer& item)
{
    receivedTag = item.tag;
    sourcePartitionId = item.sourcePartitionId;
    ((cMemCommBuffer*)buffer)->swap(item.buffer);
    delete item.buffer;
}

bool cInProcessCommunications::receive(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId, bool blocking)
{
    if (useSpinlock) {
        if (filtTag == PARSIM_ANY_TAG) {
            std::unique_lock<Spinlock> lock(spinlock);
            if (blocking)
                spinConditionVariable.wait(lock, [this] { return !receiveQueue.empty(); });
            if (!receiveQueue.empty()) {
                extract(buffer, receivedTag, sourcePartitionId, receiveQueue.front());
                receiveQueue.pop_front();
                return true;
            }
        }
        else {
            std::unique_lock<Spinlock> lock(spinlock);
            if (blocking)
                spinConditionVariable.wait(lock, [this, filtTag] { return std::find_if(receiveQueue.begin(), receiveQueue.end(), [filtTag](const ReceivedBuffer& elem) { return elem.tag == filtTag; }) != receiveQueue.end(); });
            auto it = std::find_if(receiveQueue.begin(), receiveQueue.end(), [filtTag](const ReceivedBuffer& elem) { return elem.tag == filtTag; });
            if (it != receiveQueue.end()) {
                extract(buffer, receivedTag, sourcePartitionId, *it);
                receiveQueue.erase(it);
                return true;
            }
        }

    }
    else {
        // mutex
        if (filtTag == PARSIM_ANY_TAG) {
            std::unique_lock<std::mutex> lock(mutex);
            if (blocking)
                queueNotEmpty.wait(lock, [this] { return !receiveQueue.empty(); });
            if (!receiveQueue.empty()) {
                extract(buffer, receivedTag, sourcePartitionId, receiveQueue.front());
                receiveQueue.pop_front();
                return true;
            }
        }
        else {
            std::unique_lock<std::mutex> lock(mutex);
            if (blocking)
                queueNotEmpty.wait(lock, [this, filtTag] { return std::find_if(receiveQueue.begin(), receiveQueue.end(), [filtTag](const ReceivedBuffer& elem) { return elem.tag == filtTag; }) != receiveQueue.end(); });
            auto it = std::find_if(receiveQueue.begin(), receiveQueue.end(), [filtTag](const ReceivedBuffer& elem) { return elem.tag == filtTag; });
            if (it != receiveQueue.end()) {
                extract(buffer, receivedTag, sourcePartitionId, *it);
                receiveQueue.erase(it);
                return true;
            }
        }
    }

    return false;
}

bool cInProcessCommunications::doReceive(cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId, bool blocking)
{
    return receive(PARSIM_ANY_TAG, buffer, receivedTag, sourcePartitionId, blocking);
}

bool cInProcessCommunications::receiveBlocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId)
{
    return receive(filtTag, buffer, receivedTag, sourcePartitionId, true);
}

bool cInProcessCommunications::receiveNonblocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId)
{
    return receive(filtTag, buffer, receivedTag, sourcePartitionId, false);
}

}  // namespace omnetpp

