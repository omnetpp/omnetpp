//=========================================================================
//  CLINKDELAYLOOKAHEAD.H - part of
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

#ifndef __OMNETPP_CLINKDELAYLOOKAHEAD_H
#define __OMNETPP_CLINKDELAYLOOKAHEAD_H

#include "cnmplookahead.h"

namespace omnetpp {

class cGate;

/**
 * @brief Lookahead calculation based on inter-partition link delays only.
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
    int numSeg = 0;                    // number of partitions
    PartitionInfo *segInfo = nullptr;  // partition info array, size numSeg

    // calculate the total delay along the path ending the given gate
    simtime_t collectPathDelay(cGate *pathEndGate);

  public:
    /**
     * Constructor.
     */
    cLinkDelayLookahead() {}

    /**
     * Destructor.
     */
    virtual ~cLinkDelayLookahead();

    /**
     * Sets up algorithm for new simulation run.
     */
    virtual void startRun() override;

    /**
     * Called at end of simulation run.
     */
    virtual void endRun() override;

    /**
     * Updates lookahead information, based on the delay of the link
     * where message is sent out. Returns EOT.
     */
    virtual simtime_t getCurrentLookahead(cMessage *msg, int procId, void *data) override;

    /**
     * Returns minimum of link delays toward the given partition.
     */
    virtual simtime_t getCurrentLookahead(int procId) override;
};

}  // namespace omnetpp


#endif


