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
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>  // sprintf
#include <cstring>  // strlen
#include <sstream>
#include "omnetpp/globals.h"
#include "omnetpp/cfsm.h"

namespace omnetpp {

using std::ostream;

Register_Class(cFSM);

cFSM::cFSM(const char *name) :
    cOwnedObject(name)
{
    state = 0;
    stateName = "INIT";
}

void cFSM::copy(const cFSM& vs)
{
    stateName = vs.stateName;
    state = vs.state;
}

cFSM& cFSM::operator=(const cFSM& vs)
{
    if (this == &vs)
        return *this;
    cOwnedObject::operator=(vs);
    copy(vs);
    return *this;
}

std::string cFSM::str() const
{
    std::stringstream out;
    if (!stateName)
        out << "state: " << state;
    else
        out << "state: " << stateName << " (" << state << ")";
    return out.str();
}

void cFSM::parsimPack(cCommBuffer *buffer) const
{
    throw cRuntimeError(this, "parsimPack() not implemented");
}

void cFSM::parsimUnpack(cCommBuffer *buffer)
{
    throw cRuntimeError(this, "parsimUnpack() not implemented");
}

}  // namespace omnetpp

