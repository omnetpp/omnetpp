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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

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
#include "stringpool.h"
#include "util.h"

USING_NAMESPACE

using std::ostream;


/*
 * Interpretation of gate Ids (32-bit int):
 *     H (12+ bits) + L (20 bits)
 *
 * When H=0: nonvector gate
 *     L/2 = descIndex
 *     L&1 = 0: inputgate, 1: outputgate
 *   note: this allows ~500,000 scalar gates
 *
 * When H>0: vector gate
 *     H = descIndex+1  (so that H>0)
 *     bit19 of L = input (0) or output (1)
 *     bits0..18 of L = array index into inputgate[] or outputgate[]
 *   note: gateId must not be negative (for historical reasons, -1 is used as "none"),
 *         so H is effectively 11 bits, allowing ~2046 vector gates max.
 *   note2: 19 bits allow a maximum vector size of ~500,000
 *
 * Mostly affected methods are cGate::id() and cModule::gate(int id).
 */


// non-refcounting pool for gate fullnames
static CommonStringPool gateFullnamePool;


cGate::Name::Name(const char *name, Type type)
{
    this->name = name;
    this->type = type;
    if (type==cGate::INOUT) {
        namei = opp_concat(name, "$i");
        nameo = opp_concat(name, "$o");
    }
}

bool cGate::Name::operator<(const Name& other) const
{
    int d = opp_strcmp(name.c_str(), other.name.c_str());
    if (d<0)
        return true;
    else if (d>0)
        return false;
    else
        return type < other.type;
}

cGate::cGate()
{
    desc = NULL;
    pos = 0;
    fromgatep = togatep = NULL;
    channelp = NULL;
}

cGate::~cGate()
{
    dropAndDelete(channelp);
}

void cGate::clearFullnamePool()
{
    gateFullnamePool.clear();
}

void cGate::forEachChild(cVisitor *v)
{
    if (channelp)
        v->visit(channelp);
}

const char *cGate::baseName() const
{
    return desc->namep->name.c_str();
}

const char *cGate::name() const
{
    if (desc->namep->type==INOUT)
        return desc->isInput(this) ? desc->namep->namei.c_str() : desc->namep->nameo.c_str();
    else
        return desc->namep->name.c_str();
}

const char *cGate::fullName() const
{
    // if not in a vector, normal name() will do
    if (!isVector())
        return name();

    // otherwise, produce fullname in a temp buffer, and return its stringpooled copy
    // note: this implementation assumes that this method will be called infrequently
    // (ie. we reproduce the string every time).
    if (opp_strlen(name()) > 100)
        throw cRuntimeError(this, "fullName(): gate name too long, should be under 100 characters");

    static char tmp[128];
    strcpy(tmp, name());
    opp_appendindex(tmp, index());
    return gateFullnamePool.get(tmp); // non-refcounted stringpool
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
        return "not connected";

    std::stringstream out;
    out << arrow;

    out << (g->ownerModule()==ownerModule()->parentModule() ? "<parent>" : g->ownerModule()->fullName());
    out << "." << g->fullName();

    if (chan)
        out << ", " << chan->componentType()->fullName() << " " << chan->info();

    return out.str();
}

cObject *cGate::owner() const
{
    // note: this function cannot go inline because of circular dependencies
    return desc->ownerp;
}

cModule *cGate::ownerModule() const
{
    return desc->ownerp;
}

int cGate::id() const
{
    int descIndex = desc - desc->ownerp->descv;
    int id;
    if (!desc->isVector())
        id = (descIndex<<1)|(pos&1);
    else
        // note: we use descIndex+1 otherwise h can remain zero after <<LBITS
        id = ((descIndex+1)<<GATEID_LBITS) | ((pos&1)<<(GATEID_LBITS-1)) | (pos>>1);
    return id;
}

cProperties *cGate::properties() const
{
    cComponent *component = check_and_cast<cComponent *>(owner());
    cComponentType *componentType = component->componentType();
    cProperties *props = componentType->gateProperties(baseName());
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
    EVCB.connectionDeleted(this);

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

bool cGate::pathContains(cModule *mod, int gate)
{
    cGate *g;

    for (g=this; g!=NULL; g=g->fromgatep)
        if (g->ownerModule()==mod && (gate==-1 || g->id()==gate))
            return true;
    for (g=togatep; g!=NULL; g=g->togatep)
        if (g->ownerModule()==mod && (gate==-1 || g->id()==gate))
            return true;
    return false;
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

bool cGate::isPathOK() const
{
    return sourceGate()->ownerModule()->isSimple() &&
           destinationGate()->ownerModule()->isSimple();
}

cDisplayString& cGate::displayString()
{
    if (!channel()) {
        cChannel *channel = cChannelType::createIdealChannel("channel", ownerModule());
        channel->finalizeParameters();
        setChannel(channel);
    }
    return channel()->displayString();
}

void cGate::setDisplayString(const char *dispstr)
{
    displayString().set(dispstr);
}

