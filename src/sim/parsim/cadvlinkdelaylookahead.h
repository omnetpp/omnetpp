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

#ifndef __OMNETPP_CADVLINKDELAYLOOKAHEAD_H
#define __OMNETPP_CADVLINKDELAYLOOKAHEAD_H

#include "cnmplookahead.h"

namespace omnetpp {

/**
 * @brief Lookahead calculation based on inter-partition link delays only.
 *
 * @ingroup Parsim
 */
class SIM_API cAdvancedLinkDelayLookahead : public cNMPLookahead
{
  protected:
    struct LinkOut
    {
        simtime_t lookahead; // lookahead on this link (currently the link delay)
        simtime_t eot;       // current EOT on this link (last msg sent + lookahead)
    };
    struct PartitionInfo
    {
        int numLinks;        // size of links[] array
        LinkOut **links;     // information on outgoing links (needed for EOT calculation)
        simtime_t lookahead; // lookahead to partition (minimum of all link lookaheads)
    };

    // partition information
    int numSeg;              // number of partitions
    PartitionInfo *segInfo;  // partition info array, size numSeg

  public:
    /**
     * Constructor.
     */
    cAdvancedLinkDelayLookahead();

    /**
     * Destructor.
     */
    virtual ~cAdvancedLinkDelayLookahead();

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

}  // namespace omnetpp


#endif


