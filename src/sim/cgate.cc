//========================================================================
//  CGATE.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//   Member functions of
//    cGate      : a module gate
//
//  Author: Andras Varga
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>            // pow
#include <stdio.h>           // sprintf
#include <string.h>          // strcpy
#include "cmessage.h"
#include "cmodule.h"
#include "carray.h"
#include "cenvir.h"
#include "csimulation.h"
#include "globals.h"
#include "cgate.h"
#include "cchannel.h"
#include "cbasicchannel.h"
#include "cdisplaystring.h"
#include "ccomponenttype.h"
#include "stringutil.h"
#include "util.h"

USING_NAMESPACE

using std::ostream;

cStringPool cGate::stringPool;

cGate::cGate(Desc *d)
{
    desc = d;
    gateId = -1; // to be set later
    fullname = NULL;

    fromgatep = togatep = NULL;
    channelp = NULL;
}

cGate::~cGate()
{
    dropAndDelete(channelp);
    delete [] fullname;
}

void cGate::forEachChild(cVisitor *v)
{
    if (channelp)
        v->visit(channelp);
}

const char *cGate::name() const
{
    if (!desc->isInout())
        return desc->namep;
    else {
        // this is one half of an inout gate, append "$i" or "$o"
        const char *suffix = type()==INPUT ? "$i" : "$o";
        return stringPool.get(opp_concat(desc->namep, suffix));
    }
}

const char *cGate::fullName() const
{
    // if not in a vector, normal name() will do
    if (!isVector())
        return name();

    // otherwise, produce fullname if not already there
    if (!fullname)
    {
        fullname = new char[opp_strlen(name())+10];
        strcpy(fullname, name());
        opp_appendindex(fullname, index());
    }
    return fullname;
}

std::string cGate::info() const
{
    const char *arrow;
    cGate const *g;
    cGate const *conng;
    cChannel const *chan;

    if (type()==OUTPUT)
        {arrow = "--> "; g = togatep; conng = this; chan = channelp; }
    else if (type()==INPUT)
        {arrow = "<-- "; g = fromgatep; conng = fromgatep; chan = fromgatep ? fromgatep->channelp : NULL;}
    else
        ASSERT(0);  // a cGate is never INOUT

    // append useful info to buf
    if (!g)
        return std::string("not connected");

    std::stringstream out;
    out << arrow;

    out << (g->ownerModule()==ownerModule()->parentModule() ? "<parent>" : g->ownerModule()->fullName());
    out << "." << g->fullName();

    if (chan)
        out << ", channel: " << chan->info();

    return out.str();
}

cObject *cGate::owner() const
{
    // note: this function cannot go inline because of circular dependencies
    return desc->ownerp;
}

void cGate::setGateId(int id)
{
    gateId = id;

    // invalidate fullname, as it may have changed (it'll be recreated on demand)
    if (fullname)
    {
        delete [] fullname;
        fullname = NULL;
    }
}

cGate::Type cGate::type() const
{
    if (!desc->isInout())
        return desc->isInput() ? INPUT : OUTPUT;

    // otherwise see which gate id range in desc contains this gate's id
    if (desc->isScalar())
        return gateId==desc->inGateId ? INPUT : OUTPUT;
    if (desc->isInInputIdRange(gateId))
        return INPUT;
    if (desc->isInOutputIdRange(gateId))
        return OUTPUT;
    throw cRuntimeError(this, "internal data structure inconsistency");
}

int cGate::index() const
{
    // if not vector, return 0
    if (desc->isScalar())
        return 0;

    // otherwise see which gate id range in desc contains this gate's id
    if (desc->isInput() && desc->isInInputIdRange(gateId))
        return gateId - desc->inGateId;
    if (desc->isOutput() && desc->isInOutputIdRange(gateId))
        return gateId - desc->outGateId;
    throw cRuntimeError(this, "internal data structure inconsistency");
}

cProperties *cGate::properties() const
{
    cComponent *component = check_and_cast<cComponent *>(owner());
    cComponentType *componentType = component->componentType();
    cModule *parent = component->parentModule();
    cProperties *props;
    if (parent)
        props = parent->componentType()->subcomponentGateProperties(component->name(), componentType->fullName(), baseName());
    else
        props = componentType->gateProperties(baseName());
    return props;
}

void cGate::connectTo(cGate *g, cChannel *chan)
{
    if (togatep)
        throw cRuntimeError(this, "connectTo(): gate already connected");
    if (!g)
        throw cRuntimeError(this, "connectTo(): destination gate cannot be NULL pointer");
    if (g->fromgatep)
        throw cRuntimeError(this, "connectTo(): destination gate already connected");

    // build new connection
    togatep = g;
    togatep->fromgatep = this;
    if (chan)
        setChannel(chan);

    EVCB.connectionCreated(this);
}

void cGate::disconnect()
{
    if (!togatep) return;

    // notify envir that old conn gets removed
    EVCB.connectionRemoved(this);

    // remove connection
    togatep->fromgatep = NULL;
    togatep = NULL;

    // and channel object
    dropAndDelete(channelp);
    channelp = NULL;
}

void cGate::setChannel(cChannel *ch)
{
    if (ch == channelp)
        return;

    dropAndDelete(channelp);
    channelp = ch;
    if (channelp)
    {
        channelp->setFromGate(this);
        take(channelp);
    }
}

cGate *cGate::sourceGate() const
{
    const cGate *g;
    for (g=this; g->fromgatep!=NULL; g=g->fromgatep);
    return const_cast<cGate *>(g);
}

cGate *cGate::destinationGate() const
{
    const cGate *g;
    for (g=this; g->togatep!=NULL; g=g->togatep);
    return const_cast<cGate *>(g);
}

bool cGate::deliver(cMessage *msg, simtime_t t)
{
    if (togatep==NULL)
    {
        ownerModule()->arrived(msg, id(), t);
        return true;
    }
    else
    {
        if (channelp) {
            return channelp->deliver(msg, t);
        } else {
            EVCB.messageSendHop(msg, this);
            return togatep->deliver(msg, t);
        }
    }
}

bool cGate::isBusy() const
{
    cBasicChannel *ch = dynamic_cast<cBasicChannel *>(channelp);
    return ch ? ch->isBusy() : false;
}

simtime_t cGate::transmissionFinishes() const
{
    return channelp ? channelp->transmissionFinishes() : simulation.simTime();
}

int cGate::routeContains(cModule *mod, int gate)
{
    cGate *g;

    for (g=this; g!=NULL; g=g->fromgatep)
        if( g->ownerModule()==mod && (gate==-1 || g->id()==gate) )
            return 1;
    for (g=togatep; g!=NULL; g=g->togatep)
        if( g->ownerModule()==mod && (gate==-1 || g->id()==gate) )
            return 1;
    return 0;
}

bool cGate::isConnectedOutside() const
{
    if (type()==INPUT)
        return fromgatep!=NULL;
    else
        return togatep!=NULL;
}

bool cGate::isConnectedInside() const
{
    if (type()==INPUT)
        return togatep!=NULL;
    else
        return fromgatep!=NULL;
}

bool cGate::isConnected() const
{
    // for compound modules, both inside and outside must be non-NULL,
    // for simple modules, only check outside.
    if (!ownerModule()->isSimple())
        return fromgatep!=NULL && togatep!=NULL;
    else
        return isConnectedOutside();
}

bool cGate::isRouteOK() const
{
    return sourceGate()->ownerModule()->isSimple() &&
           destinationGate()->ownerModule()->isSimple();
}

