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

#include <list>
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

struct Animation {
    enum AnimType {
        ANIM_ON_CONN,
        ANIM_METHODCALL,
        ANIM_SENDDIRECT
    } type;

    enum AnimDirection {
        DIR_ASCENT,
        DIR_DESCENT,
        DIR_HORIZ
    } direction;

    SendAnimMode mode;

    QString text;
    cMessage *msg = nullptr;

    float time = 0;
    float duration;
    MessageItem *messageItem = nullptr;
    // only used in senddirect and methodcall modes to temporarily connect the modules
    ConnectionItem *connectionItem = nullptr;
    ModuleInspector *inspector;

    QPointF src;
    QPointF dest;
    void setToTime(float time);
    void advance(float delta);
    bool ended();

    int groupIndex = 0;

    QString info();

    Animation(ModuleInspector *insp, AnimType type, AnimDirection direction, SendAnimMode mode, QPointF src, QPointF dest, cMessage *msg, const QString &text = "");
    Animation(ModuleInspector *insp, AnimType type, AnimDirection direction, const QPointF &src, const QPointF &dest, cMessage *msg = nullptr, const QString &text = "");
    ~Animation();
};

class Animator : public QObject
{
    Q_OBJECT

    std::list<Animation *> animations;
    std::map<std::pair<ModuleInspector *, cMessage *>, MessageItem *> messageItems;

    static const int frameRate = 40;
    bool inHurry = false;

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
    bool finished();
    void clear();
    void hurry();

    // removes any animation and items on the given inspector,
    // so it can be deleted safely. called from the ModuleInspector dtor
    void clearInspector(ModuleInspector *insp);
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_ANIMATOR_H
