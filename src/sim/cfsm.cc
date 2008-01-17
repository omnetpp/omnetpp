//========================================================================
//  CFSM.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//   Member functions of
//    cFSM  : Finite State Machine state
//
//  Author: Andras Varga
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>       // sprintf
#include <string.h>      // strlen
#include "globals.h"
#include "cfsm.h"

USING_NAMESPACE

using std::ostream;


Register_Class(cFSM);


cFSM::cFSM(const char *name) :
  cOwnedObject(name)
{
  _state=0;
  _statename="INIT";
}

cFSM& cFSM::operator=(const cFSM& vs)
{
    if (this==&vs) return *this;

    cOwnedObject::operator=(vs);
    _statename=vs._statename;
    _state=vs._state;
    return *this;
}

std::string cFSM::info() const
{
    std::stringstream out;
    if (!_statename)
        out << "state: " << _state;
    else
        out << "state: " << _statename << " (" << _state << ")";
    return out.str();
}

void cFSM::netPack(cCommBuffer *buffer)
{
    throw cRuntimeError(this,"netPack() not implemented");
}

void cFSM::netUnpack(cCommBuffer *buffer)
{
    throw cRuntimeError(this,"netUnpack() not implemented");
}

