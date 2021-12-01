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
#include "cmemcommbuffer.h"
#include "parsimutil.h"

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

struct PipeHeader
{
    int tag;
    int contentLength;
};

cNamedPipeCommunications::cNamedPipeCommunications()
{
    prefix = getEnvir()->getConfig()->getAsString(CFGID_PARSIM_NAMEDPIPECOMM_PREFIX);
}

cNamedPipeCommunications::~cNamedPipeCommunications()
{
    delete[] rpipes;
    delete[] wpipes;

    for (auto item : receivedBuffers)
        delete item.buffer;
}

void cNamedPipeCommunications::init(int np)
{
    // store parameter
    numPartitions = np;

    // get myProcId from "-p" command-line option
    myProcId = getProcIdFromCommandLineArgs(numPartitions, "cNamedPipeCommunications");

    EV << "cNamedPipeCommunications: started as process " << myProcId << " out of " << numPartitions << ".\n";

    // create and open pipes for read
    int i;
    rpipes = new int[numPartitions];
    for (i = 0; i < numPartitions; i++) {
        if (i == myProcId) {
            rpipes[i] = -1;
            continue;
        }

        char fname[256];
        sprintf(fname, "%spipe-%d-%d", prefix.buffer(), myProcId, i);
        EV << "cNamedPipeCommunications: creating and opening pipe '" << fname << "' for read...\n";
        unlink(fname);
        if (mknod(fname, S_IFIFO|0600, 0) == -1)
            throw cRuntimeError("cNamedPipeCommunications: Cannot create pipe '%s': %s", fname, strerror(errno));
        rpipes[i] = open(fname, O_RDONLY|O_NONBLOCK);
        if (rpipes[i] == -1)
            throw cRuntimeError("cNamedPipeCommunications: Cannot open pipe '%s' for read: %s", fname, strerror(errno));

        if (rpipes[i] > maxFdPlus1)
            maxFdPlus1 = rpipes[i];
    }
    maxFdPlus1 += 1;

    // open pipes for write
    wpipes = new int[numPartitions];
    for (i = 0; i < numPartitions; i++) {
        if (i == myProcId) {
            wpipes[i] = -1;
            continue;
        }

        char fname[256];
        sprintf(fname, "%spipe-%d-%d", prefix.buffer(), i, myProcId);
        EV << "cNamedPipeCommunications: opening pipe '" << fname << "' for write...\n";
        wpipes[i] = open(fname, O_WRONLY);
        for (int k = 0; k < 30 && wpipes[i] == -1; k++) {
            sleep(1);
            wpipes[i] = open(fname, O_WRONLY);
        }
        if (wpipes[i] == -1)
            throw cRuntimeError("cNamedPipeCommunications: Cannot open pipe '%s' for write: %s", fname, strerror(errno));
    }
}

void cNamedPipeCommunications::shutdown()
{
    for (int i = 0; i < numPartitions; i++) {
        close(rpipes[i]);
        close(wpipes[i]);
    }
}

int cNamedPipeCommunications::getNumPartitions() const
{
    return numPartitions;
}

int cNamedPipeCommunications::getProcId() const
{
    return myProcId;
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
    if (write(fd, &ph, sizeof(ph)) == -1)
        throw cRuntimeError("cNamedPipeCommunications: Cannot write pipe to procId=%d: %s", destination, strerror(errno));
    if (write(fd, b->getBuffer(), ph.contentLength) == -1)
        throw cRuntimeError("cNamedPipeCommunications: Cannot write pipe to procId=%d: %s", destination, strerror(errno));
}

bool cNamedPipeCommunications::receive(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId, bool blocking)
{
    // return one from the previously buffered ones, if exist
    for (auto it = receivedBuffers.begin(); it != receivedBuffers.end(); ++it) {
        if (it->receivedTag == filtTag || filtTag == PARSIM_ANY_TAG) {
            receivedTag = it->receivedTag;
            sourceProcId = it->sourceProcId;
            ((cMemCommBuffer*)buffer)->swap(it->buffer);
            delete it->buffer;
            receivedBuffers.erase(it);
            return true;
        }
    }

    // receive from pipe
    bool recv = doReceive(buffer, receivedTag, sourceProcId, blocking);

    // if received one with a wrong tag, store it for later and return false
    if (recv && filtTag != PARSIM_ANY_TAG && filtTag != receivedTag) {
        cMemCommBuffer *copy = new cMemCommBuffer();
        ((cMemCommBuffer*)buffer)->swap(copy);
        receivedBuffers.push_back({receivedTag, sourceProcId, copy});
        return false;
    }
    return recv;
}

bool cNamedPipeCommunications::doReceive(cCommBuffer *buffer, int& receivedTag, int& sourceProcId, bool blocking)
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
                                        "to procId=%d: %s", sourceProcId, strerror(errno));

                sourceProcId = i;
                receivedTag = ph.tag;
                b->allocateAtLeast(ph.contentLength);
                b->setMessageSize(ph.contentLength);

                if (readBytes(rpipes[i], b->getBuffer(), ph.contentLength) == -1)
                    throw cRuntimeError("cNamedPipeCommunications: Cannot read from pipe "
                                        "to procId=%d: %s", sourceProcId, strerror(errno));
                return true;
            }
        }
    }

    return false;
}

bool cNamedPipeCommunications::receiveBlocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId)
{
    // select() call inside receive() will block for max 1s, yielding CPU
    // to other processes in the meantime
    while (!receive(filtTag, buffer, receivedTag, sourceProcId, true)) {
        if (getEnvir()->idle())
            return false;
    }
    return true;
}

bool cNamedPipeCommunications::receiveNonblocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId)
{
    return receive(filtTag, buffer, receivedTag, sourceProcId, false);
}

}  // namespace omnetpp

#endif /* (!USE_WINDOWS_PIPES) */
