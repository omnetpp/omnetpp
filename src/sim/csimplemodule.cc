//======================================================================
//  CSIMPLEMODULE.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//   Member functions of
//    cSimpleModule   : base for simple module objects
//
//  Author: Andras Varga
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
#include "omnetpp/csimulation.h"
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

bool cSimpleModule::stackCleanupRequested;
cSimpleModule *cSimpleModule::afterCleanupTransferTo;

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
        fprintf(stderr, "INTERNAL ERROR: switch to the fiber of a module already terminated");
        abort();
    }

    // The starter message should be the same as the timeoutmsg member of
    // cSimpleModule. If not, then something is wrong...
    cMessage *starter = simulation->msgForActivity;
    if (starter != mod->timeoutMessage) {
        // hand exception to cSimulation::transferTo() and switch back
        mod->setFlag(FL_ISTERMINATED, true);
        mod->setFlag(FL_STACKALREADYUNWOUND, true);
        simulation->exception = new cRuntimeError("scheduleStart() should have been called for dynamically created module '%s'", mod->getFullPath().c_str());
        simulation->transferToMain();
        fprintf(stderr, "INTERNAL ERROR: switch to the fiber of a module already terminated");
        abort();
    }

    // rename message
    starter->setKind(MK_TIMEOUT);
    char buf[24];
    sprintf(buf, "timeout-%d", mod->getId());
    starter->setName(buf);

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
        fprintf(stderr, "INTERNAL ERROR: switch to the fiber of a module already terminated");
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
        fprintf(stderr, "INTERNAL ERROR: switch to the fiber of a module already terminated");
        abort();
    }
    else {
        // Some exception (likely cRuntimeError, cTerminationException, or
        // cDeleteModuleException) occurred within the activity() function.
        // Pass this exception to the main coroutine so that it can be displayed as
        // an error dialog or the like.
        simulation->exception = exception;
        simulation->transferToMain();
        fprintf(stderr, "INTERNAL ERROR: switch to the fiber of a module already terminated");
        abort();
    }
}

// legacy constructor, only for backwards compatiblity; first two args are unused
cSimpleModule::cSimpleModule(const char *, cModule *, unsigned stackSize)
{
    coroutine = nullptr;
    setFlag(FL_USESACTIVITY, stackSize != 0);
    setFlag(FL_ISTERMINATED, false);
    setFlag(FL_STACKALREADYUNWOUND, false);

    // for an activity() module, timeoutmsg will be created in scheduleStart()
    // which must always be called
    timeoutMessage = nullptr;

    if (usesActivity())
    {
       // setup coroutine, allocate stack for it
       coroutine = new cCoroutine;
       if (!coroutine->setup(cSimpleModule::activate, this, stackSize+getEnvir()->getExtraStackForEnvir()))
           throw cRuntimeError("Cannot create coroutine with %d+%d bytes of stack space for module '%s' -- "
                               "see Manual for hints on how to increase the number of coroutines that can be created, "
                               "or rewrite modules to use handleMessage() instead of activity()",
                               stackSize, getEnvir()->getExtraStackForEnvir(), getFullPath().c_str());
    }
}

cSimpleModule::cSimpleModule(unsigned stackSize)
{
    coroutine = nullptr;
    setFlag(FL_USESACTIVITY, stackSize != 0);
    setFlag(FL_ISTERMINATED, false);
    setFlag(FL_STACKALREADYUNWOUND, false);

    // for an activity() module, timeoutmsg will be created in scheduleStart()
    // which must always be called
    timeoutMessage = nullptr;

    if (usesActivity())
    {
       // setup coroutine, allocate stack for it
       coroutine = new cCoroutine;
       if (!coroutine->setup(cSimpleModule::activate, this, stackSize+getEnvir()->getExtraStackForEnvir()))
           throw cRuntimeError("Cannot create coroutine with %d+%d bytes of stack space for module '%s' -- "
                               "see Manual for hints on how to increase the number of coroutines that can be created, "
                               "or rewrite modules to use handleMessage() instead of activity()",
                               stackSize, getEnvir()->getExtraStackForEnvir(), getFullPath().c_str());
    }
}

cSimpleModule::~cSimpleModule()
{
    if (getSimulation()->getContext() == this)
        // NOTE: subclass destructors will not be called, but the simulation will stop anyway
        throw cRuntimeError(this, "Cannot delete itself, only via deleteModule()");

    if (usesActivity()) {
        // clean up user's objects on coroutine stack by forcing an exception inside the coroutine
        if ((flags & FL_STACKALREADYUNWOUND) == 0) {
            //FIXME: check this is OK for brand new modules too (no transferTo() yet)
            stackCleanupRequested = true;
            afterCleanupTransferTo = getSimulation()->getActivityModule();
            ASSERT(!afterCleanupTransferTo || afterCleanupTransferTo->usesActivity());
            getSimulation()->transferTo(this);
            stackCleanupRequested = false;
        }

        // delete timeoutmsg if not currently scheduled (then it'll be deleted by message queue)
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

void cSimpleModule::deleteModule()
{
    // if a coroutine wants to delete itself, that has to be handled from
    // another coroutine (i.e. from main). Control is passed there by
    // throwing an exception that gets transferred to the main coroutine
    // by activate(), and handled in cSimulation::transferTo().
    // execution must be immediately suspended in activity() and handleMessage()
    // if deleteModule() is called (i.e. we are deleting ourselves)
    // the exception will be handled in the executeEvent loop and the module
    // will be deleted from the globalContext
    if (getSimulation()->getContextModule() == this)
        throw cDeleteModuleException();

    // else fall back to the base class implementation
    cModule::deleteModule();
}

#define TRY(code, msgprefix)    try { code; } catch (cRuntimeError& e) { e.prependMessage(msgprefix); throw; }

void cSimpleModule::sendDelayed(cMessage *msg, simtime_t delay, const char *gateName, int gateIndex)
{
    cGate *outGate;
    TRY(outGate = gate(gateName, gateIndex), "send()/sendDelayed()");
    sendDelayed(msg, delay, outGate);
}

void cSimpleModule::sendDelayed(cMessage *msg, simtime_t delay, int gateId)
{
    cGate *outGate;
    TRY(outGate = gate(gateId), "send()/sendDelayed()");
    sendDelayed(msg, delay, outGate);
}

void cSimpleModule::sendDelayed(cMessage *msg, simtime_t delay, cGate *outGate)
{
    // error checking:
    if (outGate == nullptr)
        throw cRuntimeError("send()/sendDelayed(): Gate pointer is nullptr");
    if (outGate->getType() == cGate::INPUT)
        throw cRuntimeError("send()/sendDelayed(): Cannot send via an input gate ('%s')", outGate->getFullName());
    if (!outGate->getNextGate())  // NOTE: without this error check, msg would become self-message
        throw cRuntimeError("send()/sendDelayed(): Gate '%s' not connected", outGate->getFullName());
    if (outGate->getPreviousGate())
        throw cRuntimeError("send()/sendDelayed(): Gate '%s' is not the start of a connection path (path starts at gate %s)",
                outGate->getFullName(), outGate->getPathStartGate()->getFullPath().c_str());
    if (msg == nullptr)
        throw cRuntimeError("send()/sendDelayed(): Message pointer is nullptr");
    if (msg->getOwner() != this) {
        bool inFES = msg->getOwner() == getSimulation()->getFES();
        if (this != getSimulation()->getContextModule() && getSimulation()->getContextModule() != nullptr)
            throw cRuntimeError("send()/sendDelayed() of module (%s)%s called in the context of "
                                "module (%s)%s: method called from the latter module "
                                "lacks Enter_Method() or Enter_Method_Silent()? "
                                "Also, if message to be sent is passed from that module, "
                                "you'll need to call take(msg) after Enter_Method() as well",
                                getClassName(), getFullPath().c_str(),
                                getSimulation()->getContextModule()->getClassName(),
                                getSimulation()->getContextModule()->getFullPath().c_str());
        else if (inFES && msg->isSelfMessage() && msg->getArrivalModuleId() == getId())
            throw cRuntimeError("send()/sendDelayed(): Cannot send message (%s)%s, it is "
                                "currently scheduled as a self-message for this module",
                                msg->getClassName(), msg->getName());
        else if (inFES && msg->isSelfMessage())
            throw cRuntimeError("send()/sendDelayed(): Cannot send message (%s)%s, it is "
                                "currently scheduled as a self-message for ANOTHER module",
                                msg->getClassName(), msg->getName());
        else if (inFES)
            throw cRuntimeError("send()/sendDelayed(): Cannot send message (%s)%s, it is "
                                "currently in scheduled-events, being underway between two modules",
                                msg->getClassName(), msg->getName());
        else
            throw cRuntimeError("send()/sendDelayed(): Cannot send message (%s)%s, "
                                "it is currently contained/owned by (%s)%s",
                                msg->getClassName(), msg->getName(), msg->getOwner()->getClassName(),
                                msg->getOwner()->getFullPath().c_str());
    }
    if (delay < SIMTIME_ZERO)
        throw cRuntimeError("sendDelayed(): Negative delay %s", SIMTIME_STR(delay));

    // set message parameters and send it
    simtime_t delayEndTime = simTime() + delay;
    msg->setSentFrom(this, outGate->getId(), delayEndTime);
    if (msg->isPacket())
        ((cPacket *)msg)->setDuration(SIMTIME_ZERO);

    EVCB.beginSend(msg);
    bool keepMsg = outGate->deliver(msg, delayEndTime);
    if (!keepMsg) {
        delete msg;  // event log for this sending will end with "DM" (DeleteMessage) instead of "ES" (EndSend)
    }
    else {
        EVCB.endSend(msg);
    }
}

void cSimpleModule::sendDirect(cMessage *msg, cModule *mod, const char *gateName, int gateIndex)
{
    sendDirect(msg, SIMTIME_ZERO, SIMTIME_ZERO, mod, gateName, gateIndex);
}

void cSimpleModule::sendDirect(cMessage *msg, cModule *mod, int gateId)
{
    sendDirect(msg, SIMTIME_ZERO, SIMTIME_ZERO, mod, gateId);
}

void cSimpleModule::sendDirect(cMessage *msg, cGate *toGate)
{
    sendDirect(msg, SIMTIME_ZERO, SIMTIME_ZERO, toGate);
}

void cSimpleModule::sendDirect(cMessage *msg, simtime_t propagationDelay, simtime_t duration,
        cModule *mod, const char *gateName, int gateIndex)
{
    if (!mod)
        throw cRuntimeError("sendDirect(): Destination module pointer is nullptr");
    cGate *toGate;
    TRY(toGate = mod->gate(gateName, gateIndex), "sendDirect()");
    sendDirect(msg, propagationDelay, duration, toGate);
}

void cSimpleModule::sendDirect(cMessage *msg, simtime_t propagationDelay, simtime_t duration, cModule *mod, int gateId)
{
    if (!mod)
        throw cRuntimeError("sendDirect(): Destination module pointer is nullptr");
    cGate *toGate;
    TRY(toGate = mod->gate(gateId), "sendDirect()");
    sendDirect(msg, propagationDelay, duration, toGate);
}

void cSimpleModule::sendDirect(cMessage *msg, simtime_t propagationDelay, simtime_t duration, cGate *toGate)
{
    // Note: it is permitted to send to an output gate. It is especially useful
    // with several submodules sending to a single output gate of their parent module.
    if (toGate == nullptr)
        throw cRuntimeError("sendDirect(): Destination gate pointer is nullptr");
    if (toGate->getPreviousGate())
        throw cRuntimeError("sendDirect(): Module must have dedicated gate(s) for receiving via sendDirect()"
                            " (\"from\" side of dest. gate '%s' should NOT be connected)", toGate->getFullPath().c_str());
    if (propagationDelay < SIMTIME_ZERO || duration < SIMTIME_ZERO)
        throw cRuntimeError("sendDirect(): The propagation time and duration parameters cannot be negative");
    if (msg == nullptr)
        throw cRuntimeError("sendDirect(): Message pointer is nullptr");
    if (msg->getOwner() != this) {
        // try to give a meaningful error message
        bool inFES = msg->getOwner() == getSimulation()->getFES();
        if (this != getSimulation()->getContextModule() && getSimulation()->getContextModule() != nullptr)
            throw cRuntimeError("sendDirect() of module (%s)%s called in the context of "
                                "module (%s)%s: method called from the latter module "
                                "lacks Enter_Method() or Enter_Method_Silent()? "
                                "Also, if message to be sent is passed from that module, "
                                "you'll need to call take(msg) after Enter_Method() as well",
                                getClassName(), getFullPath().c_str(),
                                getSimulation()->getContextModule()->getClassName(),
                                getSimulation()->getContextModule()->getFullPath().c_str());
        else if (inFES && msg->isSelfMessage() && msg->getArrivalModuleId()==getId())
            throw cRuntimeError("sendDirect(): Cannot send message (%s)%s, it is "
                                "currently scheduled as a self-message for this module",
                                msg->getClassName(), msg->getName());
        else if (inFES && msg->isSelfMessage())
            throw cRuntimeError("sendDirect(): Cannot send message (%s)%s, it is "
                                "currently scheduled as a self-message for ANOTHER module",
                                msg->getClassName(), msg->getName());
        else if (inFES)
            throw cRuntimeError("sendDirect(): Cannot send message (%s)%s, it is "
                                "currently in scheduled-events, being underway between two modules",
                                msg->getClassName(), msg->getName());
        else
            throw cRuntimeError("sendDirect(): Cannot send message (%s)%s, "
                                "it is currently contained/owned by (%s)%s",
                                msg->getClassName(), msg->getName(), msg->getOwner()->getClassName(),
                                msg->getOwner()->getFullPath().c_str());
    }

    // set message parameters and send it
    msg->setSentFrom(this, -1, simTime());

    EVCB.beginSend(msg);
    if (msg->isPacket())
        ((cPacket *)msg)->setDuration(duration);
    else if (duration != SIMTIME_ZERO)
        throw cRuntimeError("sendDirect(): Cannot send non-packet message (%s)%s when nonzero duration is specified",
                            msg->getClassName(), msg->getName());
    EVCB.messageSendDirect(msg, toGate, propagationDelay, duration);
    bool keepit = toGate->deliver(msg, simTime() + propagationDelay);
    if (!keepit)
        delete msg;  //FIXME problem: tell tkenv somehow that msg has been deleted, otherwise animation will crash
    else
        EVCB.endSend(msg);
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
    EVCB.messageScheduled(msg);
    getSimulation()->insertEvent(msg);
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

void cSimpleModule::arrived(cMessage *msg, cGate *ongate, simtime_t t)
{
    if (isTerminated())
        throw cRuntimeError(E_MODFIN, getFullPath().c_str());
    if (t < simTime())
        throw cRuntimeError("Causality violation: Message '%s' arrival time %s at module '%s' "
                            "is earlier than current simulation time",
                            msg->getName(), SIMTIME_STR(t), getFullPath().c_str());
    msg->setArrival(getId(), ongate->getId());
    bool isStart = ongate->getDeliverOnReceptionStart();
    if (msg->isPacket()) {
        cPacket *pkt = (cPacket *)msg;
        pkt->setReceptionStart(isStart);
        pkt->setArrivalTime(isStart ? t : t + pkt->getDuration());
    }
    else {
        msg->setArrivalTime(t);
    }
    getSimulation()->insertEvent(msg);
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

    cMessage *newMsg = getSimulation()->msgForActivity;

    if (newMsg != timeoutMessage)
        throw cRuntimeError("Message arrived during wait() call ((%s)%s); if this "
                            "should be allowed, use waitAndEnqueue() instead of wait()",
                newMsg->getClassName(), newMsg->getFullName());

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

        cMessage *newMsg = getSimulation()->msgForActivity;

        if (newMsg == timeoutMessage)
            break;
        else
            queue->insert(newMsg);
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

    cMessage *newmsg = getSimulation()->msgForActivity;
    DEBUG_TRAP_IF_REQUESTED;  // MODULE IS ABOUT TO PROCESS THE EVENT YOU REQUESTED TO DEBUG -- SELECT "STEP" IN YOUR DEBUGGER
    return newmsg;
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

    cMessage *newMsg = getSimulation()->msgForActivity;

    if (newMsg == timeoutMessage) {  // timeout expired
        take(timeoutMessage);
        DEBUG_TRAP_IF_REQUESTED;  // MODULE IS ABOUT TO PROCESS THE EVENT YOU REQUESTED TO DEBUG -- SELECT "STEP" IN YOUR DEBUGGER
        return nullptr;
    }
    else {  // message received OK
        take(cancelEvent(timeoutMessage));
        DEBUG_TRAP_IF_REQUESTED;  // MODULE IS ABOUT TO PROCESS THE EVENT YOU REQUESTED TO DEBUG -- SELECT "STEP" IN YOUR DEBUGGER
        return newMsg;
    }
}

//-------------

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

