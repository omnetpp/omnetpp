//=========================================================================
//  CNAMEDPIPECOMM.CC - part of
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

#include "cnamedpipecomm.h"

#ifndef USE_WINDOWS_PIPES

#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include "omnetpp/cexception.h"
#include "omnetpp/clog.h"
#include "omnetpp/globals.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cmessage.h"
#include "cmemcommbuffer.h"

namespace omnetpp {

Register_Class(cNamedPipeCommunications);

Register_GlobalConfigOption(CFGID_PARSIM_NAMEDPIPECOMM_PREFIX, "parsim-namedpipecommunications-prefix", CFG_STRING, "comm/", "When `cNamedPipeCommunications` is selected as parsim communications class: selects the prefix (directory+potential filename prefix) where name pipes are created in the file system.");

static int readBytes(int fd, void *buf, int len)
{
    int tot = 0;
    while (tot < len) {
        int n = read(fd, (char *)buf+tot, len-tot);
        if (n == -1) {
            if (errno == EAGAIN)  // this is not an error
                n = 0;
            else
                return -1;
        }
        tot += n;
    }
    return tot;
}

static int writeBytes(int fd, void *buf, int len)
{
    int tot = 0;
    while (tot < len) {
        int n = write(fd, (char *)buf+tot, len-tot);
        if (n == -1) {
            if (errno == EAGAIN)  // this is not an error
                n = 0;
            else
                return -1;
        }
        tot += n;
    }
    return tot;
}

struct PipeHeader
{
    int tag;
    int contentLength;
};

cNamedPipeCommunications::cNamedPipeCommunications()
{
}

cNamedPipeCommunications::~cNamedPipeCommunications()
{
    delete[] rpipes;
    delete[] wpipes;

    for (auto item : receivedBuffers)
        delete item.buffer;
}

void cNamedPipeCommunications::configure(cSimulation *sim, cConfiguration *cfg, int np, int partitionId)
{
    simulation = sim;
    numPartitions = np;
    myPartitionId = partitionId;
    if (numPartitions == -1)
        throw cRuntimeError("%s: Number of partitions not specified", getClassName());
    if (myPartitionId == -1)
        throw cRuntimeError("%s: partitionId not specified", getClassName());
    if (numPartitions < 1 || myPartitionId < 0 || myPartitionId >= numPartitions)
        throw cRuntimeError("%s: Invalid value for the number of partitions (%d) or partitionId (%d)", getClassName(), np, partitionId);

    prefix = cfg->getAsString(CFGID_PARSIM_NAMEDPIPECOMM_PREFIX);

    EV << "cNamedPipeCommunications: started as process " << myPartitionId << " out of " << numPartitions << ".\n";

    // create and open pipes for read
    rpipes = new int[numPartitions];
    std::fill_n(rpipes, numPartitions, -1);
    for (int i = 0; i < numPartitions; i++) {
        if (i == myPartitionId)
            continue;

        char fname[256];
        sprintf(fname, "%spipe-%d-%d", prefix.buffer(), myPartitionId, i);
        EV << "cNamedPipeCommunications: creating and opening pipe '" << fname << "' for read...\n";
        unlink(fname);
        if (mkfifo(fname, 0600) == -1)
            throw cRuntimeError("cNamedPipeCommunications: Cannot create pipe '%s': %s", fname, strerror(errno));
        rpipes[i] = open(fname, O_RDONLY|O_NONBLOCK);
        if (rpipes[i] == -1)
            throw cRuntimeError("cNamedPipeCommunications: Cannot open pipe '%s' for read: %s", fname, strerror(errno));
        fcntl(rpipes[i], F_SETPIPE_SZ, 1024*1024);
        if (rpipes[i] > maxFdPlus1)
            maxFdPlus1 = rpipes[i];
    }
    maxFdPlus1 += 1;

    // open pipes for write
    wpipes = new int[numPartitions];
    std::fill_n(wpipes, numPartitions, -1);
    for (int i = 0; i < numPartitions; i++) {
        if (i == myPartitionId)
            continue;

        char fname[256];
        sprintf(fname, "%spipe-%d-%d", prefix.buffer(), i, myPartitionId);
        EV << "cNamedPipeCommunications: opening pipe '" << fname << "' for write...\n";
        wpipes[i] = open(fname, O_WRONLY|O_NONBLOCK);
        for (int k = 0; k < 100 && wpipes[i] == -1; k++) {
            usleep(10000);
            wpipes[i] = open(fname, O_WRONLY|O_NONBLOCK);
        }
        if (wpipes[i] == -1)
            throw cRuntimeError("cNamedPipeCommunications: Cannot open pipe '%s' for write: %s", fname, strerror(errno));
        fcntl(wpipes[i], F_SETPIPE_SZ, 1024*1024);
    }
}

void cNamedPipeCommunications::shutdown()
{
    for (int i = 0; i < numPartitions; i++) {
        if (rpipes != nullptr && rpipes[i] != -1)
            close(rpipes[i]);
        if (wpipes != nullptr && wpipes[i] != -1)
            close(wpipes[i]);
    }
}

int cNamedPipeCommunications::getNumPartitions() const
{
    return numPartitions;
}

int cNamedPipeCommunications::getPartitionId() const
{
    return myPartitionId;
}

bool cNamedPipeCommunications::packMessage(cCommBuffer *buffer, cMessage *msg, int destPartitionId)
{
    buffer->packObject(msg);
    return false;
}

cMessage *cNamedPipeCommunications::unpackMessage(cCommBuffer *buffer)
{
    return (cMessage *)buffer->unpackObject();
}

cCommBuffer *cNamedPipeCommunications::createCommBuffer()
{
    return new cMemCommBuffer();
}

void cNamedPipeCommunications::recycleCommBuffer(cCommBuffer *buffer)
{
    delete buffer;
}

void cNamedPipeCommunications::send(cCommBuffer *buffer, int tag, int destination)
{
    cMemCommBuffer *b = (cMemCommBuffer *)buffer;
    int fd = wpipes[destination];

    struct PipeHeader ph;
    ph.tag = tag;
    ph.contentLength = b->getMessageSize();
    if (writeBytes(fd, &ph, sizeof(ph)) == -1)
        throw cRuntimeError("cNamedPipeCommunications: Cannot write pipe to partitionId=%d: %s", destination, strerror(errno));
    if (writeBytes(fd, b->getBuffer(), ph.contentLength) == -1)
        throw cRuntimeError("cNamedPipeCommunications: Cannot write pipe to partitionId=%d: %s", destination, strerror(errno));
}

inline void cNamedPipeCommunications::extract(cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId, const ReceivedBuffer& item)
{
    receivedTag = item.receivedTag;
    sourcePartitionId = item.sourcePartitionId;
    ((cMemCommBuffer*)buffer)->swap(item.buffer);
    delete item.buffer;
}

bool cNamedPipeCommunications::receive(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId, bool blocking)
{
    if (filtTag == PARSIM_ANY_TAG) {
        // try returning a previously received one
        if (!receivedBuffers.empty()) {
            extract(buffer, receivedTag, sourcePartitionId, receivedBuffers.front());
            receivedBuffers.pop_front();
            return true;
        }

        // receive from pipe
        bool received = doReceive(buffer, receivedTag, sourcePartitionId, blocking);
        while (!received && blocking)
            received = doReceive(buffer, receivedTag, sourcePartitionId, blocking);
        return received;
    }
    else {
        // try returning a previously received one
        auto it = std::find_if(receivedBuffers.begin(), receivedBuffers.end(), [filtTag](const ReceivedBuffer& elem) { return elem.receivedTag == filtTag; });
        if (it != receivedBuffers.end()) {
            extract(buffer, receivedTag, sourcePartitionId, *it);
            receivedBuffers.erase(it);
            return true;
        }

        // receive from pipe
        bool received;
        do {
            received = doReceive(buffer, receivedTag, sourcePartitionId, blocking);

            // if received one with a wrong tag, store it for later
            if (received && filtTag != receivedTag) {
                cMemCommBuffer *copy = new cMemCommBuffer();
                ((cMemCommBuffer*)buffer)->swap(copy);
                receivedBuffers.push_back({receivedTag, sourcePartitionId, copy});
                received = false; // continue trying if blocking
            }
        } while (blocking && !received);

        return received;
    }
}

bool cNamedPipeCommunications::doReceive(cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId, bool blocking)
{
    cMemCommBuffer *b = (cMemCommBuffer *)buffer;
    b->reset();

    // create file descriptor set for select() call
    int i;
    fd_set fdset;
    FD_ZERO(&fdset);
    for (i = 0; i < numPartitions; i++)
        if (rpipes[i] != -1)
            FD_SET(rpipes[i], &fdset);
        else
            ASSERT(i == myPartitionId);


    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = blocking ? 100000 : 0;  // if blocking, wait 0.1 sec

    int ret = select(maxFdPlus1, &fdset, nullptr, nullptr, &tv);
    if (ret > 0) {
        rrBase = (rrBase+1)%numPartitions;
        for (int k = 0; k < numPartitions; k++) {
            i = (rrBase+k)%numPartitions;  // shift by rrBase for Round-Robin query
            if (rpipes[i] != -1 && FD_ISSET(rpipes[i], &fdset)) {
                struct PipeHeader ph;
                if (readBytes(rpipes[i], &ph, sizeof(ph)) == -1)
                    throw cRuntimeError("cNamedPipeCommunications: Cannot read from pipe "
                                        "to partitionId=%d: %s", sourcePartitionId, strerror(errno));

                sourcePartitionId = i;
                receivedTag = ph.tag;
                b->allocateAtLeast(ph.contentLength);
                b->setMessageSize(ph.contentLength);

                if (readBytes(rpipes[i], b->getBuffer(), ph.contentLength) == -1)
                    throw cRuntimeError("cNamedPipeCommunications: Cannot read from pipe "
                                        "to partitionId=%d: %s", sourcePartitionId, strerror(errno));
                return true;
            }
        }
    }

    return false;
}

bool cNamedPipeCommunications::receiveBlocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId)
{
    // select() call inside receive() will block for max 1s, yielding CPU
    // to other processes in the meantime
    while (!receive(filtTag, buffer, receivedTag, sourcePartitionId, true)) {
        if (getEnvir()->idle())
            return false;
    }
    return true;
}

bool cNamedPipeCommunications::receiveNonblocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId)
{
    return receive(filtTag, buffer, receivedTag, sourcePartitionId, false);
}

}  // namespace omnetpp

#endif /* (!USE_WINDOWS_PIPES) */
