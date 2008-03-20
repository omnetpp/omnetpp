//==========================================================================
//   CPLACEHOLDERMOD.CC  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cplaceholdermod.h"
#include "cproxygate.h"

USING_NAMESPACE


cPlaceHolderModule::cPlaceHolderModule()
{
}

cPlaceHolderModule::~cPlaceHolderModule()
{
}

void cPlaceHolderModule::arrived(cMessage *msg, int n, simtime_t t)
{
    throw cRuntimeError(this, "internal error: arrived() called");
}

void cPlaceHolderModule::scheduleStart(simtime_t t)
{
    // do nothing
}

cGate *cPlaceHolderModule::createGateObject(cGate::Desc *desc)
{
    if (desc->isInput())
        return new cProxyGate(desc);
    else
        return cModule::createGateObject(desc);
}

//----------------------------------------
// as usual: tribute to smart linkers
#include "cfilecomm.h"
#include "cnamedpipecomm.h"
//#include "cmpicomm.h"
//#include "cptrpassingmpicomm.h"
#include "cnosynchronization.h"
#include "cnullmessageprot.h"
#include "cispeventlogger.h"
#include "cidealsimulationprot.h"
#include "clinkdelaylookahead.h"
#include <stdio.h>
void parsim_dummy()
{
    cFileCommunications fc;
    cNamedPipeCommunications npc;
    //cMPICommunications mc;
    //cPtrPassingMPICommunications ppmc;
    cNoSynchronization ns;
    cNullMessageProtocol np;
    cISPEventLogger iel;
    cIdealSimulationProtocol ip;
    cLinkDelayLookahead ldla;
    // prevent "unused variable" warnings:
    (void)fc; (void)npc; (void)ns; (void)np; (void)iel; (void)ip; (void)ldla;
}

