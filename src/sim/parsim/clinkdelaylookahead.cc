//=========================================================================
//  CLINKDELAYLOOKAHEAD.CC - part of
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

#include "omnetpp/csimulation.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cparsimcomm.h"
#include "omnetpp/cdataratechannel.h"
#include "omnetpp/globals.h"
#include "omnetpp/regmacros.h"
#include "clinkdelaylookahead.h"
#include "cnullmessageprot.h"
#include "cparsimpartition.h"
#include "cplaceholdermod.h"
#include "cproxygate.h"

namespace omnetpp {

Register_Class(cLinkDelayLookahead);

cLinkDelayLookahead::~cLinkDelayLookahead()
{
    delete[] segInfo;
}

void cLinkDelayLookahead::startRun()
{
    EV << "starting Link Delay Lookahead...\n";

    delete[] segInfo;

    numSeg = comm->getNumPartitions();
    segInfo = new PartitionInfo[numSeg];
    int myProcId = comm->getProcId();

    // temporarily initialize everything to zero.
    for (int i = 0; i < numSeg; i++)
        segInfo[i].minDelay = -1;

    // fill in minDelays
    EV << "  calculating minimum link delays...\n";
    for (int modId = 0; modId <= sim->getLastComponentId(); modId++) {
        cPlaceholderModule *mod = dynamic_cast<cPlaceholderModule *>(sim->getModule(modId));
        if (mod) {
            for (cModule::GateIterator it(mod); !it.end(); ++it) {
                // if this is a properly connected proxygate, process it
                cGate *g = *it;
                cProxyGate *pg = dynamic_cast<cProxyGate *>(g);
                if (pg && !pg->getPathStartGate()->getOwnerModule()->isPlaceholder()) {
                    ASSERT(pg->getRemoteProcId() >= 0);
                    // check we have a delay on this link (it gives us lookahead)
                    simtime_t linkDelay = collectPathDelay(pg);
                    if (linkDelay <= 0.0)
                        throw cRuntimeError("cLinkDelayLookahead: Zero delay on path that ends at proxy gate '%s', no lookahead for parallel simulation",
                                pg->getFullPath().c_str());

                    // store
                    int procId = pg->getRemoteProcId();
                    if (segInfo[procId].minDelay == -1 || segInfo[procId].minDelay > linkDelay)
                        segInfo[procId].minDelay = linkDelay;
                }
            }
        }
    }

    // if two partitions are not connected, the lookeahead is "infinity"
    for (int i = 0; i < numSeg; i++)
        if (i != myProcId && segInfo[i].minDelay == -1)
            segInfo[i].minDelay = SIMTIME_MAX;


    for (int i = 0; i < numSeg; i++)
        if (i != myProcId)
            EV << "    lookahead to procId=" << i << " is " << segInfo[i].minDelay << "\n";


    EV << "  setup done.\n";
}

simtime_t cLinkDelayLookahead::collectPathDelay(cGate *g)
{
    simtime_t sum = 0;
    while (g->getPreviousGate()) {
        cGate *prevg = g->getPreviousGate();
        cChannel *chan = prevg->getChannel();
        simtime_t delay = (chan && chan->hasPar("delay")) ? chan->par("delay").doubleValue() : 0.0;
        sum += delay;
        g = prevg;
    }
    return sum;
}

void cLinkDelayLookahead::endRun()
{
    delete[] segInfo;
    segInfo = nullptr;
}

simtime_t cLinkDelayLookahead::getCurrentLookahead(cMessage *, int procId, void *)
{
    return segInfo[procId].minDelay;
}

simtime_t cLinkDelayLookahead::getCurrentLookahead(int procId)
{
    return segInfo[procId].minDelay;
}

}  // namespace omnetpp

