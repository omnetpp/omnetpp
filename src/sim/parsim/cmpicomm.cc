//=========================================================================
//  CMPICOMM.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, 2003
//          Dept. of Electrical and Computer Systems Engineering,
//          Monash University, Melbourne, Australia
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <mpi.h>
#include "cmpicomm.h"
#include "cmpicommbuffer.h"
#include "globals.h"
#include "regmacros.h"
#include "cenvir.h"
#include "cconfiguration.h"
#include "platmisc.h"

USING_NAMESPACE

Register_Class(cMPICommunications);

Register_GlobalConfigEntry(CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER, "parsim-mpicommunications-mpibuffer", CFG_INT, NULL, "When cMPICommunications is selected as parsim communications class: specifies the size of the MPI communications buffer. The default is to calculate a buffer size based on the number of partitions.");

// default is 256k. If too small, simulation can block in MPI send calls.
#define MPI_SEND_BUFFER_PER_PARTITION (256*1024)


cMPICommunications::cMPICommunications()
{
    recycledBuffer = NULL;
}

cMPICommunications::~cMPICommunications()
{
    // do nothing: MPI_finalize() has been called in shutdown()
    delete recycledBuffer;
}

void cMPICommunications::init()
{
    // sanity check
    int argc = ev.getArgCount();
    char **argv = ev.getArgVector();
    for (int i=1; i<argc; i++)
        if (argv[i][0]=='-' && argv[i][1]=='p')
            ev.printf("WARNING: cMPICommunications doesn't need -p command-line option, ignored\n");

    // init MPI
    MPI_Init(&argc, &argv);

    // get numPartitions and myRank from MPI
    MPI_Comm_size(MPI_COMM_WORLD, &numPartitions);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    ev.printf("cMPICommunications: started as process %d out of %d.\n", myRank, numPartitions);
    if (numPartitions==1)
        ev.printf("WARNING: MPI thinks this process is the only one in the session "
                  "(did you use mpirun to start this program?)\n");

    // set up MPI send buffer (+16K prevents MPI_Buffer_attach() error if numPartitions==1)
    int defaultBufSize = MPI_SEND_BUFFER_PER_PARTITION * (numPartitions-1) + 16384;
    int bufSize = ev.getConfig()->getAsInt(CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER);
    if (bufSize<=0) bufSize = defaultBufSize;
    char *buf = new char[bufSize];
    MPI_Buffer_attach(buf, bufSize);
}

void cMPICommunications::shutdown()
{
    // wait a little before exiting MPI, to prevent peers getting killed by SIGPIPE
    // on a write before they would get a chance to process the broadcastException.
    usleep(1000000);  // 1s
    MPI_Finalize();
}

int cMPICommunications::getNumPartitions() const
{
    return numPartitions;
}

int cMPICommunications::getProcId() const
{
    return myRank;
}

cMPICommBuffer *cMPICommunications::doCreateCommBuffer()
{
    return new cMPICommBuffer();
}

cCommBuffer *cMPICommunications::createCommBuffer()
{
    // we pool only one reusable buffer -- additional buffers are created/deleted on demand
    cMPICommBuffer *buffer;
    if (recycledBuffer)
    {
        buffer = recycledBuffer;
        buffer->reset();
        recycledBuffer = NULL;
    }
    else
    {
        buffer = doCreateCommBuffer();
    }
    return buffer;
}

void cMPICommunications::recycleCommBuffer(cCommBuffer *buffer)
{
    // we pool only one reusable buffer -- additional buffer are created/deleted on demand
    if (!recycledBuffer)
        recycledBuffer = (cMPICommBuffer *)buffer;
    else
        delete buffer;
}

void cMPICommunications::send(cCommBuffer *buffer, int tag, int destination)
{
    cMPICommBuffer *b = (cMPICommBuffer *)buffer;
    // Note: we must use *buffered* send, otherwise we may block here and
    // cause deadlock
    int status = MPI_Bsend(b->getBuffer(), b->getMessageSize(), MPI_PACKED, destination, tag, MPI_COMM_WORLD);
    if (status!=MPI_SUCCESS)
        throw cRuntimeError("cMPICommunications::send(): MPI error %d", status);
}

void cMPICommunications::broadcast(cCommBuffer *buffer, int tag)
{
    // TBD make use of MPI call instead
    cParsimCommunications::broadcast(buffer, tag);
}

bool cMPICommunications::receiveBlocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId)
{
    // use MPI_Probe() to determine message size, then receive it
    cMPICommBuffer *b = (cMPICommBuffer *)buffer;
    MPI_Status status;
    int msgsize;
    if (filtTag==PARSIM_ANY_TAG) filtTag=MPI_ANY_TAG;
    MPI_Probe(MPI_ANY_SOURCE, filtTag, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_PACKED, &msgsize);
    b->allocateAtLeast(msgsize);
    int err = MPI_Recv(b->getBuffer(), b->getBufferLength(), MPI_PACKED, MPI_ANY_SOURCE, filtTag, MPI_COMM_WORLD, &status);
    if (err!=MPI_SUCCESS)
        throw cRuntimeError("cMPICommunications::receiveBlocking(): MPI error %d", err);
    b->setMessageSize(msgsize);
    receivedTag = status.MPI_TAG;
    sourceProcId = status.MPI_SOURCE;
    return true;
}

bool cMPICommunications::receiveNonblocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId)
{
    // probe if we have something to receive ...
    cMPICommBuffer *b = (cMPICommBuffer *)buffer;
    MPI_Status status;
    int flag;
    if (filtTag==PARSIM_ANY_TAG) filtTag=MPI_ANY_TAG;
    MPI_Iprobe(MPI_ANY_SOURCE, filtTag, MPI_COMM_WORLD, &flag, &status);

    // ... and receive it if we do
    if (flag)
    {
        int msgsize;
        MPI_Get_count(&status, MPI_PACKED, &msgsize);
        b->allocateAtLeast(msgsize);
        int err = MPI_Recv(b->getBuffer(), b->getBufferLength(), MPI_PACKED, MPI_ANY_SOURCE, filtTag, MPI_COMM_WORLD, &status);
        if (err!=MPI_SUCCESS)
            throw cRuntimeError("cMPICommunications::receiveNonBlocking(): MPI error %d", err);
        b->setMessageSize(msgsize);
        receivedTag = status.MPI_TAG;
        sourceProcId = status.MPI_SOURCE;
        return true;
    }
    return false;
}


