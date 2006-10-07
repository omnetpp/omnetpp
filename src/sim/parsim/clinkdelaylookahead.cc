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


#include "clinkdelaylookahead.h"
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
    int i;
    for (i=0; i<numSeg; i++)
        segInfo[i].minDelay = -1;

    // fill in minDelays
    ev << "  calculating minimum link delays..." << endl;
    for (int modId=0; modId<=sim->lastModuleId(); modId++)
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
                    cChannel *chan = fromg->channel();
                    cBasicChannel *basicChan = dynamic_cast<cBasicChannel *>(chan);
                    double linkDelay = basicChan ? basicChan->delay() : 0.0;
                    if (linkDelay<=0.0)
                        throw new cRuntimeError("cLinkDelayLookahead: zero delay on link from gate `%s', no lookahead for parallel simulation", fromg->fullPath().c_str());

                    // store
                    int procId = pg->getRemoteProcId();
                    if (segInfo[procId].minDelay==-1 || segInfo[procId].minDelay<linkDelay)
                        segInfo[procId].minDelay = linkDelay;
                }
            }
        }
    }

    // FIXME what to do if 2 procIds are not connected (minDelay=-1)?
    // set mindelay to INFINITY?
    //for (i=0; i<numSeg; i++)
    //    if (i!=myProcId && segInfo[procId].minDelay==-1)
    //        ev << "    not connected\n";//????????????

    for (i=0; i<numSeg; i++)
        if (i!=myProcId)
            ev << "    lookahead to procId=" << i << " is " << simtimeToStr(segInfo[i].minDelay) << endl;

    ev << "  setup done.\n";
}

void cLinkDelayLookahead::endRun()
{
    delete [] segInfo;
    segInfo = NULL;
}

double cLinkDelayLookahead::getCurrentLookahead(cMessage *, int procId, void *)
{
    return segInfo[procId].minDelay;
}

double cLinkDelayLookahead::getCurrentLookahead(int procId)
{
    return segInfo[procId].minDelay;
}



