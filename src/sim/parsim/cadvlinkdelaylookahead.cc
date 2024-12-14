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
#include "omnetpp/cchannel.h"
#include "omnetpp/globals.h"
#include "omnetpp/regmacros.h"
#include "cadvlinkdelaylookahead.h"
#include "cnullmessageprot.h"
#include "cparsimpartition.h"
#include "cplaceholdermod.h"
#include "cproxygate.h"

namespace omnetpp {

Register_Class(cAdvancedLinkDelayLookahead);

cAdvancedLinkDelayLookahead::cAdvancedLinkDelayLookahead()
{
    numSeg = 0;
    segInfo = nullptr;
}

cAdvancedLinkDelayLookahead::~cAdvancedLinkDelayLookahead()
{
    delete[] segInfo;
}

void cAdvancedLinkDelayLookahead::configure(cSimulation *simulation, cConfiguration *cfg, cParsimPartition *partition)
{
    this->simulation = simulation;
    this->partition = partition;
}

void cAdvancedLinkDelayLookahead::startRun()
{
    EV << "starting Link Delay Lookahead...\n";

    delete[] segInfo;

    cParsimCommunications *comm = partition->getCommunications();

    numSeg = comm->getNumPartitions();
    segInfo = new PartitionInfo[numSeg];

    // temporarily initialize everything to zero.
    for (int i = 0; i < numSeg; i++) {
        segInfo[i].numLinks = 0;
        segInfo[i].links = nullptr;
    }

    // fill numLinks and links[]
    EV << "  collecting links...\n";

    // step 1: count gates
    for (int modId = 0; modId <= simulation->getLastComponentId(); modId++) {
        cPlaceholderModule *mod = dynamic_cast<cPlaceholderModule *>(simulation->getModule(modId));
        if (mod) {
            for (cModule::GateIterator i(mod); !i.end(); i++) {
                cGate *g = i();
                cProxyGate *pg = dynamic_cast<cProxyGate *>(g);
                if (pg && pg->getPreviousGate() && pg->getRemotePartitionId() >= 0)
                    segInfo[pg->getRemotePartitionId()].numLinks++;
            }
        }
    }

    // step 2: allocate links[]
    for (int i = 0; i < numSeg; i++) {
        int numLinks = segInfo[i].numLinks;
        segInfo[i].links = new LinkOut *[numLinks];
        for (int k = 0; k < numLinks; k++)
            segInfo[i].links[k] = nullptr;
    }

    // step 3: fill in
    for (int modId = 0; modId <= simulation->getLastComponentId(); modId++) {
        cPlaceholderModule *mod = dynamic_cast<cPlaceholderModule *>(simulation->getModule(modId));
        if (mod) {
            for (cModule::GateIterator i(mod); !i.end(); i++) {
                // if this is a properly connected proxygate, process it
                // FIXME leave out gates from other cPlaceholderModules
                cGate *g = i();
                cProxyGate *pg = dynamic_cast<cProxyGate *>(g);
                if (pg && pg->getPreviousGate() && pg->getRemotePartitionId() >= 0) {
                    // check we have a delay on this link (it gives us lookahead)
                    cGate *fromg = pg->getPreviousGate();
                    cChannel *chan = fromg ? fromg->getChannel() : nullptr;
                    cDatarateChannel *datarateChan = dynamic_cast<cDatarateChannel *>(chan);
                    cPar *delaypar = datarateChan ? datarateChan->getDelay() : nullptr;
                    double linkDelay = delaypar ? delaypar->doubleValue() : 0;
                    if (linkDelay <= 0.0)
                        throw cRuntimeError("cAdvancedLinkDelayLookahead: Zero delay on link from gate '%s', no lookahead for parallel simulation", fromg->getFullPath().c_str());

                    // store
                    int partitionId = pg->getRemotePartitionId();
                    int k = 0;
                    while (segInfo[partitionId].links[k])
                        k++;  // find 1st empty slot
                    LinkOut *link = new LinkOut;
                    segInfo[partitionId].links[k] = link;
                    pg->setSynchData(link);
                    link->lookahead = linkDelay;
                    link->eot = 0.0;

                    EV << "    link " << k << " to partitionId=" << partitionId << " on gate '" << fromg->getFullPath() <<"': delay=" << linkDelay << "\n";
                }
            }
        }
    }

    EV << "  setup done.\n";
}

void cAdvancedLinkDelayLookahead::endRun()
{
}

simtime_t cAdvancedLinkDelayLookahead::getCurrentLookahead(cMessage *msg, int partitionId, void *data)
{
    // find LinkOut structure in segInfo[destPartitionId].
    LinkOut *link = (LinkOut *)data;
    if (!link)
        throw cRuntimeError("Internal parallel simulation error: cProxyGate has no associated data pointer");

    // calculate EOT
    simtime_t eot;
    simtime_t now = simulation->getSimTime();
    simtime_t newLinkEot = now + link->lookahead;

    // TBD finish...
    return 0.0;
}

simtime_t cAdvancedLinkDelayLookahead::getCurrentLookahead(int partitionId)
{
    return segInfo[partitionId].lookahead;
}

}  // namespace omnetpp

