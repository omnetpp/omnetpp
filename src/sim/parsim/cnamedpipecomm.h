//=========================================================================
//
// CNAMEDPIPECOMM.H - part of
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


#include "stdio.h"
#include "util.h"
#include "parsim/cparsimcomm.h"

#ifndef __CNAMEDPIPECOMM_H__
#define __CNAMEDPIPECOMM_H__

#if defined(_WIN32) && !defined(__CYGWIN32__)
#define MEAN_AND_LEAN
#include <windows.h>
typedef HANDLE PIPE;
#else
typedef int PIPE;
#endif


/**
 * Implementation of the communications layer which uses named pipes.
 * Pipes are created at initialization time, and are used throughout
 * the whole simulation.
 */
class cNamedPipeCommunications : public cParsimCommunications
{
  protected:
    int numPartitions;
    int myProcId;

    opp_string prefix;
    PIPE *rpipes;
    PIPE *wpipes;
    int maxFdPlus1;
    int rrBase;

  protected:
    // common impl. for receiveBlocking() and receiveNonblocking()
    bool receive(cCommBuffer *buffer, int& receivedTag, int& sourceProcId, bool blocking);

  public:
    /**
     * Constructor.
     */
    cNamedPipeCommunications();

    /**
     * Destructor.
     */
    virtual ~cNamedPipeCommunications();

    /** @name Redefined methods from cParsimCommunications */
    //@{
    /**
     * Init the library. Here we create and open the named pipes.
     */
    virtual void init();

    /**
     * Shutdown the communications library. Closes and removes the named pipes.
     */
    virtual void shutdown();

    /**
     * Returns total number of partitions.
     */
    virtual int getNumPartitions();

    /**
     * Returns the id of this partition.
     */
    virtual int getProcId();

    /**
     * Creates an empty buffer of type cMemCommBuffer.
     */
    virtual cCommBuffer *createCommBuffer();

    /**
     * Recycle communication buffer after use.
     */
    virtual void recycleCommBuffer(cCommBuffer *buffer);

    /**
     * Sends packed data with given tag to destination.
     */
    virtual void send(cCommBuffer *buffer, int tag, int destination);

    /**
     * Sends packed data with given tag to all partitions.
     */
    virtual void broadcast(cCommBuffer *buffer, int tag);

    /**
     * Receives packed data, and also returns tag and source procId.
     */
    virtual void receiveBlocking(cCommBuffer *buffer, int& receivedTag, int& sourceProcId);

    /**
     * Receives packed data, and also returns tag and source procId.
     * Call is non-blocking -- it returns true if something has been
     * received, false otherwise.
     */
    virtual bool receiveNonblocking(cCommBuffer *buffer,  int& receivedTag, int& sourceProcId);

    /**
     * Blocks until all partitions call the same method. This is not implemented,
     * and will throw an exception when called.
     */
    virtual void synchronize();
    //@}
};

#endif


