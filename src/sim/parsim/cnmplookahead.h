//=========================================================================
//  CNMPLOOKAHEAD.H - part of
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

#ifndef __CNMPLOOKAHEAD_H__
#define __CNMPLOOKAHEAD_H__

#include "cpolymorphic.h"

// forward declarations
class cMessage;
class cSimulation;
class cParsimCommunications;
class cParsimPartition;


/**
 * Base class of lookahead calculations for cNullMessageProtocol
 * which implements the "null message algorithm".
 *
 * @ingroup Parsim
 */
class cNMPLookahead : public cPolymorphic
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
    virtual double getCurrentLookahead(cMessage *msg, int procId, void *data) = 0;

    /**
     * Returns current lookahead.
     */
    virtual double getCurrentLookahead(int procId) = 0;

};

#endif
