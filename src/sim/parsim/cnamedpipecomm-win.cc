//=========================================================================
// CNAMEDPIPECOMM-WIN.CC - part of
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

#ifdef USE_WINDOWS_PIPES

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>

#include "omnetpp/cexception.h"
#include "omnetpp/globals.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/clog.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "cmemcommbuffer.h"
#include "parsimutil.h"

namespace omnetpp {

Register_Class(cNamedPipeCommunications);

Register_GlobalConfigOption(CFGID_PARSIM_NAMEDPIPECOMM_PREFIX, "parsim-namedpipecommunications-prefix", CFG_STRING, "omnetpp", "When `cNamedPipeCommunications` is selected as parsim communications class: selects the name prefix for Windows named pipes created.");

#define PIPE_INBUFFERSIZE    (1024*1024) /*1MB*/

// FIXME resolve duplication -- we have this in Envir as well
static std::string getWindowsError()
{
    long errorcode = GetLastError();
    LPVOID lpMsgBuf;
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                   FORMAT_MESSAGE_FROM_SYSTEM |
                   FORMAT_MESSAGE_IGNORE_INSERTS,
                   nullptr,
                   errorcode,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPTSTR) &lpMsgBuf,
                   0,
                   nullptr );
    ((char *)lpMsgBuf)[strlen((char *)lpMsgBuf)-3] = '\0';  // chop ".\r\n"

    std::stringstream out;
    out << "error " << errorcode << ": " << (const char *)lpMsgBuf;
    LocalFree(lpMsgBuf);
    return out.str();
}

struct PipeHeader
{
    int tag;
    unsigned long contentLength;
};

cNamedPipeCommunications::cNamedPipeCommunications()
{
    prefix = getEnvir()->getConfig()->getAsString(CFGID_PARSIM_NAMEDPIPECOMM_PREFIX);
}

cNamedPipeCommunications::~cNamedPipeCommunications()
{
    delete[] rpipes;
    delete[] wpipes;
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
    rpipes = new HANDLE[numPartitions];
    for (i = 0; i < numPartitions; i++) {
        if (i == myProcId) {
            rpipes[i] = INVALID_HANDLE_VALUE;
            continue;
        }

        char fname[256];
        sprintf(fname, "\\\\.\\pipe\\%s-%d-%d", prefix.buffer(), myProcId, i);
        EV << "cNamedPipeCommunications: creating pipe '" << fname << "' for read...\n";

        int openMode = PIPE_ACCESS_INBOUND;
        int pipeMode = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT;
        rpipes[i] = CreateNamedPipe(fname, openMode, pipeMode, 1, 0, PIPE_INBUFFERSIZE, ~0UL, nullptr);
        if (rpipes[i] == INVALID_HANDLE_VALUE)
            throw cRuntimeError("cNamedPipeCommunications: CreateNamedPipe operation failed: %s", getWindowsError().c_str());
    }

    // open pipes for write
    wpipes = new HANDLE[numPartitions];
    for (i = 0; i < numPartitions; i++) {
        if (i == myProcId) {
            wpipes[i] = INVALID_HANDLE_VALUE;
            continue;
        }

        char fname[256];
        sprintf(fname, "\\\\.\\pipe\\%s-%d-%d", prefix.buffer(), i, myProcId);
        EV << "cNamedPipeCommunications: opening pipe '" << fname << "' for write...\n";
        for (int k = 0; k < 60; k++) {
            if (k > 0 && k%5 == 0)
                EV << "retry " << k << " of 60...\n";
            wpipes[i] = CreateFile(fname, GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (wpipes[i] != INVALID_HANDLE_VALUE)
                break;
            usleep(1000000);  // 1s
        }
        if (wpipes[i] == INVALID_HANDLE_VALUE)
            throw cRuntimeError("cNamedPipeCommunications: CreateFile operation failed: %s", getWindowsError().c_str());
    }

    // now wait until everybody else also opens the pipes for write
    for (i = 0; i < numPartitions; i++) {
        if (i == myProcId)
            continue;
        EV << "cNamedPipeCommunications: opening pipe from procId=" << i << " for read...\n";
        if (!ConnectNamedPipe(rpipes[i], nullptr) && GetLastError() != ERROR_PIPE_CONNECTED)
            throw cRuntimeError("cNamedPipeCommunications: ConnectNamedPipe operation failed: %s", getWindowsError().c_str());
    }
}

void cNamedPipeCommunications::shutdown()
{
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
    HANDLE h = wpipes[destination];

    struct PipeHeader ph;
    ph.tag = tag;
    ph.contentLength = b->getMessageSize();

    unsigned long bytesWritten;
    if (!WriteFile(h, &ph, sizeof(ph), &bytesWritten, 0))
        throw cRuntimeError("cNamedPipeCommunications: Cannot write pipe to procId=%d: %s", destination, getWindowsError().c_str());
    if (!WriteFile(h, b->getBuffer(), ph.contentLength, &bytesWritten, 0))
        throw cRuntimeError("cNamedPipeCommunications: Cannot write pipe to procId=%d: %s", destination, getWindowsError().c_str());
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

    // TODO should block for some time in the blocking=true case (right now just returns immediately)

    // select pipe to read
    int i, k;
    for (k = 0; k < numPartitions; k++) {
        i = (rrBase+k)%numPartitions;  // shift by rrBase for Round-Robin query
        if (i == myProcId)
            continue;
        unsigned long bytesAvail, bytesLeft;
        if (!PeekNamedPipe(rpipes[i], nullptr, 0, nullptr, &bytesAvail, &bytesLeft))
            throw cRuntimeError("cNamedPipeCommunications: Cannot peek pipe to procId=%d: %s",
                    i, getWindowsError().c_str());
        if (bytesAvail > 0)
            break;
    }
    if (k == numPartitions)
        return false;

    rrBase = (rrBase+1)%numPartitions;
    sourceProcId = i;
    HANDLE h = rpipes[i];

    // read message from selected pipe (handle h)
    unsigned long bytesRead;
    struct PipeHeader ph;
    if (!ReadFile(h, &ph, sizeof(ph), &bytesRead, nullptr))
        throw cRuntimeError("cNamedPipeCommunications: Cannot read from pipe to procId=%d: %s",
                sourceProcId, getWindowsError().c_str());
    if (bytesRead < sizeof(ph))
        throw cRuntimeError("cNamedPipeCommunications: ReadFile returned less data than expected");

    receivedTag = ph.tag;
    b->allocateAtLeast(ph.contentLength);
    b->setMessageSize(ph.contentLength);

    if (!ReadFile(h, b->getBuffer(), ph.contentLength, &bytesRead, nullptr))
        throw cRuntimeError("cNamedPipeCommunications: Cannot read from pipe to procId=%d: %s",
                sourceProcId, getWindowsError().c_str());
    if (bytesRead < ph.contentLength)
        throw cRuntimeError("cNamedPipeCommunications: ReadFile returned less data than expected");
    return true;
}

bool cNamedPipeCommunications::receiveBlocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId)
{
    // receive() currently doesn't handle blocking (PeekNamedPipe() returns
    // immediately if nothing has been received), so we need to sleep a little
    // between invocations, in order to save CPU cycles.
    while (!receive(filtTag, buffer, receivedTag, sourceProcId, true)) {
        if (getEnvir()->idle())
            return false;
        usleep(10000);  // be polite and wait 0.01s
    }
    return true;
}

bool cNamedPipeCommunications::receiveNonblocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId)
{
    return receive(filtTag, buffer, receivedTag, sourceProcId, false);
}

}  // namespace omnetpp

#endif /* USE_WINDOWS_PIPES */

