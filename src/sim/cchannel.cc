//========================================================================
//  CCHANNEL.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//   Member functions of
//    cChannel : channel class
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cchannel.h"
#include "cpar.h"
#include "cgate.h"
#include "cmessage.h"
#include "cmodule.h"
#include "cenvir.h"
#include "csimulation.h"
#include "globals.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

using std::ostream;

Register_Class(cChannel);


cChannel::cChannel(const char *name) : cComponent(name)
{
    fromgatep = NULL;
}

cChannel::~cChannel()
{
}

std::string cChannel::info() const
{
    // print all parameters
    std::stringstream out;
    for (int i=0; i<params(); i++)
    {
        cPar& p = const_cast<cChannel *>(this)->par(i);
        out << p.fullName() << "=" << p.info() << " ";
    }
    return out.str();
}

void cChannel::forEachChild(cVisitor *v)
{
    cDefaultList::forEachChild(v);
}

void cChannel::netPack(cCommBuffer *buffer)
{
    throw cRuntimeError(this,"netPack() not implemented");
}

void cChannel::netUnpack(cCommBuffer *buffer)
{
    throw cRuntimeError(this,"netUnpack() not implemented");
}

void cChannel::callInitialize()
{
    cComponent::callInitialize();
}

bool cChannel::callInitialize(int stage)
{
    // This is the interface for calling initialize(). Channels don't contain further
    // subcomponents, so just we just invoke initialize() in the right context here.
    int numStages = numInitStages();
    if (stage < numStages)
    {
        // temporarily switch context for the duration of the call
        cContextSwitcher tmp(this);
        cContextTypeSwitcher tmp2(CTX_INITIALIZE);
        initialize(stage);
    }

    bool moreStages = stage < numStages-1;
    return moreStages; // return true if there's more stages to do
}

void cChannel::callFinish()
{
    // This is the interface for calling finish(). Channels don't contain further
    // subcomponents, so just we just invoke finish() in the right context here.
    cContextSwitcher tmp(this);
    cContextTypeSwitcher tmp2(CTX_FINISH);
    finish();
}

cModule *cChannel::parentModule() const
{
    // find which (compound) module contains this connection
    if (!fromgatep)
        return NULL;
    cModule *ownerMod = fromgatep->ownerModule();
    if (!ownerMod)
        return NULL;
    return fromgatep->type()==cGate::INPUT ? ownerMod : ownerMod->parentModule();
}

bool cChannel::deliver(cMessage *msg, simtime_t t)
{
    // just hand over msg to next gate
    return fromGate()->toGate()->deliver(msg, t);
}


