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
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

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
int cNetMod::isLocalMachineIn(cArray& list)
{
        for (int i=0; i<list.items(); i++)
           if ( &list[i]!=NULL &&
                opp_strcmp( localhost(), (char *)(cPar&)list[i] )==0
              )
              return TRUE;
        return FALSE;
}

cNetMod& cNetMod::operator=(cNetMod& other)
{
        cModule::operator=(other);
        segments=other.segments;
        return *this;
}
