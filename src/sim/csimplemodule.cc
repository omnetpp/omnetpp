//======================================================================
//  CSIMPLEMODULE.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//   Member functions of
//    cSimpleModule   : base for simple module objects
//
//======================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cassert>
#include <cstdio>  // sprintf
#include <cstring>  // strcpy
#include <exception>
#include "common/commonutil.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cpacket.h"
#include "omnetpp/ccoroutine.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/ccontextswitcher.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/carray.h"
#include "omnetpp/cmsgpar.h"
#include "omnetpp/cqueue.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cexception.h"
#include "omnetpp/platdep/platmisc.h"  // for DEBUG_TRAP

using namespace omnetpp::common;

namespace omnetpp {

#ifdef NDEBUG
#define DEBUG_TRAP_IF_REQUESTED    /*no-op*/
#else
#define DEBUG_TRAP_IF_REQUESTED    { if (getSimulation()->trapOnNextEvent) { getSimulation()->trapOnNextEvent = false; if (getEnvir()->ensureDebugger()) DEBUG_TRAP; } }
#endif

// Note: order of next 2 definitions is important, as SendOptions::DEFAULT uses DURATION_UNSPEC in its ctor call
const simtime_t SendOptions::DURATION_UNSPEC = SimTime::fromRaw(-1);
const SendOptions SendOptions::DEFAULT;

auto& DURATION_UNSPEC = SendOptions::DURATION_UNSPEC; // shorthand for local use

cMessage *cSimpleModule::msgForActivity;
bool cSimpleModule::stackCleanupRequested;
cSimpleModule *cSimpleModule::afterCleanupTransferTo;


std::string SendOptions::str() const
{
    std::stringstream os;
    if (sendDelay != SIMTIME_ZERO)
        os << "sendDelay=" << sendDelay.ustr() << " ";
    if (propagationDelay_ != SIMTIME_ZERO)
        os << "propagationDelay=" << propagationDelay_.ustr() << " ";
    if (duration_ != DURATION_UNSPEC)
        os << "duration=" << duration_.ustr() << " ";
    if (isUpdate)
        os << "isUpdate ";
    if (transmissionId_ != -1)
        os << "transmissionId=" << transmissionId_ << " ";
    if (remainingDuration != DURATION_UNSPEC)
        os << "remainingDuration=" << remainingDuration.ustr() << " ";
    std::string res = os.str();
    if (!res.empty())
        res.resize(res.length()-1); // trim trailing space
    else
        res = "default";
    return res;
}

void cSimpleModule::activate(void *p)
{
    cSimpleModule *mod = (cSimpleModule *)p;
    cSimulation *simulation = cSimulation::getActiveSimulation();

    if (stackCleanupRequested) {
        // module has just been created, but already deleted
        mod->setFlag(FL_ISTERMINATED, true);
        mod->setFlag(FL_STACKALREADYUNWOUND, true);
        if (afterCleanupTransferTo)
            simulation->transferTo(afterCleanupTransferTo);
        else
            simulation->transferToMain();
        fprintf(stderr, "INTERNAL ERROR: Switch to the fiber of a module already terminated, line %d\n", __LINE__);
        abort();
    }

    // The starter message should be the same as the timeoutmsg member of
    // cSimpleModule. If not, then something is wrong...
    cMessage *starterMessage = msgForActivity;
    if (starterMessage != mod->timeoutMessage) {
        // hand exception to cSimulation::transferTo() and switch back
        mod->setFlag(FL_ISTERMINATED, true);
        mod->setFlag(FL_STACKALREADYUNWOUND, true);
        simulation->exception = new cRuntimeError("scheduleStart() should have been called for dynamically created module '%s'", mod->getFullPath().c_str());
        simulation->transferToMain();
        fprintf(stderr, "INTERNAL ERROR: Switch to the fiber of a module already terminated, line %d\n", __LINE__);
        abort();
    }

    // rename message
    starterMessage->setKind(MK_TIMEOUT);
    char buf[24];
    sprintf(buf, "timeout-%d", mod->getId());
    starterMessage->setName(buf);

    cException *exception = nullptr;
    try {
        //
        // call activity(). At this point, initialize() has already been called
        // from cSimulation::startRun(), or manually in the case of dynamically
        // created modules.
        //
        mod->activity();
    }
    catch (cRuntimeError *e) {  // compat
        // IMPORTANT: No transferTo() in catch blocks! See Note 2 below.
        exception = new cRuntimeError("%s [NOTE: exception was thrown by pointer. "
                                      "In OMNeT++ 4.0+, exceptions have to be thrown by value. "
                                      "Please delete 'new' from 'throw new ...' in the code]",
                                      e->what());
        delete e;
    }
    catch (cException *e) {  // compat
        // IMPORTANT: No transferTo() in catch blocks! See Note 2 below.
        exception = new cRuntimeError("%s [NOTE: exception was thrown with pointer. "
                                      "In OMNeT++ 4.0+, exceptions have to be thrown by value. "
                                      "Please delete 'new' from 'throw new ...' in the code]",
                                      e->what());
        delete e;
    }
    catch (cException& e) {
        // IMPORTANT: No transferTo() in catch blocks! See Note 2 below.
        exception = e.dup();
    }
    catch (std::exception& e) {
        // IMPORTANT: No transferTo() in catch blocks! See Note 2 below.
        // We need to wrap std::exception into cRuntimeError.
        exception = new cRuntimeError("%s: %s", opp_typename(typeid(e)), e.what());
    }
    //
    // Note 1: catch(...) is probably not a good idea because makes just-in-time debugging impossible on Windows
    // Note 2: with Visual C++, SwitchToFiber() calls in catch blocks mess up exception handling;
    // see http://forums.microsoft.com/MSDN/ShowPost.aspx?PostID=835791&SiteID=1&mode=1
    //

    // When we get here, the module is already terminated. No further cStackCleanupException
    // will need to be thrown, as the stack has has already been unwound by an exception
    // or by having returned from activity() normally.
    mod->setFlag(FL_ISTERMINATED, true);
    mod->setFlag(FL_STACKALREADYUNWOUND, true);

    if (!exception) {
        // Module function terminated normally, without exception. Just mark
        // the module as finished, and transfer to the main coroutine (fiber).
        simulation->transferToMain();
        fprintf(stderr, "INTERNAL ERROR: Switch to the fiber of a module already terminated, line %d\n", __LINE__);
        abort();
    }
    else if (dynamic_cast<cStackCleanupException *>(exception)) {
        // A cStackCleanupException exception has been thrown on purpose,
        // to force stack unwinding in the coroutine (fiber) function, activity().
        // Just transfer back to whoever forced the stack cleanup (the main coroutine
        // or some other simple module) and nothing else to do.
        delete exception;
        if (afterCleanupTransferTo)
            simulation->transferTo(afterCleanupTransferTo);
        else
            simulation->transferToMain();
        fprintf(stderr, "INTERNAL ERROR: Switch to the fiber of a module already terminated, line %d\n", __LINE__);
        abort();
    }
    else {
        // Some exception (likely cRuntimeError, cTerminationException, or
        // cDeleteModuleException) occurred within the activity() function.
        // Pass this exception to the main coroutine so that it can be displayed as
        // an error dialog or the like.
        simulation->exception = exception;
        simulation->transferToMain();
        fprintf(stderr, "INTERNAL ERROR: Switch to the fiber of a module already terminated, line %d\n", __LINE__);
        abort();
    }
}

cSimpleModule::cSimpleModule(unsigned stackSize)
{
    coroutine = nullptr;
    setFlag(FL_USESACTIVITY, stackSize != 0);
    setFlag(FL_ISTERMINATED, false);
    setFlag(FL_STACKALREADYUNWOUND, false);

    // for an activity() module, timeout msg will be created in scheduleStart()
    // which must always be called
    timeoutMessage = nullptr;

    if (usesActivity()) {
       // setup coroutine, allocate stack for it
       coroutine = new cCoroutine;
       if (!coroutine->setup(cSimpleModule::activate, this, stackSize+getEnvir()->getExtraStackForEnvir()))
           throw cRuntimeError("Cannot create coroutine with %d+%d bytes of stack space for module '%s' -- "
                               "see Manual for hints on how to increase the number of coroutines that can be created, "
                               "or rewrite modules to use handleMessage() instead of activity()",
                               stackSize, getEnvir()->getExtraStackForEnvir(), getFullPath().c_str());
    }

    ASSERT(SendOptions::DEFAULT.duration_ == SendOptions::DURATION_UNSPEC); // catch errors from wrong initialization order
    ASSERT(SendOptions::DEFAULT.remainingDuration == SendOptions::DURATION_UNSPEC);
}

cSimpleModule::~cSimpleModule()
{
    // ensure we are invoked from deleteModule()
    if (componentId !=-1 && (flags&FL_DELETING) == 0) {
        // note: C++ forbids throwing in a destructor, and noexcept(false) is not workable
        getEnvir()->alert(cRuntimeError(this, "Fatal: Direct deletion of a module is illegal, use deleteModule() instead; ABORTING").getFormattedMessage().c_str());
        abort();
    }

    if (usesActivity()) {
        // clean up user's objects on coroutine stack by forcing an exception inside the coroutine
        if ((flags & FL_STACKALREADYUNWOUND) == 0) {
            stackCleanupRequested = true;
            cSimpleModule *currentActivityModule = getSimulation()->getActivityModule();
            assert(this != currentActivityModule); // ensured by deleteModule()
            afterCleanupTransferTo = currentActivityModule;
            getSimulation()->transferTo(this);
            stackCleanupRequested = false;
        }

        // delete timeoutMessage unless it is currently scheduled (if it is, it will be discarded by the scheduler or the FES later)
        if (timeoutMessage && !timeoutMessage->isScheduled())
            delete timeoutMessage;

        // deallocate coroutine
        delete coroutine;
    }

    // Note: deletion of pending events for this module is not done since version 3.1,
    // because it made it very long to clean up large models. Instead, such messages
    // are discarded in the scheduler when they're met.
}

std::string cSimpleModule::str() const
{
    std::stringstream out;
    out << "id=" << getId();
    return out.str();
}

void cSimpleModule::forEachChild(cVisitor *v)
{
    cModule::forEachChild(v);
}

void cSimpleModule::halt()
{
    if (!usesActivity())
        throw cRuntimeError("halt() can only be invoked from activity()-based simple modules");

    setFlag(FL_ISTERMINATED, true);
    getSimulation()->transferToMain();
    assert(stackCleanupRequested);
    throw cStackCleanupException();
}

#define BUFLEN    512

void cSimpleModule::error(const char *fmt...) const
{
    char buf[BUFLEN];
    VSNPRINTF(buf, BUFLEN, fmt);
    throw cRuntimeError(E_USER, buf);
}

#undef BUFLEN

//---------

void cSimpleModule::scheduleStart(simtime_t t)
{
    // Note: Simple modules using handleMessage() don't need starter
    // messages, but nevertheless, we still define scheduleStart()
    // for them. The benefit is that this way code that creates simple
    // modules dynamically doesn't have to know whether the module is
    // using activity() or handleMessage(); the same code (which
    // contains a call to scheduleStart()) can be used for both.
    if (usesActivity()) {
        if (timeoutMessage != nullptr)
            throw cRuntimeError("scheduleStart(): Module '%s' already started", getFullPath().c_str());

        Enter_Method_Silent("scheduleStart()");

        // create timeoutmsg, used as internal timeout message
        char buf[24];
        sprintf(buf, "starter-%d", getId());
        timeoutMessage = new cMessage(buf, MK_STARTER);

        // initialize message fields
        timeoutMessage->setSentFrom(nullptr, -1, SIMTIME_ZERO);
        timeoutMessage->setArrival(getId(), -1, t);

        // use timeoutmsg as the activation message; insert it into the FES
        EVCB.messageScheduled(timeoutMessage);
        getSimulation()->insertEvent(timeoutMessage);
    }

    // delegate to base class for doing the same for submodules
    cModule::scheduleStart(t);
}

#define TRY(code, msgprefix)    try { code; } catch (cRuntimeError& e) { e.prependMessage(msgprefix); throw; }

void cSimpleModule::send(cMessage *msg, const SendOptions& options, cGate *outGate)
{
    if (msg == nullptr)
        throw cRuntimeError("send()/sendDelayed(): Message pointer is nullptr");
    if (msg->getOwner() != this)
        throwNotOwnerOfMessage("send()/sendDelayed()", msg);
    if (outGate == nullptr)
        throw cRuntimeError("send()/sendDelayed(): Gate pointer is nullptr");
    if (outGate->getType() == cGate::INPUT)
        throw cRuntimeError("send()/sendDelayed(): Cannot send via an input gate ('%s')", outGate->getFullName());
    if (!outGate->getNextGate())  // NOTE: without this error check, msg would become self-message
        throw cRuntimeError("send()/sendDelayed(): Gate '%s' not connected", outGate->getFullName());
    if (outGate->getPreviousGate())
        throw cRuntimeError("send()/sendDelayed(): Gate '%s' is not the start of a connection path (path starts at gate %s)",
                outGate->getFullName(), outGate->getPathStartGate()->getFullPath().c_str());
    if (options.sendDelay < SIMTIME_ZERO)
        throw cRuntimeError("send()/sendDelayed(): Negative delay %s", SIMTIME_STR(options.sendDelay));
    if (!options.propagationDelay_.isZero())
        throw cRuntimeError("send()/sendDelayed(): Specifying a propagation delay in the options is only allowed for sendDirect()");

    // set message parameters and send it
    simtime_t delayEndTime = simTime() + options.sendDelay;
    msg->setSentFrom(this, outGate->getId(), delayEndTime);
    if (msg->isPacket()) {
        cPacket *pkt = (cPacket *)msg;
        pkt->setIsUpdate(options.isUpdate);
        pkt->setTransmissionId(options.transmissionId_);
        if (options.isUpdate && options.transmissionId_ == -1)
            throw cRuntimeError("send(): No transmissionId specified in SendOptions for a transmission update");
        pkt->setDuration(SIMTIME_ZERO);
        pkt->setRemainingDuration(SIMTIME_ZERO);
        pkt->clearTxChannelEncountered();
    }
    else {
        if (&options != &SendOptions::DEFAULT && (options.duration_ != DURATION_UNSPEC || options.remainingDuration != DURATION_UNSPEC || options.isUpdate || options.transmissionId_ != -1))
            throw cRuntimeError("send(): (%s)%s is a message, and the duration, remainingDuration, isUpdate "
                                "and transmissionId send options are only allowed on packets",
                                msg->getClassName(), msg->getName());
    }

    EVCB.beginSend(msg, options);
    bool keepMsg = outGate->deliver(msg, options, delayEndTime);
    if (!keepMsg)
        delete msg;  // event log for this sending will end with "DM" (DeleteMessage) instead of "ES" (EndSend)
    else
        EVCB.endSend(msg);
}

cGate *cSimpleModule::resolveSendDirectGate(cModule *mod, int gateId)
{
    if (!mod)
        throw cRuntimeError("sendDirect(): Destination module pointer is nullptr");
    TRY(return mod->gate(gateId), "sendDirect()");
}

cGate *cSimpleModule::resolveSendDirectGate(cModule *mod, const char *gateName, int gateIndex)
{
    if (!mod)
        throw cRuntimeError("sendDirect(): Destination module pointer is nullptr");
    TRY(return mod->gate(gateName, gateIndex), "sendDirect()");
}

SendOptions cSimpleModule::resolveSendDirectOptions(simtime_t propagationDelay, simtime_t duration)
{
    SendOptions options;
    options.propagationDelay(propagationDelay);
    if (!duration.isZero())
        options.duration(duration);
    return options;
}

void cSimpleModule::sendDirect(cMessage *msg, const SendOptions& options, cGate *toGate)
{
    // Note: it is permitted to send to an output gate. It is especially useful
    // with several submodules sending to a single output gate of their parent module.
    if (msg == nullptr)
        throw cRuntimeError("sendDirect(): Message pointer is nullptr");
    if (msg->getOwner() != this)
        throwNotOwnerOfMessage("sendDirect()", msg);
    if (toGate == nullptr)
        throw cRuntimeError("sendDirect(): Destination gate pointer is nullptr");
    if (toGate->getPreviousGate())
        throw cRuntimeError("sendDirect(): Module must have dedicated gate(s) for receiving via sendDirect()"
                            " (\"from\" side of dest. gate '%s' should NOT be connected)", toGate->getFullPath().c_str());
    if (options.sendDelay < SIMTIME_ZERO)
        throw cRuntimeError("sendDirect(): Negative send delay %s", options.sendDelay.ustr().c_str());
    if (options.propagationDelay_ < SIMTIME_ZERO)
        throw cRuntimeError("sendDirect(): Negative propagation delay %s", options.propagationDelay_.ustr().c_str());

    // set message parameters and send it
    msg->setSentFrom(this, -1, simTime());

    EVCB.beginSend(msg, options); // note: records sendDelay and transmissionId

    cChannel::Result result;
    if (msg->isPacket()) {
        cPacket *pkt = (cPacket *)msg;
        bool isUpdate = options.isUpdate;
        pkt->setIsUpdate(isUpdate);
        pkt->setTransmissionId(options.transmissionId_);
        if (options.isUpdate && options.transmissionId_ == -1)
            throw cRuntimeError("sendDirect(): No transmissionId specified in SendOptions for a transmission update");
        bool haveDuration = (options.duration_ != DURATION_UNSPEC);
        bool haveRemainingDuration = (options.remainingDuration != DURATION_UNSPEC);
        simtime_t duration, remainingDuration;

        if (isUpdate) {
            if (haveDuration != haveRemainingDuration)
                throw cRuntimeError("sendDirect(): For transmission update packets, the duration and remainingDuration options may only occur together");
            if (haveDuration) {
                duration = options.duration_;
                remainingDuration = options.remainingDuration;
                if (duration < SIMTIME_ZERO)
                    throw cRuntimeError("sendDirect(): Negative duration %s", duration.ustr().c_str());
                if (remainingDuration < SIMTIME_ZERO || remainingDuration > duration)
                    throw cRuntimeError("sendDirect(): remainingDuration (%s) is negative or larger than duration (%s)", remainingDuration.ustr().c_str(), duration.ustr().c_str());
            }
        }
        else {
            if (haveRemainingDuration)
                throw cRuntimeError("sendDirect(): remainingDuration may only be specified for transmission update packets");
            if (haveDuration) {
                duration = remainingDuration = options.duration_;
                if (duration < SIMTIME_ZERO)
                    throw cRuntimeError("sendDirect(): Negative duration %s", duration.ustr().c_str());
            }
        }

        pkt->setDuration(duration);
        pkt->setRemainingDuration(remainingDuration);
        pkt->setTxChannelEncountered(haveDuration);

        result.duration = duration;
        result.remainingDuration = remainingDuration;
    }
    else {
        if (&options != &SendOptions::DEFAULT && (options.duration_ != DURATION_UNSPEC || options.remainingDuration != DURATION_UNSPEC || options.isUpdate || options.transmissionId_ != -1))
            throw cRuntimeError("sendDirect(): (%s)%s is a message, and the duration, remainingDuration, isUpdate "
                                "and transmissionId send options are only allowed on packets",
                                msg->getClassName(), msg->getName());
    }
    result.delay = options.propagationDelay_;

    EVCB.messageSendDirect(msg, toGate, result);
    bool keepit = toGate->deliver(msg, options, simTime() + options.sendDelay + result.delay);
    if (!keepit)
        delete msg;  // event log for this sending will end with "DM" (DeleteMessage) instead of "ES" (EndSend)
    else
        EVCB.endSend(msg);
}

void cSimpleModule::throwNotOwnerOfMessage(const char *sendOp, cMessage *msg)
{
    // try to give a meaningful error message
    bool inFES = msg->getOwner() == getSimulation()->getFES();
    if (this != getSimulation()->getContextModule() && getSimulation()->getContextModule() != nullptr)
        throw cRuntimeError("%s of module (%s)%s called in the context of "
                            "module (%s)%s: method called from the latter module "
                            "lacks Enter_Method() or Enter_Method_Silent()? "
                            "Also, if message to be sent is passed from that module, "
                            "you'll need to call take(msg) after Enter_Method() as well",
                            sendOp, getClassName(), getFullPath().c_str(),
                            getSimulation()->getContextModule()->getClassName(),
                            getSimulation()->getContextModule()->getFullPath().c_str());
    else if (inFES && msg->isSelfMessage() && msg->getArrivalModuleId()==getId())
        throw cRuntimeError("%s: Cannot send message (%s)%s, it is currently scheduled "
                            "as a self-message for this module",
                            sendOp, msg->getClassName(), msg->getName());
    else if (inFES && msg->isSelfMessage())
        throw cRuntimeError("%s: Cannot send message (%s)%s, it is currently scheduled "
                            "as a self-message for ANOTHER module",
                            sendOp, msg->getClassName(), msg->getName());
    else if (inFES)
        throw cRuntimeError("%s: Cannot send message (%s)%s, it is currently in scheduled-events, "
                            "in transit between two modules",
                            sendOp, msg->getClassName(), msg->getName());
    else
        throw cRuntimeError("%s: Cannot send message (%s)%s, it is currently contained/owned by (%s)%s",
                            sendOp, msg->getClassName(), msg->getName(), msg->getOwner()->getClassName(),
                            msg->getOwner()->getFullPath().c_str());
}

void cSimpleModule::scheduleAt(simtime_t t, cMessage *msg)
{
    if (msg == nullptr)
        throw cRuntimeError("scheduleAt(): Message pointer is nullptr");
    if (t < simTime())
        throw cRuntimeError(E_BACKSCHED, msg->getClassName(), msg->getName(), SIMTIME_DBL(t));
    if (msg->getOwner() != this) {
        bool inFES = msg->getOwner() == getSimulation()->getFES();
        if (this != getSimulation()->getContextModule() && getSimulation()->getContextModule() != nullptr)
            throw cRuntimeError("scheduleAt() of module (%s)%s called in the context of "
                                "module (%s)%s: method called from the latter module "
                                "lacks Enter_Method() or Enter_Method_Silent()?",
                                getClassName(), getFullPath().c_str(),
                                getSimulation()->getContextModule()->getClassName(),
                                getSimulation()->getContextModule()->getFullPath().c_str());
        else if (inFES && msg->isSelfMessage() && msg->getArrivalModuleId() == getId())
            throw cRuntimeError("scheduleAt(): Message (%s)%s is currently scheduled, "
                                "use cancelEvent() before rescheduling",
                                msg->getClassName(), msg->getName());
        else if (inFES && msg->isSelfMessage())
            throw cRuntimeError("scheduleAt(): Cannot schedule message (%s)%s, it is "
                                "currently scheduled as self-message for ANOTHER module",
                                msg->getClassName(), msg->getName());

         else if (inFES)
            throw cRuntimeError("scheduleAt(): Cannot schedule message (%s)%s, it is "
                                "currently in scheduled-events, being underway between two modules",
                                msg->getClassName(), msg->getName());
        else
            throw cRuntimeError("scheduleAt(): Cannot schedule message (%s)%s, "
                                "it is currently contained/owned by (%s)%s",
                                msg->getClassName(), msg->getName(), msg->getOwner()->getClassName(),
                                msg->getOwner()->getFullPath().c_str());
    }

    // set message parameters and schedule it
    msg->setSentFrom(this, -1, simTime());
    msg->setArrival(getId(), -1, t);
    getSimulation()->insertEvent(msg);
    EVCB.messageScheduled(msg);
}

void cSimpleModule::scheduleAfter(simtime_t delay, cMessage *msg)
{
    scheduleAt(simTime() + delay, msg);
}

void cSimpleModule::rescheduleAt(simtime_t t, cMessage *msg)
{
    cancelEvent(msg);
    scheduleAt(t, msg);
}

void cSimpleModule::rescheduleAfter(simtime_t delay, cMessage *msg)
{
    cancelEvent(msg);
    scheduleAfter(delay, msg);
}

cMessage *cSimpleModule::cancelEvent(cMessage *msg)
{
    // make sure we really have the message and it is scheduled
    if (msg == nullptr)
        throw cRuntimeError("cancelEvent(): Message pointer is nullptr");

    // now remove it from future events and return pointer
    if (msg->isScheduled()) {
        if (!msg->isSelfMessage())
            throw cRuntimeError("cancelEvent(): Message (%s)%s is not a self-message", msg->getClassName(), msg->getFullName());
        if (msg->getArrivalModuleId() != getId())
            throw cRuntimeError("cancelEvent(): Cannot cancel another module's self-message");

        getSimulation()->getFES()->remove(msg);
        EVCB.messageCancelled(msg);
        msg->setPreviousEventNumber(getSimulation()->getEventNumber());
    }

    return msg;
}

void cSimpleModule::cancelAndDelete(cMessage *msg)
{
    if (msg)
        delete cancelEvent(msg);
}

void cSimpleModule::arrived(cMessage *msg, cGate *ongate, const SendOptions& options, simtime_t t)
{
    if (isTerminated())
        throw cRuntimeError(E_MODFIN, getFullPath().c_str());
    if (t < simTime())
        throw cRuntimeError("Causality violation: Message '%s' arrival time %s at module '%s' "
                            "is earlier than current simulation time",
                            msg->getName(), SIMTIME_STR(t), getFullPath().c_str());
    msg->setArrival(getId(), ongate->getId());
    if (msg->isPacket()) {
        cPacket *pkt = (cPacket *)msg;
        bool isUpdate = pkt->isUpdate();
        bool deliverImmediately = ongate->getDeliverImmediately();
        bool isTransmission = pkt->getTxChannelEncountered();

        if (!isTransmission && &options != &SendOptions::DEFAULT && (options.duration_ != DURATION_UNSPEC || options.remainingDuration != DURATION_UNSPEC))
            throw cRuntimeError("Error sending (%s)%s: The duration and remainingDuration send options are only allowed when "
                                "sending through a transmission channel",
                                msg->getClassName(), msg->getName());

        if (deliverImmediately || !isTransmission)
            pkt->setArrivalTime(t);
        else {
            pkt->setArrivalTime(t + pkt->getRemainingDuration());
            pkt->setRemainingDuration(SimTime::ZERO);
        }

        if (isUpdate) {
            if (deliverImmediately) {
                if (!supportsTxUpdates())
                    throw cRuntimeError("Transmission update message (%s)%s delivered to a module which is "
                            "not prepared to handle transmission updates ((%s)%s); "
                            "call setTxUpdateSupport(true) in the initialization code of the module "
                            "to turn off this check",
                            msg->getClassName(), msg->getName(), getClassName(), getFullPath().c_str());
            }
            else {
                // Original transmission (or last, now-obsolete update) must still be in the FES, remove it.
                deleteObsoletedTransmissionFromFES(pkt->getTransmissionId(), pkt);
            }
        }
    }
    else {
        msg->setArrivalTime(t);
    }
    getSimulation()->insertEvent(msg);
}

void cSimpleModule::deleteObsoletedTransmissionFromFES(txid_t transmissionId, cPacket *updatePkt)
{
    // Linear search might seem like a poor choice, but alternatives (like maintaining
    // an index) would also affect (slow down) simulations which don't use the updateTx
    // feature at all, and also, deliver-at-end plus updates is a relatively rare combination
    // so it doesn't need to be super optimal.
    cPacket *obsoletedPacket = nullptr;
    cFutureEventSet *fes = getSimulation()->getFES();
    int n = fes->getLength();
    for (int i = 0; i < n; i++) {
        cEvent *event = fes->get(i);
        if (event->isPacket()) {
            cPacket *pkt = (cPacket *)event;
            if (pkt && pkt->getTransmissionId() == transmissionId) {
                obsoletedPacket = pkt;
                break;
            }
        }
    }
    if (!obsoletedPacket)
        throw cRuntimeError("Cannot send transmission update packet (%s)%s with transmissionId=%" PRId64 " "
                            "to a gate with the default (=false) deliverImmediately setting: "
                            "The original packet (or obsoleted update) identified by transmissionId "
                            "was not found in the FES, so it is not possible to replace it with the "
                            "update packet. Perhaps it has already been delivered to the target module. "
                            "Possible solutions include sending the update earlier than the transmission end time, "
                            "adding a propagation delay to the channel, and playing with event priorities "
                            "to force a different event execution order",
                            updatePkt->getClassName(), updatePkt->getName(), transmissionId);
    delete fes->remove(obsoletedPacket);
}

void cSimpleModule::wait(simtime_t t)
{
    if (!usesActivity())
        throw cRuntimeError(E_NORECV);
    if (t < SIMTIME_ZERO)
        throw cRuntimeError(E_NEGTIME);

    timeoutMessage->setArrival(getId(), -1, simTime() + t);
    EVCB.messageScheduled(timeoutMessage);
    getSimulation()->insertEvent(timeoutMessage);

    getSimulation()->transferToMain();
    if (stackCleanupRequested)
        throw cStackCleanupException();

    cMessage *receivedMessage = msgForActivity;

    if (receivedMessage != timeoutMessage)
        throw cRuntimeError("Message arrived during wait() call ((%s)%s); if this "
                            "should be allowed, use waitAndEnqueue() instead of wait()",
                receivedMessage->getClassName(), receivedMessage->getFullName());

    DEBUG_TRAP_IF_REQUESTED;  // MODULE IS ABOUT TO PROCESS THE EVENT YOU REQUESTED TO DEBUG -- SELECT "STEP" IN YOUR DEBUGGER
}

void cSimpleModule::waitAndEnqueue(simtime_t t, cQueue *queue)
{
    if (!usesActivity())
        throw cRuntimeError(E_NORECV);
    if (t < SIMTIME_ZERO)
        throw cRuntimeError(E_NEGTIME);
    if (!queue)
        throw cRuntimeError("waitAndEnqueue(): Queue pointer is nullptr");

    timeoutMessage->setArrival(getId(), -1, simTime() + t);
    EVCB.messageScheduled(timeoutMessage);
    getSimulation()->insertEvent(timeoutMessage);

    for (;;) {
        getSimulation()->transferToMain();
        if (stackCleanupRequested)
            throw cStackCleanupException();

        cMessage *receivedMessage = msgForActivity;
        if (receivedMessage == timeoutMessage)
            break;

        queue->insert(receivedMessage);
    }

    DEBUG_TRAP_IF_REQUESTED;  // MODULE IS ABOUT TO PROCESS THE EVENT YOU REQUESTED TO DEBUG -- SELECT "STEP" IN YOUR DEBUGGER
}

//-------------

cMessage *cSimpleModule::receive()
{
    if (!usesActivity())
        throw cRuntimeError(E_NORECV);

    getSimulation()->transferToMain();
    if (stackCleanupRequested)
        throw cStackCleanupException();

    cMessage *receivedMessage = msgForActivity;
    DEBUG_TRAP_IF_REQUESTED;  // MODULE IS ABOUT TO PROCESS THE EVENT YOU REQUESTED TO DEBUG -- SELECT "STEP" IN YOUR DEBUGGER
    return receivedMessage;
}

cMessage *cSimpleModule::receive(simtime_t t)
{
    if (!usesActivity())
        throw cRuntimeError(E_NORECV);
    if (t < SIMTIME_ZERO)
        throw cRuntimeError(E_NEGTOUT);

    timeoutMessage->setArrival(getId(), -1, simTime() + t);
    EVCB.messageScheduled(timeoutMessage);
    getSimulation()->insertEvent(timeoutMessage);

    getSimulation()->transferToMain();
    if (stackCleanupRequested)
        throw cStackCleanupException();

    cMessage *receivedMessage = msgForActivity;

    if (receivedMessage == timeoutMessage) {  // timeout expired
        take(timeoutMessage);
        DEBUG_TRAP_IF_REQUESTED;  // MODULE IS ABOUT TO PROCESS THE EVENT YOU REQUESTED TO DEBUG -- SELECT "STEP" IN YOUR DEBUGGER
        return nullptr;
    }
    else {  // message received OK
        take(cancelEvent(timeoutMessage));
        DEBUG_TRAP_IF_REQUESTED;  // MODULE IS ABOUT TO PROCESS THE EVENT YOU REQUESTED TO DEBUG -- SELECT "STEP" IN YOUR DEBUGGER
        return receivedMessage;
    }
}

//-------------

void cSimpleModule::doMessageEvent(cMessage *msg)
{
    // switch to the module's context
    simulation->setContext(this);

    // get ownership
    take(msg);

    if (!initialized())
        throw cRuntimeError(this, "Module not initialized (did you forget to invoke "
                                  "callInitialize() for a dynamically created module?)");

    if (usesActivity()) {
        // switch to the coroutine of the module's activity(). We'll get back control
        // when the module executes a receive() or wait() call.
        // If there was an error during simulation, the call will throw an exception
        // (which originally occurred inside activity()).
        msgForActivity = msg;
        simulation->transferTo(this);
    }
    else {
        // YOU ARE ABOUT TO ENTER THE handleMessage() CALL YOU REQUESTED -- SELECT "STEP INTO" IN YOUR DEBUGGER
        DEBUG_TRAP_IF_REQUESTED; handleMessage(msg);
    }
}

void cSimpleModule::activity()
{
    // default thread function
    // the user must redefine this for the module to do anything useful
    throw cRuntimeError("Redefine activity() or specify zero stack size to use handleMessage()");
}

void cSimpleModule::handleMessage(cMessage *)
{
    // handleMessage is an alternative to activity()
    // this is the default version
    throw cRuntimeError("Redefine handleMessage() or specify non-zero stack size to use activity()");
}

//-------------

void cSimpleModule::endSimulation()
{
    throw cTerminationException(E_ENDSIM);
}

void cSimpleModule::snapshot(cObject *object, const char *label)
{
    getSimulation()->snapshot(object ? object : getSimulation(), label);
}

bool cSimpleModule::hasStackOverflow() const
{
    return coroutine ? coroutine->hasStackOverflow() : false;
}

unsigned cSimpleModule::getStackSize() const
{
    return coroutine ? coroutine->getStackSize() : 0;
}

unsigned cSimpleModule::getStackUsage() const
{
    return coroutine ? coroutine->getStackUsage() : 0;
}

}  // namespace omnetpp

