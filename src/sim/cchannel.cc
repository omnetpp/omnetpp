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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

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

USING_NAMESPACE

using std::ostream;

Register_Class(cIdealChannel);


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
    cContextTypeSwitcher tmp(CTX_INITIALIZE);
    int stage = 0;
    while (initializeChannel(stage))
        ++stage;
}

bool cChannel::callInitialize(int stage)
{
    // note: numInitStages() will be checked inside initializeChannel()
    cContextTypeSwitcher tmp(CTX_INITIALIZE);
    return initializeChannel(stage);
}

bool cChannel::initializeChannel(int stage)
{
    // channels don't contain further subcomponents, so just we just invoke
    // initialize(stage) in the right context here.
    if (simulation.contextType()!=CTX_INITIALIZE)
        throw cRuntimeError("internal function initializeChannel() may only be called via callInitialize()");

    int numStages = numInitStages();
    if (stage < numStages)
    {
        ev << "Initializing channel " << fullPath() << ", stage " << stage << "\n";

        // switch context for the duration of the call
        cContextSwitcher tmp(this);
        initialize(stage);
    }

    bool moreStages = stage < numStages-1;
    return moreStages;
}

void cChannel::callFinish()
{
    // This is the interface for calling finish(). Channels don't contain further
    // subcomponents, so just we just invoke finish() in the right context here.
    cContextSwitcher tmp(this);
    cContextTypeSwitcher tmp2(CTX_FINISH);
    recordParametersAsScalars();
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

cProperties *cChannel::properties() const
{
    cModule *parent = parentModule();
    cComponentType *type = componentType();
    cProperties *props;
    if (parent)
        props = parent->componentType()->connectionProperties(connId, type->fullName());
    else
        props = type->properties();
    return props;
}

//----

bool cIdealChannel::deliver(cMessage *msg, simtime_t t)
{
    // just hand over msg to next gate
    EVCB.messageSendHop(msg, fromGate());
    return fromGate()->toGate()->deliver(msg, t);
}

simtime_t cIdealChannel::transmissionFinishes() const
{
    return simulation.simTime();
}


