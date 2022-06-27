//==========================================================================
//  messageanimator.cc - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "messageanimator.h"

#include "qtenv.h"
#include "moduleinspector.h"  // for the layer
#include "messageitem.h"
#include "messageanimations.h"
#include "graphicsitems.h"

#include <QtCore/QDebug>
#include <omnetpp/cfutureeventset.h>
#include <omnetpp/cpacket.h>
#include <omnetpp/csimplemodule.h>
#include <utility>
#include <common/stlutil.h>
#include <memory>

#define emit

namespace omnetpp {

using namespace common;

namespace qtenv {

const MessageAnimator::MessageSendKey MessageAnimator::METHODCALL = {-1, -1, -1, -1, -1, -1};

// --------  MessageAnimator::MessageSendPath::Hop  --------

std::string MessageAnimator::MessageSendPath::Hop::str() const
{
    std::stringstream ss;
    ss << "Hop {";

    ss << " directSrcModule: " << directSrcModule;
    ss << " directDestGate: " << directDestGate;
    ss << " connSrcGate: " << connSrcGate;
    ss << " isLastHop: " << std::boolalpha << isLastHop;
    ss << " discard: " << std::boolalpha << discard;
    ss << " hopStartTime: " << hopStartTime;
    ss << " propDelay: " << propDelay;
    ss << " transDuration: " << transDuration;

    ss << "}";
    return ss.str();
}


// --------  MessageAnimator::MessageSendPath  --------


simtime_t MessageAnimator::MessageSendPath::getStartArrivalTime()
{
    return hops.empty() ? simTime() : (hops.back().hopStartTime + hops.back().propDelay);
}

void MessageAnimator::MessageSendPath::messageDuplicated(cMessage *msg, cMessage *dup)
{
    if (this->msg == msg) {
        ASSERT(this->dupMsg == nullptr);
        this->dupMsg = dup;
    }
}

void MessageAnimator::MessageSendPath::removeMessagePointer(cMessage *msg)
{
    if (this->dupMsg == msg)
        this->dupMsg = nullptr;
    if (this->msg == msg)
        this->msg = nullptr;
}


// --------  MessageAnimator::MessageSendKey  --------


MessageAnimator::MessageSendKey MessageAnimator::MessageSendKey::fromMessage(cMessage *msg)
{
    txid_t txid = msg->isPacket() ? static_cast<cPacket*>(msg)->getTransmissionId() : -1;
    return MessageAnimator::MessageSendKey {
                (txid == -1) ? msg->getId() : -1,
                txid,
                msg->getSenderModuleId(), msg->getSenderGateId(),
                msg->getArrivalModuleId(), msg->getArrivalGateId()
            };
}

std::string MessageAnimator::MessageSendKey::str() const
{
    return "{" + std::to_string(messageId) + ", "   + std::to_string(transmissionId)
        + ": "   + std::to_string(senderModuleId)  + "(" + getSimulation()->getComponent(senderModuleId)->getFullPath()  + ")." + std::to_string(senderGateId)
        + " -> " + std::to_string(arrivalModuleId) + "(" + getSimulation()->getComponent(arrivalModuleId)->getFullPath() + ")." + std::to_string(arrivalGateId)
        + "}";
}


// --------  MessageAnimator  --------


void MessageAnimator::redrawMessages()
{
    // TODO: maybe add some messages as animations, not all of them as waiting on the dest module boundary

    clearMessages();

    // this thingy is only needed if animation is going on
    if (!(getQtenv()->animating && getQtenv()->opt->animationEnabled))
        return;

    // loop through all messages in the event queue and display them
    cFutureEventSet *fes = getSimulation()->getFES();
    int fesLen = fes->getLength();
    for (int i = 0; i < fesLen; i++) {
        cEvent *event = fes->get(i);
        if (!event->isMessage() || event->getArrivalTime() != simTime())
            continue;
        cMessage *msg = (cMessage *)event;

        cModule *arrivalMod = msg->getArrivalModule();

        if (!arrivalMod)
            continue;

        cModule *showIn = arrivalMod->getParentModule();

        if (!showIn || !showIn->getBuiltinAnimationsAllowed())
            continue;

        for (auto& insp : getQtenv()->getInspectors()) {
            auto moduleInsp = dynamic_cast<ModuleInspector *>(insp);

            if (moduleInsp && !willAnimate(msg)
                    && showIn == moduleInsp->getObject()
                    && msg->getArrivalGateId() >= 0) {
                cGate *arrivalGate = msg->getArrivalGate();

                auto messageItem = new SymbolMessageItem(moduleInsp->getAnimationLayer());
                MessageItemUtil::setupSymbolFromDisplayString(messageItem, msg, moduleInsp->getImageSizeFactor());

                // if arrivalGate is connected, msg arrived on a connection, otherwise via sendDirect()
                messageItem->setPos(arrivalGate->getPreviousGate()
                             ? moduleInsp->getConnectionLine(arrivalGate->getPreviousGate()).p2()
                             : moduleInsp->getSubmodCoords(arrivalMod));
                messageItems[std::make_pair(moduleInsp, msg)] = messageItem;
            }
        }
    }
}

void MessageAnimator::skipCurrentHoldingAnims()
{
    delete deliveries;
    deliveries = nullptr;

    for (auto& a : animations)
        if (a->isHolding()) {
            delete a;
            a = nullptr;
        }

    animations.removeValues(nullptr);

    holdRequests.clear();

    currentMethodCall = nullptr;
}

void MessageAnimator::methodcallBegin(cComponent *fromComp, cComponent *toComp, const char *methodText, bool silent)
{
    auto anim = new MethodcallAnimation(fromComp, toComp, methodText, silent);

    if (currentMethodCall)
        currentMethodCall->addOperation(anim);

    currentMethodCall = anim;
}

void MessageAnimator::methodcallEnd()
{
    ASSERT(currentMethodCall);

    auto parent = currentMethodCall->getParent();

    if (!parent) {
        // when the root method call returns, add the whole tree recursively to all inspectors
        for (auto insp : getQtenv()->getInspectors())
            currentMethodCall->addToInspector(insp);

        animations.putMulti(METHODCALL, currentMethodCall);
    }

    currentMethodCall = parent;

    updateAnimations();
}

void MessageAnimator::beginSend(cMessage *msg, const SendOptions& options)
{
    ASSERT(!currentSending);
    currentSending = new MessageSendPath(msg);
}

void MessageAnimator::sendDirect(cMessage *msg, cModule *srcModule, cGate *destGate, const cChannel::Result& result)
{
    ASSERT(currentSending);
    ASSERT(currentSending->msg == msg);
    currentSending->addHop(MessageSendPath::Hop(currentSending->getStartArrivalTime(), srcModule, destGate, result.delay, result.remainingDuration));
}

void MessageAnimator::sendHop(cMessage *msg, cGate *srcGate, bool isLastHop)
{
    ASSERT(currentSending);
    ASSERT(currentSending->msg == msg);
    currentSending->addHop(MessageSendPath::Hop(currentSending->getStartArrivalTime(), srcGate, isLastHop));
}

void MessageAnimator::sendHop(cMessage *msg, cGate *srcGate, bool isLastHop, const cChannel::Result& result)
{
    ASSERT(currentSending);
    ASSERT(currentSending->msg == msg);
    currentSending->addHop(MessageSendPath::Hop(currentSending->getStartArrivalTime(), srcGate, isLastHop, result.delay, result.remainingDuration, result.discard));

    if (result.discard)
        endSend(msg); // At the moment, the animation of a discarded message simply ends halfway on the path
}

void MessageAnimator::endSend(cMessage *msg)
{
    ASSERT(currentSending);
    ASSERT(currentSending->msg == msg);
    ASSERT2(!(currentSending->hops.empty()), "No messageSendDirect() nor messageSendHop() was called between beginSend() and endSend()");

    bool isUpdatePacket = false;
    if (msg->isPacket()) {
        cPacket *packet = static_cast<cPacket *>(msg);
        if (packet->isUpdate()) {
            cutUpdatedPacketAnimation(packet);
            isUpdatePacket = true;
        }
    }

    auto dup = getQtenv()->getLogBuffer()->getLastMessageDup(msg);
    ASSERT(dup);
    // have to do it before turning the sending into an animation, because  ???? TODO
    messageDuplicated(msg, dup);

    updateAnimations();

    // extract the transmission duration from the single hop that may have it as a non-zero value
    simtime_t transDuration = 0;
    for (const auto& h : currentSending->hops) {
        if (!h.transDuration.isZero()) {
            ASSERT(transDuration.isZero() || transDuration == h.transDuration);
            transDuration = h.transDuration;
            // TODO break as optimization? - then the ASSERT above makes less sense...
        }
    }

    // turn the collected hops into either a sequence or a group animation
    Animation *sendAnim;
    if (transDuration.isZero()) {
        // not a transmission

        AnimationSequence *animSeq = new AnimationSequence();
        sendAnim = animSeq;

        for (const auto& h : currentSending->hops) {
            if (h.directSrcModule) {
                ASSERT(h.directDestGate);
                ASSERT(!h.connSrcGate);

                if (h.propDelay.isZero() && transDuration.isZero() && !isUpdatePacket)
                    animSeq->addAnimation(new SendDirectAnimation(h.directSrcModule, msg, h.directDestGate));
                else
                    animSeq->addAnimation(new SendDirectAnimation(h.directSrcModule, msg, h.directDestGate, h.hopStartTime, h.propDelay, transDuration));
            }
            else {
                // it was sent on a connection
                ASSERT(!h.directDestGate);
                ASSERT(h.connSrcGate);

                if (h.propDelay.isZero() && transDuration.isZero() && !isUpdatePacket)
                    animSeq->addAnimation(new SendOnConnAnimation(h.connSrcGate, msg));
                else
                    animSeq->addAnimation(new SendOnConnAnimation(h.connSrcGate, msg, h.hopStartTime, h.propDelay, transDuration, h.discard));
            }
        }

        if (!animSeq->isEmpty())
            animSeq->advance();
    }
    else {
        // a transmission

        // an AnimationGroup is needed here, so the transmission (message in line form)
        // can be shown and animated on multiple connections on the same path at the same time
        AnimationGroup *animGroup = new AnimationGroup();
        sendAnim = animGroup;

        for (const auto& h : currentSending->hops) {
            if (h.directSrcModule) {
                ASSERT(h.directDestGate);
                ASSERT(!h.connSrcGate);

                animGroup->addAnimation(new SendDirectAnimation(h.directSrcModule, msg, h.directDestGate, h.hopStartTime, h.propDelay, transDuration));
            }
            else {
                ASSERT(h.connSrcGate);
                ASSERT(!h.directDestGate);

                animGroup->addAnimation(new SendOnConnAnimation(h.connSrcGate, msg, h.hopStartTime, h.propDelay, transDuration, h.discard));
            }
        }

        if (!animGroup->isEmpty())
            animGroup->advance();
    }

    sendAnim->messageDuplicated(msg, dup);

    for (auto insp : getQtenv()->getInspectors())
        sendAnim->addToInspector(insp);

    // if we are in a method, and this whole message sequence will take 0 SimTime, performing it in the method
    if (currentMethodCall && sendAnim->isHolding() && transDuration.isZero())
        currentMethodCall->addOperation(sendAnim);
    else {
        // otherwise after the method.
        // XXX maybe split it in two, so if the first few parts are instantaneous, play them, and continue later?
        MessageSendKey key = MessageSendKey::fromMessage(msg);
        animations.putMulti(key, sendAnim);
    }

    delete currentSending;
    currentSending = nullptr;

    updateAnimations();
}

void MessageAnimator::deliveryDirect(cMessage *msg)
{
    ASSERT(!currentSending);

    cGate *g = msg->getArrivalGate();
    ASSERT(g);
    cModule *destmod = g->getOwnerModule();
    cModule *mod = destmod->getParentModule();

    addDeliveryAnimation(msg, mod, new DeliveryAnimation(msg));
}

void MessageAnimator::delivery(cMessage *msg)
{
    ASSERT(!currentSending);

    cGate *g = msg->getArrivalGate();
    ASSERT(g);
    g = g->getPreviousGate();
    ASSERT(g);

    cModule *mod = g->getOwnerModule();
    if (g->getType() == cGate::OUTPUT)
        mod = mod->getParentModule();

    addDeliveryAnimation(msg, mod, new DeliveryAnimation(g, msg));
}

bool MessageAnimator::willAnimate(cMessage *msg)
{
    for (auto p : animations)
        if (p && p->willAnimate(msg))
            return true;

    return currentSending && currentSending->msg == msg;
}

void MessageAnimator::setMarkedModule(cModule *mod)
{
    markedModule = mod;
    updateNextEventMarkers();
}

void MessageAnimator::clearDeliveries()
{
    delete deliveries;
    deliveries = nullptr;
}

void MessageAnimator::updateAnimations()
{
    // Always updating all non-holding animations first that are already playing.
    // But not beginning any pending/waiting anims before the deliveries.
    for (auto& p : animations)
        if (!p->isHolding() && (p->getState() == Animation::PLAYING)) {
            p->update();
            if (p->getState() == Animation::DONE) {
                delete p;
                p = nullptr;
            }
        }
    // Removing the ones that are done.
    animations.removeValues(nullptr);

    // Then come the deliveries, if any.
    if (deliveries && deliveries->advance())
        return;
    else {
        // If done, removing it.
        delete deliveries;
        deliveries = nullptr;
    }

    // If there were no deliveries (or the last one just ended):

    // Then advancing the holding anims that we haven't already.
    for (auto& p : animations)
        if (!p->isHolding() && (p->getState() != Animation::PLAYING) && !p->advance()) {
            delete p;
            p = nullptr;
        }
    // Removing the ones that are done.
    animations.removeValues(nullptr);

    // Finally continuing with the rest of the animations, that are holding.

    if (getQtenv()->getPref("concurrent-anim", false).toBool()) {
        bool first = true;

        // This is the ID of the source module of the first animation.
        // (The real source, the message sender, not just the "line" source.)
        // -1 if not known or not available
        int broadcastingModuleId = -1;

        // If the next animation is a methodcall, we only advance that.
        // If it is a holding messagesend, then we animate all holding
        // messagesends at the beginning of the animations "list" together.
        for (auto& p : animations)
            if (p->isHolding()) {
                bool isMethodcall = dynamic_cast<MethodcallAnimation *>(p);
                if (isMethodcall && !first)
                    break;

                int srcId = p->getSourceModuleId();

                if (first)
                    broadcastingModuleId = srcId;
                else // Do not animate any other messages concurrently that originated somewhere else.
                    if (broadcastingModuleId == -1 || broadcastingModuleId != srcId)
                        break;

                if (!p->advance()) {
                    delete p;
                    p = nullptr;
                } else {
                    first = false;
                    if (isMethodcall)
                        break;
                }
            }
    }
    else {
        // Only advancing them until one is found that is not instantly done.
        for (auto& p : animations)
            if (p->isHolding()) {
                if (!p->advance()) {
                    delete p;
                    p = nullptr;
                }
                // isHolding can change
                if (p && p->isHolding())
                    break;
            }
    }

    // And removing the ones that are done.
    animations.removeValues(nullptr);
}

void MessageAnimator::clearMessages()
{
    for (auto i : messageItems)
        delete i.second;
    messageItems.clear();
}

void MessageAnimator::updateNextEventMarkers()
{
    // Have to add the markers we might have cleared before.
    for (auto in : getQtenv()->getInspectors())
        if (auto mi = dynamic_cast<ModuleInspector*>(in))
            if (!containsKey(nextEventMarkers, mi)) {
                auto marker = new QGraphicsRectItem;
                mi->getAnimationLayer()->addItem(marker);
                nextEventMarkers[mi] = marker;
            }

    for (auto p : nextEventMarkers)
        p.second->setVisible(false);

    // this thingy is only needed if animation is going on
    if (!getQtenv()->animating || !markedModule || !getQtenv()->opt->showNextEventMarkers)
        return;

    for (auto p : nextEventMarkers) {
        ModuleInspector *mi = static_cast<ModuleInspector *>(p.first);
        cModule *mod = static_cast<cModule *>(mi->getObject());
        auto item = p.second;

        cModule *modParent = markedModule;
        while (modParent && modParent->getParentModule() != mod)
            modParent = modParent->getParentModule();

        if (modParent) {
            item->setVisible(true);
            item->setRect(mi->getSubmodRect(modParent).adjusted(-2, -2, 2, 2));
            item->setBrush(Qt::NoBrush);
            item->setPen(QPen(QColor("red"), markedModule == modParent ? 2 : 1));
            item->setZValue(10);
        } else {
            item->setVisible(false);
        }
    }
}

void MessageAnimator::addDeliveryAnimation(cMessage *msg, cModule *showIn, DeliveryAnimation *anim)
{
    auto dup = getQtenv()->getLogBuffer()->getLastMessageDup(msg);
    ASSERT(dup);

    anim->messageDuplicated(msg, dup);

    for (auto in : getQtenv()->getInspectors())
        if (auto insp = isModuleInspectorFor(showIn, in))
            anim->addToInspector(insp);

    if (!deliveries)
        deliveries = new AnimationSequence();

    deliveries->addAnimation(anim);

    anim->removeMessagePointer(msg);

    updateAnimations();
}

void MessageAnimator::cutUpdatedPacketAnimation(cPacket *updatePacket)
{
    // the original animation was supposed to take the same path
    MessageSendKey key = MessageSendKey::fromMessage(updatePacket);

    if (animations.containsKey(key)) {
        // This getLast() will make sure that always the last (of the previous ones) animation
        // will be cut short (in case this is not the first update packet of this transmission).
        Animation *lastAnim = animations.getLast(key);

        auto group = dynamic_cast<AnimationGroup*>(lastAnim);
        auto sequence = dynamic_cast<AnimationSequence*>(lastAnim);
        ASSERT(group != nullptr || sequence != nullptr);

        if (group) {
            SimTime duration;
            for (Animation *part : group->getParts())
                if (MessageAnimation *msgAnim = dynamic_cast<MessageAnimation *>(part)) {
                    duration = simTime() - msgAnim->getStartTime();
                    break;
                }

            for (Animation *part : group->getParts())
                if (MessageAnimation *msgAnim = dynamic_cast<MessageAnimation *>(part))
                    if (!msgAnim->getTransmissionDuration().isZero())
                        msgAnim->setTransmissionDuration(duration);
        }

        // we have nothing to do in case it is a sequence, that only
        // animates zero-length (non-transmission) sends anyway
    }
    else {
        // The animation for the updated packet has already ended, or this is a
        // bogus update. Either way, we ignore it now. Should we be stricter?
    }
}

MethodcallAnimation *MessageAnimator::getCurrentMethodCallRoot() const
{
    MethodcallAnimation *cur = currentMethodCall;
    while (cur && cur->getParent())
        cur = cur->getParent();
    return cur;
}

double MessageAnimator::getAnimationHoldEndTime() const
{
    double rem = -1;
    for (auto a : holdRequests)
        rem = std::max(rem, a->getHoldEndTime());

    return rem;
}

void MessageAnimator::setAnimationSpeed(double speed, const Animation *source)
{
    if (speed <= 0.0)
        animSpeedMap.erase(source);
    else
        animSpeedMap[source] = speed;
}

double MessageAnimator::getAnimationSpeed()
{
    double speed = DBL_MAX;

    for (auto p : animSpeedMap)
        speed = std::min(speed, p.second);

    if (speed == DBL_MAX)
        speed = 0;

    return speed;
}

void MessageAnimator::clear()
{
    delete deliveries;
    deliveries = nullptr;

    for (auto a : animations)
        delete a;

    animations.clear();

    for (auto a : nextEventMarkers)
        delete a.second;

    nextEventMarkers.clear();

    animSpeedMap.clear();
    holdRequests.clear();

    currentMethodCall = nullptr;
    markedModule = nullptr;

    delete currentSending;
    currentSending = nullptr;

    clearMessages();
}

void MessageAnimator::addInspector(ModuleInspector *insp)
{
    redrawMessages();
    for (auto &p : animations)
        p->addToInspector(insp);
    if (deliveries)
        deliveries->addToInspector(insp);
    updateAnimations();
    updateNextEventMarkers();
}

void MessageAnimator::updateInspector(ModuleInspector *insp)
{
    redrawMessages();
    for (auto &p : animations)
        p->updateInInspector(insp);
    if (deliveries)
        deliveries->updateInInspector(insp);
    updateAnimations();
    updateNextEventMarkers();
}

void MessageAnimator::clearInspector(ModuleInspector *insp)
{
    for (auto &p : animations)
        p->removeFromInspector(insp);

    if (deliveries)
        deliveries->removeFromInspector(insp);

    for (auto it = messageItems.begin(); it != messageItems.end();  /* blank */)
        if ((*it).first.first == insp) {
            delete (*it).second;
            messageItems.erase(it++);
        }
        else
            ++it;

    delete nextEventMarkers[insp];
    nextEventMarkers.erase(insp);

    updateAnimations();
}

void MessageAnimator::messageDuplicated(cMessage *msg, cMessage *dup)
{
    for (auto a : animations)
        a->messageDuplicated(msg, dup);

    if (currentSending)
        currentSending->messageDuplicated(msg, dup);

    MethodcallAnimation *methodCallRoot = getCurrentMethodCallRoot();
    if (methodCallRoot)
        methodCallRoot->messageDuplicated(msg, dup);

    if (deliveries)
        deliveries->messageDuplicated(msg, dup);
}

void MessageAnimator::removeMessagePointer(cMessage *msg)
{
    for (auto i : messageItems)
        if (i.first.second == msg)
            i.second->setData(ITEMDATA_COBJECT, QVariant());

    for (auto a : animations)
        a->removeMessagePointer(msg);

    if (currentSending)
        // most likely a channel was disabled, or discarded the message for some other reason - between a beginSend()/endSend() pair.
        currentSending->removeMessagePointer(msg);

    MethodcallAnimation *methodCallRoot = getCurrentMethodCallRoot();
    if (methodCallRoot)
        methodCallRoot->removeMessagePointer(msg);

    if (deliveries)
        deliveries->removeMessagePointer(msg);
}

}  // namespace qtenv
}  // namespace omnetpp
