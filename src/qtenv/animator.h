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
protected:
    float time = 0;

public:

    enum State { PENDING, WAITING, PLAYING, FINISHED, DONE } state = PENDING;
    // PENDING -> init() -> WAITING -> begin() ->
    // -> PLAYING -> end() -> FINISHED -> cleanup() -> DONE
    //      '|,      /'
    //  setProgress()

    virtual void advance(float delta) = 0; // delta is in seconds

    State getState() const { return state; }
    float getTime() const { return time; }
    virtual float getDuration() = 0;

    virtual bool isAnimating(cMessage *msg) { return false; }

    // only needed for debugging
    virtual QString info() const = 0;

    // all of these (except setProgress) must be called from the subclasses' overrides
    virtual void init() { state = WAITING; } // when the group is started
    virtual void begin() { state = PLAYING; } // when this anim is started in the group
    virtual void end() { state = FINISHED; } // when this anim ended (called by advance)
    virtual void cleanup() { state = DONE; } // when the group is ended

    virtual ~Animation() { }
};

class SequentialAnimation;

class Animator : public QObject
{
    Q_OBJECT

    SequentialAnimation *animation;
    std::map<std::pair<ModuleInspector *, cMessage *>, MessageItem *> messageItems;

    QTimer timer;

    static const int frameRate;
    // the max amount of pixels an arriving message will move inside the dest submodule rectangle
    static const double msgEndCreep;
    bool inHurry = false;

    struct sPathEntry {
       cModule *from; // nullptr if descent
       cModule *to;   // nullptr if ascent
    };
    typedef std::vector<sPathEntry> PathVec;

    void findDirectPath(cModule *frommodule, cModule *tomodule, PathVec& pathvec);

public:
    explicit Animator();

    void redrawMessages();
    void animateMethodcall(cComponent *fromComp, cComponent *toComp, const char *methodText);
    void animateSendDirect(cMessage *msg, cModule *srcModule, cGate *destGate);
    void animateSendHop(cMessage *msg, cGate *srcGate, bool isLastHop);
    void animateDelivery(cMessage *msg);
    void animateDeliveryDirect(cMessage *msg);

    bool isAnimating(cMessage *msg);

    ~Animator();

signals:
    void finish();

public slots:
    void addAnimation(Animation *anim);
    void onFrameTimer();
    void play();
    void clearMessages();
    void clear();
    void hurry();

    // removes any animation and items on the given inspector,
    // so it can be deleted safely. called from the ModuleInspector dtor
    void clearInspector(ModuleInspector *insp);
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_ANIMATOR_H
