//==========================================================================
//  messageanimator.h - part of
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

#ifndef __OMNETPP_QTENV_MESSAGEANIMATOR_H
#define __OMNETPP_QTENV_MESSAGEANIMATOR_H

#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <utility>
#include <functional>
#include <QGraphicsRectItem>
#include "omnetpp/simtime_t.h"
#include "qtutil.h"

namespace omnetpp {

class cMessage;

namespace qtenv {

class GraphicsLayer;
class ModuleInspector;
class ConnectionItem;
class MessageItem;
class Animation;
class AnimationSequence;
class MethodcallAnimation;
class DeliveryAnimation;

class QTENV_API MessageAnimator
{
    // This stores the animation speed requests of all running,
    // non-empty, non-holding animations.
    std::map<const Animation*, double> animSpeedMap;

    // The Animations that are currently requesting a hold.
    // If empty, no hold is in effect.
    std::set<const Animation*> holdRequests;

    // cursor in the tree
    MethodcallAnimation *currentMethodCall = nullptr;

    // Used to keep track of time in the current send sequence.
    // -1 if not between a beginSend / endSend pair.
    SimTime lastHopTime = -1;
    // For accumulation of message hop animations between beginSend / endSend.
    AnimationSequence *currentMessageSend = nullptr;

    // The non-delivery animations.
    // Additionally, nullptr is used as a key for methodcall animations,
    // since they are potentially interleaved.
    OrderedMultiMap<cMessage *, Animation *> animations;

    // The delivery animation(s), these take precedence over the rest.
    AnimationSequence *deliveries = nullptr;

    // the static items waiting for delivery on the border or center of the arrival module
    std::map<std::pair<ModuleInspector *, cMessage *>, MessageItem *> messageItems;

    // the red rectangles around the submodule that is expected to execute the next event.
    // The idea is that we (almost) always have exactly one rectangle on the animation
    // layer of all open moduleinspectors, just hide it if it's not needed.
    std::map<ModuleInspector *, QGraphicsRectItem *> nextEventMarkers;

    void clearMessages();

    void updateNextEventMarkers();

    // Simple utility function to deduplicate the delivery functions.
    void addDeliveryAnimation(cMessage *msg, cModule *showIn, DeliveryAnimation *anim);

    cModule *markedModule = nullptr; // the next event marker will be drawn around this, and its parents. none if nullptr

public:

    // Must be used by the animations only!
    void requestHold(const Animation *source) { holdRequests.insert(source); }
    void clearHold(const Animation *source) { holdRequests.erase(source); }
    // DisplayUpdateController can query how the holds are using these:
    bool isHoldActive() { return !holdRequests.empty(); }
    double getAnimationHoldEndTime() const; // ONLY AN ESTIMATE!

    // Must be used by the animations only!
    void setAnimationSpeed(double speed, const Animation *source);
    // DisplayUpdateController can query what the animations requested using this:
    double getAnimationSpeed();

    void updateAnimations();
    void redrawMessages();
    void skipCurrentHoldingAnims();

    // These two must always be called in a regular tree pattern!
    void methodcallBegin(cComponent *fromComp, cComponent *toComp, const char *methodText, bool silent);
    void methodcallEnd();

    void beginSend(cMessage *msg);
    void sendDirect(cMessage *msg, cModule *srcModule, cGate *destGate, simtime_t prop, simtime_t trans);
    void sendHop(cMessage *msg, cGate *srcGate, bool isLastHop);
    void sendHop(cMessage *msg, cGate *srcGate, bool isLastHop, simtime_t prop, simtime_t trans, bool discard);
    void endSend(cMessage *msg);

    void deliveryDirect(cMessage *msg);
    void delivery(cMessage *msg);

    // see Animation::willAnimate
    bool willAnimate(cMessage *msg);

    void setMarkedModule(cModule *mod);

    void clearDeliveries();
    void clear();

    // Makes the animation show itself in an inspector, if applicable.
    void addInspector(ModuleInspector *insp);

    // Adjusts existing animations in an inspector.
    // Called for example when the user zoomed or relayouted.
    void updateInspector(ModuleInspector *insp);

    // Removes any animation and items on the given inspector,
    // so it can be deleted safely. called from the ModuleInspector dtor.
    void clearInspector(ModuleInspector *insp);

    // Called by the LogBuffer when it clones a message. XXX: should be a slot?
    void messageDuplicated(cMessage *msg, cMessage *dup);

    // Removes the cMessage* data from any (static or animated) items
    // that had msg set on them as data(1), so they will not point
    // to an invalid, already deleted message.
    void removeMessagePointer(cMessage *msg);

    ~MessageAnimator() { clear(); }
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_MESSAGEANIMATOR_H
