//=========================================================================
//
// CNETMOD.CC- part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Contents:
//      Method definitions of the cNetMod class.
//
//   Written by:  Zoltan Vass, 1996
//   Modifications: Andras Varga
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*---------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include "cpar.h"
#include "cenvir.h"
#include "cnetmod.h"

#define NETM_STACK 16384

//=========================================================================
// Member functions of cNetMod
//=========================================================================

// constructor
cNetMod::cNetMod() : cModule("network-interface",NULL)
{
}

// isLocalMachineIn()
//    Finds out whether the local host is among the machine names passed
//    (which is the list of hosts it was mapped to be run on).
int cNetMod::isLocalMachineIn(const cArray& list)
{
    for (int i=0; i<list.items(); i++)
        if (list[i]!=NULL && !opp_strcmp( localhost(), ((cPar *)list[i])->stringValue()))
            return true;
    return false;
}

cNetMod& cNetMod::operator=(const cNetMod& other)
{
    if (this==&other) return *this;

    cModule::operator=(other);
    segments=other.segments;
    return *this;
}

bool cNetMod::callInitialize(int stage)
{
    // code copied from cSimpleModule...

    int numStages = numInitStages();
    if (stage < numStages)
    {
        // switch to the module's context for the duration of the initialize() call.
        cModule *oldcontext = simulation.contextModule();
        simulation.setContextModule( this );

        initialize( stage );

        if (oldcontext)
            simulation.setContextModule( oldcontext );
        else
            simulation.setGlobalContext();
    }
    return stage < numStages-1;  // return true if there's more stages to do
}

void cNetMod::callFinish()
{
    // code copied from cSimpleModule...

    cModule *oldcontext = simulation.contextModule();
    simulation.setContextModule( this );

    finish();

    if (oldcontext)
        simulation.setContextModule( oldcontext );
    else
        simulation.setGlobalContext();
}


