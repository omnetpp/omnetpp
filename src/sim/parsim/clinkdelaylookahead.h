//=========================================================================
//  CLINKDELAYLOOKAHEAD.H - part of
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

#ifndef __CLINKDELAYLOOKAHEAD_H__
#define __CLINKDELAYLOOKAHEAD_H__

#include "cnmplookahead.h"

NAMESPACE_BEGIN

class cGate;

/**
 * Lookahead calculation based on inter-partition link delays only.
 *
 * @ingroup Parsim
 */
class SIM_API cLinkDelayLookahead : public cNMPLookahead
{
  protected:
    struct PartitionInfo
    {
        simtime_t minDelay;  // minimum of all link delays to given partition
    };

    // partition information
    int numSeg;              // number of partitions
    PartitionInfo *segInfo;  // partition info array, size numSeg

    // calculate the total delay along the path ending the given gate
    simtime_t collectPathDelay(cGate *pathEndGate);

  public:
    /**
     * Constructor.
     */
    cLinkDelayLookahead();

    /**
     * Destructor.
     */
    virtual ~cLinkDelayLookahead();

    /**
     * Sets up algorithm for new simulation run.
     */
    virtual void startRun();

    /**
     * Called at end of simulation run.
     */
    virtual void endRun();

    /**
     * Updates lookahead information, based on the delay of the link
     * where message is sent out. Returns EOT.
     */
    virtual simtime_t getCurrentLookahead(cMessage *msg, int procId, void *data);

    /**
     * Returns minimum of link delays toward the given partition.
     */
    virtual simtime_t getCurrentLookahead(int procId);
};

NAMESPACE_END


#endif


