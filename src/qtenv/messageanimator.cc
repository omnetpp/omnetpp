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
#include "logbuffer.h"

#include <QtCore/QDebug>
#include <omnetpp/cfutureeventset.h>
#include <omnetpp/cpacket.h>
#include <omnetpp/csimplemodule.h>
#include <utility>
#include <common/stlutil.h>
#include <memory>

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
    if (!(showAnimations && getQtenv()->opt->animationEnabled))
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

        for (auto moduleInsp : inspectors) {
            if (!willAnimate(msg)
                    && showIn == moduleInsp->getObject()
                    && msg->getArrivalGateId() >= 0) {
                cGate *arrivalGate = msg->getArrivalGate();
                cGate *previousGate = arrivalGate->getPreviousGate();

                // skip any degenerate connections made dynamically at runtime (crossing module boundaries without gates)
                if (previousGate && !(previousGate->getOwnerModule() == showIn || previousGate->getOwnerModule()->getParentModule() == showIn))
                    continue;

                auto messageItem = new SymbolMessageItem(moduleInsp->getAnimationLayer());
                MessageItemUtil::setupSymbolFromDisplayString(messageItem, msg, moduleInsp->getImageSizeFactor());

                // if arrivalGate is connected, msg arrived on a connection, otherwise via sendDirect()
                messageItem->setPos(previousGate
                             ? moduleInsp->getConnectionLine(previousGate).p2()
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

    for (auto& e : holdingAnims)
        delete e.second;
    holdingAnims.clear();

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
        for (auto insp : inspectors)
            currentMethodCall->addToInspector(insp);

        holdingAnims.push_back({METHODCALL, currentMethodCall});
    }

    currentMethodCall = parent;

    if (getQtenv()->getSimulationRunMode() != RUNMODE_FAST)
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

    if (getQtenv()->getSimulationRunMode() != RUNMODE_FAST)
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

    std::vector<MessageAnimation *> parts;
    parts.reserve(currentSending->hops.size());

    // turn the collected hops into either a sequence or a group animation
    for (const auto& h : currentSending->hops) {
        MessageAnimation *hopAnim = nullptr;

        if (h.directSrcModule) {
            // it was a direct send
            ASSERT(h.directDestGate);
            ASSERT(!h.connSrcGate);

            if (transDuration.isZero() && h.propDelay.isZero() && !isUpdatePacket)
                hopAnim = new SendDirectAnimation(h.directSrcModule->getId(), msg, h.directDestGate->getOwnerModule()->getId());
            else
                hopAnim = new SendDirectAnimation(h.directSrcModule->getId(), msg, h.directDestGate->getOwnerModule()->getId(), h.hopStartTime, h.propDelay, transDuration);
        }
        else {
            // it was sent on a connection
            ASSERT(!h.directDestGate);
            ASSERT(h.connSrcGate);

            if (transDuration.isZero() && h.propDelay.isZero() && !isUpdatePacket)
                hopAnim = new SendOnConnAnimation(h.connSrcGate, msg);
            else
                hopAnim = new SendOnConnAnimation(h.connSrcGate, msg, h.hopStartTime, h.propDelay, transDuration, h.discard);
        }

        ASSERT(hopAnim);
        animationsForMessages.insert(std::make_pair(msg, hopAnim));
        parts.push_back(hopAnim);
    }

    MessageSendKey key = MessageSendKey::fromMessage(msg);

    Animation *sendAnim;
    if (transDuration.isZero()) {
        AnimationSequence *animSeq = new AnimationSequence();
        sendAnim = animSeq;

        for (MessageAnimation *p : parts)
            animSeq->addAnimation(p);
    }
    else {
        // an AnimationGroup is needed here, so the transmission (message in line form)
        // can be shown and animated on multiple connections on the same path at the same time
        AnimationGroup *animGroup = new AnimationGroup();
        sendAnim = animGroup;

        for (MessageAnimation *p : parts)
            animGroup->addAnimation(p);
    }

    for (auto insp : getQtenv()->getInspectors())
        sendAnim->addToInspector(insp);

    // if we are in a method, and this whole message sequence will take 0 SimTime, performing it in the method
    if (currentMethodCall && sendAnim->isHolding() && transDuration.isZero())
        currentMethodCall->addOperation(sendAnim);
    else {
        // otherwise after the method.
        // XXX maybe split it in two, so if the first few parts are instantaneous, play them, and continue later?
        nonHoldingAnims[key].push_back(sendAnim);
    }

    auto dup = logBuffer->getLastMessageDup(msg);
    ASSERT(dup);
    messageDuplicated(msg, dup);

    delete currentSending;
    currentSending = nullptr;

    removeMessagePointer(msg);

    if (getQtenv()->getSimulationRunMode() != RUNMODE_FAST)
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
    auto range = animationsForMessages.equal_range(msg);
    for (auto p = range.first; p != range.second; p++)
        if (p->second && p->second->willAnimate(msg))
            return true;

    cMessage *msgDup = logBuffer->getLastMessageDup(msg);
    if (msgDup) {
        auto rangeDup = animationsForMessages.equal_range(msgDup);
        for (auto p = rangeDup.first; p != rangeDup.second; p++)
            if (p->second && p->second->willAnimate(msgDup))
                return true;
    }

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
    for (auto& e : nonHoldingAnims) {
        for (auto& p : e.second)
            if (p->getState() == Animation::PLAYING) {
                p->update();
                if (p->getState() == Animation::FINISHED)
                    p->cleanup();
                if (p->getState() == Animation::DONE) {
                    delete p;
                    p = nullptr;
                }
            }
    }
    cleanupNonHoldingAnims();

    // Then come the deliveries, if any.
    if (deliveries && deliveries->advance())
        return;
    else {
        // If done, removing it.
        delete deliveries;
        deliveries = nullptr;
    }

    // If there were no deliveries (or the last one just ended):

    // Then advancing the non-holding anims that we haven't already.
    for (auto& e : nonHoldingAnims) {
        for (auto& p : e.second)
            if (!p->isHolding() && (p->getState() != Animation::PLAYING) && (p->getState() == Animation::DONE || !p->advance())) {
                delete p;
                p = nullptr;
            }
    }
    cleanupNonHoldingAnims();

    // Finally continuing with the rest of the animations, that are holding.

    if (broadcastAnimation) {
        bool first = true;

        // This is the ID of the source module of the first animation.
        // (The real source, the message sender, not just the "line" source.)
        // -1 if not known or not available
        int broadcastingModuleId = -1;

        // If the next animation is a methodcall, we only advance that.
        // If it is a holding messagesend, then we animate all holding
        // messagesends at the beginning of the animations "list" together.
        for (auto& e : holdingAnims) {
            auto& p = e.second;
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
        // Only advancing them until one is found that is not instantly done,
        // AND doesn't turn into a non-holding one immediately.
        for (auto& e : holdingAnims) {
            auto& p = e.second;
            if (!p->advance()) {
                // This anim has just ended, remove it and carry on.
                delete p;
                p = nullptr;
            } // no `else break;` here, breaking has further conditions:
            // If the anim has not ended yet (didn't get removed), but has turned
            // into non-holding instead (`isHolding` can change in `advance`), we
            // can still go on to the next holding anim. So, we should only stop
            // if the current animation is still not done, and is still holding.
            if (p && p->isHolding())
                break;
        }
    }

    cleanupHoldingAnims();
}

void MessageAnimator::cleanupNonHoldingAnims()
{
    // Removing the ones that are done, moving the holding ones to holding...
    for (auto it = nonHoldingAnims.begin(); it != nonHoldingAnims.end(); ) {
        for (auto vectorIt = it->second.begin(); vectorIt != it->second.end(); ) {
            if (*vectorIt == nullptr)
                vectorIt = it->second.erase(vectorIt);
            else if ((*vectorIt)->isHolding()) {
                holdingAnims.push_back({it->first, *vectorIt});
                vectorIt = it->second.erase(vectorIt);
            }
            else
                ++vectorIt;
        }

        if (it->second.empty())
            it = nonHoldingAnims.erase(it);
        else
            ++it;
    }
}

void MessageAnimator::cleanupHoldingAnims()
{
    // And removing the ones that are done, moving the nonholding ones to nonholding...
    for (auto it = holdingAnims.begin(); it != holdingAnims.end(); ) {
        if (it->second == nullptr)
            it = holdingAnims.erase(it);
        else if (!it->second->isHolding()) {
            nonHoldingAnims[it->first].push_back(it->second);
            it = holdingAnims.erase(it);
        }
        else
            ++it;
    }
}

void MessageAnimator::clearMessages()
{
    for (auto i : messageItems)
        delete i.second;
    messageItems.clear();
}

void MessageAnimator::setShowAnimations(bool show)
{
    if (showAnimations != show) {
        if (show) {
            for (ModuleInspector *mi : inspectors)
                addGraphicsToInspector(mi);
        }
        else {
            for (ModuleInspector *mi : inspectors)
                removeGraphicsFromInspector(mi);
        }
    }

    showAnimations = show;
}

void MessageAnimator::updateNextEventMarkers()
{
    // Have to add the markers we might have cleared before.
    for (auto in : inspectors)
        if (auto mi = dynamic_cast<ModuleInspector*>(in))
            if (!containsKey(nextEventMarkers, mi)) {
                auto marker = new QGraphicsRectItem;
                mi->getAnimationLayer()->addItem(marker);
                nextEventMarkers[mi] = marker;
            }

    for (auto p : nextEventMarkers)
        p.second->setVisible(false);

    // this thingy is only needed if animation is going on
    if (!showAnimations || !markedModule || !getQtenv()->opt->showNextEventMarkers)
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
            item->setPen(QPen(colors::RED, markedModule == modParent ? 2 : 1));
            item->setZValue(10);
        } else {
            item->setVisible(false);
        }
    }
}

void MessageAnimator::addDeliveryAnimation(cMessage *msg, cModule *showIn, DeliveryAnimation *anim)
{
    auto dup = logBuffer->getLastMessageDup(msg);
    ASSERT(dup);

    anim->messageDuplicated(msg, dup);

    for (auto mi : inspectors)
        if (mi->getObject() == showIn)
            anim->addToInspector(mi);

    if (!deliveries)
        deliveries = new AnimationSequence();

    deliveries->addAnimation(anim);

    anim->removeMessagePointer(msg);

    animationsForMessages.insert(std::make_pair(dup, anim));

    if (getQtenv()->getSimulationRunMode() != RUNMODE_FAST)
        updateAnimations();
}

void MessageAnimator::cutUpdatedPacketAnimation(cPacket *updatePacket)
{
    // the original animation was supposed to take the same path
    MessageSendKey key = MessageSendKey::fromMessage(updatePacket);

    auto iter = nonHoldingAnims.find(key);
    if (iter != nonHoldingAnims.end()) {
        Animation *lastAnim = iter->second.back();

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
        // The animation for the updated packet has already ended, is holding
        // at the moment (which it should never be), or this is a bogus update.
        // Either way, we ignore it now. Should we be stricter?
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

void MessageAnimator::messageAnimationDeleted(MessageAnimation *anim)
{
    std::vector<cMessage *> messages = anim->collectAnimatedMessages();

    for (cMessage *msg : messages) {
        auto range = animationsForMessages.equal_range(msg);
        for (auto it = range.first; it != range.second;) {
            if (it->second == anim)
                it = animationsForMessages.erase(it);
            else
                ++it;
        }
    }
}

void MessageAnimator::setAnimationSpeed(double speed, const Animation *source)
{
    auto existingRequest = animSpeedMap.find(source); // may be end() if not found

    if (speed <= 0.0) {
        // Cancelling an animation speed request.
        // If an animation speed request was cancelled, and it was the current minimum, the cached
        // value should be cleared, in case there are no other requests left with the same speed.
        if (existingRequest != animSpeedMap.end() && existingRequest->second == minAnimSpeed)
            minAnimSpeed = -1;
        animSpeedMap.erase(source); // doing this last to not invalidate `existingRequest`
    }
    else {
        // Adding a new animation speed request or updating an existing one.
        if (minAnimSpeed >= 0) {
            // If we have a cached minimum speed value..
            if (speed < minAnimSpeed)
                // ... and the newly inserted one is smaller, we can simply update the cached value.
                minAnimSpeed = speed;
            else
                // ... otherwise, if an existing request is updated, which also happened to be the
                // minimum until now, the minimum might be higher now so we have to clear the cached value.
                if (existingRequest != animSpeedMap.end() && existingRequest->second == minAnimSpeed)
                    minAnimSpeed = -1;
        }
        animSpeedMap[source] = speed; // doing this last to not invalidate `existingRequest`
    }
}

double MessageAnimator::getAnimationSpeed()
{
    if (minAnimSpeed >= 0)
        return minAnimSpeed;

    if (animSpeedMap.empty())
        return 0;

    double speed = DBL_MAX;

    for (auto p : animSpeedMap)
        speed = std::min(speed, p.second);

    ASSERT(speed != DBL_MAX);

    minAnimSpeed = speed;
    return minAnimSpeed;
}

void MessageAnimator::clear()
{
    delete deliveries;
    deliveries = nullptr;

    for (auto a : holdingAnims)
        delete a.second;

    holdingAnims.clear();

    for (auto& a : nonHoldingAnims)
        for (auto p : a.second)
            delete p;

    nonHoldingAnims.clear();


    for (auto a : nextEventMarkers)
        delete a.second;

    nextEventMarkers.clear();

    animSpeedMap.clear();
    minAnimSpeed = -1;
    holdRequests.clear();

    currentMethodCall = nullptr;
    markedModule = nullptr;

    delete currentSending;
    currentSending = nullptr;

    clearMessages();
}

void MessageAnimator::addInspector(ModuleInspector *insp)
{
    inspectors.push_back(insp);
    if (showAnimations)
        addGraphicsToInspector(insp);
}

void MessageAnimator::addGraphicsToInspector(ModuleInspector *insp)
{
    redrawMessages();
    for (auto& p : holdingAnims)
        p.second->addToInspector(insp);
    for (auto& p : nonHoldingAnims)
        for (auto& a : p.second)
            a->addToInspector(insp);
    if (deliveries)
        deliveries->addToInspector(insp);
    updateAnimations();
    updateNextEventMarkers();
}

void MessageAnimator::updateInspector(ModuleInspector *insp)
{
    redrawMessages();
    for (auto& p : holdingAnims)
        p.second->updateInInspector(insp);
    for (auto& p : nonHoldingAnims)
        for (auto& a : p.second)
            a->updateInInspector(insp);
    if (deliveries)
        deliveries->updateInInspector(insp);
    updateAnimations();
    updateNextEventMarkers();
}

void MessageAnimator::removeInspector(ModuleInspector *insp)
{
    if (showAnimations)
        removeGraphicsFromInspector(insp);
    remove(inspectors, insp);
}

void MessageAnimator::removeGraphicsFromInspector(ModuleInspector *insp)
{
    for (auto& p : holdingAnims)
        p.second->removeFromInspector(insp);
    for (auto& p : nonHoldingAnims)
        for (auto& a : p.second)
            a->removeFromInspector(insp);

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
    auto range = animationsForMessages.equal_range(msg);
    std::vector<std::pair<cMessage *, MessageAnimation *>> toInsert;
     for (auto a = range.first; a != range.second; ++a) {
        if ((*a).second->messageDuplicated(msg, dup))
            toInsert.push_back({dup, (*a).second});
     }

    for (auto p : toInsert)
        animationsForMessages.insert(p);

    if (currentSending)
        currentSending->messageDuplicated(msg, dup);
}

void MessageAnimator::removeMessagePointer(cMessage *msg)
{
    for (auto i : messageItems)
        if (i.first.second == msg)
            i.second->setData(ITEMDATA_COBJECT, QVariant());

    auto range = animationsForMessages.equal_range(msg);
    for (auto it = range.first; it != range.second;) {
        it->second->removeMessagePointer(msg);
        it = animationsForMessages.erase(it);
    }

    if (currentSending)
        // most likely a channel was disabled, or discarded the message for some other reason - between a beginSend()/endSend() pair.
        currentSending->removeMessagePointer(msg);
}

void MessageAnimator::dump()
{
    std::cout << "========" << std::endl;
    std::cout << "holding anims:" << std::endl;
    for (auto a : holdingAnims)
        std::cout << a.first.str() << " -> " << (a.second ? a.second->str().toStdString() : "<NULL>") << std::endl;
    std::cout << "--------" << std::endl;
    std::cout << "nonholding anims:" << std::endl;
    for (auto p : nonHoldingAnims) {
        std::cout << p.first.str() << " -> " << std::endl;
        for (auto& a : p.second)
            std::cout << "    " << (a ? a->str().toStdString() : "<NULL>") << std::endl;
    }
}

}  // namespace qtenv
}  // namespace omnetpp
