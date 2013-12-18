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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>       // sprintf
#include <string.h>      // strlen
#include <sstream>
#include "globals.h"
#include "cfsm.h"

NAMESPACE_BEGIN

using std::ostream;


Register_Class(cFSM);


cFSM::cFSM(const char *name) :
  cOwnedObject(name)
{
  _state=0;
  _statename="INIT";
}

void cFSM::copy(const cFSM& vs)
{
    _statename=vs._statename;
    _state=vs._state;
}

cFSM& cFSM::operator=(const cFSM& vs)
{
    if (this==&vs) return *this;
    cOwnedObject::operator=(vs);
    copy(vs);
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

void cFSM::parsimPack(cCommBuffer *buffer)
{
    throw cRuntimeError(this,"parsimPack() not implemented");
}

void cFSM::parsimUnpack(cCommBuffer *buffer)
{
    throw cRuntimeError(this,"parsimUnpack() not implemented");
}

NAMESPACE_END

