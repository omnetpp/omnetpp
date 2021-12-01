//========================================================================
//  CFSM.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//   Member functions of
//    cFSM  : Finite State Machine state
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

}  // namespace omnetpp

