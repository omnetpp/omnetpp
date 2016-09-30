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

#include <QDebug>
#include <QEventLoop>
#include <QGraphicsView>
#include <omnetpp/cfutureeventset.h>
#include <utility>

#define emit

namespace omnetpp {
namespace qtenv {

// helper for animateSendDirect() functions
static cModule *findSubmoduleTowards(cModule *parentmod, cModule *towardsgrandchild)
{
    if (parentmod == towardsgrandchild)
        return nullptr;  // shortcut -- we don't have to go up to the top to see we missed

    // search upwards from 'towardsgrandchild'
    cModule *m = towardsgrandchild;
    while (m && m->getParentModule() != parentmod)
        m = m->getParentModule();
    return m;
}

const double MessageAnimator::msgEndCreep = 10;

void MessageAnimator::findDirectPath(cModule *srcmod, cModule *destmod, MessageAnimator::PathVec& pathvec)
{
    // for animation purposes, we assume that the message travels up
    // in the module hierarchy until it finds the first compound module
    // that also contains the destination module (possibly somewhere deep),
    // and then it descends to the destination module. We have to find the
    // list of modules visited during the travel.

    // first, find "lowest common ancestor" module
    cModule *commonparent = findCommonAncestor(srcmod, destmod);
    ASSERT(commonparent != nullptr);  // commonparent should exist, worst case it's the system module

    // animate the ascent of the message until commonparent (excluding).
    // The second condition, destmod==commonparent covers case when we're sending
    // to an output gate of the parent (grandparent, etc) gate.
    cModule *mod = srcmod;
    while (mod != commonparent && (mod->getParentModule() != commonparent || destmod == commonparent)) {
        pathvec.push_back({ mod, nullptr });
        mod = mod->getParentModule();
    }

    // animate within commonparent
    if (commonparent != srcmod && commonparent != destmod) {
        cModule *from = findSubmoduleTowards(commonparent, srcmod);
        cModule *to = findSubmoduleTowards(commonparent, destmod);
        pathvec.push_back({ from, to });
    }

    // descend from commonparent to destmod
    mod = findSubmoduleTowards(commonparent, destmod);
    if (mod && srcmod != commonparent)
        mod = findSubmoduleTowards(mod, destmod);
    while (mod) {
        // animate descent towards destmod
        pathvec.push_back({ nullptr, mod });
        // find module 'under' mod, towards destmod (this will return nullptr if mod==destmod)
        mod = findSubmoduleTowards(mod, destmod);
    }
}

void MessageAnimator::insertSerial(Animation *anim)
{
    // this will reset the staged broadcast message animation
    messageStages = nullptr;
    animation->addAnimation(anim);
}

void MessageAnimator::insertConcurrent(Animation *anim, cMessage *msg)
{
    if (!messageStages) {
        // we have to start (again) the concurrent staging
        messageStages = new SequentialAnimation(AnimationGroup::DEFER_PARTS_INIT);
        animation->addAnimation(messageStages);
    }

    auto& parts = messageStages->getParts();

    if (getQtenv()->getPref("concurrent-anim").value<bool>()) {
        // if we want concurrent "broadcast" animation
        // lets find the first stage where this message is
        // is not yet scheduled to animate
        size_t stage = 0;
        for (stage = 0; stage < parts.size(); ++stage)
            if (!parts[stage]->willAnimate(msg))
                break;
        if (stage >= parts.size())
            // if all stages so far animate this message
            // (or there aren't any), add a new, empty stage
            messageStages->addAnimation(new ParallelAnimation(ParallelAnimation::STRETCH_TIME));

        // and finally add animation to the stage we just found
        ((ParallelAnimation *)parts[stage])->addAnimation(anim);
    }
    else {
        // if we don't want concurrent "broadcast" animation
        // we just check if the last (non-concurrent in this case)
        // stage belongs to the current message
        if (!(parts.empty() || parts.back()->willAnimate(msg))) {
            // and if not, or there aren't any stages yet,
            // lets add a new, empty stage
            messageStages = new SequentialAnimation(AnimationGroup::DEFER_PARTS_INIT);
            animation->addAnimation(messageStages);
        }
        // and finally add the animation as a next stage
        messageStages->addAnimation(anim);
    }
}

MessageAnimator::~MessageAnimator()
{
    clear();
    delete animation;
}

void MessageAnimator::redrawMessages()
{
    clearMessages();

    // this thingy is only needed if animation is going on
    if (!getQtenv()->animating)
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

void MessageAnimator::removeMessagePointer(cMessage *msg)
{
    for (auto i : messageItems)
        if (i.first.second == msg)
            i.second->setData(1, QVariant());

    animation->removeMessagePointer(msg);
}

void MessageAnimator::animateMethodcall(cComponent *fromComp, cComponent *toComp, const char *methodText)
{
    if (!fromComp->isModule() || !toComp->isModule())
        return;  // calls to/from channels are not yet animated

    cModule *from = (cModule *)fromComp;
    cModule *to = (cModule *)toComp;

    const auto& inspectors = getQtenv()->getInspectors();

    // find modules along the way
    PathVec pathvec;
    findDirectPath(from, to, pathvec);

    PathVec::iterator i;

    auto group = new SequentialAnimation(AnimationGroup::DEFER_PARTS_CLEANUP);

    for (i = pathvec.begin(); i != pathvec.end(); i++) {
        auto subgroup = new ParallelAnimation(AnimationGroup::DEFER_PARTS_CLEANUP);

        if (i->to == nullptr) {
            // animate ascent from source module
            cModule *mod = i->from;
            cModule *enclosingmod = mod->getParentModule();
            for (auto in : inspectors)
                if (auto insp = isModuleInspectorFor(enclosingmod, in))
                    subgroup->addAnimation(new MethodcallAnimation(insp, mod, nullptr, methodText));
        }
        else if (i->from == nullptr) {
            // animate descent towards destination module
            cModule *mod = i->to;
            cModule *enclosingmod = mod->getParentModule();
            for (auto in : inspectors)
                if (auto insp = isModuleInspectorFor(enclosingmod, in))
                    subgroup->addAnimation(new MethodcallAnimation(insp, nullptr, mod, methodText));
        }
        else {
            cModule *enclosingmod = i->from->getParentModule();
            for (auto in : inspectors)
                if (auto insp = isModuleInspectorFor(enclosingmod, in))
                    subgroup->addAnimation(new MethodcallAnimation(insp, i->from, i->to, methodText));
        }
        group->addAnimation(subgroup);
    }

    // this one i necessary to clean up the arrows after the methodcall ended
    // this is not using LAZY_CLEANUP, the one above is, and that should be
    auto container = new SequentialAnimation();
    container->addAnimation(group);

    insertSerial(container);
}

void MessageAnimator::animateSendDirect(cMessage *msg, cModule *srcModule, cGate *destGate)
{
    PathVec pathvec;
    findDirectPath(srcModule, destGate->getOwnerModule(), pathvec);

    // for checking whether the sendDirect target module is also the final destination of the msg
    cModule *arrivalmod = destGate->getNextGate() == nullptr ? destGate->getOwnerModule() : nullptr;

    const auto& inspectors = getQtenv()->getInspectors();

    PathVec::iterator i;
    for (i = pathvec.begin(); i != pathvec.end(); i++) {
        auto subgroup = new ParallelAnimation(
          AnimationGroup::DEFER_PARTS_INIT | AnimationGroup::DEFER_PARTS_CLEANUP | ParallelAnimation::STRETCH_TIME);

        if (i->to == nullptr) {
            // ascent
            cModule *mod = i->from;
            cModule *enclosingmod = mod->getParentModule();

            for (auto in : inspectors) {
                if (auto insp = isModuleInspectorFor(enclosingmod, in)) {
                    auto start = insp->getSubmodCoords(mod);
                    QPointF end(start.x() + start.y() / 4, 0);
                    subgroup->addAnimation(new SendDirectAnimation(insp, SendDirectAnimation::DIR_ASCENT, ANIM_BEGIN, start, end, msg));
                }
            }
        }
        else if (i->from == nullptr) {
            // descent
            cModule *mod = i->to;
            cModule *enclosingmod = mod->getParentModule();
            bool isArrival = (mod == arrivalmod);
            for (auto in : inspectors)
                if (auto insp = isModuleInspectorFor(enclosingmod, in)) {
                    auto end = insp->getSubmodCoords(mod);
                    QPointF start(end.x() - end.y() / 4, 0);
                    subgroup->addAnimation(new SendDirectAnimation(insp, SendDirectAnimation::DIR_DESCENT, isArrival ? ANIM_BEGIN : ANIM_THROUGH, start, end, msg));
                }
        }
        else {
            // level
            cModule *enclosingmod = i->from->getParentModule();
            bool isArrival = (i->to == arrivalmod);
            for (auto in : inspectors)
                if (auto insp = isModuleInspectorFor(enclosingmod, in)) {
                    auto start = insp->getSubmodCoords(i->from);
                    auto end = insp->getSubmodCoords(i->to);
                    subgroup->addAnimation(new SendDirectAnimation(insp,
                        SendDirectAnimation::DIR_HORIZ, isArrival ? ANIM_BEGIN : ANIM_THROUGH,
                        start, end, msg));
                }
        }
        insertConcurrent(subgroup, msg);
    }
}

void MessageAnimator::animateSendHop(cMessage *msg, cGate *srcGate, bool isLastHop)
{
    cModule *mod = srcGate->getOwnerModule();
    if (srcGate->getType() == cGate::OUTPUT)
        mod = mod->getParentModule();

    auto subgroup = new ParallelAnimation(ParallelAnimation::STRETCH_TIME);
    for (auto in : getQtenv()->getInspectors())
        if (auto insp = isModuleInspectorFor(mod, in)) {
            QLineF connLine = insp->getConnectionLine(srcGate);

            subgroup->addAnimation(new SendOnConnAnimation(insp,
                isLastHop ? ANIM_BEGIN : ANIM_THROUGH,
                connLine.p1(), connLine.p2(), msg));
        }

    insertConcurrent(subgroup, msg);
}

void MessageAnimator::animateDelivery(cMessage *msg)
{
    // find suitable inspectors and do animate the message...
    cGate *g = msg->getArrivalGate();
    ASSERT(g);
    g = g->getPreviousGate();
    ASSERT(g);

    cModule *mod = g->getOwnerModule();
    if (g->getType() == cGate::OUTPUT)
        mod = mod->getParentModule();

    auto subgroup = new ParallelAnimation(ParallelAnimation::STRETCH_TIME);

    for (auto in : getQtenv()->getInspectors())
        if (auto insp = isModuleInspectorFor(mod, in)) {
            QLineF connLine = insp->getConnectionLine(g);

            QPointF srcPos = connLine.p2();
            cModule *dest = g->getNextGate()->getOwnerModule();
            QPointF destCenterPos = insp->getSubmodCoords(dest);
            QPointF fromEdgeToCenter = destCenterPos - connLine.p2();
            double length = std::sqrt(fromEdgeToCenter.x() * fromEdgeToCenter.x() + fromEdgeToCenter.y() * fromEdgeToCenter.y());
            ASSERT(length > 0.0);  // there is a minimum bounding box size on the modules, so the edge can't be in the center
            QPointF destPos = connLine.p2() + fromEdgeToCenter / length * std::min(length, msgEndCreep);

            subgroup->addAnimation(new SendOnConnAnimation(insp, ANIM_END, srcPos, destPos, msg));
        }

    insertSerial(subgroup);
}

void MessageAnimator::animateDeliveryDirect(cMessage *msg)
{
    // find suitable inspectors and do animate the message...
    cGate *g = msg->getArrivalGate();
    ASSERT(g);
    cModule *destmod = g->getOwnerModule();
    cModule *mod = destmod->getParentModule();

    auto subgroup = new ParallelAnimation();

    for (auto in : getQtenv()->getInspectors())
        if (auto insp = isModuleInspectorFor(mod, in)) {
            auto end = insp->getSubmodCoords(destmod);
            subgroup->addAnimation(new SendDirectAnimation(insp,
                SendDirectAnimation::DIR_DELIVERY, ANIM_END, end, end, msg));
        }

    insertSerial(subgroup);
}

void MessageAnimator::frame()
{
    if (animation->isEmpty() || animation->state != Animation::PLAYING)
        return;

    double animTime = getQtenv()->getAnimationTime();
    animation->advance(animTime - lastFrameTime);
    lastFrameTime = animTime;

    if (animation->getState() == Animation::FINISHED) {
        animation->cleanup();
        clear();
        redrawMessages();
    }
}

void MessageAnimator::hold()
{
    double delta = animation->getDuration() - animation->getTime();
    getQtenv()->holdSimulationFor(delta);
}

void MessageAnimator::start()
{
    if (!animation->isEmpty() && animation->state == Animation::PENDING) {
        // not starting empty animation

        // if not empty, we still prune it, to reduce delay
        animation->prune();

        lastFrameTime = getQtenv()->getAnimationTime();

        animation->init();
        animation->begin();
    }

    redrawMessages();

    hold();
}

void MessageAnimator::clearMessages()
{
    for (auto i : messageItems)
        delete i.second;

    messageItems.clear();
}

void MessageAnimator::clear()
{
    clearMessages();

    delete animation;
    animation = new SequentialAnimation();

    messageStages = nullptr;
}

void MessageAnimator::clearInspector(ModuleInspector *insp)
{
    animation->clearInspector(insp);

    for (auto it = messageItems.begin(); it != messageItems.end();  /* blank */) {
        if ((*it).first.first == insp) {
            delete (*it).second;
            messageItems.erase(it++);
        }
        else
            ++it;
    }
}

}  // namespace qtenv
}  // namespace omnetpp
