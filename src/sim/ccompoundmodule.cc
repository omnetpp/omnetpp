//======================================================================
//  CCOMPOUNDMODULE.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//======================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "ccompoundmodule.h"
#include "ccomponenttype.h"
#include "cmessage.h"

USING_NAMESPACE


//
// Note: cModule,cSimpleModule,cCompoundModule are left unregistered.
//   One should never create modules by createOne(cSimpleModule) or the like.
//


cCompoundModule::cCompoundModule()
{
}

cCompoundModule::~cCompoundModule()
{
}

std::string cCompoundModule::info() const
{
    std::stringstream out;
    out << "id=" << getId();
    return out.str();
}

void cCompoundModule::doBuildInside()
{
    // ask module type to create submodules and internal connections
    getModuleType()->buildInside(this);
}

void cCompoundModule::arrived(cMessage *msg, int g, simtime_t)
{
    cGate *gt = gate(g);
    throw cRuntimeError("Gate `%s' of compound module (%s)%s is not connected on the %s, "
                        "upon arrival of message (%s)%s",
                        gt->getFullName(),
                        getClassName(), getFullPath().c_str(),
                        (gt->isConnectedOutside() ? "inside" : "outside"),
                        msg->getClassName(), msg->getName());
}

void cCompoundModule::scheduleStart(simtime_t t)
{
    for (SubmoduleIterator submod(this); !submod.end(); submod++)
        submod()->scheduleStart(t);
}


