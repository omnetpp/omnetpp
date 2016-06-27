//==========================================================================
//  ANIMATOR.H - part of
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

#ifndef __OMNETPP_QTENV_ANIMATOR_H
#define __OMNETPP_QTENV_ANIMATOR_H

#include <vector>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QObject>
#include <QTimer>
#include <omnetpp/cmessage.h>
#include "omnetpp/simtime_t.h"
#include "messageitem.h"
#include "connectionitem.h"

namespace omnetpp {
namespace qtenv {

enum SendAnimMode {ANIM_BEGIN, ANIM_END, ANIM_THROUGH};

class GraphicsLayer;
class ModuleInspector;


class Animation {
    friend class Animator;

protected:
    ModuleInspector *inspector;

    QPointF src;
    QPointF dest;

    float time = 0;
    float duration;

    Animation(ModuleInspector *insp, const QPointF &src, const QPointF &dest, float duration);

    virtual QString info() const = 0;

    enum State { PENDING, WAITING, PLAYING, FINISHED, DONE };
    // PENDING -> init() -> WAITING -> begin() ->
    // -> PLAYING -> end() -> FINISHED -> cleanup() -> DONE
    //      ||
    //  setProgress()
    State state = PENDING;

public:
    void advance(float delta); // delta is in seconds

    // all of these (except setProgress) must be called from the subclasses' overrides
    virtual void init() { state = WAITING; }
    virtual void begin() { state = PLAYING; }
    virtual void setProgress(float t) = 0; // t is always 0..1, must set state to FINISHED when t >= 1-epsilon
    virtual void end() { state = FINISHED; }
    virtual void cleanup() { state = DONE; }

    // they are "in the same group", so their inits will be called
    // at the same time, before any of them is played
    // and their cleanups after all of them has ended
    virtual bool inSameGroup(const Animation *other) const { return this == other; }

    // they should be played simultaneously - should always imply relatedTo(other)
    // their begins will be called at the same time,
    // but the ends are not guaranteed to be synchronized
    virtual bool concurrentWith(const Animation *other) const { return this == other; }

    virtual ~Animation() { }
};

class Animator : public QObject
{
    Q_OBJECT

    std::vector<Animation *> animations;
    std::map<std::pair<ModuleInspector *, cMessage *>, MessageItem *> messageItems;

    static const int frameRate;
    // the max amount of pixels an arriving message will move inside the dest submodule rectangle
    static const double msgEndCreep;
    bool inHurry = false;

    Animation *firstAnimInState(Animation::State state) const;

public:
    explicit Animator();
    ~Animator();
    QTimer timer;

    void redrawMessages();
    virtual void animateMethodcallAscent(ModuleInspector *insp, cModule *srcSubmodule, const char *methodText);
    virtual void animateMethodcallDescent(ModuleInspector *insp, cModule *destSubmodule, const char *methodText);
    virtual void animateMethodcallHoriz(ModuleInspector *insp, cModule *srcSubmodule, cModule *destSubmodule, const char *methodText);
    virtual void animateSendOnConn(ModuleInspector *insp, cGate *srcGate, cMessage *msg, SendAnimMode mode);
    virtual void animateSenddirectAscent(ModuleInspector *insp, cModule *srcSubmodule, cMessage *msg);
    virtual void animateSenddirectDescent(ModuleInspector *insp, cModule *destSubmodule, cMessage *msg, SendAnimMode mode);
    virtual void animateSenddirectHoriz(ModuleInspector *insp, cModule *srcSubmodule, cModule *destSubmodule, cMessage *msg, SendAnimMode mode);
    virtual void animateSenddirectDelivery(ModuleInspector *insp, cModule *destSubmodule, cMessage *msg);

signals:
    void finish();

public slots:
    void addAnimation(Animation *anim);
    void onFrameTimer();
    void play();
    void clear();
    void hurry();

    // removes any animation and items on the given inspector,
    // so it can be deleted safely. called from the ModuleInspector dtor
    void clearInspector(ModuleInspector *insp);
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_ANIMATOR_H
