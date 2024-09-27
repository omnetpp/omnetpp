//=========================================================================
//  CMPICOMM.H - part of
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

#ifndef __OMNETPP_CMPICOMM_H
#define __OMNETPP_CMPICOMM_H

#include "omnetpp/cparsimcomm.h"

namespace omnetpp {

class cMPICommBuffer;

/**
 * @brief MPI implementation of the basic parallel simulation communications layer.
 *
 * @ingroup Parsim
 */
class SIM_API cMPICommunications : public cParsimCommunications
{
  protected:
    cMPICommBuffer *recycledBuffer;
    int numPartitions;
    int myRank;

  protected:
    /**
     * Factory method behind createCommBuffer()
     */
    virtual cMPICommBuffer *doCreateCommBuffer();

  public:
    /**
     * Constructor.
     */
    cMPICommunications();

    /**
     * Destructor calls MPI_Finalize()
     */
    virtual ~cMPICommunications();

    /** @name Redefined methods from cParsimCommunications */
    //@{
    /**
     * Init the library.
     */
    virtual void init(int numPartitions) override;

    /**
     * Shutdown the communications library.
     */
    virtual void shutdown() override;

    /**
     * Returns total number of partitions.
     */
    virtual int getNumPartitions() const override;

    /**
     * Returns the MPI rank of this partition.
     */
    virtual int getProcId() const override;

    /**
     * Creates an empty buffer of type cMPICommBuffer.
     */
    virtual cCommBuffer *createCommBuffer() override;

    /**
     * Recycle communication buffer after use.
     */
    virtual void recycleCommBuffer(cCommBuffer *buffer) override;

    /**
     * Sends packed data with given tag to destination.
     */
    virtual void send(cCommBuffer *buffer, int tag, int destination) override;

    /**
     * Sends packed data with given tag to all partitions.
     */
    virtual void broadcast(cCommBuffer *buffer, int tag) override;

    /**
     * Receives packed data with given tag from given destination.
     * Normally returns true; false is returned if blocking was interrupted by the user.
     */
    virtual bool receiveBlocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourceProcId) override;

    /**
     * Receives packed data with given tag from given destination.
     * Call is non-blocking -- it returns true if something has been
     * received, false otherwise.
     */
    virtual bool receiveNonblocking(int filtTag, cCommBuffer *buffer,  int& receivedTag, int& sourceProcId) override;
    //@}
};

}  // namespace omnetpp


#endif

#endif
