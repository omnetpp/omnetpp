//========================================================================
//  CGATE.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//   Member functions of
//    cGate      : a module gate
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
#include "omnetpp/ccontextswitcher.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cdataratechannel.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/simutil.h"
#include "omnetpp/cmodelchange.h"
#include "omnetpp/cexception.h"

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
static StaticStringPool gateFullnamePool;

int cGate::lastConnectionId = -1;

cGate::Name::Name(const char *name, Type type) : name(name), type(type)
{
    if (type == cGate::INOUT) {
        int len = strlen(name);
        namei.reserve(len+3); strcpy(namei.buffer(), name); strcpy(namei.buffer()+len, "$i");
        nameo.reserve(len+3); strcpy(nameo.buffer(), name); strcpy(nameo.buffer()+len, "$o");
    }
}

bool cGate::Name::operator<(const Name& other) const
{
    int d = opp_strcmp(name.c_str(), other.name.c_str());
    if (d < 0)
        return true;
    else if (d > 0)
        return false;
    else
        return type < other.type;
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

bool cGate::isGateHalf() const
{
    return desc->name->type == INOUT;
}

cGate *cGate::getOtherHalf() const
{
    if (!isGateHalf())
        return nullptr; // has no other half
    else
        return getOwnerModule()->gateHalf(getBaseName(), getType()==INPUT ? OUTPUT : INPUT, isVector() ? getIndex() : -1);
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
    if (!isVector())
        return getName();
    else {
        static char buf[128];
        opp_indexedname(buf, sizeof(buf), getName(), getIndex());
        return gateFullnamePool.get(buf);  // non-refcounted stringpool
    }
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

int cGate::getIndex() const
{
    if (!desc->isVector())
        throw cRuntimeError(this, "getIndex(): Gate is not member of a gate vector");
    return desc->indexOf(this);
}

int cGate::getVectorSize() const
{
    if (!desc->isVector())
        throw cRuntimeError(this, "getVectorSize(): Gate is not member of a gate vector");
    return desc->gateSize();
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

    cModule *parentModule = channel->getParentModule();
    parentModule->insertChannel(chan);
}

void cGate::disconnect()
{
    doDisconnect(true);
}

void cGate::doDisconnect(bool shouldCallPreDelete)
{
    if (!nextGate)
        return;

    if (channel && shouldCallPreDelete)
        channel->callPreDelete(channel);

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
    cChannel *oldChannel = channel;
    if (channel) {
        cModule *parentModule = channel->getParentModule();
        parentModule->removeChannel(channel);
        channel = nullptr;
    }

    cGate *oldNextGate = nextGate;
    nextGate->prevGate = nullptr;
    nextGate = nullptr;
    connectionId = -1;


#ifdef SIMFRONTEND_SUPPORT
    mod->updateLastChangeSerial();
#endif

    // notify post-change listeners
    if (mod->hasListeners(POST_MODEL_CHANGE)) {
        cPostGateDisconnectNotification tmp;
        tmp.gate = this;
        tmp.targetGate = oldNextGate;
        tmp.channel = oldChannel;
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
    if (oldChannel) {
        oldChannel->setFlag(cComponent::FL_DELETING, true);
        drop(oldChannel);
        cContextSwitcher tmp(oldChannel); // channel must be in context so that it is allowed to delete potential model objects it owns
        delete oldChannel;
    }
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

void cGate::setDeliverImmediately(bool d)
{
    if (!getOwnerModule()->isSimple())
        throw cRuntimeError(this, "setDeliverImmediately() may only be invoked on a simple module gate");
    if (getType() != INPUT)
        throw cRuntimeError(this, "setDeliverImmediately() may only be invoked on an input gate");

    // set b1 on pos
    if (d)
        pos |= 2;
    else
        pos &= ~2;
}

bool cGate::deliver(cMessage *msg, const SendOptions& options, simtime_t t)
{
    if (!nextGate) {
        getOwnerModule()->arrived(msg, this, options, t);
        return true;
    }
    else if (!channel) {
        EVCB.messageSendHop(msg, this);
        return nextGate->deliver(msg, options, t);
    }
    else {
        if (!channel->initialized())
            throw cRuntimeError(channel, "Channel not initialized (did you forget to invoke "
                                         "callInitialize() for a dynamically created channel or "
                                         "a dynamically created compound module that contains it?)");

        // let the channel process the message
        cChannel::Result result = channel->processMessage(msg, options, t);
        EVCB.messageSendHop(msg, this, result);
        if (result.discard)
            return false;
        return nextGate->deliver(msg, options, t + result.delay);
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
    cModule *module = getOwnerModule();
    if (module->isSimple() || module->isPlaceholder())
        return isConnectedOutside();
    else
        return prevGate != nullptr && nextGate != nullptr;
}

bool cGate::isPathOK() const
{
    return getPathStartGate()->getOwnerModule()->isSimple() &&
           getPathEndGate()->getOwnerModule()->isSimple();
}

cDisplayString& cGate::getDisplayString()
{
    if (!getChannel()) {
        if (!getNextGate())
            throw cRuntimeError(this, "getDisplayString(): Connection display string is not available, because this gate is not the source gate of a connection (i.e. getNextGate() is nullptr)");
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

