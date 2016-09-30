//==========================================================================
//  messageanimator.h - part of
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
#include "messageanimations.h"

namespace omnetpp {
namespace qtenv {

class GraphicsLayer;
class ModuleInspector;

class MessageAnimator : public QObject
{
    Q_OBJECT

    AnimationGroup *animation = new SequentialAnimation();
    std::map<std::pair<ModuleInspector *, cMessage *>, MessageItem *> messageItems;

    double lastFrameTime = 0;

    // the max amount of pixels an arriving message will move inside the dest submodule rectangle
    static const double msgEndCreep;

    struct sPathEntry {
       cModule *from; // nullptr if descent
       cModule *to;   // nullptr if ascent
    };
    typedef std::vector<sPathEntry> PathVec;

    static void findDirectPath(cModule *frommodule, cModule *tomodule, PathVec& pathvec);

    // just a helper pointer for the "broadcast" grouping
    AnimationGroup *messageStages = nullptr;

    // for deliveries and "complete", "self-contained"
    // methodcall sequence animations, will be sequential
    void insertSerial(Animation *anim);

    // for message hops, this will group each message-send
    // sequence into parallel stages if enabled
    void insertConcurrent(Animation *anim, cMessage *msg);

    void hold();

    void clearMessages();

public:
    void start();
    void frame();

    void redrawMessages();

    void animateMethodcall(cComponent *fromComp, cComponent *toComp, const char *methodText);
    void animateSendDirect(cMessage *msg, cModule *srcModule, cGate *destGate);
    void animateSendHop(cMessage *msg, cGate *srcGate, bool isLastHop);
    void animateDelivery(cMessage *msg);
    void animateDeliveryDirect(cMessage *msg);

    bool willAnimate(cMessage *msg) { return animation->willAnimate(msg); }

    ~MessageAnimator();

public slots:
    void clear();

    // Removes any animation and items on the given inspector,
    // so it can be deleted safely. called from the ModuleInspector dtor.
    void clearInspector(ModuleInspector *insp);

    // Removes the cMessage* data from any (static or animated) items
    // that had msg set on them as data(1), so they will not point
    // to an invalid, already deleted message.
    void removeMessagePointer(cMessage *msg);
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_ANIMATOR_H
