//=========================================================================
//  CPROXYGATE.H - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2005 Andras Varga
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CPROXYGATE_H
#define __CPROXYGATE_H

#include "cgate.h"

// forward declaration
class cParsimPartition;

/**
 * A gate that belongs to a cross-partition link and represents
 * the remote gate on the local partition. cProxyGate's belong to
 * cPlaceHolderModule objects, which represent a "remote" module
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
class cProxyGate : public cGate
{
  private:
    cParsimPartition *partition;
    int remoteProcId;
    int remoteModuleId;
    int remoteGateId;
    void *data;

  public:
    /** @name Constructor, destructor, copying */
    //@{
    /**
     * Constructor.
     */
    cProxyGate(const char *name, char tp);

    /**
     * Copy constructor.
     */
    cProxyGate(const cProxyGate& gate);

    /**
     * Destructor.
     */
    virtual ~cProxyGate()  {}
    //@}

    /** @name Redefined cObject member functions */
    //@{
    /**
     * Duplicates the gate object.
     */
    virtual cPolymorphic *dup() const   {return new cProxyGate(*this);}

    /**
     * Assigment operator.
     */
    cProxyGate& operator=(const cProxyGate& gate);

    /**
     * Redefined here to display remoteProcId, remoteModId and remoteGateId.
     */
    virtual std::string info() const;
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
    virtual bool deliver(cMessage *msg, simtime_t at);
    //@}

    /** @name Address of remote gate */
    //@{

    /**
     * Sets remote gate address.
     */
    void setRemoteGate(int procId, int moduleId, int gateId);

    /**
     * Returns partition where remote gate resides.
     */
    int getRemoteProcId()   {return remoteProcId;}

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

#endif

