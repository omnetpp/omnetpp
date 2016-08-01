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
#include "moduleinspector.h"  // for the layer

#include <QDebug>
#include <QEventLoop>
#include <QGraphicsView>
#include <omnetpp/cfutureeventset.h>
#include <utility>

#define emit

namespace omnetpp {
namespace qtenv {

//  --------  Animation subclass declarations  --------

class AnimationGroup : public Animation
{
  protected:
    std::vector<Animation *> parts;
    int flags;

  public:

    // flag constants

    // If this is set, the init() of the parts will be
    // called just before their begin() calls,
    // (at some time in this groups advance())
    // and not in this groups init() or begin().
    // If not set, they will be called in the groups begin()
    static const int DEFER_PARTS_INIT = 1 << 0;

    // If this is set, the cleanup() of the parts will be
    // called in this groups cleanup().
    // If not set, they will be called in this groups end().
    static const int DEFER_PARTS_CLEANUP = 1 << 1;

    explicit AnimationGroup(int flags = 0) : flags(flags) {}

    void end() override {
        if (!(flags & DEFER_PARTS_CLEANUP)) {
            for (auto p : parts) {
                p->cleanup();
            }
        }
        Animation::end();
    }

    void cleanup() override {
        if (flags & DEFER_PARTS_CLEANUP) {
            for (auto p : parts) {
                p->cleanup();
            }
        }
        Animation::cleanup();
    }

    bool willAnimate(cMessage *msg) override {
        return std::any_of(parts.begin(), parts.end(),
                           [&msg](Animation *p) {
            return p->willAnimate(msg);
        });
    }

    void prune() {
        for (auto& p : parts) {
            // for every group part
            if (auto g = dynamic_cast<AnimationGroup *>(p)) {
                // if it is empty, throw it right out
                if (g->isEmpty()) {
                    delete p;
                    p = nullptr;  // the nullptrs will be erased from the vector below
                }
                else {
                    // otherwise keep it, but prune it
                    g->prune();
                }
            }
        }

        // erasing the nullptrs introduced above
        parts.erase(std::remove(parts.begin(), parts.end(), nullptr), parts.end());
    }

    bool isEmpty() override {
        return std::all_of(parts.begin(), parts.end(),
                           [](Animation *p) {
            return p->isEmpty();
        });
    }

    void begin() override {
        if (!(flags & DEFER_PARTS_INIT)) {
            for (auto p : parts) {
                p->init();
            }
        }
        Animation::begin();
    }

    virtual void addAnimation(Animation *a) {
        parts.push_back(a);
    }

    const std::vector<Animation *>& getParts() const {
        return parts;
    }

    void clearInspector(ModuleInspector *insp);
    void removeMessagePointer(cMessage *msg);

    QString info() const override {
        QString result = "AnimationGroup of " + QString::number(parts.size()) + " parts";
        for (Animation *a : parts) {
            result += "\n    - " + a->info().replace("\n", "\n      ");
        }
        return result;
    }

    virtual ~AnimationGroup() {
        for (auto p : parts) delete p;
    }
};

class SequentialAnimation : public AnimationGroup
{
    size_t currentPart = 0;

  public:
    using AnimationGroup::AnimationGroup;

    float getDuration() override {
        float duration = 0;
        for (auto p : parts) {
            duration += p->getDuration();
        }
        return duration;
    }

    void begin() override {
        AnimationGroup::begin();
        if (!parts.empty()) {
            if (flags & DEFER_PARTS_INIT)
                parts[0]->init();
            parts[0]->begin();
        }
    }

    void advance(float delta) {
        if (currentPart >= parts.size()) {
            end();
            return;
        }

        parts[currentPart]->advance(delta);

        if (parts[currentPart]->getState() == FINISHED) {
            ++currentPart;
            if (currentPart < parts.size()) {
                if (flags & DEFER_PARTS_INIT)
                    parts[currentPart]->init();
                parts[currentPart]->begin();
            }
            else {
                end();
            }
        }
    }

    QString info() const override { return "Sequential" + AnimationGroup::info(); }
};

class ParallelAnimation : public AnimationGroup
{
    float getDuration() override {
        float duration = 0;
        for (auto p : parts) {
            duration = std::max(duration, p->getDuration());
        }
        return duration;
    }

    void begin() override {
        AnimationGroup::begin();
        for (auto p : parts) {
            if (flags & DEFER_PARTS_INIT)
                p->init();
            p->begin();
        }
    }

    void advance(float delta) {
        float duration = getDuration();
        bool somePlaying = false;
        for (auto p : parts) {
            if (p->getState() == PLAYING) {
                p->advance(delta * ((flags & STRETCH_TIME) ? (p->getDuration() / duration) : 1));
                // have to chek again, it might have ended in the advance call
                if (p->getState() == PLAYING) {
                    somePlaying = true;
                }
            }
        }

        if (!somePlaying) {
            end();
        }
    }

    QString info() const override { return "Parallel" + AnimationGroup::info(); }

  public:

    using AnimationGroup::AnimationGroup;

    // If set, all parts will be played for the duration of the longest one.
    // If unset, each of the parts' own duration will be respected
    static const int STRETCH_TIME = 1 << 8;
    // shift by 8 just to ensure independence from superclass flags
};

class SimpleAnimation : public Animation
{
    friend class AnimationGroup;

  protected:
    ModuleInspector *inspector;

    QPointF src, dest;

    float duration;
    float getDuration() override { return duration; }

    SimpleAnimation(ModuleInspector *insp, const QPointF& src, const QPointF& dest, float duration)
        : inspector(insp), src(src), dest(dest), duration(duration) {}

    void advance(float delta) override {
        time += delta;

        if (time > duration) {
            end();
        }
        else {
            setProgress(time / duration);
        }
    }

    virtual void setProgress(float t) = 0;  // t is always 0..1
};

class MethodcallAnimation : public SimpleAnimation
{
    cModule *srcMod, *destMod;
    QString text;
    ConnectionItem *connectionItem;

  public:
    MethodcallAnimation(ModuleInspector *insp, cModule *srcMod, cModule *destMod, const char *text);
    void begin() override;
    void setProgress(float t) override;
    void cleanup() override;
    QString info() const override {
        return QString("MethodCall '") + text + "' from "
               + (srcMod ? srcMod->getFullName() : "NULL") + " to " +  (destMod ? destMod->getFullName() : "NULL");
    }

    ~MethodcallAnimation();
};

class SendAnimation : public SimpleAnimation
{
    friend class Animator;
    friend class AnimationGroup;

  protected:
    cMessage *msg;
    MessageItem *messageItem;
    SendAnimMode mode;
    SendAnimation(ModuleInspector *insp, SendAnimMode mode, const QPointF& src, const QPointF& dest, cMessage *msg, float duration);
    void removeMessagePointer();
    bool willAnimate(cMessage *msg) override { return state < FINISHED && msg == this->msg; }
    void begin() override;
    void setProgress(float t) override;
    void end() override;
    void cleanup() override;
    ~SendAnimation();
};

class SendOnConnAnimation : public SendAnimation
{
  public:
    SendOnConnAnimation(ModuleInspector *insp, SendAnimMode mode, const QPointF& src, const QPointF& dest, cMessage *msg);
    QString info() const override {
        return QString("SendOnConn '") + ((msg && mode != ANIM_END) ? msg->getName() : "NULL") + "' mode "
               + (mode == ANIM_BEGIN ? "begin" : mode == ANIM_END ? "end" : mode == ANIM_THROUGH ? "through" : "INVALID")
               + " in " + inspector->QWidget::windowTitle();
    }
};

class SendDirectAnimation : public SendAnimation
{
  public:
    enum Direction { DIR_ASCENT, DIR_HORIZ, DIR_DESCENT, DIR_DELIVERY };

  private:
    ConnectionItem *connectionItem = nullptr;
    Direction dir;

  public:
    SendDirectAnimation(ModuleInspector *insp, Direction dir, SendAnimMode mode, const QPointF& src, const QPointF& dest, cMessage *msg);
    void init() override { connectionItem->setVisible(getQtenv()->opt->showSendDirectArrows); }
    void setProgress(float t) override;
    void cleanup() override { connectionItem->setVisible(false); }
    QString info() const override {
        return QString("SendDirect '") + ((msg && dir != DIR_DELIVERY) ? msg->getName() : "NULL") + "' mode "
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

void Animator::findDirectPath(cModule *srcmod, cModule *destmod, Animator::PathVec& pathvec)
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

void Animator::insertSerial(Animation *anim)
{
    // this will reset the staged broadcast message animation
    messageStages = nullptr;
    animation->addAnimation(anim);
}

void Animator::insertConcurrent(Animation *anim, cMessage *msg)
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
        for (stage = 0; stage < parts.size(); ++stage) {
            if (!parts[stage]->willAnimate(msg)) {
                break;
            }
        }
        if (stage >= parts.size()) {
            // if all stages so far animate this message
            // (or there aren't any), add a new, empty stage
            messageStages->addAnimation(new ParallelAnimation(ParallelAnimation::STRETCH_TIME));
        }
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

Animator::Animator()
{
    timer.setInterval(1000 / frameRate);
    timer.setSingleShot(false);
    connect(&timer, SIGNAL(timeout()), this, SLOT(onFrameTimer()));
    animation = new SequentialAnimation();
}

Animator::~Animator()
{
    clear();
    delete animation;
}

void Animator::redrawMessages()
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
            if (!moduleInsp) {
                continue;
            }

            if (!willAnimate(msg) &&
                arrivalMod &&
                arrivalMod->getParentModule() == moduleInsp->getObject() &&
                msg->getArrivalGateId() >= 0) {
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

void Animator::removeMessagePointer(cMessage *msg)
{
    for (auto i : messageItems) {
        if (i.first.second == msg)
            i.second->setData(1, QVariant());
    }

    animation->removeMessagePointer(msg);
}

void Animator::animateMethodcall(cComponent *fromComp, cComponent *toComp, const char *methodText)
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
            for (auto in : inspectors) {
                if (auto insp = isModuleInspectorFor(enclosingmod, in)) {
                    subgroup->addAnimation(new MethodcallAnimation(insp, mod, nullptr, methodText));
                }
            }
        }
        else if (i->from == nullptr) {
            // animate descent towards destination module
            cModule *mod = i->to;
            cModule *enclosingmod = mod->getParentModule();
            for (auto in : inspectors) {
                if (auto insp = isModuleInspectorFor(enclosingmod, in)) {
                    subgroup->addAnimation(new MethodcallAnimation(insp, nullptr, mod, methodText));
                }
            }
        }
        else {
            cModule *enclosingmod = i->from->getParentModule();
            for (auto in : inspectors) {
                if (auto insp = isModuleInspectorFor(enclosingmod, in)) {
                    subgroup->addAnimation(new MethodcallAnimation(insp, i->from, i->to, methodText));
                }
            }
        }
        group->addAnimation(subgroup);
    }

    // this one i necessary to clean up the arrows after the methodcall ended
    // this is not using LAZY_CLEANUP, the one above is, and that should be
    auto container = new SequentialAnimation();
    container->addAnimation(group);

    insertSerial(container);
}

void Animator::animateSendDirect(cMessage *msg, cModule *srcModule, cGate *destGate)
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
            for (auto in : inspectors) {
                if (auto insp = isModuleInspectorFor(enclosingmod, in)) {
                    auto end = insp->getSubmodCoords(mod);
                    QPointF start(end.x() - end.y() / 4, 0);
                    subgroup->addAnimation(new SendDirectAnimation(insp, SendDirectAnimation::DIR_DESCENT, isArrival ? ANIM_BEGIN : ANIM_THROUGH, start, end, msg));
                }
            }
        }
        else {
            // level
            cModule *enclosingmod = i->from->getParentModule();
            bool isArrival = (i->to == arrivalmod);
            for (auto in : inspectors) {
                if (auto insp = isModuleInspectorFor(enclosingmod, in)) {
                    auto start = insp->getSubmodCoords(i->from);
                    auto end = insp->getSubmodCoords(i->to);
                    subgroup->addAnimation(new SendDirectAnimation(insp,
                        SendDirectAnimation::DIR_HORIZ, isArrival ? ANIM_BEGIN : ANIM_THROUGH,
                        start, end, msg));
                }
            }
        }
        insertConcurrent(subgroup, msg);
    }
}

void Animator::animateSendHop(cMessage *msg, cGate *srcGate, bool isLastHop)
{
    cModule *mod = srcGate->getOwnerModule();
    if (srcGate->getType() == cGate::OUTPUT)
        mod = mod->getParentModule();

    auto subgroup = new ParallelAnimation(ParallelAnimation::STRETCH_TIME);
    for (auto in : getQtenv()->getInspectors()) {
        if (auto insp = isModuleInspectorFor(mod, in)) {
            QLineF connLine = insp->getConnectionLine(srcGate);

            subgroup->addAnimation(new SendOnConnAnimation(insp,
                isLastHop ? ANIM_BEGIN : ANIM_THROUGH,
                connLine.p1(), connLine.p2(), msg));
        }
    }

    insertConcurrent(subgroup, msg);
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

    auto subgroup = new ParallelAnimation(ParallelAnimation::STRETCH_TIME);

    for (auto in : getQtenv()->getInspectors()) {
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
    }
    insertSerial(subgroup);
}

void Animator::animateDeliveryDirect(cMessage *msg)
{
    // find suitable inspectors and do animate the message...
    cGate *g = msg->getArrivalGate();
    ASSERT(g);
    cModule *destmod = g->getOwnerModule();
    cModule *mod = destmod->getParentModule();

    auto subgroup = new ParallelAnimation();

    for (auto in : getQtenv()->getInspectors()) {
        if (auto insp = isModuleInspectorFor(mod, in)) {
            auto end = insp->getSubmodCoords(destmod);
            subgroup->addAnimation(new SendDirectAnimation(insp,
                SendDirectAnimation::DIR_DELIVERY, ANIM_END, end, end, msg));
        }
    }
    insertSerial(subgroup);
}

void Animator::onFrameTimer()
{
    animation->advance((1.0f / frameRate) * getQtenv()->opt->animationSpeed * (inHurry ? 5 : 1));

    if (animation->getState() == Animation::FINISHED) {
        clear();
        emit finish();  // this will end the event loop in play()
        return;
    }
}

void Animator::play()
{
    inHurry = false;

    if (animation->isEmpty()) {
        // not playing empty animation
        return;
    }

    // if not empty, we still prune it, to reduce delay
    animation->prune();

    animation->init();
    animation->begin();

    QEventLoop loop;
    timer.start();
    connect(this, SIGNAL(finish()), &loop, SLOT(quit()));
    loop.exec();
    timer.stop();

    // end is called by itself in advance()
    animation->cleanup();

    redrawMessages();
    inHurry = false;
}

void Animator::clearMessages()
{
    for (auto i : messageItems) {
        delete i.second;
    }
    messageItems.clear();
}

void Animator::clear()
{
    clearMessages();

    delete animation;
    animation = new SequentialAnimation();

    messageStages = nullptr;
    inHurry = false;
}

void Animator::hurry()
{
    inHurry = true;
}

void Animator::clearInspector(ModuleInspector *insp)
{
    animation->clearInspector(insp);

    for (auto it = messageItems.begin(); it != messageItems.end();  /* blank */) {
        if ((*it).first.first == insp) {
            delete (*it).second;
            messageItems.erase(it++);
        }
        else {
            ++it;
        }
    }
}

//  --------  Animator member definitions  --------

bool Animator::willAnimate(cMessage *msg)
{
    return animation->willAnimate(msg);
}

//  --------  Animation subclasses' member definitions  --------

MethodcallAnimation::MethodcallAnimation(ModuleInspector *insp, cModule *srcMod, cModule *destMod, const char *text)
    : SimpleAnimation(insp, QPointF(), QPointF(), getQtenv()->opt->methodCallAnimDelay / 1000.0),
    srcMod(srcMod), destMod(destMod), text(text)
{
    if (srcMod)
        src = insp->getSubmodCoords(srcMod);
    if (destMod)
        dest = insp->getSubmodCoords(destMod);

    if (!srcMod)
        src = QPointF(dest.x() - dest.y() / 4, 0);
    if (!destMod)
        dest = QPointF(src.x()  + src.y()  / 4, 0);

    auto layer = insp->getAnimationLayer();
    connectionItem = new ConnectionItem(layer);
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
}

void MethodcallAnimation::begin()
{
    connectionItem->setVisible(true);
    SimpleAnimation::begin();
}

void MethodcallAnimation::setProgress(float t)
{
    bool enabled = (t > 0.2 && t < 0.4) || (t > 0.6);
    connectionItem->setLineEnabled(enabled);
    connectionItem->setArrowEnabled(enabled);
}

void MethodcallAnimation::cleanup()
{
    connectionItem->setVisible(false);
    SimpleAnimation::cleanup();
}

MethodcallAnimation::~MethodcallAnimation()
{
    delete connectionItem;
}

SendAnimation::SendAnimation(ModuleInspector *insp, SendAnimMode mode, const QPointF& src, const QPointF& dest, cMessage *msg, float duration)
    : SimpleAnimation(insp, src, dest, duration), msg(msg), mode(mode)
{
    auto layer = insp->getAnimationLayer();
    messageItem = new MessageItem(layer);
    MessageItemUtil::setupFromDisplayString(messageItem, msg, insp->getImageSizeFactor());
    messageItem->setVisible(false);
    messageItem->setPos(src);
    // so the message will be above any connection lines
    messageItem->setZValue(1);
}

void SendAnimation::removeMessagePointer() {
    messageItem->setData(1, QVariant());
}

void SendAnimation::begin()
{
    messageItem->setVisible(true);
    messageItem->setPos(src);

    SimpleAnimation::begin();

    // this will hide the corresponding static message
    // item if this anim is a delivery
    getQtenv()->getAnimator()->redrawMessages();
}

void SendAnimation::setProgress(float t)
{
    messageItem->setPos(src * (1-t) + dest * t);
}

void SendAnimation::cleanup()
{
    messageItem->setVisible(false);
    SimpleAnimation::cleanup();
}

SendAnimation::~SendAnimation()
{
    delete messageItem;
}

SendOnConnAnimation::SendOnConnAnimation(ModuleInspector *insp, SendAnimMode mode, const QPointF& src, const QPointF& dest, cMessage *msg)
    : SendAnimation(insp, mode, src, dest, msg, 0)  // duration calculated below
{
    auto delta = (dest - src) / insp->getZoomFactor();
    // at most 1 sec, otherwise 100 "units"/sec
    duration = std::min(std::sqrt(delta.x() * delta.x() + delta.y() * delta.y()) * 0.01, 1.0);
}

void SendAnimation::end()
{
    SimpleAnimation::end();

    messageItem->setVisible(false);

    // this will put the static message item
    // in place of the animated one
    getQtenv()->getAnimator()->redrawMessages();
}

SendDirectAnimation::SendDirectAnimation(ModuleInspector *insp, Direction dir, SendAnimMode mode, const QPointF& src, const QPointF& dest, cMessage *msg)
    : SendAnimation(insp, mode, src, dest, msg, (dir == DIR_DELIVERY) ? 0.5 : 1), dir(dir)
{
    auto layer = insp->getAnimationLayer();
    connectionItem = new ConnectionItem(layer);
    connectionItem->setVisible(false);
    connectionItem->setSource(src);
    connectionItem->setDestination(dest);
    connectionItem->setColor("blue");
    connectionItem->setLineStyle(Qt::DashLine);
}

void SendDirectAnimation::setProgress(float t)
{
    if (dir == DIR_DELIVERY)
        messageItem->setVisible((t < 0.2) || (t > 0.4 && t < 0.6) || (t > 0.8));
    else
        SendAnimation::setProgress(t);
}

SendDirectAnimation::~SendDirectAnimation()
{
    delete connectionItem;
}

void AnimationGroup::clearInspector(ModuleInspector *insp)
{
    for (auto& p : parts) {
        if (auto g = dynamic_cast<AnimationGroup *>(p))
            g->clearInspector(insp);
        if (auto s = dynamic_cast<SimpleAnimation *>(p))
            if (s->inspector == insp) {
                delete p;
                p = nullptr;
            }
    }

    parts.erase(std::remove(parts.begin(), parts.end(), nullptr), parts.end());
}

void AnimationGroup::removeMessagePointer(cMessage *msg)
{
    for (auto& p : parts) {
        if (auto g = dynamic_cast<AnimationGroup *>(p))
            g->removeMessagePointer(msg);
        if (auto s = dynamic_cast<SendAnimation *>(p))
            if (s->msg == msg)
                s->removeMessagePointer();
    }

    parts.erase(std::remove(parts.begin(), parts.end(), nullptr), parts.end());
}

}  // namespace qtenv
}  // namespace omnetpp

