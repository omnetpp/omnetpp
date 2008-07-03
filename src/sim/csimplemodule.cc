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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>           // sprintf
#include <string.h>          // strcpy
#include <exception>
#include "csimplemodule.h"
#include "cgate.h"
#include "cmessage.h"
#include "csimulation.h"
#include "carray.h"
#include "cmsgpar.h"
#include "cqueue.h"
#include "cenvir.h"
#include "cexception.h"

USING_NAMESPACE


bool cSimpleModule::stack_cleanup_requested;
cSimpleModule *cSimpleModule::after_cleanup_transfer_to;


void cSimpleModule::activate(void *p)
{
    cSimpleModule *mod = (cSimpleModule *)p;

    if (stack_cleanup_requested)
    {
        // module has just been created, but already deleted
        mod->setFlag(FL_ISTERMINATED, true);
        mod->setFlag(FL_STACKALREADYUNWOUND, true);
        if (after_cleanup_transfer_to)
            simulation.transferTo(after_cleanup_transfer_to);
        else
            simulation.transferToMain();
        fprintf(stderr, "INTERNAL ERROR: switch to the fiber of a module already terminated");
        abort();
    }

    // The starter message should be the same as the timeoutmsg member of
    // cSimpleModule. If not, then something is wrong...
    cMessage *starter = simulation.msg_for_activity;
    if (starter!=mod->timeoutmsg)
    {
        // hand exception to cSimulation::transferTo() and switch back
        mod->setFlag(FL_ISTERMINATED, true);
        mod->setFlag(FL_STACKALREADYUNWOUND, true);
        simulation.exception = new cRuntimeError("scheduleStart() should have been called for dynamically created module `%s'", mod->getFullPath().c_str());
        simulation.transferToMain();
        fprintf(stderr, "INTERNAL ERROR: switch to the fiber of a module already terminated");
        abort();
    }

    // rename message
    starter->setKind(MK_TIMEOUT);
    char buf[24];
    sprintf(buf,"timeout-%d", mod->getId());
    starter->setName(buf);

    cException *exception = NULL;
    try
    {
        //
        // call activity(). At this point, initialize() has already been called
        // from cSimulation::startRun(), or manually in the case of dynamically
        // created modules.
        //
        mod->activity();
    }
    catch (cRuntimeError *e) // compat
    {
        // IMPORTANT: No transferTo() in catch blocks! See Note 2 below.
        exception = new cRuntimeError("%s [NOTE: exception was thrown by pointer. "
                                      "In OMNeT++ 4.0+, exceptions have to be thrown by value. "
                                      "Please delete `new' from `throw new ...' in the code]",
                                      e->what());
        delete e;
    }
    catch (cException *e) // compat
    {
        // IMPORTANT: No transferTo() in catch blocks! See Note 2 below.
        exception = new cRuntimeError("%s [NOTE: exception was thrown with pointer. "
                                      "In OMNeT++ 4.0+, exceptions have to be thrown by value. "
                                      "Please delete `new' from `throw new ...' in the code]",
                                      e->what());
        delete e;
    }
    catch (cException& e)
    {
        // IMPORTANT: No transferTo() in catch blocks! See Note 2 below.
        exception = e.dup();
    }
    catch (std::exception& e)
    {
        // IMPORTANT: No transferTo() in catch blocks! See Note 2 below.
        // We need to wrap std::exception into cRuntimeError.
        exception = new cRuntimeError("%s: %s", opp_typename(typeid(e)), e.what());
    }

    /*
     * Note 1: catch(...) is probably not a good idea because makes just-in-time debugging impossible on Windows
     * Note 2: with Visual C++, SwitchToFiber() calls in catch blocks crash mess up exception handling,
     *        see http://forums.microsoft.com/MSDN/ShowPost.aspx?PostID=835791&SiteID=1&mode=1
     */

    // When we get here, the module is already terminated. No further cStackCleanupException
    // will need to be thrown, as the stack has has already been unwound by an exception
    // or by having returned from activity() normally.
    mod->setFlag(FL_ISTERMINATED, true);
    mod->setFlag(FL_STACKALREADYUNWOUND, true);

    if (!exception)
    {
        // Module function terminated normally, without exception. Just mark
        // the module as finished, and transfer to the main coroutine (fiber).
        simulation.transferToMain();
        fprintf(stderr, "INTERNAL ERROR: switch to the fiber of a module already terminated");
        abort();
    }
    else if (dynamic_cast<cStackCleanupException *>(exception))
    {
        // A cStackCleanupException exception has been thrown on purpose,
        // to force stack unwinding in the coroutine (fiber) function, activity().
        // Just transfer back to whoever forced the stack cleanup (the main coroutine
        // or some other simple module) and nothing else to do.
        delete exception;
        if (after_cleanup_transfer_to)
            simulation.transferTo(after_cleanup_transfer_to);
        else
            simulation.transferToMain();  //FIXME turn this into transferTo(NULL)?
        fprintf(stderr, "INTERNAL ERROR: switch to the fiber of a module already terminated");
        abort();
    }
    else
    {
        // Some exception (likely cRuntimeError, cTerminationException, or
        // cDeleteModuleException) occurred within the activity() function.
        // Pass this exception to the main coroutine so that it can be displayed as
        // an error dialog or the like.
        simulation.exception = exception;
        simulation.transferToMain();
        fprintf(stderr, "INTERNAL ERROR: switch to the fiber of a module already terminated");
        abort();
    }
}

// legacy constructor, only for backwards compatiblity; first two args are unused
cSimpleModule::cSimpleModule(const char *, cModule *, unsigned stksize)
{
    coroutine = NULL;
    setFlag(FL_USESACTIVITY, stksize!=0);
    setFlag(FL_ISTERMINATED, false);
    setFlag(FL_STACKALREADYUNWOUND, false);

    // for an activity() module, timeoutmsg will be created in scheduleStart()
    // which must always be called
    timeoutmsg = NULL;

    if (usesActivity())
    {
       // setup coroutine, allocate stack for it
       coroutine = new cCoroutine;
       if (!coroutine->setup(cSimpleModule::activate, this, stksize+ev.getExtraStackForEnvir()))
           throw cRuntimeError("Cannot create coroutine with %d+%d bytes of stack space for module `%s' -- "
                               "see Manual for hints on how to increase the number of coroutines that can be created, "
                               "or rewrite modules to use handleMessage() instead of activity()",
                               stksize,ev.getExtraStackForEnvir(),getFullPath().c_str());
    }
}

cSimpleModule::cSimpleModule(unsigned stksize)
{
    coroutine = NULL;
    setFlag(FL_USESACTIVITY, stksize!=0);
    setFlag(FL_ISTERMINATED, false);
    setFlag(FL_STACKALREADYUNWOUND, false);

    // for an activity() module, timeoutmsg will be created in scheduleStart()
    // which must always be called
    timeoutmsg = NULL;

    if (usesActivity())
    {
       // setup coroutine, allocate stack for it
       coroutine = new cCoroutine;
       if (!coroutine->setup(cSimpleModule::activate, this, stksize+ev.getExtraStackForEnvir()))
           throw cRuntimeError("Cannot create coroutine with %d+%d bytes of stack space for module `%s' -- "
                               "see Manual for hints on how to increase the number of coroutines that can be created, "
                               "or rewrite modules to use handleMessage() instead of activity()",
                               stksize,ev.getExtraStackForEnvir(),getFullPath().c_str());
    }
}

cSimpleModule::~cSimpleModule()
{
    if (simulation.getContext()==this)
        throw cRuntimeError(this, "cannot delete itself, only via deleteModule()");

    if (usesActivity())
    {
        // clean up user's objects on coroutine stack by forcing an exception inside the coroutine
        if ((flags&FL_STACKALREADYUNWOUND)==0)
        {
            //FIXME: check this is OK for brand new modules too (no transferTo() yet)
            stack_cleanup_requested = true;
            after_cleanup_transfer_to = simulation.getActivityModule();
            ASSERT(!after_cleanup_transfer_to || after_cleanup_transfer_to->usesActivity());
            simulation.transferTo(this);
            stack_cleanup_requested = false;
        }

        // delete timeoutmsg if not currently scheduled (then it'll be deleted by message queue)
        if (timeoutmsg && !timeoutmsg->isScheduled())
            delete timeoutmsg;

        // deallocate coroutine
        delete coroutine;
    }

    // deletion of pending messages for this module: not done since 3.1 because it
    // made it very long to clean up large models. Instead, such messages are
    // discarded in cSimulation::selectNextModule() when they're met.

    //for (cMessageHeap::Iterator iter(simulation.msgQueue); !iter.end(); iter++)
    //{
    //    cMessage *msg = iter();
    //    if (msg->getArrivalModuleId() == getId())
    //        delete simulation.msgQueue.get( msg );
    //}
}

std::string cSimpleModule::info() const
{
    std::stringstream out;
    out << "id=" << getId();
    return out.str();
}

void cSimpleModule::forEachChild(cVisitor *v)
{
    cModule::forEachChild(v);
}

void cSimpleModule::setId(int n)
{
    cModule::setId(n);

    if (timeoutmsg)
        timeoutmsg->setArrival(this,n);
}

void cSimpleModule::halt()
{
    if (!usesActivity())
        throw cRuntimeError("halt() can only be invoked from activity()-based simple modules");

    setFlag(FL_ISTERMINATED, true);
    simulation.transferToMain();
    assert(stack_cleanup_requested);
    throw cStackCleanupException();
}

void cSimpleModule::error(const char *fmt...) const
{
    va_list va;
    va_start(va, fmt);
    char buf[256];
    vsprintf(buf,fmt,va);
    va_end(va);

    throw cRuntimeError(eUSER,buf);
}

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
        if (timeoutmsg!=NULL)
            throw cRuntimeError("scheduleStart(): module `%s' already started",getFullPath().c_str());

        // create timeoutmsg, used as internal timeout message
        char buf[24];
        sprintf(buf,"starter-%d", getId());
        timeoutmsg = new cMessage(buf,MK_STARTER);

        // initialize message fields
        timeoutmsg->setSentFrom(NULL, -1, 0);
        timeoutmsg->setArrival(this, -1, t);

        // use timeoutmsg as the activation message; insert it into the FES
        Enter_Method("scheduleStart");
        EVCB.messageScheduled(timeoutmsg);
        simulation.insertMsg(timeoutmsg);
    }

    for (SubmoduleIterator submod(this); !submod.end(); submod++)
        submod()->scheduleStart(t);
}

void cSimpleModule::deleteModule()
{
    if (simulation.getContextModule()==this) //XXX revise. it was getActivityModule(), but it was no good with handleMessage()
    {
        // this module is committing suicide: gotta get outta here, and leave
        // doing simulation.deleteModule(getId()) to whoever catches the exception
        throw cDeleteModuleException();
    }

    // simple case: we're being deleted from main or from another module
    delete this;
}

int cSimpleModule::sendDelayed(cMessage *msg, simtime_t delay, const char *gatename, int gateindex)
{
    cGate *outgate = gate(gatename,gateindex);
    if (outgate==NULL)
       throw cRuntimeError(gateindex<0 ? "send()/sendDelayed(): module has no gate `%s'":
                                         "send()/sendDelayed(): module has no gate `%s[%d]'",
                           gatename, gateindex);
    return sendDelayed(msg, delay, outgate);
}

int cSimpleModule::sendDelayed(cMessage *msg, simtime_t delay, int g)
{
    cGate *outgate = gate(g);
    if (outgate==NULL)
        throw cRuntimeError("send()/sendDelayed(): module has no gate #%d",g);
    return sendDelayed(msg, delay, outgate);
}

int cSimpleModule::sendDelayed(cMessage *msg, simtime_t delay, cGate *outgate)
{
    // error checking:
    if (outgate==NULL)
       throw cRuntimeError("send()/sendDelayed(): gate pointer is NULL");
    if (outgate->getType()==cGate::INPUT)
       throw cRuntimeError("send()/sendDelayed(): cannot send via an input gate (`%s')",outgate->getName());
    if (!outgate->getToGate())  // NOTE: without this error check, msg would become self-message
       throw cRuntimeError("send()/sendDelayed(): gate `%s' not connected",outgate->getFullName());
    if (msg==NULL)
        throw cRuntimeError("send()/sendDelayed(): message pointer is NULL");
    if (msg->getOwner()!=this)
    {
        if (this!=simulation.getContextModule())
            throw cRuntimeError("send()/sendDelayed() of module (%s)%s called in the context of "
                                "module (%s)%s: method called from the latter module "
                                "lacks Enter_Method() or Enter_Method_Silent()? "
                                "Also, if message to be sent is passed from that module, "
                                "you'll need to call take(msg) after Enter_Method() as well",
                                getClassName(), getFullPath().c_str(),
                                simulation.getContextModule()->getClassName(),
                                simulation.getContextModule()->getFullPath().c_str());
        else if (msg->getOwner()==&simulation.msgQueue && msg->isSelfMessage() && msg->getArrivalModuleId()==getId())
            throw cRuntimeError("send()/sendDelayed(): cannot send message (%s)%s, it is "
                                "currently scheduled as a self-message for this module",
                                msg->getClassName(), msg->getName());
        else if (msg->getOwner()==&simulation.msgQueue && msg->isSelfMessage())
            throw cRuntimeError("send()/sendDelayed(): cannot send message (%s)%s, it is "
                                "currently scheduled as a self-message for ANOTHER module",
                                msg->getClassName(), msg->getName());
        else if (msg->getOwner()==&simulation.msgQueue)
            throw cRuntimeError("send()/sendDelayed(): cannot send message (%s)%s, it is "
                                "currently in scheduled-events, being underway between two modules",
                                msg->getClassName(), msg->getName());
        else
            throw cRuntimeError("send()/sendDelayed(): cannot send message (%s)%s, "
                                "it is currently contained/owned by (%s)%s",
                                msg->getClassName(), msg->getName(), msg->getOwner()->getClassName(),
                                msg->getOwner()->getFullPath().c_str());
    }
    if (delay < 0)
        throw cRuntimeError("sendDelayed(): negative delay %s", SIMTIME_STR(delay));

    // set message parameters and send it
    simtime_t delayEndTime = simTime()+delay;
    msg->setSentFrom(this, outgate->getId(), delayEndTime);

    EVCB.beginSend(msg);
    bool keepit = outgate->deliver(msg, delayEndTime);
    if (!keepit)
    {
        delete msg; //FIXME problem: tell tkenv somehow that msg has been deleted, otherwise animation will crash
    }
    else
    {
        EVCB.messageSent_OBSOLETE(msg); //FIXME obsolete
        EVCB.endSend(msg);
    }
    return 0;
}

int cSimpleModule::sendDirect(cMessage *msg, simtime_t propdelay, cModule *mod, int g)
{
    return sendDirect(msg, propdelay, SIMTIME_ZERO, mod, g);
}

int cSimpleModule::sendDirect(cMessage *msg, simtime_t propdelay,
                              cModule *mod, const char *gatename, int gateindex)
{
    return sendDirect(msg, propdelay, SIMTIME_ZERO, mod, gatename, gateindex);
}

int cSimpleModule::sendDirect(cMessage *msg, simtime_t propdelay, cGate *togate)
{
    return sendDirect(msg, propdelay, SIMTIME_ZERO, togate);
}

int cSimpleModule::sendDirect(cMessage *msg, simtime_t propdelay, simtime_t transmdelay, cModule *mod, int g)
{
    cGate *togate = mod->gate(g);
    if (togate==NULL)
        throw cRuntimeError("sendDirect(): module `%s' has no gate #%d", mod->getFullPath().c_str(),g);

    return sendDirect(msg, propdelay, togate);
}

int cSimpleModule::sendDirect(cMessage *msg, simtime_t propdelay, simtime_t transmdelay,
                              cModule *mod, const char *gatename, int gateindex)
{
    if (!mod)
        throw cRuntimeError("sendDirect(): module ptr is NULL");
    cGate *togate = mod->gate(gatename,gateindex);
    if (togate==NULL)
        throw cRuntimeError(gateindex<0 ? "sendDirect(): module `%s' has no gate `%s'":
                                          "sendDirect(): module `%s' has no gate `%s[%d]'",
                            mod->getFullPath().c_str(),gatename,gateindex);
    return sendDirect(msg, propdelay, togate);
}

int cSimpleModule::sendDirect(cMessage *msg, simtime_t propdelay, simtime_t transmdelay, cGate *togate)
{
    // Note: it is permitted to send to an output gate. It is especially useful
    // with several submodules sending to a single output gate of their parent module.
    if (togate==NULL)
        throw cRuntimeError("sendDirect(): destination gate pointer is NULL");
    if (togate->getFromGate())
        throw cRuntimeError("sendDirect(): module must have dedicated gate(s) for receiving via sendDirect()"
                            " (\"from\" side of dest. gate `%s' should NOT be connected)",togate->getFullPath().c_str());
    if (propdelay<0 || transmdelay<0)
        throw cRuntimeError("sendDirect(): propagation and transmission delay cannot be negative");
    if (msg==NULL)
        throw cRuntimeError("sendDirect(): message pointer is NULL");
    if (msg->getOwner()!=this)
    {
        if (this!=simulation.getContextModule())
            throw cRuntimeError("sendDirect() of module (%s)%s called in the context of "
                                "module (%s)%s: method called from the latter module "
                                "lacks Enter_Method() or Enter_Method_Silent()? "
                                "Also, if message to be sent is passed from that module, "
                                "you'll need to call take(msg) after Enter_Method() as well",
                                getClassName(), getFullPath().c_str(),
                                simulation.getContextModule()->getClassName(),
                                simulation.getContextModule()->getFullPath().c_str());
        else if (msg->getOwner()==&simulation.msgQueue && msg->isSelfMessage() && msg->getArrivalModuleId()==getId())
            throw cRuntimeError("sendDirect(): cannot send message (%s)%s, it is "
                                "currently scheduled as a self-message for this module",
                                msg->getClassName(), msg->getName());
        else if (msg->getOwner()==&simulation.msgQueue && msg->isSelfMessage())
            throw cRuntimeError("sendDirect(): cannot send message (%s)%s, it is "
                                "currently scheduled as a self-message for ANOTHER module",
                                msg->getClassName(), msg->getName());
        else if (msg->getOwner()==&simulation.msgQueue)
            throw cRuntimeError("sendDirect(): cannot send message (%s)%s, it is "
                                "currently in scheduled-events, being underway between two modules",
                                msg->getClassName(), msg->getName());
        else
            throw cRuntimeError("sendDirect(): cannot send message (%s)%s, "
                                "it is currently contained/owned by (%s)%s",
                                msg->getClassName(), msg->getName(), msg->getOwner()->getClassName(),
                                msg->getOwner()->getFullPath().c_str());
    }

    // set message parameters and send it
    msg->setSentFrom(this, -1, simTime());

    EVCB.beginSend(msg);
    EVCB.messageSendDirect(msg, togate, propdelay, transmdelay);
    bool keepit = togate->deliver(msg, simTime()+propdelay);  //XXX NOTE: no +transmdelay! we want to deliver the msg when the *first* bit arrives, not the last one
    if (!keepit)
    {
        delete msg; //FIXME problem: tell tkenv somehow that msg has been deleted, otherwise animation will crash
    }
    else
    {
        EVCB.messageSent_OBSOLETE(msg, togate); //FIXME obsolete
        EVCB.endSend(msg);
    }
    return 0;
}

int cSimpleModule::scheduleAt(simtime_t t, cMessage *msg)
{
    if (msg==NULL)
        throw cRuntimeError("scheduleAt(): message pointer is NULL");
    if (t<simTime())
        throw cRuntimeError(eBACKSCHED, msg->getClassName(), msg->getName(), SIMTIME_DBL(t));
    if (msg->getOwner()!=this)
    {
        if (this!=simulation.getContextModule())
            throw cRuntimeError("scheduleAt() of module (%s)%s called in the context of "
                                "module (%s)%s: method called from the latter module "
                                "lacks Enter_Method() or Enter_Method_Silent()?",
                                getClassName(), getFullPath().c_str(),
                                simulation.getContextModule()->getClassName(),
                                simulation.getContextModule()->getFullPath().c_str());
        else if (msg->getOwner()==&simulation.msgQueue && msg->isSelfMessage() && msg->getArrivalModuleId()==getId())
            throw cRuntimeError("scheduleAt(): message (%s)%s is currently scheduled, "
                                "use cancelEvent() before rescheduling",
                                msg->getClassName(), msg->getName());
        else if (msg->getOwner()==&simulation.msgQueue && msg->isSelfMessage())
            throw cRuntimeError("scheduleAt(): cannot schedule message (%s)%s, it is "
                                "currently scheduled as self-message for ANOTHER module",
                                msg->getClassName(), msg->getName());

         else if (msg->getOwner()==&simulation.msgQueue)
            throw cRuntimeError("scheduleAt(): cannot schedule message (%s)%s, it is "
                                "currently in scheduled-events, being underway between two modules",
                                msg->getClassName(), msg->getName());
        else
            throw cRuntimeError("scheduleAt(): cannot schedule message (%s)%s, "
                                "it is currently contained/owned by (%s)%s",
                                msg->getClassName(), msg->getName(), msg->getOwner()->getClassName(),
                                msg->getOwner()->getFullPath().c_str());
    }

    // set message parameters and schedule it
    msg->setSentFrom(this, -1, simTime());
    msg->setArrival(this, -1, t);
    EVCB.messageSent_OBSOLETE( msg ); //XXX obsolete but needed for Tkenv
    EVCB.messageScheduled(msg);
    simulation.insertMsg(msg);
    return 0;
}

cMessage *cSimpleModule::cancelEvent(cMessage *msg)
{
    // make sure we really have the message and it is scheduled
    if (msg==NULL)
        throw cRuntimeError("cancelEvent(): message pointer is NULL");

    // now remove it from future events and return pointer
    if (msg->isScheduled())
    {
        if (!msg->isSelfMessage())
            throw cRuntimeError("cancelEvent(): message (%s)%s is not a self-message", msg->getClassName(), msg->getFullName());
        simulation.msgQueue.remove(msg);
        EVCB.messageCancelled(msg);
        msg->setPreviousEventNumber(simulation.getEventNumber());
    }

    return msg;
}

void cSimpleModule::cancelAndDelete(cMessage *msg)
{
    if (msg)
        delete cancelEvent(msg);
}

void cSimpleModule::arrived(cMessage *msg, int ongate, simtime_t t)
{
    if (isTerminated())
        throw cRuntimeError(eMODFIN,getFullPath().c_str());
    if (t < simTime())
        throw cRuntimeError("causality violation: message `%s' arrival time %s at module `%s' "
                            "is earlier than current simulation time",
                            msg->getName(), SIMTIME_STR(t), getFullPath().c_str());
    msg->setArrival(this, ongate, t);
    simulation.insertMsg(msg);
}

void cSimpleModule::wait(simtime_t t)
{
    if (!usesActivity())
        throw cRuntimeError(eNORECV);
    if (t<0)
        throw cRuntimeError(eNEGTIME);

    timeoutmsg->setArrivalTime(simTime()+t);
    EVCB.messageScheduled(timeoutmsg);
    simulation.insertMsg(timeoutmsg);

    simulation.transferToMain();
    if (stack_cleanup_requested)
        throw cStackCleanupException();

    cMessage *newmsg = simulation.msg_for_activity;

    if (newmsg!=timeoutmsg)
        throw cRuntimeError("message arrived during wait() call ((%s)%s); if this "
                            "should be allowed, use waitAndEnqueue() instead of wait()",
                            newmsg->getClassName(), newmsg->getFullName());
}

void cSimpleModule::waitAndEnqueue(simtime_t t, cQueue *queue)
{
    if (!usesActivity())
        throw cRuntimeError(eNORECV);
    if (t<0)
        throw cRuntimeError(eNEGTIME);
    if (!queue)
        throw cRuntimeError("waitAndEnqueue(): queue pointer is NULL");

    timeoutmsg->setArrivalTime(simTime()+t);
    EVCB.messageScheduled(timeoutmsg);
    simulation.insertMsg(timeoutmsg);

    for(;;)
    {
        simulation.transferToMain();
        if (stack_cleanup_requested)
            throw cStackCleanupException();

        cMessage *newmsg = simulation.msg_for_activity;

        if (newmsg==timeoutmsg)
            break;
        else
            queue->insert(newmsg);
    }
}

//-------------

cMessage *cSimpleModule::receive()
{
    if (!usesActivity())
        throw cRuntimeError(eNORECV);

    simulation.transferToMain();
    if (stack_cleanup_requested)
        throw cStackCleanupException();

    cMessage *newmsg = simulation.msg_for_activity;
    return newmsg;
}

cMessage *cSimpleModule::receive(simtime_t t)
{
    if (!usesActivity())
        throw cRuntimeError(eNORECV);
    if (t<0)
        throw cRuntimeError(eNEGTOUT);

    timeoutmsg->setArrivalTime(simTime()+t);
    EVCB.messageScheduled(timeoutmsg);
    simulation.insertMsg(timeoutmsg);

    simulation.transferToMain();
    if (stack_cleanup_requested)
        throw cStackCleanupException();

    cMessage *newmsg = simulation.msg_for_activity;

    if (newmsg==timeoutmsg)  // timeout expired
    {
        take(timeoutmsg);
        return NULL;
    }
    else  // message received OK
    {
        take(cancelEvent(timeoutmsg));
        return newmsg;
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
    throw cTerminationException(eENDSIM);
}

bool cSimpleModule::snapshot(cOwnedObject *object, const char *label)
{
    return simulation.snapshot(object, label);
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


