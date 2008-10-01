//==========================================================================
//   CPLACEHOLDERMOD.CC  -  header for
//
//                    OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, 2003
//          Dept. of Electrical and Computer Systems Engineering,
//          Monash University, Melbourne, Australia
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cplaceholdermod.h"
#include "cproxygate.h"

USING_NAMESPACE


cPlaceholderModule::cPlaceholderModule()
{
}

cPlaceholderModule::~cPlaceholderModule()
{
}

void cPlaceholderModule::arrived(cMessage *msg, cGate *ongate, simtime_t t)
{
    throw cRuntimeError(this, "internal error: arrived() called");
}

void cPlaceholderModule::scheduleStart(simtime_t t)
{
    // do nothing
}

cGate *cPlaceholderModule::createGateObject(cGate::Type type)
{
    if (type==cGate::INPUT)
        return new cProxyGate();
    else
        return cModule::createGateObject(type);
}

//----------------------------------------
// as usual: tribute to smart linkers
#include "cfilecomm.h"
#include "cnamedpipecomm.h"
#include "cmpicomm.h"
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
#ifdef WITH_MPI
    cMPICommunications mc;
#endif
    cNoSynchronization ns;
    cNullMessageProtocol np;
    cISPEventLogger iel;
    cIdealSimulationProtocol ip;
    cLinkDelayLookahead ldla;
    // prevent "unused variable" warnings:
    (void)fc; (void)npc; (void)ns; (void)np; (void)iel; (void)ip; (void)ldla;
}

