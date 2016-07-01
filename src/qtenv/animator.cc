//==========================================================================
//  ANIMATOR.CC - part of
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

#include "animator.h"

#include "qtenv.h"
#include "moduleinspector.h" // for the layer

#include <QDebug>
#include <QEventLoop>
#include <QGraphicsView>
#include <omnetpp/cfutureeventset.h>
#include <utility>

#define emit

namespace omnetpp {
namespace qtenv {

//  --------  Animation subclass declarations  --------

class MethodcallAnimation : public Animation {
    cModule *srcMod, *destMod;
    QString text;
    ConnectionItem *connectionItem;
public:
    MethodcallAnimation(ModuleInspector *insp, cModule *srcMod, cModule *destMod, const char *text);
    void setProgress(float t) override;
    bool inSameGroup(const Animation *other) const override;
    bool concurrentWith(const Animation *other) const override;
    void cleanup() override;
    QString info() const override { return QString("MethodCall '") + text + "' from "
                + (srcMod ? srcMod->getFullName() : "NULL") + " to " +  (destMod ? destMod->getFullName() : "NULL"); }
    ~MethodcallAnimation();
};

class SendAnimation : public Animation {
    friend class Animator;
protected:
    cMessage *msg;
    MessageItem *messageItem;
    SendAnimMode mode;
    SendAnimation(ModuleInspector *insp, SendAnimMode mode, const QPointF &src, const QPointF &dest, cMessage *msg, float duration);
    void begin() override;
    void setProgress(float t) override;
    void end() override;
    void cleanup() override;
    bool inSameGroup(const Animation *other) const override;
    ~SendAnimation();
};

class SendOnConnAnimation : public SendAnimation {
public:
    SendOnConnAnimation(ModuleInspector *insp, SendAnimMode mode, const QPointF &src, const QPointF &dest, cMessage *msg);
    void setProgress(float t) override;
    bool concurrentWith(const Animation *other) const override;
    QString info() const override { return QString("SendOnConn '") + ((msg && mode != ANIM_END)? msg->getName() : "NULL") + "' mode "
                + (mode == ANIM_BEGIN ? "begin" : mode == ANIM_END ? "end" : mode == ANIM_THROUGH ? "through" : "INVALID"); }
};

class SendDirectAnimation : public SendAnimation {
public:
    enum Direction { DIR_ASCENT, DIR_HORIZ, DIR_DESCENT, DIR_DELIVERY };
private:
    ConnectionItem *connectionItem = nullptr;
    Direction dir;
public:
    SendDirectAnimation(ModuleInspector *insp, Direction dir, SendAnimMode mode, const QPointF &src, const QPointF &dest, cMessage *msg);
    void setProgress(float t) override;
    bool concurrentWith(const Animation *other) const override;
    QString info() const override { return QString("SendDirect '") + ((msg && dir != DIR_DELIVERY) ? msg->getName() : "NULL") + "' mode "
                + (mode == ANIM_BEGIN ? "begin" : mode == ANIM_END ? "end" : mode == ANIM_THROUGH ? "through" : "INVALID") + " "
                + (dir == DIR_ASCENT ? "ascent" : dir == DIR_DESCENT ? "descent" : dir == DIR_HORIZ ? "horiz" : dir == DIR_DELIVERY ? "delivery" : "INVALID");
                                  }
    ~SendDirectAnimation();
};



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

//  --------  Animator member definitions  --------

const int Animator::frameRate = 60;
const double Animator::msgEndCreep = 10;

Animation *Animator::firstAnimInState(Animation::State state) const
{
    Animation *result = nullptr;
    for (auto a : animations) {
        if (a->state == state) {
            result = a;
            break;
        }
    }
    return result;
}

void Animator::findDirectPath(cModule *srcmod, cModule *destmod, Animator::PathVec &pathvec)
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
        pathvec.push_back(sPathEntry(mod, nullptr));
        mod = mod->getParentModule();
    }

    // animate within commonparent
    if (commonparent != srcmod && commonparent != destmod) {
        cModule *from = findSubmoduleTowards(commonparent, srcmod);
        cModule *to = findSubmoduleTowards(commonparent, destmod);
        pathvec.push_back(sPathEntry(from, to));
    }

    // descend from commonparent to destmod
    mod = findSubmoduleTowards(commonparent, destmod);
    if (mod && srcmod != commonparent)
        mod = findSubmoduleTowards(mod, destmod);
    while (mod) {
        // animate descent towards destmod
        pathvec.push_back(sPathEntry(nullptr, mod));
        // find module 'under' mod, towards destmod (this will return nullptr if mod==destmod)
        mod = findSubmoduleTowards(mod, destmod);
    }
}

Animator::Animator()
{
    timer.setInterval(1000 / frameRate);
    timer.setSingleShot(false);
    connect(&timer, SIGNAL(timeout()), this, SLOT(onFrameTimer()));
}

Animator::~Animator() {
    clear();
}

void Animator::redrawMessages() {
    qDebug() << "redrawMessages";
    clear();

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

        for (auto &insp : getQtenv()->getInspectors()) {
            auto moduleInsp = dynamic_cast<ModuleInspector *>(insp);
            if (!moduleInsp) {
                continue;
            }

            if (arrivalMod &&
                arrivalMod->getParentModule() == moduleInsp->getObject() &&
                msg->getArrivalGateId() >= 0)
            {
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

void Animator::animateMethodcall(cComponent *fromComp, cComponent *toComp, const char *methodText)
{
    if (!fromComp->isModule() || !toComp->isModule())
        return;  // calls to/from channels are not yet animated

    cModule *from = (cModule *)fromComp;
    cModule *to = (cModule *)toComp;

    const auto &inspectors = getQtenv()->getInspectors();

    // find modules along the way
    PathVec pathvec;
    findDirectPath(from, to, pathvec);

    PathVec::iterator i;

    for (i = pathvec.begin(); i != pathvec.end(); i++) {
        if (i->to == nullptr) {
            // animate ascent from source module
            cModule *mod = i->from;
            cModule *enclosingmod = mod->getParentModule();
            for (auto in : inspectors) {
                if (auto insp = isModuleInspectorFor(enclosingmod, in)) {
                    addAnimation(new MethodcallAnimation(insp, mod, nullptr, methodText));
                }
            }
        }
        else if (i->from == nullptr) {
            // animate descent towards destination module
            cModule *mod = i->to;
            cModule *enclosingmod = mod->getParentModule();
             for (auto in : inspectors) {
                if (auto insp = isModuleInspectorFor(enclosingmod, in)) {
                    addAnimation(new MethodcallAnimation(insp, nullptr, mod, methodText));
                }
            }
        }
        else {
            cModule *enclosingmod = i->from->getParentModule();
            for (auto in : inspectors) {
                if (auto insp = isModuleInspectorFor(enclosingmod, in)) {
                    addAnimation(new MethodcallAnimation(insp, i->from, i->to, methodText));
                }
            }
        }
    }
}

void Animator::animateSendDirect(cMessage *msg, cModule *srcModule, cGate *destGate)
{
    PathVec pathvec;
    findDirectPath(srcModule, destGate->getOwnerModule(), pathvec);

    // for checking whether the sendDirect target module is also the final destination of the msg
    cModule *arrivalmod = destGate->getNextGate()==nullptr ? destGate->getOwnerModule() : nullptr;

    const auto &inspectors = getQtenv()->getInspectors();

    PathVec::iterator i;
    for (i = pathvec.begin(); i != pathvec.end(); i++) {
        if (i->to == nullptr) {
            // ascent
            cModule *mod = i->from;
            cModule *enclosingmod = mod->getParentModule();
            for (auto in : inspectors) {
                if (auto insp = isModuleInspectorFor(enclosingmod, in))
                    animateSenddirectAscent(insp, mod, msg);
            }
        }
        else if (i->from == nullptr) {
            // descent
            cModule *mod = i->to;
            cModule *enclosingmod = mod->getParentModule();
            bool isArrival = (mod == arrivalmod);
            for (auto in : inspectors) {
                if (auto insp = isModuleInspectorFor(enclosingmod, in))
                    animateSenddirectDescent(insp, mod, msg, isArrival ? ANIM_BEGIN : ANIM_THROUGH);
            }
        }
        else {
            // level
            cModule *enclosingmod = i->from->getParentModule();
            bool isArrival = (i->to == arrivalmod);
            for (auto in : inspectors) {
                if (auto insp = isModuleInspectorFor(enclosingmod, in))
                    animateSenddirectHoriz(insp, i->from, i->to, msg, isArrival ? ANIM_BEGIN : ANIM_THROUGH);
            }
        }
    }
}

void Animator::animateSendHop(cMessage *msg, cGate *srcGate, bool isLastHop)
{
    cModule *mod = srcGate->getOwnerModule();
    if (srcGate->getType() == cGate::OUTPUT)
        mod = mod->getParentModule();
    for (auto in : getQtenv()->getInspectors()) {
        if (auto insp = isModuleInspectorFor(mod, in))
            animateSendOnConn(insp, srcGate, msg, (isLastHop ? ANIM_BEGIN : ANIM_THROUGH));
    }
}

void Animator::animateDelivery(cMessage *msg)
{
    // find suitable inspectors and do animate the message...
    cGate *g = msg->getArrivalGate();
    ASSERT(g);
    g = g->getPreviousGate();
    ASSERT(g);

    cModule *mod = g->getOwnerModule();
    if (g->getType() == cGate::OUTPUT)
        mod = mod->getParentModule();

    for (auto in : getQtenv()->getInspectors()) {
        if (auto insp = isModuleInspectorFor(mod, in))
            animateSendOnConn(insp, g, msg, ANIM_END);
    }
}

void Animator::animateDeliveryDirect(cMessage *msg)
{
    // find suitable inspectors and do animate the message...
    cGate *g = msg->getArrivalGate();
    ASSERT(g);
    cModule *destmod = g->getOwnerModule();
    cModule *mod = destmod->getParentModule();

    for (auto in : getQtenv()->getInspectors()) {
        if (auto insp = isModuleInspectorFor(mod, in))
            animateSenddirectDelivery(insp, destmod, msg);
    }
}
void Animator::addAnimation(Animation *anim) {
    animations.push_back(anim);
}

void Animator::onFrameTimer() {
    bool any = false;

    for (auto a : animations) {
        if (a->state != Animation::DONE) {
            any = true;
            break;
        }
    }

    // all animations are DONE, we have to end the current animation batch
    if (animations.empty() || !any) {
        clear();
        emit finish(); // this will end the event loop in play()
        return;
    }

    any = false;
    // if there are any PLAYING anims, advance them, and that's it
    for (auto a : animations) {
        if (a->state == Animation::PLAYING) {
            a->advance(/*0.1     *  */     (1.0f / frameRate) * getQtenv()->opt->animationSpeed * (inHurry ? 5 : 1));
            any = true;
        }
    }
    if (any) return;

    any = false;
    // if none of them are playing, start some WAITING anims
    Animation *leader = firstAnimInState(Animation::WAITING);
    for (auto a : animations) {
        if (a->state == Animation::WAITING
                && (a == leader || a->concurrentWith(leader))) {

            // if the currently beginning animation is a SendAnimation,
            // we have to hide the corresponding static messageItem
            if (auto sa = dynamic_cast<SendAnimation *>(a)) {
                for (auto &i : messageItems) {
                    if (i.first == std::make_pair(sa->inspector, sa->msg)) {
                        i.second->setVisible(false);
                    }
                }
            }

            a->begin();

            any = true;
        }
    }
    if (any) return;

    qDebug() << "nobody is waiting";
    // if no animation is WAITING, lets init() a new group

    //but first clean up the ones from before
    for (auto a : animations) {
        if (a->state == Animation::FINISHED) {
            qDebug() << "cleaning up" << a->info();
            a->cleanup();
        }
    }

    Animation *first = firstAnimInState(Animation::PENDING);
    for (auto a : animations) {
        if (a->state == Animation::PENDING
                && (a == first || a->inSameGroup(first))) {
            qDebug() << "initing" << a->info();
            a->init();
        }
    }
}

void Animator::play() {
    qDebug() << "playing" << animations.size() << "anims";

    inHurry = false;

    for (auto a : animations) {
        qDebug() << a->info();
    }
    if (!animations.empty()) {
        QEventLoop loop;
        timer.start();
        connect(this, SIGNAL(finish()), &loop, SLOT(quit()));
        loop.exec();
        timer.stop();
    }
    qDebug() << "animations done";
    redrawMessages();
    inHurry = false;
}

void Animator::clear() {
    for (auto a : animations) {
        delete a;
    }
    animations.clear();

    for (auto i : messageItems) {
        delete i.second;
    }
    messageItems.clear();

    inHurry = false;
}

void Animator::hurry()
{
    inHurry = true;
}

void Animator::clearInspector(ModuleInspector *insp) {
    for (auto &anim : animations) {
        if (anim->inspector == insp) {
            delete anim;
            anim = nullptr;
        }
    }
    animations.erase(std::remove(animations.begin(), animations.end(), nullptr), animations.end());

    for (auto it = messageItems.begin(); it != messageItems.end(); /* blank */ ) {
        if ((*it).first.first == insp) {
            delete (*it).second;
            messageItems.erase(it++);
        } else {
            ++it;
        }
    }
}

void Animation::advance(float delta) {
    time += delta;

    if (time > duration) {
        end();
    } else {
        setProgress(time / duration);
    }
}

Animation::Animation(ModuleInspector *insp, const QPointF &src, const QPointF &dest, float duration)
    : inspector(insp), src(src), dest(dest), duration(duration)
{

}

//  --------  Animator member definitions  --------

void Animator::animateSendOnConn(ModuleInspector *insp, cGate *srcGate, cMessage *msg, SendAnimMode mode)
{
    qDebug() << __FUNCTION__;
    QLineF connLine = insp->getConnectionLine(srcGate);

    QPointF srcPos = connLine.p1();
    QPointF destPos = connLine.p2();

    if (mode == ANIM_END) {
        srcPos = connLine.p2();
        cModule *dest = srcGate->getNextGate()->getOwnerModule();
        QPointF destCenterPos = insp->getSubmodCoords(dest);
        QPointF fromEdgeToCenter = destCenterPos - connLine.p2();
        double length = std::sqrt(fromEdgeToCenter.x() * fromEdgeToCenter.x() + fromEdgeToCenter.y() * fromEdgeToCenter.y());
        ASSERT(length > 0.0); // there is a minimum bounding box size on the modules, so the edge can't be in the center
        destPos = connLine.p2() + fromEdgeToCenter / length * std::min(length, msgEndCreep);
    }

    addAnimation(new SendOnConnAnimation(insp, mode, srcPos, destPos, msg));
}

void Animator::animateSenddirectAscent(ModuleInspector *insp, cModule *srcSubmod, cMessage *msg)
{
    qDebug() << __FUNCTION__;
    auto start = insp->getSubmodCoords(srcSubmod);
    QPointF end(start.x() + start.y() / 4, 0);
    addAnimation(new SendDirectAnimation(insp, SendDirectAnimation::DIR_ASCENT, ANIM_BEGIN, start, end, msg));
}

void Animator::animateSenddirectDescent(ModuleInspector *insp, cModule *destSubmod, cMessage *msg, SendAnimMode mode)
{
    qDebug() << __FUNCTION__;
    auto end = insp->getSubmodCoords(destSubmod);
    QPointF start(end.x() - end.y() / 4, 0);
    addAnimation(new SendDirectAnimation(insp, SendDirectAnimation::DIR_DESCENT, mode, start, end, msg));
}

void Animator::animateSenddirectHoriz(ModuleInspector *insp, cModule *srcSubmod, cModule *destSubmod, cMessage *msg, SendAnimMode mode)
{
    qDebug() << __FUNCTION__;
    auto start = insp->getSubmodCoords(srcSubmod);
    auto end = insp->getSubmodCoords(destSubmod);
    addAnimation(new SendDirectAnimation(insp, SendDirectAnimation::DIR_HORIZ, mode, start, end, msg));
}

void Animator::animateSenddirectDelivery(ModuleInspector *insp, cModule *destSubmod, cMessage *msg)
{
    qDebug() << __FUNCTION__;
    auto end = insp->getSubmodCoords(destSubmod);
    addAnimation(new SendDirectAnimation(insp, SendDirectAnimation::DIR_DELIVERY, ANIM_END, end, end, msg));
}

//  --------  Animation subclasses' member definitions  --------

MethodcallAnimation::MethodcallAnimation(ModuleInspector *insp, cModule *srcMod, cModule *destMod, const char *text)
        : Animation(insp, QPointF(), QPointF(), getQtenv()->opt->methodCallAnimDelay / 1000.0),
          srcMod(srcMod), destMod(destMod), text(text)
{
    if (srcMod)  src  = insp->getSubmodCoords(srcMod);
    if (destMod) dest = insp->getSubmodCoords(destMod);

    if (!srcMod)  src  = QPointF(dest.x() - dest.y() / 4, 0);
    if (!destMod) dest = QPointF(src.x()  + src.y()  / 4, 0);

    auto layer = insp->getAnimationLayer();
    connectionItem = new ConnectionItem(layer);
    connectionItem->setSource(src);
    connectionItem->setDestination(dest);
    connectionItem->setColor("red");
    connectionItem->setLineStyle(Qt::DashLine);
    connectionItem->setWidth(2);
    connectionItem->setText(text);
    connectionItem->setTextPosition(Qt::AlignCenter);
    connectionItem->setTextColor("black");
    connectionItem->setTextBackgroundColor("#F0F0F0");
    connectionItem->setTextOutlineColor("transparent");
    connectionItem->setVisible(false);
}

void MethodcallAnimation::setProgress(float t)
{
    connectionItem->setVisible(t > 0);

    bool enabled = (t > 0.2 && t < 0.4) || (t > 0.6);
    connectionItem->setLineEnabled(enabled);
    connectionItem->setArrowEnabled(enabled);
}

bool MethodcallAnimation::inSameGroup(const Animation *other) const
{
    auto o = dynamic_cast<const MethodcallAnimation *>(other);
    return o && o->text == text;
}

bool MethodcallAnimation::concurrentWith(const Animation *other) const
{
    const MethodcallAnimation *o = dynamic_cast<const MethodcallAnimation *>(other);
    return o && (o->srcMod == srcMod) && (o->destMod == destMod) && (o->text == text);
}

void MethodcallAnimation::cleanup()
{
    qDebug() << "methodanim cleanup";
    connectionItem->setVisible(false);
    Animation::cleanup();
}

MethodcallAnimation::~MethodcallAnimation()
{
    delete connectionItem;
}

SendAnimation::SendAnimation(ModuleInspector *insp, SendAnimMode mode, const QPointF &src, const QPointF &dest, cMessage *msg, float duration)
        : Animation(insp, src, dest, duration), msg(msg), mode(mode) {
    auto layer = insp->getAnimationLayer();
    messageItem = new MessageItem(layer);
    MessageItemUtil::setupFromDisplayString(messageItem, msg, insp->getImageSizeFactor());
    messageItem->setVisible(false);
    messageItem->setPos(src);
    //messageItem->setRotation(180);
}

void SendAnimation::begin()
{
    messageItem->setVisible(true);
    Animation::begin();
}

void SendAnimation::setProgress(float t)
{
    messageItem->setPos(src * (1-t) + dest * t);
}

void SendAnimation::cleanup()
{
    qDebug() << "sendanim Cleanup";
    messageItem->setVisible(false);
    Animation::cleanup();
}

bool SendAnimation::inSameGroup(const Animation *other) const
{
    auto o = dynamic_cast<const SendAnimation *>(other);
    return o && o->mode == mode
            && (getQtenv()->getPref("concurrent-anim").value<bool>() || o->msg == msg);
}

SendAnimation::~SendAnimation()
{
    delete messageItem;
}

SendOnConnAnimation::SendOnConnAnimation(ModuleInspector *insp, SendAnimMode mode, const QPointF &src, const QPointF &dest, cMessage *msg)
    : SendAnimation(insp, mode, src, dest, msg, 0) // duration calculated below
{
    auto delta = (dest - src) / insp->getZoomFactor();
    // at most 1 sec, otherwise 100 "units"/sec
    duration = std::min(std::sqrt(delta.x() * delta.x() + delta.y() * delta.y()) * 0.01, 1.0);
}

void SendOnConnAnimation::setProgress(float t)
{
    if (mode != ANIM_BEGIN)
        messageItem->setRotation(t*1000);
    SendAnimation::setProgress(t);
}

void SendAnimation::end()
{
    if (mode != ANIM_BEGIN) {
        messageItem->setVisible(false);
    }
    Animation::end();
}

bool SendOnConnAnimation::concurrentWith(const Animation *other) const
{
    const SendOnConnAnimation *o = dynamic_cast<const SendOnConnAnimation *>(other);

    return o && o->msg && msg && o->mode == mode
        && (o->msg == msg || getQtenv()->getPref("concurrent-anim").value<bool>())
        && o->msg->getSenderModule() == msg->getSenderModule()
        && ((o->inspector->getObject() == inspector->getObject())
            || (mode == o->mode));
}

SendDirectAnimation::SendDirectAnimation(ModuleInspector *insp, Direction dir, SendAnimMode mode, const QPointF &src, const QPointF &dest, cMessage *msg)
    : SendAnimation(insp, mode, src, dest, msg, (mode == ANIM_END) ? 0.5 : 1), dir(dir)
{
    auto layer = insp->getAnimationLayer();
    connectionItem = new ConnectionItem(layer);
    connectionItem->setSource(src);
    connectionItem->setDestination(dest);
    connectionItem->setColor("blue");
    connectionItem->setLineStyle(Qt::DashLine);
    connectionItem->setWidth(2);
    connectionItem->setVisible(getQtenv()->opt->showSendDirectArrows);
    // so the message will be on top (they have the same Z value)
    connectionItem->stackBefore(messageItem);

}

void SendDirectAnimation::setProgress(float t)
{
    if (dir == DIR_DELIVERY) {
        bool visible = (t > 0 && t < 0.2) || (t > 0.4 && t < 0.6) || (t > 0.8 && t < 1);
        messageItem->setVisible(visible);
    } else {
        SendAnimation::setProgress(t);
    }
}

bool SendDirectAnimation::concurrentWith(const Animation *other) const
{
    const SendDirectAnimation *o = dynamic_cast<const SendDirectAnimation *>(other);

    return o && o->msg && msg && o->mode == mode
        && (o->msg == msg || getQtenv()->getPref("concurrent-anim").value<bool>())
        && o->msg->getSenderModule() == msg->getSenderModule()
        && ((o->inspector->getObject() == inspector->getObject())
            || (mode == o->mode && dir == o->dir));
}

SendDirectAnimation::~SendDirectAnimation()
{
    delete connectionItem;
}

} // namespace qtenv
} // namespace omnetpp
