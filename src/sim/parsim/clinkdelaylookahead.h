//=========================================================================
//  CLINKDELAYLOOKAHEAD.H - part of
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

#ifndef __CLINKDELAYLOOKAHEAD_H__
#define __CLINKDELAYLOOKAHEAD_H__

#include "cnmplookahead.h"

/**
 * Lookahead calculation based on inter-partition link delays only.
 *
 * @ingroup Parsim
 */
class cLinkDelayLookahead : public cNMPLookahead
{
  protected:
    struct PartitionInfo
    {
        double minDelay;    // minimum of all link delays to given partition
    };

    // partition information
    int numSeg;            // number of partitions
    PartitionInfo *segInfo;  // partition info array, size numSeg

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
    virtual double getCurrentLookahead(cMessage *msg, int procId, void *data);

    /**
     * Returns minimum of link delays toward the given partition.
     */
    virtual double getCurrentLookahead(int procId);
};

#endif


