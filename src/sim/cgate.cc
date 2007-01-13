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
#include "util.h"
#include "ccomponenttype.h"

using std::ostream;


cGate::cGate(Desc *d)
{
    desc = d;
    gateId = -1; // to be set later
    fullname = NULL;

    fromgatep = togatep = NULL;
    channelp = NULL;
    dispstr = NULL;
}

cGate::~cGate()
{
    dropAndDelete(channelp);
    delete dispstr;
    delete [] fullname;
}

void cGate::forEachChild(cVisitor *v)
{
    if (channelp)
        v->visit(channelp);
}

const char *cGate::name() const
{
    return desc->namep;  //FIXME somehow add $i or $o for members of an inout gate...
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

    if (type()==OUTPUT)
        {arrow = "--> "; g = togatep; conng = this;}
    else if (type()==INPUT)
        {arrow = "<-- "; g = fromgatep; conng = fromgatep;}
    else // INOUT
        {arrow = "<--> "; g = fromgatep; conng = fromgatep;}

    // append useful info to buf
    if (desc->size==0)
        return std::string("(placeholder for zero-size vector)");
    if (!g)
        return std::string("not connected");

    std::stringstream out;
    out << arrow;

    if (channelp)
        out << channelp->info() << arrow;

    out << (g->ownerModule()==ownerModule()->parentModule() ? "<parent>" : g->ownerModule()->fullName());
    out << "." << g->fullName();
    return out.str();
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

int cGate::index() const
{
    // if not vector, return 0
    if (desc->size < 0)
        return 0;
    // otherwise see which gate id range in desc contains this gate's id
    if (desc->inGateId>=0 && gateId >= desc->inGateId && gateId < desc->inGateId + desc->size)
        return gateId - desc->inGateId;
    if (desc->outGateId>=0 && gateId >= desc->outGateId && gateId < desc->outGateId + desc->size)
        return gateId - desc->outGateId;
    throw cRuntimeError(this, "internal data structure inconsistency");
}

cProperties *cGate::properties() const
{
    return cComponentType::getPropertiesFor(this);
}

void cGate::connectTo(cGate *g, cChannel *chan)
{
    if (desc->size==0)
        throw cRuntimeError(this, "connectTo(): gate vector size is zero");
    if (togatep)
        throw cRuntimeError(this, "connectTo(): gate already connected");
    if (!g)
        throw cRuntimeError(this, "connectTo(): destination gate cannot be NULL pointer");
    if (g->desc->size==0)
        throw cRuntimeError(this, "connectTo(): destination gate vector size is zero");
    if (g->fromgatep)
        throw cRuntimeError(this, "connectTo(): destination gate already connected");

    // build new connection
    togatep = g;
    togatep->fromgatep = this;
    if (chan)
        setChannel(chan);

    ev.connectionCreated(this);
}

void cGate::disconnect()
{
    if (!togatep) return;

    // notify envir that old conn gets removed
    ev.connectionRemoved(this);

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
        if (channelp)
            return channelp->deliver(msg, t);
        else
            return togatep->deliver(msg, t);
    }
}

bool cGate::isBusy() const
{
    cBasicChannel *ch = dynamic_cast<cBasicChannel *>(channelp);
    return ch ? ch->isBusy() : false;
}

simtime_t cGate::transmissionFinishes() const
{
    cBasicChannel *ch = dynamic_cast<cBasicChannel *>(channelp);
    return ch ? ch->transmissionFinishes() : 0.0;
}

int cGate::routeContains( cModule *mod, int gate )
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

cDisplayString& cGate::displayString()
{
    if (!dispstr)
    {
        dispstr = new cDisplayString();
        dispstr->setRoleToConnection(this);
    }
    return *dispstr;
}

// DEPRECATED
void cGate::setDisplayString(const char *s, bool immediate)
{
    displayString().parse(s);
}


