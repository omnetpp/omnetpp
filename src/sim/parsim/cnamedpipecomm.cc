//=========================================================================
//  CNAMEDPIPECOMM.CC - part of
//
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

#ifndef USE_WINDOWS_PIPES

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "cexception.h"
#include "cmemcommbuffer.h"
#include "macros.h"
#include "cenvir.h"
#include "cconfig.h"
#include "parsimutil.h"


Register_Class(cNamedPipeCommunications);


int readBytes(int fd, void *buf, int len)
{
    int tot = 0;
    while (tot<len)
    {
        int n = read(fd, (char *)buf+tot, len-tot);
        if (n==-1)
        {
            if (errno==EAGAIN) // this is not an error
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
    prefix = ev.config()->getAsString("General", "parsim-namedpipecommunications-prefix", "comm/");
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
    rpipes = new int[numPartitions];
    for (i=0; i<numPartitions; i++)
    {
        if (i==myProcId)
        {
            rpipes[i] = -1;
            continue;
        }

        char fname[256];
        sprintf(fname,"%spipe-%d-%d", prefix.buffer(), myProcId, i);
        ev.printf("cNamedPipeCommunications: creating and opening pipe '%s' for read...\n", fname);
        unlink(fname);
        if (mknod(fname, S_IFIFO|0600, 0)==-1)
            throw new cRuntimeError("cNamedPipeCommunications: cannot create pipe '%s': %s", fname, strerror(errno));
        rpipes[i] = open(fname,O_RDONLY|O_NONBLOCK);
        if (rpipes[i]==-1)
            throw new cRuntimeError("cNamedPipeCommunications: cannot open pipe '%s' for read: %s", fname, strerror(errno));

        if (rpipes[i] > maxFdPlus1)
            maxFdPlus1 = rpipes[i];
    }
    maxFdPlus1 += 1;

    // open pipes for write
    wpipes = new int[numPartitions];
    for (i=0; i<numPartitions; i++)
    {
        if (i==myProcId)
        {
            wpipes[i] = -1;
            continue;
        }

        char fname[256];
        sprintf(fname,"%spipe-%d-%d", prefix.buffer(), i, myProcId);
        ev.printf("cNamedPipeCommunications: opening pipe '%s' for write...\n", fname);
        wpipes[i] = open(fname,O_WRONLY);
        for (int k=0; k<30 && wpipes[i]==-1; k++)
        {
            sleep(1);
            wpipes[i] = open(fname,O_WRONLY);
        }
        if (wpipes[i]==-1)
            throw new cRuntimeError("cNamedPipeCommunications: cannot open pipe '%s' for write: %s", fname, strerror(errno));
    }
}

void cNamedPipeCommunications::shutdown()
{
    for (int i=0; i<numPartitions; i++)
    {
        close(rpipes[i]);
        close(wpipes[i]);
    }
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
    int fd = wpipes[destination];

    struct PipeHeader ph;
    ph.tag = tag;
    ph.contentLength = b->getMessageSize();
    if (write(fd, &ph, sizeof(ph))==-1)
        throw new cRuntimeError("cNamedPipeCommunications: cannot write pipe to procId=%d: %s", destination, strerror(errno));
    if (write(fd, b->getBuffer(), ph.contentLength)==-1)
        throw new cRuntimeError("cNamedPipeCommunications: cannot write pipe to procId=%d: %s", destination, strerror(errno));
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

    // create file descriptor set for select() call
    int i;
    fd_set fdset;
    FD_ZERO(&fdset);
    for (i=0; i<numPartitions; i++)
        if (rpipes[i]!=-1)
            FD_SET(rpipes[i], &fdset);

    struct timeval tv;
    tv.tv_sec = blocking ? 1 : 0; // if blocking, wait 1 sec
    tv.tv_usec = 0;

    int ret = select(maxFdPlus1, &fdset, NULL, NULL, &tv);
    if (ret>0)
    {
        rrBase = (rrBase+1)%numPartitions;
        for (int k=0; k<numPartitions; k++)
        {
            i = (rrBase+k)%numPartitions; // shift by rrBase for Round-Robin query
            if (rpipes[i]!=-1 && FD_ISSET(rpipes[i],&fdset))
            {
                struct PipeHeader ph;
                if (readBytes(rpipes[i], &ph, sizeof(ph))==-1)
                    throw new cRuntimeError("cNamedPipeCommunications: cannot read from pipe "
                                            "to procId=%d: %s", sourceProcId, strerror(errno));

                sourceProcId = i;
                receivedTag = ph.tag;
                b->allocateAtLeast(ph.contentLength);
                b->setMessageSize(ph.contentLength);

                if (readBytes(rpipes[i], b->getBuffer(), ph.contentLength)==-1)
                    throw new cRuntimeError("cNamedPipeCommunications: cannot read from pipe "
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
    while (!receive(filtTag, buffer, receivedTag, sourceProcId, true))
    {
        if (ev.idle())
            return false;
    }
    return true;
}

bool cNamedPipeCommunications::receiveNonblocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId)
{
    return receive(filtTag, buffer, receivedTag, sourceProcId, false);
}

#endif /* (!USE_WINDOWS_PIPES) */


