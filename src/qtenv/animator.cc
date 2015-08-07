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

void Animator::addAnimation(Animation *anim) {
    animations.push_back(anim);

    if (anim->mode == ANIM_END) {
        play();
    }
}

void Animator::onFrameTimer() {
    for (auto &a : animations) {
        if (a && (a->groupIndex == 0) && a->ended()) {
            delete a;
            a = nullptr;
        }
    }

    animations.remove(nullptr);

    if (finished()) {
        emit finish();
        return;
    }

    for (auto it = animations.begin(); it != animations.end(); ++it) {
        (*it)->groupIndex = std::count_if(animations.begin(), it, [&it](Animation *a) { return  (*it)->msg == a->msg; });
    }

    if (animations.empty()) {
        return;
    }

    for (auto &a : animations) {
        if (a->groupIndex == 0) {
            for (auto &i : messageItems) {
                if (i.first == std::make_pair(a->inspector, a->msg)) {
                    i.second->setVisible(false);
                }
            }
            a->advance((1.0f / frameRate) * getQtenv()->opt->animationSpeed * (inHurry ? 5 : 1));
        }
    }
}

void Animator::play() {
    if (!finished()) {
        QEventLoop loop;
        timer.start();
        connect(this, SIGNAL(finish()), &loop, SLOT(quit()));
        loop.exec();
        timer.stop();
    }

    inHurry = false;
}

bool Animator::finished() {
    return std::count_if(animations.begin(), animations.end(), [](Animation *anim) { return anim && !anim->ended(); }) == 0;
}

void Animator::clear() {
    if (!finished()) {
        for (auto a : animations) {
            delete a;
        }
        animations.clear();
        emit finish();
    }
    for (auto i : messageItems) {
        delete i.second;
    }
    messageItems.clear();
}

void Animator::hurry()
{
    if (!finished())
        inHurry = true;
}

void Animator::clearInspector(ModuleInspector *insp) {
    for (auto &anim : animations) {
        if (anim->inspector == insp) {
            delete anim;
            anim = nullptr;
        }
    }
    animations.remove(nullptr);

    for (auto it = messageItems.begin(); it != messageItems.end(); /* blank */ ) {
        if ((*it).first.first == insp) {
            delete (*it).second;
            messageItems.erase(it++);
        } else {
            ++it;
        }
    }
}


void Animation::setToTime(float time) {
    this->time = time;
    float t = time/duration;

    if (ended()) {
        if (mode != ANIM_BEGIN || direction == DIR_ASCENT || direction == DIR_DESCENT) {
            if (connectionItem) connectionItem->setVisible(false);
            if (messageItem) messageItem->setVisible(false);
        }
        if (type == ANIM_SENDDIRECT && messageItem) {
            messageItem->setPos(dest);
            messageItem->setVisible(true);
        }
    } else {
        if (connectionItem) connectionItem->setVisible(true);
        if (messageItem) messageItem->setVisible(true);
    }

    switch (type) {
    case ANIM_SENDDIRECT:
        if (mode == ANIM_END) {
            messageItem->setVisible((t < 0.2) || (t > 0.4 && t < 0.6) || (t > 0.8 && t < 1));
            break;
        } // if not and END - a.k.a. delivery, we just slide
    case ANIM_ON_CONN:
        messageItem->setPos((1.0f - t) * src + t * dest);
        break;
    case ANIM_METHODCALL:
        connectionItem->setDashOffset(-t * 20);
        break;
    }
}

void Animation::advance(float delta) {
    // std::min so messages won't go past their destination
    setToTime(std::min(duration, time + delta));
}

bool Animation::ended() {
    return time >= duration;
}

// this is only needed for debugging
QString Animation::info()
{
    QString typeString;
    switch (type) {
    case ANIM_ON_CONN: typeString = "on conn"; break;
    case ANIM_METHODCALL: typeString = "metdhodcall"; break;
    case ANIM_SENDDIRECT: typeString = "senddirect"; break;
    }

    QString directionString;
    switch (direction) {
    case DIR_ASCENT: directionString = "ascent"; break;
    case DIR_DESCENT: directionString = "descent"; break;
    case DIR_HORIZ: directionString = "horiz"; break;
    }

    QString modeString;
    switch (mode) {
    case ANIM_BEGIN: modeString = "begin"; break;
    case ANIM_END: modeString = "end"; break;
    case ANIM_THROUGH: modeString = "through"; break;
    }
    return typeString + " " + directionString + " " + modeString + " \"" + text + "\"";

    /*

        QString text;
        cMessage *msg = nullptr;

        float time = 0;
        float duration;
        QGraphicsEllipseItem *dot = nullptr;
        QGraphicsLineItem *line = nullptr;
        GraphicsLayer *layer;

        QPointF src;
        QPointF dest;
        void setToTime(float time);
        void advance(float delta);
        bool ended();

        int groupIndex = 0;
    */
}

Animation::Animation(ModuleInspector *insp, Animation::AnimType type, AnimDirection direction, SendAnimMode mode, QPointF src, QPointF dest, cMessage *msg, const QString &text)
    : type(type), direction(direction), mode(mode), text(text), msg(msg), inspector(insp), src(src), dest(dest)
{
    auto layer = insp->getAnimationLayer();

    if (type != ANIM_ON_CONN) {
        connectionItem = new ConnectionItem(layer);
        connectionItem->setSource(src);
        connectionItem->setDestination(dest);
        connectionItem->setColor((type == ANIM_METHODCALL) ? "red" : "blue");
        connectionItem->setLineStyle(Qt::DashLine);
    }

    if (type != ANIM_METHODCALL) {
        messageItem = new MessageItem(layer);
        MessageItemUtil::setupFromDisplayString(messageItem, msg, insp->getImageSizeFactor());
        messageItem->setVisible(false);
        messageItem->setPos(src);
    }

    auto delta = dest - src;

               // if SENDDIRECT is in END, that's the flashing, so make it a bit quicker
    duration = (type == ANIM_SENDDIRECT) ? ((mode == ANIM_END) ? 0.5 : 1) :
               // at most 1 sec, otherwise 100 pixels/sec
               (type == ANIM_ON_CONN) ? std::min(std::sqrt(delta.x() * delta.x() + delta.y() * delta.y()) * 0.01, 1.0) :
               // method calls are fixed duration
               0.5;
}

Animation::Animation(ModuleInspector *insp, Animation::AnimType type, Animation::AnimDirection direction, const QPointF &src, const QPointF &dest, cMessage *msg, const QString &text)
    :Animation(insp, type, direction, ANIM_BEGIN, src, dest, msg, text) {
    // the delegated ctor does the job
}

Animation::~Animation() {
    delete connectionItem;
    delete messageItem;
}



void Animator::animateMethodcallAscent(ModuleInspector *insp, cModule *srcSubmod, const char *methodText)
{
    auto start = insp->getSubmodCoords(srcSubmod);
    QPointF end(start.x() + start.y() / 4, 0);
    addAnimation(new Animation(insp, Animation::ANIM_METHODCALL, Animation::DIR_ASCENT, start, end, nullptr, methodText));
}

void Animator::animateMethodcallDescent(ModuleInspector *insp, cModule *destSubmod, const char *methodText)
{
    auto end = insp->getSubmodCoords(destSubmod);
    QPointF start(end.x() - end.y() / 4, 0);
    addAnimation(new Animation(insp, Animation::ANIM_METHODCALL, Animation::DIR_DESCENT, start, end, nullptr, methodText));
}

void Animator::animateMethodcallHoriz(ModuleInspector *insp, cModule *srcSubmod, cModule *destSubmod, const char *methodText)
{
    auto start = insp->getSubmodCoords(srcSubmod);
    auto end = insp->getSubmodCoords(destSubmod);
    addAnimation(new Animation(insp, Animation::ANIM_METHODCALL, Animation::DIR_HORIZ, start, end, nullptr, methodText));
}

void Animator::animateSendOnConn(ModuleInspector *insp, cGate *srcGate, cMessage *msg, SendAnimMode mode)
{
    cModule *mod = srcGate->getOwnerModule();
    cGate *destGate = srcGate->getNextGate();

    // check if this is a two way connection (an other connection is pointing back
    // to the this gate's pair from the next gate's pair)
    bool twoWayConnection = false;
    // check if this gate is really part of an in/out gate pair
    // gate      o-------------------->o dest_gate
    // gate_pair o<--------------------o dest_gate_pair
    if (srcGate->getNameSuffix()[0]) {
        const cGate *gatePair = mod->gateHalf(srcGate->getBaseName(),
                    srcGate->getType() == cGate::INPUT ? cGate::OUTPUT : cGate::INPUT,
                    srcGate->isVector() ? srcGate->getIndex() : -1);

        if (destGate->getNameSuffix()[0]) {
            const cGate *destGatePair = destGate->getOwnerModule()->gateHalf(destGate->getBaseName(),
                        destGate->getType() == cGate::INPUT ? cGate::OUTPUT : cGate::INPUT,
                        destGate->isVector() ? destGate->getIndex() : -1);
            twoWayConnection = destGatePair == gatePair->getPreviousGate();
        }
    }

    auto src = mod;
    auto dest = destGate->getOwnerModule();
    QLineF connLine = insp->getConnectionLine(srcGate);

    QPointF srcPos = connLine.p1();
    QPointF destPos = connLine.p2();

    if (mode == ANIM_END) {
        srcPos = connLine.p2();
        destPos = insp->getSubmodCoords(dest);
    }

    addAnimation(new Animation(insp, Animation::ANIM_ON_CONN, Animation::DIR_HORIZ,
                               mode, srcPos, destPos, msg));
}

void Animator::animateSenddirectAscent(ModuleInspector *insp, cModule *srcSubmod, cMessage *msg)
{
    auto start = insp->getSubmodCoords(srcSubmod);
    QPointF end(start.x() + start.y() / 4, 0);
    addAnimation(new Animation(insp, Animation::ANIM_SENDDIRECT, Animation::DIR_ASCENT, start, end, msg));
}

void Animator::animateSenddirectDescent(ModuleInspector *insp, cModule *destSubmod, cMessage *msg, SendAnimMode mode)
{
    auto end = insp->getSubmodCoords(destSubmod);
    QPointF start(end.x() - end.y() / 4, 0);
    addAnimation(new Animation(insp, Animation::ANIM_SENDDIRECT, Animation::DIR_DESCENT, mode, start, end, msg));
}

void Animator::animateSenddirectHoriz(ModuleInspector *insp, cModule *srcSubmod, cModule *destSubmod, cMessage *msg, SendAnimMode mode)
{
    auto start = insp->getSubmodCoords(srcSubmod);
    auto end = insp->getSubmodCoords(destSubmod);
    addAnimation(new Animation(insp, Animation::ANIM_SENDDIRECT, Animation::DIR_HORIZ, mode, start, end, msg));
}

void Animator::animateSenddirectDelivery(ModuleInspector *insp, cModule *destSubmod, cMessage *msg)
{
    auto end = insp->getSubmodCoords(destSubmod);
    addAnimation(new Animation(insp, Animation::ANIM_SENDDIRECT, Animation::DIR_HORIZ, ANIM_END, end, end, msg));
}

} // namespace qtenv
} // namespace omnetpp
