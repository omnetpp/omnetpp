//=========================================================================
//  CNMPLOOKAHEAD.H - part of
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

#ifndef __OMNETPP_CNMPLOOKAHEAD_H
#define __OMNETPP_CNMPLOOKAHEAD_H

#include "omnetpp/cobject.h"
#include "omnetpp/simtime_t.h"

namespace omnetpp {

class cMessage;
class cSimulation;
class cParsimCommunications;
class cParsimPartition;


/**
 * @brief Base class of lookahead calculations for cNullMessageProtocol
 * which implements the "null message algorithm".
 *
 * @ingroup Parsim
 */
class SIM_API cNMPLookahead : public cObject
{
  protected:
    cSimulation *sim;
    cParsimCommunications *comm; // TBD this is just to access numPartitions -- optimize out?
    cParsimPartition *partition;
    // cNullMessageProtocol *nullmsgprot;

  public:
    /**
     * Constructor.
     */
    cNMPLookahead() {};

    /**
     * Destructor.
     */
    virtual ~cNMPLookahead() {}

    /**
     * Pass objects it has to cooperate with.
     */
    void setContext(cSimulation *simul, cParsimPartition *seg, cParsimCommunications *co)
       {sim = simul; partition = seg; comm = co;}

    /**
     * Hook called at start of simulation run.
     */
    virtual void startRun() = 0;

    /**
     * Hook called at end of simulation run.
     */
    virtual void endRun() = 0;

    /**
     * Performs lookahead calculation when a message gets sent out from the partition.
     */
    virtual simtime_t getCurrentLookahead(cMessage *msg, int procId, void *data) = 0;

    /**
     * Returns current lookahead.
     */
    virtual simtime_t getCurrentLookahead(int procId) = 0;

};

}  // namespace omnetpp


#endif
