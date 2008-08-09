//=========================================================================
//  CLINKDELAYLOOKAHEAD.CC - part of
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


#include "clinkdelaylookahead.h"
#include "csimulation.h"
#include "cmessage.h"
#include "cenvir.h"
#include "cnullmessageprot.h"
#include "cparsimcomm.h"
#include "cparsimpartition.h"
#include "cplaceholdermod.h"
#include "cproxygate.h"
#include "cdataratechannel.h"
#include "globals.h"
#include "regmacros.h"

USING_NAMESPACE


Register_Class(cLinkDelayLookahead);


cLinkDelayLookahead::cLinkDelayLookahead()
{
    numSeg = 0;
    segInfo = NULL;
}

cLinkDelayLookahead::~cLinkDelayLookahead()
{
    delete [] segInfo;
}

void cLinkDelayLookahead::startRun()
{
    ev << "starting Link Delay Lookahead...\n";

    delete [] segInfo;

    numSeg = comm->getNumPartitions();
    segInfo = new PartitionInfo[numSeg];
    int myProcId = comm->getProcId();

    // temporarily initialize everything to zero.
    for (int i=0; i<numSeg; i++)
        segInfo[i].minDelay = -1;

    // fill in minDelays
    ev << "  calculating minimum link delays...\n";
    for (int modId=0; modId<=sim->getLastModuleId(); modId++)
    {
        cPlaceholderModule *mod = dynamic_cast<cPlaceholderModule *>(sim->getModule(modId));
        if (mod)
        {
            for (cModule::GateIterator i(mod); !i.end(); i++)
            {
                // if this is a properly connected proxygate, process it
                // FIXME leave out gates from other cPlaceholderModules
                cGate *g = i();
                cProxyGate *pg  = dynamic_cast<cProxyGate *>(g);
                if (pg && pg->getPreviousGate() && pg->getRemoteProcId()>=0)
                {
                    // check we have a delay on this link (it gives us lookahead)
                    cGate *fromg  = pg->getPreviousGate();
                    cChannel *chan = fromg->getChannel();
                    cDatarateChannel *datarateChan = dynamic_cast<cDatarateChannel *>(chan);
                    simtime_t linkDelay = datarateChan ? datarateChan->getDelay() : 0.0;
                    if (linkDelay<=0.0)
                        throw cRuntimeError("cLinkDelayLookahead: zero delay on link from gate `%s', no lookahead for parallel simulation", fromg->getFullPath().c_str());

                    // store
                    int procId = pg->getRemoteProcId();
                    if (segInfo[procId].minDelay==-1 || segInfo[procId].minDelay > linkDelay)
                        segInfo[procId].minDelay = linkDelay;
                }
            }
        }
    }

    // if two partitions are not connected, the lookeahead is "infinity"
    for (int i=0; i<numSeg; i++)
        if (i!=myProcId && segInfo[i].minDelay==-1)
            segInfo[i].minDelay = MAXTIME;

    for (int i=0; i<numSeg; i++)
        if (i!=myProcId)
            ev << "    lookahead to procId=" << i << " is " << segInfo[i].minDelay << "\n";

    ev << "  setup done.\n";
}

void cLinkDelayLookahead::endRun()
{
    delete [] segInfo;
    segInfo = NULL;
}

simtime_t cLinkDelayLookahead::getCurrentLookahead(cMessage *, int procId, void *)
{
    return segInfo[procId].minDelay;
}

simtime_t cLinkDelayLookahead::getCurrentLookahead(int procId)
{
    return segInfo[procId].minDelay;
}



