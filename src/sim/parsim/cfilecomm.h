//=========================================================================
//  CFILECOMM.H - part of
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

#ifndef __OMNETPP_CFILECOMM_H
#define __OMNETPP_CFILECOMM_H

#include "omnetpp/cparsimcomm.h"
#include "omnetpp/simutil.h"

namespace omnetpp {


/**
 * @brief Implementation of the communications layer which works via files.
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
    cSimulation *simulation = nullptr;
    int numPartitions = -1;
    int myProcId = -1;

    int seqNum = 0;
    opp_string commDirPrefix;
    opp_string readDirPrefix;
    bool preserveReadFiles = false;

  public:
    /**
     * Constructor.
     */
    cFileCommunications();

    /**
     * Destructor.
     */
    virtual ~cFileCommunications() {}

    /** @name Redefined methods from cParsimCommunications */
    //@{
    /**
     * Init the library.
     */
    virtual void configure(cSimulation *simulation, cConfiguration *cfg, int numPartitions, int procId) override;

    /**
     * Shutdown the communications library.
     */
    virtual void shutdown() override;

    /**
     * Returns the associated simulation instance.
     */
    cSimulation *getSimulation() const override {return simulation;}

    /**
     * Returns total number of partitions.
     */
    virtual int getNumPartitions() const override;

    /**
     * Returns the id of this partition.
     */
    virtual int getProcId() const override;

    /**
     * Sending pointers is not supported.
     */
    virtual bool supportsTransferringPointers() override {return false;}

    /**
     * Serializes the message into the buffer.
     */
    virtual bool packMessage(cCommBuffer *buffer, cMessage *msg, int destProcId) override;

    /**
     * Deserializes the message from the buffer.
     */
    virtual cMessage *unpackMessage(cCommBuffer *buffer) override;

    /**
     * Creates an empty buffer of type cFileCommBuffer.
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


