//========================================================================
//
//  CFSM.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
//   Member functions of
//    cFSM  : Finite State Machine state
//
//  Author: Andras Varga
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>       // sprintf
#include <string.h>      // strlen
#include "macros.h"
#include "cfsm.h"

using std::ostream;

//=== registration
Register_Class(cFSM);

//=========================================================================

cFSM::cFSM(const char *name) :
  cObject(name)
{
  _state=0;
  _statename="INIT";
}

cFSM& cFSM::operator=(const cFSM& vs)
{
    if (this==&vs) return *this;

    cObject::operator=(vs);
    _statename=vs._statename;
    _state=vs._state;
    return *this;
}

void cFSM::info(char *buf)
{
     if (!_statename)
        sprintf(buf, "state: %d", _state);
     else
        sprintf(buf, "state: %s (%d)", _statename, _state);
}

void cFSM::writeContents(ostream& os)
{
   cObject::writeContents(os);

   if (!_statename)
       os << "\n  State: " << _state << "\n";
   else
       os << "\n  State: " << _statename << " (" << _state << ")\n";
}

void cFSM::netPack(cCommBuffer *buffer)
{
    throw new cException(this,"netPack() not implemented");
}

void cFSM::netUnpack(cCommBuffer *buffer)
{
    throw new cException(this,"netUnpack() not implemented");
}

