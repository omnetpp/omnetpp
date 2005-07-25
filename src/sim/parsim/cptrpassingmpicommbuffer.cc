//=========================================================================
//  CPTRPASSINGMPICOMMBUFFER.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2005
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <mpi.h>
#include "cmpicommbuffer.h"
#include "macros.h"

Register_Class(cPtrPassingMPICommBuffer);


cPtrPassingMPICommBuffer::cPtrPassingMPICommBuffer()
{
}

void cPtrPassingMPICommBuffer::packObject(cObject *obj)
{
    long d = (void *)obj;
    extendBufferFor(sizeof(long));
    if (MPI_Pack(&d, 1, MPI_LONG, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cRuntimeError("cPtrPassingMPICommBuffer::packObject(): MPI_Pack() returned error");
}

cObject *cPtrPassingMPICommBuffer::unpackObject()
{
    long d;
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, &d, 1, MPI_LONG, MPI_COMM_WORLD))
        throw new cRuntimeError("cPtrPassingMPICommBuffer::unpackObject(): MPI_Unpack() returned error");
    return (cObject *)(void *)d;
}

