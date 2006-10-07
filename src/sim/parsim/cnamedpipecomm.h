//=========================================================================
//  CNAMEDPIPECOMM.H - part of
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


#ifndef __CNAMEDPIPECOMM_H__
#define __CNAMEDPIPECOMM_H__


#include <stdio.h>

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <deque>

#include "util.h"
#include "opp_string.h"
#include "cparsimcomm.h"


// decide platform
#if defined(_WIN32)
#define USE_WINDOWS_PIPES
#endif


#ifdef USE_WINDOWS_PIPES
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
 *
 * @ingroup Parsim
 */
class cNamedPipeCommunications : public cParsimCommunications
{
  protected:
    int numPartitions;
    int myProcId;

    // pipes
    opp_string prefix;
    PIPE *rpipes;
    PIPE *wpipes;
    int maxFdPlus1;
    int rrBase;

    // reordering buffer needed because of tag filtering support (filtTag)
    std::deque<cCommBuffer*> storedBuffers;

  protected:
    // common impl. for receiveBlocking() and receiveNonblocking()
    bool receive(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId, bool blocking);
    bool doReceive(cCommBuffer *buffer, int& receivedTag, int& sourceProcId, bool blocking);

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
     * Receives packed data, and also returns tag and source procId.
     * Normally returns true; false is returned if blocking was interrupted by the user.
     */
    virtual bool receiveBlocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId);

    /**
     * Receives packed data, and also returns tag and source procId.
     * Call is non-blocking -- it returns true if something has been
     * received, false otherwise.
     */
    virtual bool receiveNonblocking(int filtTag, cCommBuffer *buffer,  int& receivedTag, int& sourceProcId);
    //@}
};

#endif


