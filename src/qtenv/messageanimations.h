//==========================================================================
//  messageanimations.h - part of
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

#ifndef __OMNETPP_QTENV_MESSAGEANIMATIONS_H
#define __OMNETPP_QTENV_MESSAGEANIMATIONS_H

#include "messageitem.h"
#include "connectionitem.h"

namespace omnetpp {
namespace qtenv {

enum SendAnimMode {ANIM_BEGIN, ANIM_END, ANIM_THROUGH};

class Animation {
public:

    enum State { PENDING, WAITING, PLAYING, FINISHED, DONE } state = PENDING;
    // PENDING -> init() -> WAITING -> begin() ->
    // -> PLAYING -> end() -> FINISHED -> cleanup() -> DONE
    //      '|,      /'
    //  setProgress()

    virtual void advance(float delta) = 0; // delta is in seconds

    State getState() const { return state; }
    virtual float getTime() const = 0;
    virtual float getDuration() = 0;

    virtual bool willAnimate(cMessage *msg) { return false; }
    virtual bool didAnimate(cMessage *msg) { return false; }
    virtual bool isEmpty() { return false; }

    // only needed for debugging
    virtual QString str() const = 0;

    // all of these must be called from the subclasses' overrides
    // TODO make the ASSERTs true in all cases
    virtual void init() { /* ASSERT2(state == PENDING, str().toStdString().c_str()); */ state = WAITING; } // when the group is started
    virtual void begin() { /* ASSERT2(state == WAITING, str().toStdString().c_str()); */ state = PLAYING; } // when this anim is started in the group
    virtual void end() { /* ASSERT2(state == PLAYING, str().toStdString().c_str()); */ state = FINISHED; } // when this anim ended (called by advance)
    virtual void cleanup() { /* ASSERT2(state == FINISHED, str().toStdString().c_str()); */ state = DONE; } // when the group is ended

    virtual ~Animation() { }
};

class AnimationGroup : public Animation
{
  protected:
    std::vector<Animation *> parts;
    int flags;

  public:

    // flag constants. Not two simple bools to make the calls to the constructor more verbose.

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

    void prune();
    bool isEmpty() override;

    void begin() override;
    void end() override;
    void cleanup() override;

    bool willAnimate(cMessage *msg) override;

    virtual void addAnimation(Animation *a) { parts.push_back(a); }

    const std::vector<Animation *>& getParts() const { return parts; }

    void clearInspector(ModuleInspector *insp);
    void removeMessagePointer(cMessage *msg);

    QString str() const override;

    virtual ~AnimationGroup() {
        for (auto p : parts)
            delete p;
    }
};

class SequentialAnimation : public AnimationGroup
{
    size_t currentPart = 0;

  public:
    using AnimationGroup::AnimationGroup;

    float getTime() const override;
    float getDuration() override;

    void begin() override;
    void advance(float delta);

    QString str() const override { return "Sequential" + AnimationGroup::str(); }
};

class ParallelAnimation : public AnimationGroup
{
    float time = 0;

    float getTime() const override { return time; }
    float getDuration() override;

    void begin() override;
    void advance(float delta);

    QString str() const override { return "Parallel" + AnimationGroup::str(); }

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

    float time = 0;
    float duration;

    float getTime() const override { return time; }
    float getDuration() override { return duration; }

    SimpleAnimation(ModuleInspector *insp, const QPointF& src, const QPointF& dest, float duration)
        : inspector(insp), src(src), dest(dest), duration(duration) { }

    void advance(float delta) override;

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
    QString str() const override;

    ~MethodcallAnimation() { delete connectionItem; }
};

class SendAnimation : public SimpleAnimation
{
    friend class MessageAnimator;
    friend class AnimationGroup;

  protected:
    cMessage *msg;
    MessageItem *messageItem;
    SendAnimMode mode;
    SendAnimation(ModuleInspector *insp, SendAnimMode mode, const QPointF& src, const QPointF& dest, cMessage *msg, float duration);
    void removeMessagePointer();
    bool willAnimate(cMessage *msg) override { return state < FINISHED && msg == this->msg; }
    void begin() override;
    void setProgress(float t) override { messageItem->setPos(src * (1-t) + dest * t); }
    void end() override;
    void cleanup() override;
    ~SendAnimation() { delete messageItem; }
};

class SendOnConnAnimation : public SendAnimation
{
  public:
    SendOnConnAnimation(ModuleInspector *insp, SendAnimMode mode, const QPointF& src, const QPointF& dest, cMessage *msg);
    QString str() const override;
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
    void init() override;
    void setProgress(float t) override;
    void cleanup() override { connectionItem->setVisible(false); }
    QString str() const override;

    ~SendDirectAnimation() { delete connectionItem; }
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_MESSAGEANIMATIONS_H
