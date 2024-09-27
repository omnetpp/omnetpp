//=========================================================================
//  CMPICOMM.CC - part of
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

#include "omnetpp/platdep/config.h"

#ifdef WITH_MPI

#include <mpi.h>  // Intel MPI wants <mpi.h> to precede <cstdio>
#include <cstdio>
#include "omnetpp/clog.h"
#include "omnetpp/globals.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/platdep/platmisc.h"
#include "cmpicomm.h"
#include "cmpicommbuffer.h"

namespace omnetpp {

Register_Class(cMPICommunications);

Register_GlobalConfigOption(CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER, "parsim-mpicommunications-mpibuffer", CFG_INT, nullptr, "When `cMPICommunications` is selected as parsim communications class: specifies the size of the MPI communications buffer. The default is to calculate a buffer size based on the number of partitions.");

// default is 256k. If too small, simulation can block in MPI send calls.
#define MPI_SEND_BUFFER_PER_PARTITION    (256*1024)

cMPICommunications::cMPICommunications()
{
    recycledBuffer = nullptr;
}

cMPICommunications::~cMPICommunications()
{
    // do nothing: MPI_finalize() has been called in shutdown()
    delete recycledBuffer;
}

void cMPICommunications::init(int np)
{
    // store parameter
    numPartitions = np;

    // sanity check
    int argc = getEnvir()->getArgCount();
    char **argv = getEnvir()->getArgVector();
    for (int i = 1; i < argc; i++)
        if (argv[i][0] == '-' && argv[i][1] == 'p')
            EV_WARN << "cMPICommunications doesn't need -p command-line option, ignored\n";

    // init MPI
    MPI_Init(&argc, &argv);

    // get group size and myRank from MPI
    int mpiGroupSize;
    MPI_Comm_size(MPI_COMM_WORLD, &mpiGroupSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    EV << "cMPICommunications: started as process " << myRank << " out of " << mpiGroupSize << ".\n";
    if (mpiGroupSize == 1)
        EV_WARN << "MPI thinks this process is the only one in the session (did you use mpirun to start this program?)\n";
    if (numPartitions != mpiGroupSize)
        throw cRuntimeError("cMPICommunications: The number of partitions configured (%d) differs from the number of instances started by MPI (%d)", numPartitions, mpiGroupSize);

    // set up MPI send buffer (+16K prevents MPI_Buffer_attach() error if numPartitions==1)
    int defaultBufSize = MPI_SEND_BUFFER_PER_PARTITION * (numPartitions-1) + 16384;
    int bufSize = getEnvir()->getConfig()->getAsInt(CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER);
    if (bufSize <= 0)
        bufSize = defaultBufSize;
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
    if (recycledBuffer) {
        buffer = recycledBuffer;
        buffer->reset();
        recycledBuffer = nullptr;
    }
    else {
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
    if (status != MPI_SUCCESS)
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
    if (filtTag == PARSIM_ANY_TAG)
        filtTag = MPI_ANY_TAG;
    MPI_Probe(MPI_ANY_SOURCE, filtTag, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_PACKED, &msgsize);
    b->allocateAtLeast(msgsize);
    // Note: Source needs to be specific source not MPI_SOURCE_ANY, see example here: http://www.mpi-forum.org/docs/mpi-11-html/node50.html
    int err = MPI_Recv(b->getBuffer(), b->getBufferLength(), MPI_PACKED, status.MPI_SOURCE, filtTag, MPI_COMM_WORLD, &status);
    if (err != MPI_SUCCESS)
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
    if (filtTag == PARSIM_ANY_TAG)
        filtTag = MPI_ANY_TAG;
    MPI_Iprobe(MPI_ANY_SOURCE, filtTag, MPI_COMM_WORLD, &flag, &status);

    // ... and receive it if we do
    if (flag) {
        int msgsize;
        MPI_Get_count(&status, MPI_PACKED, &msgsize);
        b->allocateAtLeast(msgsize);
        // Note: source needs to be specific source not MPI_SOURCE_ANY, see example here: http://www.mpi-forum.org/docs/mpi-11-html/node50.html
        int err = MPI_Recv(b->getBuffer(), b->getBufferLength(), MPI_PACKED, status.MPI_SOURCE, filtTag, MPI_COMM_WORLD, &status);
        if (err != MPI_SUCCESS)
            throw cRuntimeError("cMPICommunications::receiveNonBlocking(): MPI error %d", err);
        b->setMessageSize(msgsize);
        receivedTag = status.MPI_TAG;
        sourceProcId = status.MPI_SOURCE;
        return true;
    }
    return false;
}

}  // namespace omnetpp

#endif  // WITH_MPI

