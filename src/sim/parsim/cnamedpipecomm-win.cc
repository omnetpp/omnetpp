//=========================================================================
//
// CNAMEDPIPECOMM-WIN.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2005 Andras Varga
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "cnamedpipecomm.h"

#ifdef USE_WINDOWS_PIPES

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>   // _sleep()
#include <string>
#include <iostream>

#include "cexception.h"
#include "cmemcommbuffer.h"
#include "macros.h"
#include "cenvir.h"
#include "cconfig.h"
#include "parsimutil.h"


Register_Class(cNamedPipeCommunications);


#define sleep(x)  _sleep((x)*1000)
#define usleep(x) _sleep((x)/1000)



#define PIPE_INBUFFERSIZE  (1024*1024) /*1MB*/


// TBD resolve duplication -- we have this in Envir as well
static std::string getWindowsError()
{
    long errorcode = GetLastError();
    LPVOID lpMsgBuf;
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                   FORMAT_MESSAGE_FROM_SYSTEM |
                   FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL,
                   errorcode,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPTSTR) &lpMsgBuf,
                   0,
                   NULL );
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
    prefix = ev.config()->getAsString("General", "parsim-namedpipecommunications-prefix", "omnetpp");
    rpipes = NULL;
    wpipes = NULL;
    rrBase = 0;
}

cNamedPipeCommunications::~cNamedPipeCommunications()
{
    delete [] rpipes;
    delete [] wpipes;
}

void cNamedPipeCommunications::init()
{
    // get numPartitions and myProcId from "-p" command-line option
    getProcIdFromCommandLineArgs(myProcId, numPartitions, "cNamedPipeCommunications");
    ev.printf("cNamedPipeCommunications: started as process %d out of %d.\n", myProcId, numPartitions);

    // create and open pipes for read
    int i;
    rpipes = new HANDLE[numPartitions];
    for (i=0; i<numPartitions; i++)
    {
        if (i==myProcId)
        {
            rpipes[i] = INVALID_HANDLE_VALUE;
            continue;
        }

        char fname[256];
        sprintf(fname,"\\\\.\\pipe\\%s-%d-%d", prefix.buffer(), myProcId, i);
        ev.printf("cNamedPipeCommunications: creating pipe '%s' for read...\n", fname);

        int openMode = PIPE_ACCESS_INBOUND;
        int pipeMode = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT;
        rpipes[i] = CreateNamedPipe(fname, openMode, pipeMode, 1, 0, PIPE_INBUFFERSIZE, ~0UL, NULL);
        if (rpipes[i] == INVALID_HANDLE_VALUE)
            throw new cRuntimeError("cNamedPipeCommunications: CreateNamedPipe operation failed: %s", getWindowsError().c_str());
    }

    // open pipes for write
    wpipes = new HANDLE[numPartitions];
    for (i=0; i<numPartitions; i++)
    {
        if (i==myProcId)
        {
            wpipes[i] = INVALID_HANDLE_VALUE;
            continue;
        }

        char fname[256];
        sprintf(fname,"\\\\.\\pipe\\%s-%d-%d", prefix.buffer(), i, myProcId);
        ev.printf("cNamedPipeCommunications: opening pipe '%s' for write...\n", fname);
        for (int k=0; k<60; k++)
        {
            if (k>0 && k%5==0) ev.printf("retry %d of 60...\n", k);
            wpipes[i] = CreateFile(fname, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (wpipes[i]!=INVALID_HANDLE_VALUE)
                break;
            sleep(1);
        }
        if (wpipes[i] == INVALID_HANDLE_VALUE)
            throw new cRuntimeError("cNamedPipeCommunications: CreateFile operation failed: %s", getWindowsError().c_str());
    }

    // now wait until everybody else also opens the pipes for write
    for (i=0; i<numPartitions; i++)
    {
        if (i==myProcId)
            continue;
        ev.printf("cNamedPipeCommunications: opening pipe from procId=%d for read...\n", i);
        if (!ConnectNamedPipe(rpipes[i], NULL) && GetLastError()!=ERROR_PIPE_CONNECTED)
            throw new cRuntimeError("cNamedPipeCommunications: ConnectNamedPipe operation failed: %s", getWindowsError().c_str());
    }

}

void cNamedPipeCommunications::shutdown()
{
}

int cNamedPipeCommunications::getNumPartitions()
{
    return numPartitions;
}

int cNamedPipeCommunications::getProcId()
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
        throw new cRuntimeError("cNamedPipeCommunications: cannot write pipe to procId=%d: %s", destination, getWindowsError().c_str());
    if (!WriteFile(h, b->getBuffer(), ph.contentLength, &bytesWritten, 0))
        throw new cRuntimeError("cNamedPipeCommunications: cannot write pipe to procId=%d: %s", destination, getWindowsError().c_str());
}

bool cNamedPipeCommunications::receive(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId, bool blocking)
{
    bool recv = doReceive(buffer, receivedTag, sourceProcId, blocking);
    // TBD implement tag filtering
    if (recv && filtTag!=PARSIM_ANY_TAG && filtTag!=receivedTag)
        throw new cRuntimeError("cNamedPipeCommunications: tag filtering not implemented");
    return recv;
}

bool cNamedPipeCommunications::doReceive(cCommBuffer *buffer, int& receivedTag, int& sourceProcId, bool blocking)
{
    cMemCommBuffer *b = (cMemCommBuffer *)buffer;
    b->reset();

    // TBD may be refined to handle blocking=true (right now just returns immediately)

    // select pipe to read
    int i, k;
    for (k=0; k<numPartitions; k++)
    {
        i = (rrBase+k)%numPartitions; // shift by rrBase for Round-Robin query
        if (i==myProcId)
            continue;
        unsigned long bytesAvail, bytesLeft;
        if (!PeekNamedPipe(rpipes[i], NULL, 0, NULL, &bytesAvail, &bytesLeft))
            throw new cRuntimeError("cNamedPipeCommunications: cannot peek pipe to procId=%d: %s",
                                    i, getWindowsError().c_str());
        if (bytesAvail>0)
            break;
    }
    if (k==numPartitions)
        return false;

    rrBase = (rrBase+1)%numPartitions;
    sourceProcId = i;
    HANDLE h = rpipes[i];

    // read message from selected pipe (handle h)
    unsigned long bytesRead;
    struct PipeHeader ph;
    if (!ReadFile(h, &ph, sizeof(ph), &bytesRead, NULL))
        throw new cRuntimeError("cNamedPipeCommunications: cannot read from pipe to procId=%d: %s",
                                sourceProcId, getWindowsError().c_str());
    if (bytesRead<sizeof(ph))
        throw new cRuntimeError("cNamedPipeCommunications: ReadFile returned less data than expected");

    receivedTag = ph.tag;
    b->allocateAtLeast(ph.contentLength);
    b->setMessageSize(ph.contentLength);

    if (!ReadFile(h, b->getBuffer(), ph.contentLength, &bytesRead, NULL))
        throw new cRuntimeError("cNamedPipeCommunications: cannot read from pipe to procId=%d: %s",
                                sourceProcId, getWindowsError().c_str());
    if (bytesRead<ph.contentLength)
        throw new cRuntimeError("cNamedPipeCommunications: ReadFile returned less data than expected");
    return true;

}

bool cNamedPipeCommunications::receiveBlocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId)
{
    // receive() currently doesn't handle blocking (PeekNamedPipe() returns
    // immediately if nothing has been received), so we need to sleep a little
    // between invocations, in order to save CPU cycles.
    while (!receive(filtTag, buffer, receivedTag, sourceProcId, true))
    {
        if (ev.idle())
            return false;
        usleep(10000); // be polite and wait 0.01s
    }
    return true;
}

bool cNamedPipeCommunications::receiveNonblocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId)
{
    return receive(filtTag, buffer, receivedTag, sourceProcId, false);
}

#endif  /* USE_WINDOWS_PIPES */




