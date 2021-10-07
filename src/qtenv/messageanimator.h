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
#include <QtWidgets/QGraphicsRectItem>
#include "omnetpp/cchannel.h"
#include "omnetpp/cmodule.h"
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

    // cursor in the tree - NOT necessarily the root
    MethodcallAnimation *currentMethodCall = nullptr;

    // to collect the hops between beginSend()/endSend() for a single transmission animation
    struct MessageSendPath {

        struct Hop {
            // for the optional first direct "hop"
            cModule *directSrcModule = nullptr;
            cGate *directDestGate = nullptr;

            // for hops on connections
            cGate *connSrcGate = nullptr;
            bool isLastHop = false; // currently unused
            bool discard = false; // currently unused

            // for both kinds of "hops" (direct or on-conn)
            simtime_t hopStartTime;
            simtime_t propDelay;
            simtime_t transDuration; // only ONE of the hops can have a non-zero of this, and it will apply to all hops!

            // for the optional sendDirect "hops"
            Hop(const simtime_t &startTime, cModule *srcModule, cGate *destGate, simtime_t prop, simtime_t trans) :
              directSrcModule(srcModule), directDestGate(destGate), hopStartTime(startTime), propDelay(prop), transDuration(trans) { }

            // for send hops on connections without channels
            Hop(const simtime_t &startTime, cGate *srcGate, bool isLastHop) :
              connSrcGate(srcGate), isLastHop(isLastHop), hopStartTime(startTime) { }

            // for send hops on connections with channels
            Hop(const simtime_t &startTime, cGate *srcGate, bool isLastHop, simtime_t prop, simtime_t trans, bool discard) :
              connSrcGate(srcGate), isLastHop(isLastHop), discard(discard), hopStartTime(startTime), propDelay(prop), transDuration(trans) { }

            std::string str() const;
        };

        cMessage *msg;
        cMessage *dupMsg = nullptr;

        std::vector<Hop> hops; // also includes the optional first direct "hop"

        explicit MessageSendPath(cMessage *msg) : msg(msg) { }

        void addHop(const Hop& hop) { hops.push_back(hop); }

        // When the "first part" arrived at the destination, NOT counting transmission duration!
        simtime_t getStartArrivalTime();
        void messageDuplicated(cMessage *msg, cMessage *dup);
        void removeMessagePointer(cMessage *msg);
    };

    // non-null if between a beginSend()/endSend() pair, the path hops are collected into this
    MessageSendPath *currentSending = nullptr;

    // to identify the transmissions so we can find them when they need to be updated by another send
    struct MessageSendKey {
        msgid_t messageId = 0;
        txid_t transmissionId = 0;

        int senderModuleId = 0;
        int senderGateId = 0; // this will be -1 when sendDirect() is called
        int arrivalModuleId = 0;
        int arrivalGateId = 0;

        static MessageSendKey fromMessage(cMessage *msg);

        // to make implementing operator < easier
        std::tuple<msgid_t, txid_t, int, int, int, int> asTuple() const {
            return { messageId, transmissionId, senderModuleId, senderGateId, arrivalModuleId, arrivalGateId };
        }

        // just so we can use it as a key for an std::map
        bool operator < (const MessageSendKey& other) const { return asTuple() < other.asTuple(); }

        std::string str() const;
    };
    static const MessageSendKey METHODCALL; // a special "invalid" key instance for the MethodCall animations

    // The non-delivery animations.
    // Additionally, METHODCALL is used as a key for methodcall animations,
    // since they are potentially interleaved.
    OrderedMultiMap<MessageSendKey, Animation *> animations;

    // The delivery animation(s), these take precedence over the rest.
    AnimationSequence *deliveries = nullptr;

    // the static items waiting for delivery on the border or center of the arrival module
    std::map<std::pair<ModuleInspector *, cMessage *>, MessageItem *> messageItems;

    // the red rectangles around the submodule that is expected to execute the next event.
    // The idea is that we (almost) always have exactly one rectangle on the animation
    // layer of all open moduleinspectors, just hide it if it's not needed.
    std::map<ModuleInspector *, QGraphicsRectItem *> nextEventMarkers;

    void clearMessages();

    // Simple utility function to deduplicate the delivery functions.
    void addDeliveryAnimation(cMessage *msg, cModule *showIn, DeliveryAnimation *anim);
    void cutUpdatedPacketAnimation(cPacket *packetUpdate);

    cModule *markedModule = nullptr; // the next event marker will be drawn around this, and its parents. none if nullptr

    MethodcallAnimation *getCurrentMethodCallRoot() const;

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

    void updateNextEventMarkers();
    void updateAnimations();
    void redrawMessages();
    void skipCurrentHoldingAnims();

    // These two must always be called in a regular tree pattern!
    void methodcallBegin(cComponent *fromComp, cComponent *toComp, const char *methodText, bool silent);
    void methodcallEnd();

    void beginSend(cMessage *msg, const SendOptions& options);
    void sendDirect(cMessage *msg, cModule *srcModule, cGate *destGate, const cChannel::Result& result);
    void sendHop(cMessage *msg, cGate *srcGate, bool isLastHop);
    void sendHop(cMessage *msg, cGate *srcGate, bool isLastHop, const cChannel::Result& result);
    void endSend(cMessage *msg);

    void deliveryDirect(cMessage *msg);
    void delivery(cMessage *msg);

    // see Animation::willAnimate
    bool willAnimate(cMessage *msg);

    void setMarkedModule(cModule *mod);
    cModule *getMarkedModule() const {return markedModule;}

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

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_MESSAGEANIMATOR_H
