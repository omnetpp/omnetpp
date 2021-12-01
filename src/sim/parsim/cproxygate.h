//=========================================================================
//  CPROXYGATE.H - part of
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

#ifndef __OMNETPP_CPROXYGATE_H
#define __OMNETPP_CPROXYGATE_H

#include "omnetpp/cgate.h"

namespace omnetpp {

class cParsimPartition;

/**
 * @brief A gate that belongs to a cross-partition link and represents
 * the remote gate on the local partition. cProxyGate's belong to
 * cPlaceholderModule objects, which represent a "remote" module
 * in the local partition.
 *
 * This class basically exists so that we can override the deliver()
 * method of cGate so that it doesn't pass arriving messages
 * to the module object but to parallel simulation layer (cParsimPartition)
 * instead -- so cParsimPartition can deliver them to the remote partition.
 *
 * cProxyGate contains the address of the corresponding remote gate as
 * a (procId, moduleId, gateId) triplet:
 *
 * - procId is a partition identifier (also used by cParsimCommunications;
 *   in the MPI implementation of cParsimCommunications it maps to MPI_rank)
 *
 * - moduleId and gateId identify the target ("to-") gate on the remote partition.
 *
 * @ingroup Parsim
 */
class SIM_API cProxyGate : public cGate // noncopyable
{
    friend class cPlaceholderModule;
  private:
    cParsimPartition *partition = nullptr;
    short remoteProcId = -1;
    int remoteModuleId = -1;
    int remoteGateId = -1;
    void *data = nullptr;

  protected:
    // internal: constructor is protected because only cPlaceholderModule
    // is allowed to create instances
    explicit cProxyGate();

  public:
    /** @name Redefined cObject member functions */
    //@{
    /**
     * Redefined here to display remoteProcId, remoteModId and remoteGateId.
     */
    virtual std::string str() const override;
    //@}

    /** @name Redefined cGate member functions */
    //@{
    /**
     * Redefined to pass message to the parallel simulation layer,
     * cParsimPartition.
     *
     * Invokes the cParsimPartition::processOutgoingMessage() method
     * to transmit the message, then deletes the message object.
     */
    virtual bool deliver(cMessage *msg, const SendOptions& options, simtime_t at) override;
    //@}

    /** @name Address of remote gate */
    //@{

    /**
     * Sets remote gate address.
     */
    void setRemoteGate(short procId, int moduleId, int gateId);

    /**
     * Returns partition where remote gate resides.
     */
    short getRemoteProcId()   {return remoteProcId;}

    /**
     * Returns module Id in remote partition where remote gate resides.
     */
    int getRemoteModuleId() {return remoteModuleId;}

    /**
     * Returns Id of remote gate in remote partition and remote module.
     */
    int getRemoteGateId()   {return remoteGateId;}
    //@}

    /** @name cParsimPartition pointer */
    //@{
    /**
     * Sets cParsimPartition object.
     */
    void setPartition(cParsimPartition *seg) {partition=seg;}
    /**
     * Returns pointer to cParsimPartition object.
     */
    cParsimPartition *getPartition()   {return partition;}
    //@}

    /** @name Generic associated data pointer */
    //@{
    /**
     * Sets the data pointer in this object. This pointer is used by the
     * parallel simulation synchronizaton layer (cParsimSynchronizer)
     * to store additional information about the gate.
     */
    void setSynchData(void *data) {this->data = data;}
    /**
     * Returns the data pointer in this object.
     */
    void *getSynchData()   {return data;}
    //@}
};

}  // namespace omnetpp


#endif

