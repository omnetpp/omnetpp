//=========================================================================
//  CPARSIMPROTOCOLBASE.H - part of
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

#ifndef __OMNETPP_CPARSIMPROTOCOLBASE_H
#define __OMNETPP_CPARSIMPROTOCOLBASE_H

#include "cparsimsynchr.h"

namespace omnetpp {

class cCommBuffer;

/**
 * @brief Contains utility functions for implementing parallel simulation
 * protocols.
 *
 * @ingroup Parsim
 */
class SIM_API cParsimProtocolBase : public cParsimSynchronizer
{
  protected:
    // process whatever comes from other partitions -- nonblocking
    virtual void receiveNonblocking();

    // process whatever comes from other partitions -- blocking
    // (normally returns true; false is returned if blocking was interrupted by the user)
    virtual bool receiveBlocking();

    // process buffers coming from other partitions
    virtual void processReceivedBuffer(cCommBuffer *buffer, int tag, int sourceProcId);

    // process cMessages received from other partitions
    virtual void processReceivedMessage(cMessage *msg, const SendOptions& options, int destModuleId, int destGateId, int sourceProcId);

    // utility
    SendOptions unpackOptions(cCommBuffer *buffer);
    void packOptions(cCommBuffer *buffer, const SendOptions& options);

  public:
    /**
     * Constructor.
     */
    cParsimProtocolBase() {}

    /**
     * Destructor.
     */
    virtual ~cParsimProtocolBase() {}

    /**
     * Performs no optimization, just sends out the cMessage to the given partition.
     */
    virtual void processOutgoingMessage(cMessage *msg, const SendOptions& options, int procId, int moduleId, int gateId, void *data) override;
};

}  // namespace omnetpp


#endif

