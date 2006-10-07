//=========================================================================
//  CLINKDELAYLOOKAHEAD.CC - part of
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


#include "cadvlinkdelaylookahead.h"
#include "csimul.h"
#include "cmessage.h"
#include "cenvir.h"
#include "cnullmessageprot.h"
#include "cparsimcomm.h"
#include "cparsimpartition.h"
#include "cplaceholdermod.h"
#include "cproxygate.h"
#include "cchannel.h"
#include "macros.h"


Register_Class(cAdvancedLinkDelayLookahead);


cAdvancedLinkDelayLookahead::cAdvancedLinkDelayLookahead()
{
    numSeg = 0;
    segInfo = NULL;
}

cAdvancedLinkDelayLookahead::~cAdvancedLinkDelayLookahead()
{
    delete [] segInfo;
}

void cAdvancedLinkDelayLookahead::startRun()
{
    ev << "starting Link Delay Lookahead...\n";

    delete [] segInfo;

    numSeg = comm->getNumPartitions();
    segInfo = new PartitionInfo[numSeg];
    int myProcId = comm->getProcId();

    char buf[30];
    int i;

    // temporarily initialize everything to zero.
    for (i=0; i<numSeg; i++)
    {
        segInfo[i].numLinks = 0;
        segInfo[i].links = NULL;
    }

    // fill numLinks and links[]
    ev << "  collecting links..." << endl;

    // step 1: count gates
    int modId;
    for (modId=0; modId<=sim->lastModuleId(); modId++)
    {
        cPlaceHolderModule *mod = dynamic_cast<cPlaceHolderModule *>(sim->module(modId));
        if (mod)
        {
            for (int gateId=0; gateId<mod->gates(); gateId++)
            {
                cGate *g = mod->gate(gateId);
                cProxyGate *pg  = dynamic_cast<cProxyGate *>(g);
                if (pg && pg->fromGate() && pg->getRemoteProcId()>=0)
                {
                    segInfo[pg->getRemoteProcId()].numLinks++;
                }
            }
        }
    }

    // step 2: allocate links[]
    for (i=0; i<numSeg; i++)
    {
        int numLinks = segInfo[i].numLinks;
        segInfo[i].links = new LinkOut *[numLinks];
        for (int k=0; k<numLinks; k++)
            segInfo[i].links[k] = NULL;
    }

    // step 3: fill in
    for (modId=0; modId<=sim->lastModuleId(); modId++)
    {
        cPlaceHolderModule *mod = dynamic_cast<cPlaceHolderModule *>(sim->module(modId));
        if (mod)
        {
            for (int gateId=0; gateId<mod->gates(); gateId++)
            {
                // if this is a properly connected proxygate, process it
                // FIXME leave out gates from other cPlaceHolderModules
                cGate *g = mod->gate(gateId);
                cProxyGate *pg  = dynamic_cast<cProxyGate *>(g);
                if (pg && pg->fromGate() && pg->getRemoteProcId()>=0)
                {
                    // check we have a delay on this link (it gives us lookahead)
                    cGate *fromg  = pg->fromGate();
                    cChannel *chan = fromg ? fromg->channel() : NULL;
                    cBasicChannel *basicChan = dynamic_cast<cBasicChannel *>(chan);
                    cPar *delaypar = basicChan ? basicChan->delay() : NULL;
                    double linkDelay = delaypar ? delaypar->doubleValue() : 0;
                    if (linkDelay<=0.0)
                        throw new cRuntimeError("cAdvancedLinkDelayLookahead: zero delay on link from gate `%s', no lookahead for parallel simulation", fromg->fullPath().c_str());

                    // store
                    int procId = pg->getRemoteProcId();
                    int k=0;
                    while (segInfo[procId].links[k]) k++; // find 1st empty slot
                    LinkOut *link = new LinkOut;
                    segInfo[procId].links[k] = link;
                    pg->setSynchData(link);
                    link->lookahead = linkDelay;
                    link->eot = 0.0;

                    ev << "    link " << k << " to procId=" << procId << " on gate `" << fromg->fullPath() <<"': delay=" << linkDelay << endl;
                }
            }
        }
    }

    ev << "  setup done.\n";
}

void cAdvancedLinkDelayLookahead::endRun()
{
}

double cAdvancedLinkDelayLookahead::getCurrentLookahead(cMessage *msg, int procId, void *data)
{
    // find LinkOut structure in segInfo[destProcId].
    LinkOut *link = (LinkOut *)data;
    if (!link)
        throw new cRuntimeError("internal parallel simulation error: cProxyGate has no associated data pointer");

    // calculate EOT
    simtime_t eot;
    simtime_t now = sim->simTime();
    simtime_t newLinkEot = now + link->lookahead;

    // TBD finish...
    return 0.0;
}

double cAdvancedLinkDelayLookahead::getCurrentLookahead(int procId)
{
    return segInfo[procId].lookahead;
}



