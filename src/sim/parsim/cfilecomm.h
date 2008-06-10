//=========================================================================
//  CFILECOMM.H - part of
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

#ifndef __CFILECOMM_H__
#define __CFILECOMM_H__

#include "cparsimcomm.h"
#include "util.h"

NAMESPACE_BEGIN


/**
 * Implementation of the communications layer which works via files.
 * Every message is created as a file in a communications directory.
 *
 * It is VERY slow, and only useful for understanding, debugging or
 * demonstrating parallel simulation.
 *
 * @ingroup Parsim
 */
class SIM_API cFileCommunications : public cParsimCommunications
{
  protected:
    int numPartitions;
    int myProcId;

    int seqNum;
    opp_string commDirPrefix;
    opp_string readDirPrefix;
    bool preserveReadFiles;

  public:
    /**
     * Constructor.
     */
    cFileCommunications();

    /**
     * Destructor.
     */
    virtual ~cFileCommunications();

    /** @name Redefined methods from cParsimCommunications */
    //@{
    /**
     * Init the library.
     */
    virtual void init();

    /**
     * Shutdown the communications library.
     */
    virtual void shutdown();

    /**
     * Returns total number of partitions.
     */
    virtual int getNumPartitions() const;

    /**
     * Returns the id of this partition.
     */
    virtual int getProcId() const;

    /**
     * Creates an empty buffer of type cFileCommBuffer.
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
     * Receives packed data with given tag from given destination.
     * Normally returns true; false is returned if blocking was interrupted by the user.
     */
    virtual bool receiveBlocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId);

    /**
     * Receives packed data with given tag from given destination.
     * Call is non-blocking -- it returns true if something has been
     * received, false otherwise.
     */
    virtual bool receiveNonblocking(int filtTag, cCommBuffer *buffer,  int& receivedTag, int& sourceProcId);
    //@}
};

NAMESPACE_END


#endif


