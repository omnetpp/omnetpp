//==========================================================================
//  messageanimations.cc - part of
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

#include "messageanimations.h"
#include "qtenv.h"
#include "moduleinspector.h"

namespace omnetpp {
namespace qtenv {

MethodcallAnimation::MethodcallAnimation(ModuleInspector *insp, cModule *srcMod, cModule *destMod, const char *text)
    : SimpleAnimation(insp, QPointF(), QPointF(), getQtenv()->opt->methodCallAnimDuration / 1000.0),
    srcMod(srcMod), destMod(destMod), text(text)
{
    if (srcMod)
        src = insp->getSubmodCoords(srcMod);
    if (destMod)
        dest = insp->getSubmodCoords(destMod);

    if (!srcMod)
        src = QPointF(dest.x() - dest.y() / 4, 0);
    if (!destMod)
        dest = QPointF(src.x() + src.y()  / 4, 0);

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

QString MethodcallAnimation::str() const {
    return QString("MethodCall '") + text + "' from "
            + (srcMod ? srcMod->getFullName() : "NULL") + " to " +  (destMod ? destMod->getFullName() : "NULL");
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
    messageItem->setData(ITEMDATA_COBJECT, QVariant());
}

void SendAnimation::begin()
{
    messageItem->setVisible(true);
    messageItem->setPos(src);

    SimpleAnimation::begin();

    // this will hide the corresponding static message
    // item if this anim is a delivery
    getQtenv()->getMessageAnimator()->redrawMessages();
}



void SendAnimation::cleanup()
{
    messageItem->setVisible(false);
    SimpleAnimation::cleanup();
}

SendOnConnAnimation::SendOnConnAnimation(ModuleInspector *insp, SendAnimMode mode, const QPointF& src, const QPointF& dest, cMessage *msg)
    : SendAnimation(insp, mode, src, dest, msg, 0)  // duration calculated below
{
    auto delta = (dest - src) / insp->getZoomFactor();
    // at most 1 sec, otherwise 100 "units"/sec
    duration = std::min(std::sqrt(delta.x() * delta.x() + delta.y() * delta.y()) * 0.01, 1.0);
}

QString SendOnConnAnimation::str() const {
    return QString("SendOnConn '") + ((msg && mode != ANIM_END) ? msg->getName() : "NULL") + "' mode "
            + (mode == ANIM_BEGIN ? "begin" : mode == ANIM_END ? "end" : mode == ANIM_THROUGH ? "through" : "INVALID")
            + " in " + inspector->QWidget::windowTitle();
}

void SendAnimation::end()
{
    SimpleAnimation::end();

    messageItem->setVisible(false);

    // this will put the static message item
    // in place of the animated one
    getQtenv()->getMessageAnimator()->redrawMessages();
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

void SendDirectAnimation::init() { SendAnimation::init(); connectionItem->setVisible(getQtenv()->opt->showSendDirectArrows); }

void SendDirectAnimation::setProgress(float t)
{
    if (dir == DIR_DELIVERY)
        messageItem->setVisible((t < 0.2) || (t > 0.4 && t < 0.6) || (t > 0.8));
    else
        SendAnimation::setProgress(t);
}

QString SendDirectAnimation::str() const {
    return QString("SendDirect '") + ((msg && dir != DIR_DELIVERY) ? msg->getName() : "NULL") + "' mode "
            + (mode == ANIM_BEGIN ? "begin" : mode == ANIM_END ? "end" : mode == ANIM_THROUGH ? "through" : "INVALID") + " "
            + (dir == DIR_ASCENT ? "ascent" : dir == DIR_DESCENT ? "descent" : dir == DIR_HORIZ ? "horiz" : dir == DIR_DELIVERY ? "delivery" : "INVALID");
}



void AnimationGroup::end() {
    if (!(flags & DEFER_PARTS_CLEANUP))
        for (auto p : parts)
            p->cleanup();
    Animation::end();
}

void AnimationGroup::cleanup() {
    if (flags & DEFER_PARTS_CLEANUP)
        for (auto p : parts)
            p->cleanup();
    Animation::cleanup();
}

bool AnimationGroup::willAnimate(cMessage *msg) {
    return std::any_of(parts.begin(), parts.end(),
                       [&msg](Animation *p) {
        return p->willAnimate(msg);
    });
}

void AnimationGroup::prune() {
    for (auto& p : parts)
        // for every group part
        if (auto g = dynamic_cast<AnimationGroup *>(p)) {
            if (g->isEmpty()) { // if it is empty, throw it right out
                delete p;
                p = nullptr;  // the nullptrs will be erased from the vector below
            }
            else // otherwise keep it, but prune it
                g->prune();
        }
    // erasing the nullptrs introduced above
    parts.erase(std::remove(parts.begin(), parts.end(), nullptr), parts.end());
}

bool AnimationGroup::isEmpty() {
    return std::all_of(parts.begin(), parts.end(),
                       [](Animation *p) {
        return p->isEmpty();
    });
}

void AnimationGroup::begin() {
    if (!(flags & DEFER_PARTS_INIT))
        for (auto p : parts)
            p->init();
    Animation::begin();
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

QString AnimationGroup::str() const {
    QString result = "AnimationGroup of " + QString::number(parts.size()) + " parts";
    for (Animation *a : parts)
        result += "\n    - " + a->str().replace("\n", "\n      ");
    return result;
}

float SequentialAnimation::getTime() const {
    float t = 0;

    for (size_t i = 0; i < currentPart; ++i)
        t += parts[i]->getDuration();
    if (currentPart < parts.size())
        t += parts[currentPart]->getTime();
    return t;
}

float SequentialAnimation::getDuration() {
    float duration = 0;
    for (auto p : parts)
        duration += p->getDuration();
    return duration;
}

void SequentialAnimation::begin() {
    AnimationGroup::begin();
    if (!parts.empty()) {
        if (flags & DEFER_PARTS_INIT)
            parts[0]->init();
        parts[0]->begin();
    }
}

void SequentialAnimation::advance(float delta) {
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
        else
            end();
    }
}

float ParallelAnimation::getDuration() {
    float duration = 0;
    for (auto p : parts)
        duration = std::max(duration, p->getDuration());
    return duration;
}

void ParallelAnimation::begin() {
    AnimationGroup::begin();
    for (auto p : parts) {
        if (flags & DEFER_PARTS_INIT)
            p->init();
        p->begin();
    }
}

void ParallelAnimation::advance(float delta) {
    time += delta;

    float duration = getDuration();
    bool somePlaying = false;
    for (auto p : parts)
        if (p->getState() == PLAYING) {
            p->advance(delta * ((flags & STRETCH_TIME) ? (p->getDuration() / duration) : 1));
            // have to chek again, it might have ended in the advance call
            if (p->getState() == PLAYING)
                somePlaying = true;
        }

    if (!somePlaying)
        end();
}

void SimpleAnimation::advance(float delta) {
    time += delta;

    if (time > duration)
        end();
    else
        setProgress(time / duration);
}

} // namespace qtenv
} // namespace omnetpp
