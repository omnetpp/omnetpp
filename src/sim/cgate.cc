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
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cmath>  // pow
#include <cstdio>  // sprintf
#include <cstring>  // strcpy
#include "common/stringutil.h"
#include "common/stringpool.h"
#include "omnetpp/cpacket.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/carray.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/globals.h"
#include "omnetpp/cgate.h"

#include "../include/omnetpp/cstatisticbuilder.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cdataratechannel.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/simutil.h"
#include "omnetpp/cmodelchange.h"

using namespace omnetpp::common;

namespace omnetpp {

using std::ostream;

/*
 * Interpretation of gate Ids (32-bit int):
 *     H (12+ bits) + L (20 bits)
 *
 * When H=0: nonvector gate
 *     L/2 = descIndex
 *     L&1 = 0: input.gate, 1: output.gate
 *   note: this allows ~500,000 scalar gates
 *
 * When H>0: vector gate
 *     H = descIndex+1  (so that H>0)
 *     bit19 of L = input (0) or output (1)
 *     bits0..18 of L = array index into input.gate[] or output.gate[]
 *   note: gateId must not be negative (for historical reasons, -1 is used as "none"),
 *         so H is effectively 11 bits, allowing ~2046 vector gates max.
 *   note2: 19 bits allow a maximum vector size of ~500,000
 *
 * Mostly affected methods are cGate::getId() and cModule::gate(int id).
 */

// non-refcounting pool for gate fullnames
static StringPool gateFullnamePool;

int cGate::lastConnectionId = -1;

cGate::Name::Name(const char *name, Type type)
{
    this->name = name;
    this->type = type;
    if (type == cGate::INOUT) {
        namei = opp_concat(name, "$i");
        nameo = opp_concat(name, "$o");
    }
}

bool cGate::Name::operator<(const Name& other) const
{
    int d = omnetpp::opp_strcmp(name.c_str(), other.name.c_str());
    if (d < 0)
        return true;
    else if (d > 0)
        return false;
    else
        return type < other.type;
}

cGate::cGate()
{
    desc = nullptr;
    pos = 0;
    prevGate = nextGate = nullptr;
    channel = nullptr;
    connectionId = -1;
}

cGate::~cGate()
{
    dropAndDelete(channel);
}

void cGate::clearFullnamePool()
{
    gateFullnamePool.clear();
}

void cGate::forEachChild(cVisitor *v)
{
    if (channel)
        v->visit(channel);
}

const char *cGate::getBaseName() const
{
    return desc->name->name.c_str();
}

const char *cGate::getName() const
{
    if (desc->name->type == INOUT)
        return desc->isInput(this) ? desc->name->namei.c_str() : desc->name->nameo.c_str();
    else
        return desc->name->name.c_str();
}

const char *cGate::getNameSuffix() const
{
    if (desc->name->type == INOUT)
        return desc->isInput(this) ? "$i" : "$o";
    else
        return "";
}

const char *cGate::getFullName() const
{
    // if not in a vector, normal getName() will do
    if (!isVector())
        return getName();

    // otherwise, produce fullname in a temp buffer, and return its stringpooled copy
    // note: this implementation assumes that this method will be called infrequently
    // (ie. we reproduce the string every time).
    if (omnetpp::opp_strlen(getName()) > 100)
        throw cRuntimeError(this, "getFullName(): Gate name too long, should be under 100 characters");

    static char tmp[128];
    strcpy(tmp, getName());
    opp_appendindex(tmp, getIndex());
    return gateFullnamePool.get(tmp);  // non-refcounted stringpool
}

std::string cGate::str() const
{
    const char *arrow;
    cGate const *g = nullptr;
    cChannel const *chan;

    if (getType() == OUTPUT) {
        arrow = "--> ";
        g = nextGate;
        chan = channel;
    }
    else if (getType() == INPUT) {
        arrow = "<-- ";
        g = prevGate;
        chan = prevGate ? prevGate->channel : nullptr;
    }
    else
        ASSERT(0);  // a cGate is never INOUT

    // append useful info to buf
    if (!g)
        return "not connected";

    std::stringstream out;
    out << arrow;

    out << (g->getOwnerModule() == getOwnerModule()->getParentModule() ? "<parent>" : g->getOwnerModule()->getFullName());
    out << "." << g->getFullName();

    if (chan)
        out << ", (" << chan->getComponentType()->getFullName() << ")" << chan->getFullName() << " " << chan->str();

    return out.str();
}

cObject *cGate::getOwner() const
{
    // note: this function cannot go inline because of circular dependencies
    return desc->owner;
}

cModule *cGate::getOwnerModule() const
{
    return desc->owner;
}

int cGate::getId() const
{
    int descIndex = desc - desc->owner->gateDescArray;
    int id;
    if (!desc->isVector())
        id = (descIndex << 1) | (pos & 1);
    else
        // note: we use descIndex+1 otherwise h could remain zero after <<LBITS
        id = ((descIndex+1) << GATEID_LBITS) | ((pos & 1) << (GATEID_LBITS-1)) | (pos >> 2);
    return id;
}

int cGate::getBaseId() const
{
    int descIndex = desc - desc->owner->gateDescArray;
    int id;
    if (!desc->isVector())
        id = (descIndex << 1) | (pos & 1);
    else
        // note: we use descIndex+1 otherwise h could remain zero after <<LBITS
        id = ((descIndex+1) << GATEID_LBITS) | ((pos & 1) << (GATEID_LBITS-1));
    return id;
}

const char *cGate::getTypeName(Type t)
{
    switch (t) {
        case INPUT: return "input";
        case OUTPUT: return "output";
        case INOUT: return "inout";
        default: return "(unknown type)";
    }
}

cProperties *cGate::getProperties() const
{
    cComponent *component = dynamic_cast<cComponent *>(getOwner());
    ASSERT(component != nullptr);
    cComponentType *componentType = component->getComponentType();
    cProperties *props = componentType->getGateProperties(getBaseName());
    return props;
}

#ifdef SIMFRONTEND_SUPPORT
bool cGate::hasChangedSince(int64_t lastRefreshSerial)
{
    return getOwnerModule()->hasChangedSince(lastRefreshSerial);
}

#endif

cChannel *cGate::connectTo(cGate *g, cChannel *chan, bool leaveUninitialized)
{
    if (nextGate)
        throw cRuntimeError(this, "connectTo(): Gate already connected");
    if (!g)
        throw cRuntimeError(this, "connectTo(): Destination gate cannot be nullptr");
    if (g->prevGate)
        throw cRuntimeError(this, "connectTo(): Destination gate already connected");

    // notify pre-change listeners
    cModule *mod = getOwnerModule();
    if (mod->hasListeners(PRE_MODEL_CHANGE)) {
        cPreGateConnectNotification tmp;
        tmp.gate = this;
        tmp.targetGate = g;
        tmp.channel = chan;
        mod->emit(PRE_MODEL_CHANGE, &tmp);
    }
    cGate *pathStartGate = getPathStartGate();
    cGate *pathEndGate = g->getPathEndGate();
    cModule *pathStartModule = pathStartGate->getOwnerModule();
    cModule *pathEndModule = pathEndGate->getOwnerModule();
    if (pathStartModule->hasListeners(PRE_MODEL_CHANGE) || pathEndModule->hasListeners(PRE_MODEL_CHANGE)) {
        cPrePathCreateNotification tmp;
        tmp.pathStartGate = pathStartGate;
        tmp.pathEndGate = pathEndGate;
        tmp.changedGate = this;
        pathStartModule->emit(PRE_MODEL_CHANGE, &tmp);
        pathEndModule->emit(PRE_MODEL_CHANGE, &tmp);
    }

    // build new connection
    nextGate = g;
    nextGate->prevGate = this;
    connectionId = ++lastConnectionId;
    if (chan)
        installChannel(chan);

    checkChannels();

    // notify envir
    if (chan) {
        getEnvir()->configure(chan);
        chan->addResultRecorders();
    }
    EVCB.connectionCreated(this);
#ifdef SIMFRONTEND_SUPPORT
    mod->updateLastChangeSerial();
#endif

    // initialize the channel here, to simplify dynamic connection creation.
    // Heuristics: if parent module is not yet initialized, we expect that
    // this channel will be initialized as part of it, so don't do it here;
    // otherwise initialize the channel now.
    if (chan && !leaveUninitialized && (!chan->getParentModule() || chan->getParentModule()->initialized()))
        chan->callInitialize();

    // notify post-change listeners
    if (mod->hasListeners(POST_MODEL_CHANGE)) {
        cPostGateConnectNotification tmp;
        tmp.gate = this;
        mod->emit(POST_MODEL_CHANGE, &tmp);
    }
    if (pathStartModule->hasListeners(POST_MODEL_CHANGE) || pathEndModule->hasListeners(POST_MODEL_CHANGE)) {
        cPostPathCreateNotification tmp;
        tmp.pathStartGate = pathStartGate;
        tmp.pathEndGate = pathEndGate;
        tmp.changedGate = this;
        pathStartModule->emit(POST_MODEL_CHANGE, &tmp);
        pathEndModule->emit(POST_MODEL_CHANGE, &tmp);
    }

    return chan;
}

void cGate::installChannel(cChannel *chan)
{
    ASSERT(channel == nullptr && chan != nullptr);
    channel = chan;
    channel->setSourceGate(this);
    take(channel);
}

void cGate::disconnect()
{
    if (!nextGate)
        return;

    // notify pre-change listeners
    cModule *mod = getOwnerModule();
    if (mod->hasListeners(PRE_MODEL_CHANGE)) {
        cPreGateDisconnectNotification tmp;
        tmp.gate = this;
        mod->emit(PRE_MODEL_CHANGE, &tmp);
    }
    cGate *pathStartGate = getPathStartGate();
    cGate *pathEndGate = nextGate->getPathEndGate();
    cModule *pathStartModule = pathStartGate->getOwnerModule();
    cModule *pathEndModule = pathEndGate->getOwnerModule();
    if (pathStartModule->hasListeners(PRE_MODEL_CHANGE) || pathEndModule->hasListeners(PRE_MODEL_CHANGE)) {
        cPrePathCutNotification tmp;
        tmp.pathStartGate = pathStartGate;
        tmp.pathEndGate = pathEndGate;
        tmp.changedGate = this;
        pathStartModule->emit(PRE_MODEL_CHANGE, &tmp);
        pathEndModule->emit(PRE_MODEL_CHANGE, &tmp);
    }

    // notify envir that old conn gets removed
    EVCB.connectionDeleted(this);

    // remove connection (but preserve channel object for the notification)
    cGate *oldnextgatep = nextGate;
    nextGate->prevGate = nullptr;
    nextGate = nullptr;
    connectionId = -1;

    cChannel *oldchannelp = channel;
    channel = nullptr;

#ifdef SIMFRONTEND_SUPPORT
    mod->updateLastChangeSerial();
#endif

    // notify post-change listeners
    if (mod->hasListeners(POST_MODEL_CHANGE)) {
        cPostGateDisconnectNotification tmp;
        tmp.gate = this;
        tmp.targetGate = oldnextgatep;
        tmp.channel = oldchannelp;
        mod->emit(POST_MODEL_CHANGE, &tmp);
    }
    if (pathStartModule->hasListeners(POST_MODEL_CHANGE) || pathEndModule->hasListeners(POST_MODEL_CHANGE)) {
        cPostPathCutNotification tmp;
        tmp.pathStartGate = pathStartGate;
        tmp.pathEndGate = pathEndGate;
        tmp.changedGate = this;
        pathStartModule->emit(POST_MODEL_CHANGE, &tmp);
        pathEndModule->emit(POST_MODEL_CHANGE, &tmp);
    }

    // delete channel object
    dropAndDelete(oldchannelp);
}

void cGate::checkChannels() const
{
    int n = 0;
    for (const cGate *g = getPathStartGate(); g->nextGate != nullptr; g = g->nextGate)
        if (g->channel && g->channel->isTransmissionChannel())
            n++;

    if (n > 1)
        throw cRuntimeError("More than one channel with data rate found in the "
                            "connection path between gates %s and %s",
                getPathStartGate()->getFullPath().c_str(),
                getPathEndGate()->getFullPath().c_str());
}

cChannel *cGate::reconnectWith(cChannel *channel, bool leaveUninitialized)
{
    cGate *otherGate = getNextGate();
    if (!otherGate)
        throw cRuntimeError(this, "reconnectWith(): Gate must be already connected");
    disconnect();
    return connectTo(otherGate, channel, leaveUninitialized);
}

cGate *cGate::getPathStartGate() const
{
    const cGate *g;
    for (g = this; g->prevGate != nullptr; g = g->prevGate)
        ;
    return const_cast<cGate *>(g);
}

cGate *cGate::getPathEndGate() const
{
    const cGate *g;
    for (g = this; g->nextGate != nullptr; g = g->nextGate)
        ;
    return const_cast<cGate *>(g);
}

void cGate::setDeliverOnReceptionStart(bool d)
{
    if (!getOwnerModule()->isSimple())
        throw cRuntimeError(this, "setDeliverOnReceptionStart() may only be invoked on a simple module gate");
    if (getType() != INPUT)
        throw cRuntimeError(this, "setDeliverOnReceptionStart() may only be invoked on an input gate");

    // set b1 on pos
    if (d)
        pos |= 2;
    else
        pos &= ~2;
}

bool cGate::deliver(cMessage *msg, simtime_t t)
{
    if (!nextGate) {
        getOwnerModule()->arrived(msg, this, t);
        return true;
    }
    else if (!channel) {
        EVCB.messageSendHop(msg, this);
        return nextGate->deliver(msg, t);
    }
    else {
        if (!channel->initialized())
            throw cRuntimeError(channel, "Channel not initialized (did you forget to invoke "
                                         "callInitialize() for a dynamically created channel or "
                                         "a dynamically created compound module that contains it?)");
        if (!channel->isTransmissionChannel()) {
            cChannel::result_t tmp;
            channel->processMessage(msg, t, tmp);
            EVCB.messageSendHop(msg, this, tmp.delay, SIMTIME_ZERO, tmp.discard);  // tmp.duration ignored for non-transmission channels
            if (tmp.discard)
                return false;
            return nextGate->deliver(msg, t + tmp.delay);
        }
        else {
            // transmission channel:
            // channel must be idle
            if (channel->getTransmissionFinishTime() > t)
                throw cRuntimeError("Cannot send message (%s)%s on gate %s: Channel is currently "
                                    "busy with an ongoing transmission -- please rewrite the sender "
                                    "simple module to only send when the previous transmission has "
                                    "already finished, using cGate::getTransmissionFinishTime(), scheduleAt(), "
                                    "and possibly a cQueue for storing messages waiting to be transmitted",
                        msg->getClassName(), msg->getFullName(), getFullPath().c_str());

            // message must not have its duration set already
            bool isPacket = msg->isPacket();
            if (isPacket && ((cPacket *)msg)->getDuration() != SIMTIME_ZERO)
                throw cRuntimeError(this, "Packet (%s)%s already has a duration set; there "
                                          "may be more than one channel with data rate in the connection path, or "
                                          "it was sent with a sendDirect() call that specified duration as well",
                        msg->getClassName(), msg->getName());

            // process
            cChannel::result_t tmp;
            channel->processMessage(msg, t, tmp);
            if (isPacket)
                ((cPacket *)msg)->setDuration(tmp.duration);
            EVCB.messageSendHop(msg, this, tmp.delay, tmp.duration, tmp.discard);
            if (tmp.discard)
                return false;
            return nextGate->deliver(msg, t + tmp.delay);
        }
    }
}

cChannel *cGate::findTransmissionChannel() const
{
    for (const cGate *g = this; g->nextGate != nullptr; g = g->nextGate)
        if (g->channel && g->channel->isTransmissionChannel())
            return g->channel;

    return nullptr;
}

cChannel *cGate::getTransmissionChannel() const
{
    cChannel *chan = findTransmissionChannel();
    if (chan)
        return chan;

    // transmission channel not found, try to issue a helpful error message
    if (nextGate)
        throw cRuntimeError(this, "getTransmissionChannel(): No transmission channel "
                                  "found in the connection path between gates %s and %s",
                getFullPath().c_str(),
                getPathEndGate()->getFullPath().c_str());
    else if (getType() == OUTPUT)
        throw cRuntimeError(this, "getTransmissionChannel(): No transmission channel found: "
                                  "gate is not connected");
    else
        throw cRuntimeError(this, "getTransmissionChannel(): Cannot be invoked on a "
                                  "simple module input gate (or a compound module "
                                  "input gate which is not connected on the inside)");
}

cChannel *cGate::findIncomingTransmissionChannel() const
{
    for (const cGate *g = this->prevGate; g != nullptr; g = g->prevGate)
        if (g->channel && g->channel->isTransmissionChannel())
            return g->channel;
    return nullptr;
}

cChannel *cGate::getIncomingTransmissionChannel() const
{
    cChannel *chan = findIncomingTransmissionChannel();
    if (chan)
        return chan;

    // transmission channel not found, try to issue a helpful error message
    if (prevGate)
        throw cRuntimeError(this, "getIncomingTransmissionChannel(): No transmission channel "
                                  "found in the connection path between gates %s and %s",
                getFullPath().c_str(),
                getPathStartGate()->getFullPath().c_str());
    else if (getType() == INPUT)
        throw cRuntimeError(this, "getIncomingTransmissionChannel(): No transmission channel found: "
                                  "gate is not connected");
    else
        throw cRuntimeError(this, "getIncomingTransmissionChannel(): Cannot be invoked on a "
                                  "simple module output gate (or a compound module "
                                  "output gate which is not connected on the inside)");
}

bool cGate::pathContains(cModule *mod, int gate)
{
    cGate *g;

    for (g = this; g != nullptr; g = g->prevGate)
        if (g->getOwnerModule() == mod && (gate == -1 || g->getId() == gate))
            return true;

    for (g = nextGate; g != nullptr; g = g->nextGate)
        if (g->getOwnerModule() == mod && (gate == -1 || g->getId() == gate))
            return true;

    return false;
}

bool cGate::isConnectedOutside() const
{
    if (getType() == INPUT)
        return prevGate != nullptr;
    else
        return nextGate != nullptr;
}

bool cGate::isConnectedInside() const
{
    if (getType() == INPUT)
        return nextGate != nullptr;
    else
        return prevGate != nullptr;
}

bool cGate::isConnected() const
{
    // for compound modules, both inside and outside must be non-nullptr,
    // for simple modules, only check outside.
    if (!getOwnerModule()->isSimple())
        return prevGate != nullptr && nextGate != nullptr;
    else
        return isConnectedOutside();
}

bool cGate::isPathOK() const
{
    return getPathStartGate()->getOwnerModule()->isSimple() &&
           getPathEndGate()->getOwnerModule()->isSimple();
}

cDisplayString& cGate::getDisplayString()
{
    if (!getChannel()) {
        installChannel(cIdealChannel::create("channel"));
        channel->callInitialize();
    }
    return getChannel()->getDisplayString();
}

void cGate::setDisplayString(const char *dispstr)
{
    getDisplayString().set(dispstr);
}

}  // namespace omnetpp

