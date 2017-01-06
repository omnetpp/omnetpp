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

#include <QDebug>
#include <omnetpp/cfutureeventset.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/csimplemodule.h>
#include <utility>
#include <common/stlutil.h>
#include <memory>

#define emit

namespace omnetpp {

using namespace common;

namespace qtenv {

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
        if (!event->isMessage())
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

                auto messageItem = new MessageItem(moduleInsp->getAnimationLayer());
                MessageItemUtil::setupFromDisplayString(messageItem, msg, moduleInsp->getImageSizeFactor());

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

    /*
    lastHopTime = -1;
    currentMessageSend = nullptr;

    clearMessages();
    */
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

        animations.putMulti(nullptr, currentMethodCall);
    }

    currentMethodCall = parent;

    updateAnimations();
}

void MessageAnimator::beginSend(cMessage *msg)
{
    ASSERT(!currentMessageSend);
    currentMessageSend = new AnimationSequence();

    ASSERT(lastHopTime == -1);
    lastHopTime = msg->getSendingTime();
}

void MessageAnimator::sendDirect(cMessage *msg, cModule *srcModule, cGate *destGate, simtime_t prop, simtime_t trans)
{
    ASSERT(currentMessageSend);
    currentMessageSend->addAnimation(
        (prop + trans).isZero()
            ? new SendDirectAnimation(srcModule, msg, destGate)
            : new SendDirectAnimation(srcModule, msg, destGate, lastHopTime, prop, trans));

    lastHopTime += prop;
    lastHopTime += trans;
}

void MessageAnimator::sendHop(cMessage *msg, cGate *srcGate, bool isLastHop)
{
    ASSERT(currentMessageSend);
    currentMessageSend->addAnimation(new SendOnConnAnimation(srcGate, msg));
}

void MessageAnimator::sendHop(cMessage *msg, cGate *srcGate, bool isLastHop, simtime_t prop, simtime_t trans, bool discard)
{
    ASSERT(currentMessageSend);
    if ((prop + trans).isZero())
        sendHop(msg, srcGate, isLastHop);
    else
        currentMessageSend->addAnimation(new SendOnConnAnimation(srcGate, msg, lastHopTime, prop, trans, discard));

    lastHopTime += prop;
    lastHopTime += trans;
}

void MessageAnimator::endSend(cMessage *msg)
{
    ASSERT(currentMessageSend);
    updateAnimations();

    auto dup = getQtenv()->getLogBuffer()->getLastMessageDup(msg);
    ASSERT(dup);

    messageDuplicated(msg, dup);

    for (auto insp : getQtenv()->getInspectors())
        currentMessageSend->addToInspector(insp);

    // if we are in a method, and this whole message sequence will take 0 SimTime, performing it in the method
    if (currentMethodCall && currentMessageSend->isHoldingOnly())
        currentMethodCall->addOperation(currentMessageSend);
    else {
        // otherwise after the method.
        // XXX maybe split it in two, so if the first few parts are instantaneous, play them, and continue later?
        animations.putMulti(msg, currentMessageSend);
    }

    currentMessageSend = nullptr;
    lastHopTime = -1;
    updateAnimations();
}

void MessageAnimator::deliveryDirect(cMessage *msg)
{
    ASSERT(!currentMessageSend);

    cGate *g = msg->getArrivalGate();
    ASSERT(g);
    cModule *destmod = g->getOwnerModule();
    cModule *mod = destmod->getParentModule();

    addDeliveryAnimation(msg, mod, new DeliveryAnimation(msg));
}

void MessageAnimator::delivery(cMessage *msg)
{
    ASSERT(!currentMessageSend);

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

    return currentMessageSend && currentMessageSend->willAnimate(msg);
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
        // If the next animation is a methodcall, we only advance that.
        // If it is a holding messagesend, then we animate all holding
        // messagesends at the beginnig of the animations "list" together.
        for (auto& p : animations)
            if (p->isHolding()) {
                bool isMethodcall = dynamic_cast<MethodcallAnimation *>(p);
                if (isMethodcall && !first)
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

    // If the mesage that is getting delivered right now
    // already has a "sending" animation, we are chopping the
    // end of that animation off, and appending it before
    // the delivery animation.
    // This way if a message arrived to a host on a channel
    // that has prop/trans delay, and got delivered into a
    // submodule of the host, the delivery will not be animated
    // before the end of the sending sequence (inside the host).
    if (animations.containsKey(msg)) {
        auto seq = dynamic_cast<AnimationSequence*>(animations.getLast(msg));
        ASSERT(seq);
        auto tail = seq->chopHoldingTail();
        // If there was a sending animation for the message, and it ended
        // with some zero length hops, we append the delivery to those last
        // parts and act as if this whole thing was the delivery.
        if (tail) {
            tail->addAnimation(anim);
            deliveries->addAnimation(tail);
        } else {
            // Otherwise just adding the delivery animation itself.
            deliveries->addAnimation(anim);
        }
    } else {
        deliveries->addAnimation(anim);
    }

    anim->removeMessagePointer(msg);

    updateAnimations();
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

    lastHopTime = -1;
    currentMessageSend = nullptr;

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

    if (currentMessageSend)
        currentMessageSend->messageDuplicated(msg, dup);

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

    if (deliveries)
        deliveries->removeMessagePointer(msg);
}

}  // namespace qtenv
}  // namespace omnetpp
