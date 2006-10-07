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
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ccompoundmodule.h"
#include "cmessage.h"


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
    out << "id=" << id();
    return out.str();
}

void cCompoundModule::arrived(cMessage *msg, int g, simtime_t)
{
    throw new cRuntimeError("Message (%s)`%s' arrived at COMPOUND module gate `%s' "
                            "(which is not further connected)",
                            msg->className(), msg->name(), gate(g)->fullPath().c_str());
}

void cCompoundModule::scheduleStart(simtime_t t)
{
    for (cSubModIterator submod(*this); !submod.end(); submod++)
    {
        submod()->scheduleStart( t );
    }
}


