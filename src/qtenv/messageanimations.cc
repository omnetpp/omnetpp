//==========================================================================
//  messageanimations.cc - part of
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

#include "messageanimations.h"
#include "messageanimator.h"
#include "qtenv.h"
#include "moduleinspector.h"
#include "messageitem.h"
#include "connectionitem.h"
#include "graphicsitems.h"
#include "common/stlutil.h"
#include "omnetpp/cpacket.h"

namespace omnetpp {
using namespace common;
namespace qtenv {

// --------  Helper functions  --------

namespace {

// for debugging purposes only
constexpr static const char *stateText[] = { "PENDING", "WAITING", "PLAYING", "FINISHED", "DONE" };

// helper for findDirectPath
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

static PathVec findDirectPath(cModule *srcmod, cModule *destmod)
{
    PathVec path;

    if (!srcmod || !destmod)
        return path;

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
        path.push_back({ mod, nullptr });
        mod = mod->getParentModule();
    }

    // animate within commonparent
    if (commonparent != srcmod && commonparent != destmod) {
        cModule *from = findSubmoduleTowards(commonparent, srcmod);
        cModule *to = findSubmoduleTowards(commonparent, destmod);
        path.push_back({ from, to });
    }

    // descend from commonparent to destmod
    mod = findSubmoduleTowards(commonparent, destmod);
    if (mod && srcmod != commonparent)
        mod = findSubmoduleTowards(mod, destmod);
    while (mod) {
        // animate descent towards destmod
        path.push_back({ nullptr, mod });
        // find module 'under' mod, towards destmod (this will return nullptr if mod==destmod)
        mod = findSubmoduleTowards(mod, destmod);
    }

    return path;
}

}  // namespace

// --------  Animation functions  --------

void Animation::requestAnimationSpeed(double speed)
{
    ASSERT(!holding || (speed <= 0.0));
    getQtenv()->getMessageAnimator()->setAnimationSpeed(speed, this);
}

double Animation::getHoldPosition() const
{
    ASSERT(state == PLAYING);
    ASSERT(holding);
    return (getQtenv()->getAnimationTime() - holdStartTime) / holdDuration;
}

bool Animation::holdExpired() const
{
    ASSERT(holding);
    return getQtenv()->getAnimationTime() >= (holdStartTime + holdDuration);
}

double Animation::getHoldEndTime() const
{
    if (!holding || isEmpty())
        return 0;

    switch (state) {
        case PENDING:
        case WAITING:
            return holdDuration;
        case PLAYING:
            return holdStartTime + holdDuration;
        case FINISHED:
        case DONE:
            return 0;
    }
    ASSERT(false);
    return -1;
}

void Animation::init()
{
    ASSERT2(state == PENDING, str().toStdString().c_str());
    state = WAITING;
}

void Animation::begin()
{
    ASSERT2(state == WAITING, str().toStdString().c_str());
    state = PLAYING;

    holdStartTime = getQtenv()->getAnimationTime();
    if (!isEmpty() && holding)
        getQtenv()->getMessageAnimator()->requestHold(this);
}

void Animation::update()
{
    ASSERT2(state == PLAYING, str().toStdString().c_str());
}

void Animation::end()
{
    ASSERT2(state == PLAYING, str().toStdString().c_str());
    state = FINISHED;

    getQtenv()->getMessageAnimator()->clearHold(this);
}

void Animation::cleanup()
{
    ASSERT2(state == FINISHED, str().toStdString().c_str());
    state = DONE;
}

bool Animation::advance()
{
    ASSERT(state != DONE);

    // This should not be a switch, state can change in the called methods!
    // See the diagram in the class body.

    if (state == PENDING) init();
    if (state == WAITING) begin();
    if (state == PLAYING) update();
    if (state == FINISHED) cleanup();

    return state != DONE;
}

Animation::~Animation()
{
    getQtenv()->getMessageAnimator()->clearHold(this);
}

// --------  AnimationSequence functions  --------

AnimationSequence *AnimationSequence::chopTail(size_t from)
{
    ASSERT(state < FINISHED);
    ASSERT(from > currentPart);
    ASSERT(from < parts.size());

    AnimationSequence *tail = new AnimationSequence(deferPartsInit);

    if (state == PLAYING && deferPartsInit)
        tail->state = WAITING; // have to set directly, begin() would ASSERT() wrongly

    for (size_t i = from; i < parts.size(); ++i)
        tail->parts.push_back(parts[i]); // can't use addAnimation because that would mess with the parts

    parts.resize(from);

    return tail;
}

void AnimationSequence::addAnimation(Animation *a)
{
    ASSERT(state < FINISHED);
    if (!deferPartsInit && state == PLAYING)
        a->init();
    parts.push_back(a);
}

AnimationSequence *AnimationSequence::chopHoldingTail()
{
    int first = parts.size() - 1;

    while (first >= 0 && parts[first]->isHolding())
        --first;
    ++first;

    return (first < (int)parts.size())
            ? chopTail(first)
            : nullptr;
}

bool AnimationSequence::isHolding() const
{
    return !parts.empty() && (currentPart < parts.size())
            && parts[currentPart]->isHolding();
}

bool AnimationSequence::isHoldingOnly() const
{
    for (auto p : parts)
        if (!p->isHolding())
            return false;
    return true;
}

void AnimationSequence::begin()
{
    Animation::begin();

    if (!deferPartsInit)
        for (auto p : parts)
            if (p->getState() == PENDING)
                p->init();

    while (currentPart < parts.size()) {
        // looking for the first non-empty anim

        if (deferPartsInit)
            parts[currentPart]->init();

        if (parts[currentPart]->getState() == WAITING)
            parts[currentPart]->begin();

        if (parts[currentPart]->getState() == PLAYING)
            break;

        ++currentPart;
    }

    if (currentPart == parts.size())
        end();
}

void AnimationSequence::update()
{
    Animation::update();

    Animation *cp = parts[currentPart];

    if (cp->getState() == PLAYING)
        cp->update();

    if (cp->getState() == FINISHED) {
        ++currentPart;

        while (currentPart < parts.size() && parts[currentPart]->isEmpty())
            ++currentPart;

        if (currentPart < parts.size()) {
            cp = parts[currentPart];
            if (deferPartsInit)
                cp->init();
            cp->begin();
        }
        else
            end();
    }
}

void AnimationSequence::end()
{
    Animation::end();

    for (auto p : parts)
        if (p->getState() == FINISHED)
            p->cleanup();
}

void AnimationSequence::addToInspector(Inspector *insp)
{
    for (auto p : parts)
        p->addToInspector(insp);
}

void AnimationSequence::updateInInspector(Inspector *insp)
{
    for (auto p : parts)
        p->updateInInspector(insp);
}

void AnimationSequence::removeFromInspector(Inspector *insp)
{
    for (auto p : parts)
        p->removeFromInspector(insp);
}

bool AnimationSequence::isEmpty() const
{
    return std::all_of(parts.begin(), parts.end(),
                       [](Animation *p) {
        return p->isEmpty();
    });
}

bool AnimationSequence::willAnimate(cMessage *msg)
{
    return std::any_of(parts.begin(), parts.end(),
                       [&msg](Animation *p) {
        return p->willAnimate(msg);
    });
}

void AnimationSequence::messageDuplicated(cMessage *msg, cMessage *dup)
{
    for (auto& p : parts)
        p->messageDuplicated(msg, dup);
}

void AnimationSequence::removeMessagePointer(cMessage *msg)
{
    for (auto& p : parts)
        p->removeMessagePointer(msg);
}

QString AnimationSequence::str() const
{
    QString result = "AnimationSequence of " + QString::number(parts.size()) + " parts, state " + stateText[state];
    for (Animation *p : parts)
        result += "\n    - " + p->str().replace("\n", "\n      ");
    return result;
}

AnimationSequence::~AnimationSequence()
{
    for (auto p : parts)
        delete p;
}

//  -------- AnimationGroup functions  --------

void AnimationGroup::addAnimation(Animation *a)
{
    ASSERT2(state == PENDING, "modifying the AnimationGroup after it was started is not supported");
    parts.push_back(a);
}

bool AnimationGroup::isHolding() const
{
    for (Animation *p : parts)
        if (p->getState() == PLAYING && p->isHolding())
            return true;
    return false;
}

void AnimationGroup::init()
{
    Animation::init();
    for (Animation *p : parts)
        p->init();
}

void AnimationGroup::begin()
{
    Animation::begin();
    for (Animation *p : parts)
        p->begin();
}

void AnimationGroup::update()
{
    Animation::update();

    bool anyStillPlaying = false;
    for (Animation *p : parts)
        if (p->getState() == PLAYING) { // some may have ended on their own already
            p->update();
            anyStillPlaying |= (p->getState() == PLAYING);
        }

    if (!anyStillPlaying)
        end();
}

void AnimationGroup::end()
{
    Animation::end();
    for (Animation *p : parts)
        if (p->getState() == PLAYING) // some may have ended on their own already
            p->end();
}

void AnimationGroup::cleanup()
{
    Animation::cleanup();
    for (Animation *p : parts)
        p->cleanup();
}

void AnimationGroup::addToInspector(Inspector *insp)
{
    for (Animation *p : parts)
        p->addToInspector(insp);
}

void AnimationGroup::updateInInspector(Inspector *insp)
{
    for (Animation *p : parts)
        p->updateInInspector(insp);
}

void AnimationGroup::removeFromInspector(Inspector *insp)
{
    for (Animation *p : parts)
        p->removeFromInspector(insp);
}

bool AnimationGroup::willAnimate(cMessage *msg)
{
    for (Animation *p : parts)
        if (p->willAnimate(msg))
            return true;
    return false;
}

void AnimationGroup::messageDuplicated(cMessage *msg, cMessage *dup)
{
    for (Animation *p : parts)
        p->messageDuplicated(msg, dup);
}

void AnimationGroup::removeMessagePointer(cMessage *msg)
{
    for (Animation *p : parts)
        p->removeMessagePointer(msg);
}

QString AnimationGroup::str() const
{
    QString result = QString("Animation Group of ") + QString::number(parts.size()) + " parts, state " + stateText[state];
    for (const auto &p : parts)
        result += "\n    - " + p->str().replace("\n", "\n      ");
    return result;
}

AnimationGroup::~AnimationGroup()
{
    for (Animation *p : parts)
        delete p;
}

//  -------- MethodcallAnimation functions  --------

MethodcallAnimation::MethodcallAnimation(cComponent *src, cComponent *dest, const char *text, bool silent)
    : Animation(getQtenv()->opt->methodCallAnimDuration / 1000.0),
    srcMod((src && src->isModule()) ? static_cast<cModule*>(src) : nullptr),
    destMod((dest && dest->isModule()) ? static_cast<cModule*>(dest) : nullptr),
    text(text), silent(silent), body(true)
{
    if (srcMod && destMod)
        path = findDirectPath(srcMod, destMod);
    connectionItems.resize(path.size());
}

void MethodcallAnimation::addOperation(Animation *operation)
{
    ASSERT(holding && operation->isHolding());
    if (auto mc = dynamic_cast<MethodcallAnimation*>(operation)) {
        ASSERT(mc->parent == nullptr);
        mc->parent = this;
    }
    body.addAnimation(operation);
}

void MethodcallAnimation::begin()
{
    Animation::begin();

    if (!silent)
        for (auto &m : connectionItems)
            for (auto p : m)
                p.second->setVisible(true);

    if (isEmpty())
        end();
}

void MethodcallAnimation::update()
{
    Animation::update();

    if (isEmpty()) {
        end();
        return;
    }

    if (silent || holdExpired())
        if (body.isEmpty() || !body.advance()) {
            end();
            return;
        }

    double t = getHoldPosition();

    bool enabled = (t > 0.2 && t < 0.4) || (t > 0.6);

    for (auto &m : connectionItems)
        for (auto p : m) {
            p.second->setLineEnabled(enabled);
            p.second->setArrowEnabled(enabled);
        }
}

void MethodcallAnimation::end()
{
    Animation::end();

    for (auto &m : connectionItems)
        for (auto p : m)
            p.second->setVisible(false);
}

void MethodcallAnimation::addToInspector(Inspector *insp)
{
    for (size_t i = 0; i < path.size(); ++i) {
        cModule *from = path[i].from;
        cModule *to = path[i].to;

        cModule *showIn;

        if (to)
            showIn = to->getParentModule();
        if (from)
            showIn = from->getParentModule();
        if (from && to)
            ASSERT(from->getParentModule() == to->getParentModule());

        if (!showIn->getBuiltinAnimationsAllowed())
            continue;

        if (auto mi = isModuleInspectorFor(showIn, insp)) {
            if (!mi->getShowMethodCalls())
                continue;

            QPointF src, dest;

            if (from)
                src = mi->getSubmodCoords(from);
            if (to)
                dest = mi->getSubmodCoords(to);

            if (!from)
                src = QPointF(dest.x() - dest.y() / 4 - 4, -16);
            if (!to)
                dest = QPointF(src.x() + src.y()  / 4 + 4, -16);

            QPointF offset = -QLineF(src, dest).normalVector().unitVector().translated(-src).p2() * 4.0;

            src += offset;
            dest += offset;

            auto layer = mi->getAnimationLayer();
            auto connectionItem = new ConnectionItem(layer);
            connectionItem->setVisible(false);
            connectionItem->setLineEnabled(false);
            connectionItem->setArrowEnabled(false);
            connectionItem->setSource(src);
            connectionItem->setDestination(dest);
            connectionItem->setColor("red");
            connectionItem->setLineStyle(Qt::DashLine);
            connectionItem->setText(text);
            connectionItem->setTextPosition(Qt::AlignCenter);
            connectionItem->setTextColor("black");
            connectionItem->setTextBackgroundColor("#F0F0F0");
            connectionItem->setTextOutlineColor("transparent");

            connectionItems[i][mi] = connectionItem;
        }
    }

    body.addToInspector(insp);

    if (state == PLAYING)
        update();
}

void MethodcallAnimation::updateInInspector(Inspector *insp)
{
    removeFromInspector(insp);
    addToInspector(insp);
    /*
    for (size_t i = 0; i < path.size(); ++i) {
        cModule *from = path[i].from;
        cModule *to = path[i].to;

        cModule *showIn;

        if (to)
            showIn = to->getParentModule();
        if (from)
            showIn = from->getParentModule();
        if (from && to)
            ASSERT(from->getParentModule() == to->getParentModule());

        if (auto mi = isModuleInspectorFor(showIn, insp)) {
            QPointF src, dest;

            if (from)
                src = mi->getSubmodCoords(from);
            if (to)
                dest = mi->getSubmodCoords(to);

            if (!from)
                src = QPointF(dest.x() - dest.y() / 4, 0);
            if (!to)
                dest = QPointF(src.x() + src.y()  / 4, 0);

            auto connectionItem = connectionItems[i][mi];
            ASSERT(connectionItem);
            connectionItem->setSource(src);
            connectionItem->setDestination(dest);
        }
    }

    body.addToInspector(insp);*/
}

void MethodcallAnimation::removeFromInspector(Inspector *insp)
{
    body.removeFromInspector(insp);

    if (auto mi = dynamic_cast<ModuleInspector *>(insp))
        for (auto &m : connectionItems) {
            if (containsKey(m, mi)) {
                delete m[mi];
                m[mi] = nullptr;
            }
            m.erase(mi);
        }
}

bool MethodcallAnimation::isEmpty() const
{
    if (!body.isEmpty())
        return false;

    if (silent || text.isEmpty())
        return true;

    for (auto &m : connectionItems)
        if (!m.empty())
            return false;

    return true;
}

QString MethodcallAnimation::str() const
{
    return QString("MethodCall '") + text + "' from "
            + (srcMod ? srcMod->getFullName() : "nullptr") + " to " +  (destMod ? destMod->getFullName() : "nullptr")
            + " state " + stateText[state];
}

MethodcallAnimation::~MethodcallAnimation()
{
    for (auto &m : connectionItems)
        for (auto p : m)
            delete p.second;
}

// --------  MessageAnimation methods  --------

void MessageAnimation::begin()
{
    Animation::begin();

    for (auto p : messageItems)
        p.second->setVisible(true);
}

void MessageAnimation::end()
{
    Animation::end();

    for (auto p : messageItems)
        p.second->setVisible(false);

    requestAnimationSpeed(0.0);
}

void MessageAnimation::removeMessagePointer(cMessage *msg)
{
    if (msg == this->msg) {
        this->msg = nullptr;
        for (auto &m : messageItems)
            m.second->setData(ITEMDATA_COBJECT, QVariant::fromValue((cObject*)msgDup));
    }
    if (msg == this->msgDup) {
        this->msgDup = nullptr;
        for (auto &m : messageItems)
            m.second->setData(ITEMDATA_COBJECT, QVariant::fromValue((cObject*)nullptr));
    }
}

void MessageAnimation::messageDuplicated(cMessage *msg, cMessage *dup)
{
    if (msg == this->msg && !msgDup)
        msgDup = dup;
}

void MessageAnimation::removeFromInspector(Inspector *insp)
{
    for (auto &p : messageItems)
        if ((Inspector*)p.first == insp) {
            delete p.second;
            p.second = nullptr;
        }
    messageItems.erase((ModuleInspector*)insp);

    if (isEmpty()) {
        requestAnimationSpeed(0.0);

        if (holding && state == PLAYING)
            end();
    }
}

MessageAnimation::~MessageAnimation()
{
    for (auto &p : messageItems)
        delete p.second;

    requestAnimationSpeed(0.0);
}

// --------  SendOnConnAnimation methods  --------

void SendOnConnAnimation::begin()
{
    MessageAnimation::begin();

    if (isEmpty()) {
        if (holding)
            end();
    }
    else {
        for (auto p : messageItems)
            p.second->setPos(p.first->getConnectionLine(gate).p1());
        requestAnimationSpeed(prop.dbl()+trans.dbl());
    }
}


void SendOnConnAnimation::update()
{
    MessageAnimation::update();

    cMessage *msg = msgToUse();
    if (!msg) {
        end();
        return;
    }

    if (holding) {
        if (isEmpty() || holdExpired()) {
            end();
            return;
        }

        double t = getHoldPosition();

        for (auto p : messageItems)
            p.second->setPos(p.first->getConnectionLine(gate).pointAt(t));
    }
    else {
        SimTime progr = simTime() - start;

        if (progr >= (prop+trans)) {
            end();
            return;
        }

        // the "forward", and "backward" end of the message line on the given (connection) line, range is 0.0 .. 1.0
        double t1, t2; // typically t1 >= t2, otherwise the message would travel "backward"

        if (prop.isZero()) {
            // either the entire line is visible, or none of it
            if (simTime() < start) {
                t1 = 0.0;
                t2 = 0.0;
            }
            else if (simTime() <= (start + trans)) {
                t1 = 1.0;
                t2 = 0.0;
            }
            else {
                t1 = 1.0;
                t2 = 1.0;
            }
        }
        else {
            // the message line "slides" over the (connection) line
            t1 = progr / prop;
            t2 = (progr - trans) / prop;
        }

        double animSpeed = (trans < prop)
                ? (trans.dbl() + prop.dbl())
                : ((t1 <= 1.0 || t2 >= 0.0)
                   ? prop.dbl()
                   : 2*trans.dbl());

        if (!isEmpty())
            requestAnimationSpeed(animSpeed);

        bool visible = t1 > 0.0 && t2 < 1.0;
        t1 = clip(0.0, t1, 1.0);
        t2 = clip(0.0, t2, t1);

        for (auto p : messageItems) {
            QLineF connLine = p.first->getConnectionLine(gate);
            p.second->positionOntoLine(connLine, t1, t2, msg->isPacket() && static_cast<cPacket*>(msg)->isUpdate());
            p.second->setVisible(visible);
        }
    }
}

void SendOnConnAnimation::addToInspector(Inspector *insp)
{
    cMessage *msg = msgToUse();
    if (!msg)
        return;

    bool isUpdatePacket = msg->isPacket() && static_cast<cPacket *>(msg)->isUpdate();

    cModule *mod = gate->getOwnerModule();
    if (gate->getType() == cGate::OUTPUT)
        mod = mod->getParentModule();

    if (!mod->getBuiltinAnimationsAllowed())
        return;

    if (auto mi = isModuleInspectorFor(mod, insp)) {
        auto layer = mi->getAnimationLayer();
        MessageItem *messageItem;
        if (trans.isZero() && !isUpdatePacket) {
            SymbolMessageItem *item = new SymbolMessageItem(layer);
            MessageItemUtil::setupSymbolFromDisplayString(item, msg, mi->getImageSizeFactor());
            messageItem = item;
        }
        else {
            LineMessageItem *item = new LineMessageItem(layer);
            MessageItemUtil::setupLineFromDisplayString(item, msg);
            messageItem = item;
        }

        // so the message will be above any connection lines
        messageItem->setZValue(1);
        messageItem->setVisible(state == PLAYING);

        messageItems[mi] = messageItem;
    }

    if (state == PLAYING)
        update();
}

void SendOnConnAnimation::updateInInspector(Inspector *insp)
{
    removeFromInspector(insp);
    addToInspector(insp);

    /*
    cModule *mod = gate->getOwnerModule();
    if (gate->getType() == cGate::OUTPUT)
        mod = mod->getParentModule();

    if (auto mi = isModuleInspectorFor(mod, insp)) {
        auto messageItem = messageItems[mi];
        ASSERT(messageItem);
        messageItem->setLine(getLine(mi).pointAt(getHoldPosition()));
    }*/
}

QString SendOnConnAnimation::str() const {
    return QString("SendOnConn ")
            + QString("0x%1").arg(quintptr(msg), 0, 16)
            + (holding ? " holding" : "")
            + " state: " + stateText[state]
            + " visible in " + QString::number(messageItems.size()) + " inspectors"
            + " start: " + start.str().c_str() + " prop: " + prop.str().c_str() + " trans: " + trans.str().c_str()
          //  + " (" + QString::number(100 * (getQtenv()->getAnimationTime() - holdStarted) / holdDuration) + "%)"
            ;
}

// --------  SendDirectAnimation functions  --------

SendDirectAnimation::SendDirectAnimation(cModule *src, cMessage *msg, cGate *dest)
    : MessageAnimation(msg, 1.0), srcModuleId(src->getId()), dest(dest)
{
}

SendDirectAnimation::SendDirectAnimation(cModule *src, cMessage *msg, cGate *dest, SimTime start, SimTime prop, SimTime trans)
    : MessageAnimation(msg), srcModuleId(src->getId()), dest(dest), start(start), prop(prop), trans(trans)
{
}

void SendDirectAnimation::init()
{
    MessageAnimation::init();
    for (auto &c : connectionItems)
        c.second->setVisible(getQtenv()->opt->showSendDirectArrows);
}

void SendDirectAnimation::begin()
{
    MessageAnimation::begin();
    if (isEmpty()) {
        if (holding)
            end();
    }
    else // still ok if holding
        requestAnimationSpeed(prop.dbl()+trans.dbl());
}

void SendDirectAnimation::update()
{
    MessageAnimation::update();

    cMessage *msg = msgToUse();
    if (!msg) {
        end();
        return;
    }

    auto path = findDirectPath(getSimulation()->getModule(srcModuleId), dest->getOwnerModule());

    // the "forward", and "backward" end of the message line on the given (connection) line, range is 0.0 .. 1.0
    double t1, t2; // typically t1 >= t2, otherwise the message would travel "backward"

    bool visible = true;

    if (holding) {
        if (isEmpty() || holdExpired()) {
            end();
            return;
        }

        t1 = t2 = getHoldPosition();
        visible = t1 > 0.0 && t2 < 1.0;
    }
    else {
        SimTime progr = simTime() - start;

        if (progr >= (prop+trans)) {
            end();
            return;
        }


        if (prop.isZero()) {
            // either the entire line is visible, or none of it
            if (simTime() < start) {
                t1 = 0.0;
                t2 = 0.0;
            }
            else if (simTime() <= (start + trans)) {
                t1 = 1.0;
                t2 = 0.0;
            }
            else {
                t1 = 1.0;
                t2 = 1.0;
            }
        }
        else {
            // the message line "slides" over the (connection) line
            t1 = progr / prop;
            t2 = (progr - trans) / prop;
        }

        double animSpeed = (trans < prop)
                ? (trans.dbl() + prop.dbl())
                : ((t1 <= 1.0 || t2 >= 0.0)
                   ? prop.dbl()
                   : 2*trans.dbl());

        if (!isEmpty())
            requestAnimationSpeed(animSpeed);

        visible = t1 > 0.0 && t2 < 1.0;

        t1 = clip(0.0, t1, 1.0);
        t2 = clip(0.0, t2, t1);
    }

    for (auto p : path) {
        cModule *showIn;

        if (p.to)
            showIn = p.to->getParentModule();
        if (p.from)
            showIn = p.from->getParentModule();
        if (p.from && p.to)
            ASSERT(p.from->getParentModule() == p.to->getParentModule());

        for (auto m : messageItems)
            if (isModuleInspectorFor(showIn, m.first)) {
                QPointF src, dest;

                if (p.from)
                    src = m.first->getSubmodCoords(p.from);
                if (p.to)
                    dest = m.first->getSubmodCoords(p.to);

                if (!p.from)
                    src = QPointF(dest.x() - dest.y() / 4 - 4, -16);
                if (!p.to)
                    dest = QPointF(src.x() + src.y() / 4 + 4, -16);

                QLineF fullLine(src, dest);

                m.second->positionOntoLine(fullLine, t1, t2, msg->isPacket() && static_cast<cPacket*>(msg)->isUpdate());
                m.second->setVisible(visible);
            }
    }
}

void SendDirectAnimation::end()
{
    MessageAnimation::end();

    for (auto &c : connectionItems)
        c.second->setVisible(false);
}

void SendDirectAnimation::addToInspector(Inspector *insp)
{
    cMessage *msg = msgToUse();
    if (!msg)
        return;

    auto path = findDirectPath(getSimulation()->getModule(srcModuleId), dest->getOwnerModule());

    bool isUpdatePacket = msg->isPacket() && static_cast<cPacket *>(msg)->isUpdate();
    for (auto & segment : path) {
        cModule *from = segment.from;
        cModule *to = segment.to;

        cModule *showIn;

        if (to)
            showIn = to->getParentModule();
        if (from)
            showIn = from->getParentModule();
        if (from && to)
            ASSERT(from->getParentModule() == to->getParentModule());

        if (!showIn->getBuiltinAnimationsAllowed())
            continue;

        if (auto mi = isModuleInspectorFor(showIn, insp)) {
            auto layer = mi->getAnimationLayer();
            MessageItem *messageItem;
            if (trans.isZero() && !isUpdatePacket) {
                SymbolMessageItem *item = new SymbolMessageItem(layer);
                MessageItemUtil::setupSymbolFromDisplayString(item, msg, mi->getImageSizeFactor());
                messageItem = item;
            }
            else {
                LineMessageItem *item = new LineMessageItem(layer);
                MessageItemUtil::setupLineFromDisplayString(item, msg);
                messageItem = item;
            }
            messageItem->setVisible(state == PLAYING);
            messageItem->setZValue(1);
            messageItems[mi] = messageItem;

            QPointF src, dest;

            if (from)
                src = mi->getSubmodCoords(from);
            if (to)
                dest = mi->getSubmodCoords(to);

            if (!from)
                src = QPointF(dest.x() - dest.y() / 4 - 4, -16);
            if (!to)
                dest = QPointF(src.x() + src.y()  / 4 + 4, -16);

            auto connectionItem = new ConnectionItem(layer);
            connectionItem->setVisible(getQtenv()->opt->showSendDirectArrows
                                       && (state == WAITING || state == PLAYING));
            connectionItem->setSource(src);
            connectionItem->setDestination(dest);
            connectionItem->setArrowEnabled(false);
            connectionItem->setColor("blue");
            connectionItem->setLineStyle(Qt::DashLine);
            connectionItems[mi] = connectionItem;
        }
    }

    if (state == PLAYING)
        update();
}

void SendDirectAnimation::updateInInspector(Inspector *insp)
{
    removeFromInspector(insp);
    addToInspector(insp);
}

void SendDirectAnimation::removeFromInspector(Inspector *insp)
{
    MessageAnimation::removeFromInspector(insp);

    if (auto mi = dynamic_cast<ModuleInspector *>(insp)) {
        delete connectionItems[mi];
        connectionItems[mi] = nullptr;
        connectionItems.erase(mi);
    }
}

QString SendDirectAnimation::str() const
{
    cModule *src = getSimulation()->getModule(srcModuleId);
    return QString("SendDirect ") + (msgToUse() ? msgToUse()->getFullName() : "nullptr") +
            + "from: " + (src ? src->getFullPath().c_str() : "<deleted>") + " to: " + dest->getFullPath().c_str()
            + " state: " + stateText[state];
}

SendDirectAnimation::~SendDirectAnimation()
{
    for (auto &c: connectionItems)
        delete c.second;
}

QLineF DeliveryAnimation::getLine(ModuleInspector *mi) const
{
    ASSERT(gate);
    auto connLine = mi->getConnectionLine(gate);

    // the max amount of pixels an arriving message will move inside the dest submodule rectangle
    static const double msgEndCreep = 10;

    QPointF srcPos = connLine.p2();
    cModule *dest = gate->getNextGate()->getOwnerModule();
    QPointF destCenterPos = mi->getSubmodCoords(dest);
    QPointF fromEdgeToCenter = destCenterPos - connLine.p2();
    double length = std::sqrt(fromEdgeToCenter.x() * fromEdgeToCenter.x() + fromEdgeToCenter.y() * fromEdgeToCenter.y());

    QPointF destPos = connLine.p2()
            + (length > 0.0
               ? fromEdgeToCenter / length * std::min(length, msgEndCreep)
               : QPointF());

    return QLineF(srcPos, destPos);
}

void DeliveryAnimation::begin()
{
    MessageAnimation::begin();

    if (isEmpty() || !msgToUse())
        end();
    else
        for (auto m : messageItems)
            m.second->setPos(gate
                             ? getLine(m.first).p1()
                             : m.first->getSubmodCoords(msgToUse()->getArrivalModule()));
}

void DeliveryAnimation::update()
{
    MessageAnimation::update();

    if (isEmpty() || holdExpired() || !msgToUse()) {
        end();
        return;
    }

    double t = getHoldPosition();

    for (auto p : messageItems) {
        if (gate) {
            p.second->setPos(getLine(p.first).pointAt(t));
        } else {
            p.second->setPos(p.first->getSubmodCoords(msgToUse()->getArrivalModule()));
            p.second->setVisible((t < 0.2) || (t > 0.4 && t < 0.6) || (t > 0.8));
        }
    }
}

void DeliveryAnimation::addToInspector(Inspector *insp)
{
    if (!msgToUse())
        return;

    cModule *mod;
    if (gate) {
        mod = gate->getOwnerModule();
        if (gate->getType() == cGate::OUTPUT)
            mod = mod->getParentModule();
    } else
        mod = msgToUse()->getArrivalModule()->getParentModule();

    if (!mod->getBuiltinAnimationsAllowed())
        return;

    if (auto mi = isModuleInspectorFor(mod, insp)) {
        auto layer = mi->getAnimationLayer();
        SymbolMessageItem *messageItem = new SymbolMessageItem(layer);
        MessageItemUtil::setupSymbolFromDisplayString(messageItem, msgToUse(), mi->getImageSizeFactor());
        // so the message will be above any connection lines
        messageItem->setZValue(1);
        messageItem->setVisible(state == PLAYING);

        messageItems[mi] = messageItem;
    }

    if (state == PLAYING)
        update();
}

void DeliveryAnimation::updateInInspector(Inspector *insp)
{
    removeFromInspector(insp);
    addToInspector(insp);
}

QString DeliveryAnimation::str() const
{
    return QString("delivery of") + (msgToUse() ? msgToUse()->getFullName() : "nullptr")
            + " state: " + stateText[state];
}

}  // namespace qtenv
}  // namespace omnetpp
