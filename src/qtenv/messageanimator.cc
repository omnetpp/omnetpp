//==========================================================================
//  messageanimator.cc - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

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
#include <utility>
#include <common/stlutil.h>
#include <memory>

#define emit

namespace omnetpp {

using namespace common;

namespace qtenv {

void MessageAnimator::redrawMessages()
{
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

        for (auto& insp : getQtenv()->getInspectors()) {
            auto moduleInsp = dynamic_cast<ModuleInspector *>(insp);

            if (moduleInsp && !willAnimate(msg)
                    && arrivalMod && arrivalMod->getParentModule() == moduleInsp->getObject()
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
        for (auto insp : getQtenv()->getInspectors())
            currentMethodCall->addToInspector(insp);

        animations.putMulti(nullptr, currentMethodCall);
    }

    currentMethodCall = parent;

    update();
}

void MessageAnimator::beginSend(cMessage *msg) {
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
    update();

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
    update();
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

void MessageAnimator::update()
{
    // Always updating all non-holding animations first.
    for (auto& p : animations)
        if (!p->isHolding() && !p->advance()) {
            delete p;
            p = nullptr;
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

    // If there were no deliveries (or the last one just ended),
    // continuing with the rest of the animations, that are holding.
    // Only advancing them until one is found that is not instantly done.
    // TODO: In broadcast mode, the first few are advanced, except for methodcalls.
    for (auto& p : animations)
        if (p->isHolding()) {
            if (!p->advance()) {
                delete p;
                p = nullptr;
            }
            if (p && p->isHolding()/* && !getQtenv()->getPref("concurrent-anim").toBool() // TODO */)
                break;
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

    if (animations.containsKey(msg)) {
        auto seq = dynamic_cast<AnimationSequence*>(animations.getLast(msg));
        ASSERT(seq);
        auto tail = seq->chopHoldingTail();
        if (tail) {
            tail->addAnimation(anim);
            deliveries->addAnimation(tail);
        } else {
            deliveries->addAnimation(anim);
        }
    } else {
        deliveries->addAnimation(anim);
    }

    anim->removeMessagePointer(msg);

    update();
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

    animSpeedMap.clear();
    holdRequests.clear();

    currentMethodCall = nullptr;

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
    update();
}

void MessageAnimator::updateInspector(ModuleInspector *insp)
{
    redrawMessages();
    for (auto &p : animations)
        p->updateInInspector(insp);
    if (deliveries)
        deliveries->updateInInspector(insp);
    update();
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

    update();
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
