//=========================================================================
//
// CMPICOMM.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003 Andras Varga
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <mpi.h>
#include "cmpicomm.h"
#include "cmpicommbuffer.h"
#include "macros.h"
#include "cenvir.h"

Register_Class(cMPICommunications);

// default 128k. FIXME make this configurable! if too small, simulation
// can block in MPI send calls.
#define PER_SEGMENT_MPI_SEND_BUFFER (256*1024)


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
    MPI_Init(0,0);
    MPI_Comm_size(MPI_COMM_WORLD, &numPartitions);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    // set up MPI send buffer
    int bufSize = PER_SEGMENT_MPI_SEND_BUFFER * (numPartitions-1);
    char *buf = new char[bufSize];
    MPI_Buffer_attach(buf, bufSize);

    ev.printf("cMPICommunications: started as process %d out of %d.\n", myRank, numPartitions);
}

void cMPICommunications::shutdown()
{
    MPI_Finalize();
}

int cMPICommunications::getNumPartitions()
{
    return numPartitions;
}

int cMPICommunications::getProcId()
{
    return myRank;
}

cCommBuffer *cMPICommunications::createCommBuffer()
{
    // we pool only one reusable buffer -- additional buffers are created/deleted on demand
    cMPICommBuffer *buffer;
    if (recycledBuffer)
    {
        buffer = (cMPICommBuffer *)recycledBuffer;
        buffer->reset();
        recycledBuffer = NULL;
    }
    else
    {
        buffer = new cMPICommBuffer();
    }
    return buffer;
}

void cMPICommunications::recycleCommBuffer(cCommBuffer *buffer)
{
    // we pool only one reusable buffer -- additional buffer are created/deleted on demand
    if (!recycledBuffer)
        recycledBuffer = buffer;
    else
        delete buffer;
}

void cMPICommunications::send(cCommBuffer *buffer, int tag, int destination)
{
    cMPICommBuffer *b = (cMPICommBuffer *)buffer;
    // Note: we must use *buffered* send, otherwise we may block here and
    // cause deadlock
    int status = MPI_Bsend(b->getBuffer(), b->getMessageSize(),
                                 MPI_PACKED, destination, tag, MPI_COMM_WORLD);
    if (status != MPI_SUCCESS)
        throw new cException("cMPICommunications::send(): MPI error %d", status);
}

void cMPICommunications::broadcast(cCommBuffer *buffer, int tag)
{
    // FIXME: could be done better...
    for (int i=0; i<numPartitions; i++)
        if (myRank != i)
            send(buffer, tag, i);
}

void cMPICommunications::receiveBlocking(cCommBuffer *buffer, int& receivedTag, int& sourceProcId)
{
    // use MPI_Probe() to determine message size, then receive it
    cMPICommBuffer *b = (cMPICommBuffer *)buffer;
    MPI_Status status;
    int msgsize;
    MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_PACKED, &msgsize);
    b->allocateAtLeast(msgsize);
    int err = MPI_Recv(b->getBuffer(), b->getBufferLength(), MPI_PACKED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    if (err != MPI_SUCCESS)
        throw new cException("cMPICommunications::receiveBlocking(): MPI error %d", err);
    b->setMessageSize(msgsize);
    receivedTag = status.MPI_TAG;
    sourceProcId = status.MPI_SOURCE;
}

bool cMPICommunications::receiveNonblocking(cCommBuffer *buffer, int& receivedTag, int& sourceProcId)
{
    // probe if we have something to receive ...
    cMPICommBuffer *b = (cMPICommBuffer *)buffer;
    MPI_Status status;
    int flag;
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);

    // ... and receive it if we do
    if (flag)
    {
        int msgsize;
        MPI_Get_count(&status, MPI_PACKED, &msgsize);
        b->allocateAtLeast(msgsize);
        int err = MPI_Recv(b->getBuffer(), b->getBufferLength(), MPI_PACKED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        if (err != MPI_SUCCESS)
            throw new cException("cMPICommunications::receiveNonBlocking(): MPI error %d", err);
        b->setMessageSize(msgsize);
        receivedTag = status.MPI_TAG;
        sourceProcId = status.MPI_SOURCE;
        return true;
    }
    return false;
}

void cMPICommunications::synchronize()
{
    int status = MPI_Barrier(MPI_COMM_WORLD);
    if (status != MPI_SUCCESS)
        throw new cException("cMPICommunications::synchronize(): MPI error %d", status);
}


