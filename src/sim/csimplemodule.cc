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
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include <string.h>          // strcpy
#include <assert.h>
#include <exception>
#include "csimplemodule.h"
#include "cgate.h"
#include "cmessage.h"
#include "csimul.h"
#include "carray.h"
#include "ctypes.h"
#include "cpar.h"
#include "cenvir.h"
#include "cexception.h"


bool cSimpleModule::stack_cleanup_requested;
cSimpleModule *cSimpleModule::after_cleanup_transfer_to;


void cSimpleModule::activate(void *p)
{
    if (stack_cleanup_requested)
    {
        if (after_cleanup_transfer_to)
            simulation.transferTo(after_cleanup_transfer_to);
        else
            simulation.transferToMain();
        assert(/*invoking transferTo() on an already deleted module?*/ 0);
    }

    cSimpleModule *mod = (cSimpleModule *)p;

    // The starter message should be the same as the timeoutmsg member of
    // cSimpleModule. If not, then something is wrong...
    cMessage *starter = simulation.msgQueue.getFirst();
    if (starter!=mod->timeoutmsg)
    {
        // hand exception to cSimulation::transferTo() and switch back
        simulation.exception = new cException("scheduleStart() should have been called for dynamically created module `%s'", mod->fullPath().c_str());
        simulation.exception_type = 0;
        mod->state = sENDED;

        // The End
        simulation.transferToMain(); // send back exception
        assert(!after_cleanup_transfer_to);
        simulation.transferToMain(); // for stack_cleanup_requested
        assert(/*invoking transferTo() on an already deleted module?*/ 0);
    }

    // rename message
    starter->setKind(MK_TIMEOUT);
    char buf[24];
    sprintf(buf,"timeout-%d", mod->id());
    starter->setName(buf);

    // call activity(). At this point, initialize() has already been called
    // from cSimulation::startRun(), or manually in the case of dynamically
    // created modules.
    try
    {
        mod->activity();
        mod->state = sENDED;
    }
    catch (cStackCleanupException *e)
    {
        // job done -- transfer back where we came from
        delete e;
        if (after_cleanup_transfer_to)
            simulation.transferTo(after_cleanup_transfer_to);
        else
            simulation.transferToMain();
        assert(/*invoking transferTo() on an already deleted module?*/ 0);
    }
    catch (cEndModuleException *e)
    {
        // hand exception to cSimulation::transferTo() and switch back
        simulation.exception = e;
        simulation.exception_type = 2;
    }
    catch (cTerminationException *e)
    {
        // hand exception to cSimulation::transferTo() and switch back
        simulation.exception = e;
        simulation.exception_type = 1;
    }
    catch (cException *e)
    {
        // hand exception to cSimulation::transferTo() and switch back
        simulation.exception = e;
        simulation.exception_type = 0;
    }
    catch (std::exception e)
    {
        simulation.exception = new cException("standard C++ exception %s: %s",
                                               opp_typename(typeid(e)), e.what());
        simulation.exception_type = 0;
    }
    //catch (...) -- this is probably not a good idea because makes debugging difficult
    //{
    //    simulation.exception = new cException("unknown exception occurred");
    //    simulation.exception_type = 0;
    //}

    // The End
    simulation.transferToMain(); // send back exception -- will come back sometime for stack cleanup
    if (after_cleanup_transfer_to)
        simulation.transferTo(after_cleanup_transfer_to);
    else
        simulation.transferToMain();
    assert(/*invoking transferTo() on an already deleted module?*/ 0);
}

cSimpleModule::cSimpleModule(const cSimpleModule& mod) :
  cModule(mod.name(), mod.parentModule())
{
    timeoutmsg = NULL;
    coroutine = NULL;
    setName(mod.name());
    operator=( mod );
}

cSimpleModule::cSimpleModule(const char *name, cModule *parentmod, unsigned stksize) :
  cModule(name, parentmod)
{
    state = sREADY;
    coroutine = NULL;

    usesactivity = (stksize!=0);

    // for an activity() module, timeoutmsg will be created in scheduleStart()
    // which must always be called
    timeoutmsg = NULL;

    if (usesactivity)
    {
       // setup coroutine, allocate stack for it
       coroutine = new cCoroutine;
       if (!coroutine->setup(cSimpleModule::activate, this, stksize+ev.extraStackForEnvir()))
           throw new cException("Cannot create coroutine with %d+%d bytes of stack space for module `%s'",
                             stksize,ev.extraStackForEnvir(),fullPath().c_str());
    }
}

cSimpleModule::~cSimpleModule()
{
    if (simulation.contextModule()==this)
        throw new cException(this, "cannot delete itself, only via deleteModule()");

    // timeoutmsg is deleted from cDefaultList destructor (or by the message queue)

    if (usesActivity())
    {
        // clean up user's objects on coroutine stack by forcing an exception inside the coroutine
        if (state!=sENDED)   // FIXME khmm -- the not yet started ones are also here
        {                    // FIXME is this a good place?
            stack_cleanup_requested = true;
            after_cleanup_transfer_to = simulation.runningModule();
            assert(!after_cleanup_transfer_to || after_cleanup_transfer_to->usesActivity());
            simulation.transferTo(this);
            stack_cleanup_requested = false;
        }
        delete coroutine;
    }

    // delete pending messages for this module
    for (cMessageHeap::Iterator iter(simulation.msgQueue); !iter.end(); iter++)
    {
        cMessage *msg = iter();
        if (msg->arrivalModuleId() == id())
            delete simulation.msgQueue.get( msg );
    }

}

cSimpleModule& cSimpleModule::operator=(const cSimpleModule& other)
{
    if (this==&other) return *this;

    usesactivity = other.usesactivity;
    cModule::operator=(other);
    coroutine = new cCoroutine();
    if (!coroutine->setup(cSimpleModule::activate, this, other.coroutine->stackSize()))
        throw new cException("Cannot allocate stack for module `%s' (increase total stack size!)",
                  ev.extraStackForEnvir(),fullPath().c_str());
    return *this;
}

std::string cSimpleModule::info() const
{
    std::stringstream out;
    out << "id=" << id();
    return out.str();
}

void cSimpleModule::forEachChild(cVisitor *v)
{
    cModule::forEachChild(v);
}

void cSimpleModule::setId(int n)
{
    cModule::setId( n );

    if (timeoutmsg)
        timeoutmsg->setArrival(this,n);
}

void cSimpleModule::end()
{
    state = sENDED;
    throw new cEndModuleException;
}

void cSimpleModule::error(const char *fmt...) const
{
    va_list va;
    va_start(va, fmt);
    char buf[256];
    vsprintf(buf,fmt,va);
    va_end(va);

    throw new cException(eUSER,buf);
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

    // ignore for simple modules using handleMessage()
    if (!usesactivity)
        return;

    if (timeoutmsg!=NULL)
        throw new cException("scheduleStart(): module `%s' already started",fullPath().c_str());

    // create timeoutmsg, used as internal timeout message
    char buf[24];
    sprintf(buf,"starter-%d", id());
    timeoutmsg = new cMessage(buf,MK_STARTER);

    // initialize message fields
    timeoutmsg->setSentFrom(NULL,-1, 0);
    timeoutmsg->setArrival(this,-1, t);

    // use timeoutmsg as the activation message; insert it into the FES
    simulation.msgQueue.insert( timeoutmsg );
}

void cSimpleModule::deleteModule()
{
    if (simulation.contextModule()==this)
    {
        // we're inside the currently executing module: get outta here quickly,
        // and leave simulation.deleteModule(id()) to whoever catches the exception
        throw new cEndModuleException(true);
    }

    // simple case: we're being deleted from main or from another module
    delete this;
}

int cSimpleModule::send(cMessage *msg, int g)
{
    return sendDelayed(msg, 0.0, g);
}

int cSimpleModule::send(cMessage *msg, const char *gatename, int sn)
{
    return sendDelayed(msg, 0.0, gatename, sn);
}

int cSimpleModule::send(cMessage *msg, cGate *outgate)
{
    return sendDelayed(msg, 0.0, outgate);
}

int cSimpleModule::sendDelayed(cMessage *msg, double delay, const char *gatename, int sn)
{
    cGate *outgate = gate(gatename,sn);
    if (outgate==NULL)
       throw new cException(sn<0 ? "send()/sendDelayed(): module has no gate `%s'":
                        "send()/sendDelayed(): module has no gate `%s[%d]'",gatename,sn);
    return sendDelayed(msg, delay, outgate);
}

int cSimpleModule::sendDelayed(cMessage *msg, double delay, int g)
{
    cGate *outgate = gate(g);
    if (outgate==NULL)
        throw new cException("send()/sendDelayed(): module has no gate #%d",g);
    return sendDelayed(msg, delay, outgate);
}

int cSimpleModule::sendDelayed(cMessage *msg, double delay, cGate *outgate)
{
    // error checking:
    if (outgate==NULL)
       throw new cException("send()/sendDelayed(): gate pointer is NULL");
    if (outgate->type()=='I')
       throw new cException("send()/sendDelayed(): cannot send via an input gate (`%s')",outgate->name());
    if (msg==NULL)
        throw new cException("send()/sendDelayed(): message pointer is NULL");
    if (msg->owner()!=this)
    {
        if (msg->owner()==&simulation.msgQueue && msg->isSelfMessage() && msg->arrivalModuleId()==id())
            throw new cException("send()/sendDelayed(): cannot send message (%s)%s: it is "
                                 "currently scheduled as a self-message for this module",
                                 msg->className(), msg->name());
        else if (msg->owner()==&simulation.msgQueue && msg->isSelfMessage())
            throw new cException("send()/sendDelayed(): cannot send message (%s)%s: it is "
                                 "currently scheduled as a self-message for ANOTHER module",
                                 msg->className(), msg->name());
        else if (msg->owner()==&simulation.msgQueue)
            throw new cException("send()/sendDelayed(): cannot send message (%s)%s: it is "
                                 "currently in scheduled-events, being underway between two modules",
                                 msg->className(), msg->name());
        else
            throw new cException("send()/sendDelayed(): cannot send message (%s)%s: "
                                 "it is currently contained/owned by (%s)%s",
                                 msg->className(), msg->name(), msg->owner()->className(),
                                 msg->owner()->fullPath().c_str());
    }
    if (delay<0.0)
        throw new cException("sendDelayed(): negative delay %g",delay);

    // when in debugging mode, switch back to main program for a moment
    if (pause_in_sendmsg && usesactivity)
    {
        simulation.backtomod = this;  // Ensure that scheduler will select us
        simulation.transferToMain();  // before all other modules
        simulation.backtomod = NULL;
        if (stack_cleanup_requested)
            throw new cStackCleanupException();
    }

    // set message parameters and send it
    msg->setSentFrom(this, outgate->id(), simTime()+delay);

    bool keepit = outgate->deliver(msg, simTime()+delay);
    ev.messageSent( msg );
    if (!keepit)
        delete msg;
    return 0;
}

int cSimpleModule::sendDirect(cMessage *msg, double propdelay, cModule *mod, int g)
{
    cGate *togate = mod->gate(g);
    if (togate==NULL)
        throw new cException("sendDirect(): module `%s' has no gate #%d",mod->fullPath().c_str(),g);

    return sendDirect(msg, propdelay, togate);
}

int cSimpleModule::sendDirect(cMessage *msg, double propdelay,
                              cModule *mod, const char *gatename, int sn)
{
    if (!mod)
        throw new cException("sendDirect(): module ptr is NULL");
    cGate *togate = mod->gate(gatename,sn);
    if (togate==NULL)
        throw new cException(sn<0 ? "sendDirect(): module `%s' has no gate `%s'":
                         "sendDirect(): module `%s' has no gate `%s[%d]'",
                         mod->fullPath().c_str(),gatename,sn);
    return sendDirect(msg, propdelay, togate);
}

int cSimpleModule::sendDirect(cMessage *msg, double propdelay, cGate *togate)
{
    // Note: it is permitted to send to an output gate. It is especially useful
    // with several submodules sending to a single output gate of their parent module.
    if (togate==NULL)
        throw new cException("sendDirect(): destination gate pointer is NULL");
    if (togate->fromGate())
        throw new cException("sendDirect(): module must have dedicated gate(s) for receiving via sendDirect()"
                             " (\"from\" side of dest. gate `%s' should NOT be connected)",togate->fullPath().c_str());

    if (msg==NULL)
        throw new cException("sendDirect(): message pointer is NULL");
    if (msg->owner()!=this)
    {
        if (msg->owner()==&simulation.msgQueue && msg->isSelfMessage() && msg->arrivalModuleId()==id())
            throw new cException("sendDirect(): cannot send message (%s)%s: it is "
                                 "currently scheduled as a self-message for this module",
                                 msg->className(), msg->name());
        else if (msg->owner()==&simulation.msgQueue && msg->isSelfMessage())
            throw new cException("sendDirect(): cannot send message (%s)%s: it is "
                                 "currently scheduled as a self-message for ANOTHER module",
                                 msg->className(), msg->name());
        else if (msg->owner()==&simulation.msgQueue)
            throw new cException("sendDirect(): cannot send message (%s)%s: it is "
                                 "currently in scheduled-events, being underway between two modules",
                                 msg->className(), msg->name());
        else
            throw new cException("sendDirect(): cannot send message (%s)%s: "
                                 "it is currently contained/owned by (%s)%s",
                                 msg->className(), msg->name(), msg->owner()->className(),
                                 msg->owner()->fullPath().c_str());
    }

    // to help debugging, switch back to main for a moment
    if (pause_in_sendmsg && usesactivity)
    {
        simulation.backtomod = this;  // Ensure that scheduler will select us
        simulation.transferToMain();  // before all other modules
        simulation.backtomod = NULL;
        if (stack_cleanup_requested)
            throw new cStackCleanupException();
    }

    // set message parameters and send it
    msg->setSentFrom(this, -1, simTime());
    bool keepit = togate->deliver( msg, simTime()+propdelay);
    ev.messageSent(msg, togate);
    if (!keepit)
        delete msg;
    return 0;
}

int cSimpleModule::scheduleAt(simtime_t t, cMessage *msg)
{
    if (t<simTime())
        throw new cException(eBACKSCHED);
    if (msg==NULL)
        throw new cException("scheduleAt(): message pointer is NULL");
    if (msg->owner()!=this)
    {
        if (msg->owner()==&simulation.msgQueue && msg->isSelfMessage() && msg->arrivalModuleId()==id())
            throw new cException("scheduleAt(): message (%s)%s is currently scheduled, "
                                 "use cancelEvent() before rescheduling",
                                 msg->className(), msg->name());
        else if (msg->owner()==&simulation.msgQueue && msg->isSelfMessage())
            throw new cException("scheduleAt(): cannot schedule message (%s)%s: it is "
                                 "currently scheduled as self-message for ANOTHER module",
                                 msg->className(), msg->name());

         else if (msg->owner()==&simulation.msgQueue)
            throw new cException("scheduleAt(): cannot schedule message (%s)%s: it is "
                                 "currently in scheduled-events, being underway between two modules",
                                 msg->className(), msg->name());
        else
            throw new cException("scheduleAt(): cannot schedule message (%s)%s: "
                                 "it is currently contained/owned by (%s)%s",
                                 msg->className(), msg->name(), msg->owner()->className(),
                                 msg->owner()->fullPath().c_str());
    }

    // to help debugging, switch back to main for a moment
    if (pause_in_sendmsg && usesactivity)
    {
        simulation.backtomod = this;  // Ensure that scheduler will
        simulation.transferToMain();  //   select us before all other modules
        simulation.backtomod = NULL;
        if (stack_cleanup_requested)
            throw new cStackCleanupException();
    }

    // set message parameters and schedule it
    msg->setSentFrom(this, -1, simTime());
    msg->setArrival(this, -1, t);
    ev.messageSent( msg );
    simulation.msgQueue.insert( msg );
    return 0;
}

cMessage *cSimpleModule::cancelEvent(cMessage *msg)
{
    // make sure we really have the message and it is scheduled
    if (msg==NULL)
        throw new cException("cancelEvent(): message pointer is NULL");

    // now remove it from future events and return pointer
    if (msg->isScheduled())
        simulation.msgQueue.get( msg );
    return msg;
}

void cSimpleModule::arrived( cMessage *msg, int ongate, simtime_t t)
{
    if (state==sENDED)
        throw new cException(eMODFIN,fullPath().c_str());
    if (t<simTime())
        throw new cException("causality violation: message `%s' arrival time %s at module `%s' "
                             "is earlier than current simulation time",
                             msg->name(), simtimeToStr(t), fullPath().c_str());
    msg->setArrival(this, ongate, t);
    simulation.msgQueue.insert( msg );
}

void cSimpleModule::wait(simtime_t t)
{
    if (!usesactivity)
        throw new cException(eNORECV);
    if (t<0)
        throw new cException(eNEGTIME);

    timeoutmsg->setArrivalTime(simTime()+t);
    simulation.msgQueue.insert( timeoutmsg );

    simulation.transferToMain();
    if (stack_cleanup_requested)
        throw new cStackCleanupException();

    cMessage *newmsg = simulation.msgQueue.getFirst();

    if (newmsg!=timeoutmsg)
        throw new cException("message arrived during wait() call ((%s)%s); if this "
                             "should be allowed, use waitAndEnqueue() instead of wait()",
                             newmsg->className(), newmsg->fullName());
}

void cSimpleModule::waitAndEnqueue(simtime_t t, cQueue *queue)
{
    if (!usesactivity)
        throw new cException(eNORECV);
    if (t<0)
        throw new cException(eNEGTIME);
    if (!queue)
        throw new cException("waitAndEnqueue(): queue pointer is NULL");

    timeoutmsg->setArrivalTime(simTime()+t);
    simulation.msgQueue.insert( timeoutmsg );

    for(;;)
    {
        simulation.transferToMain();
        if (stack_cleanup_requested)
            throw new cStackCleanupException();

        cMessage *newmsg = simulation.msgQueue.getFirst();

        if (newmsg==timeoutmsg)
            break;
        else
        {
            ev.messageDelivered( newmsg );
            queue->insert( newmsg );
        }
    }
}

//-------------

cMessage *cSimpleModule::receive()
{
    if (!usesactivity)
        throw new cException(eNORECV);

    simulation.transferToMain();
    if (stack_cleanup_requested)
        throw new cStackCleanupException();

    cMessage *newmsg = simulation.msgQueue.getFirst();

    ev.messageDelivered( newmsg );

    return newmsg;
}

cMessage *cSimpleModule::receive(simtime_t t)
{
    if (!usesactivity)
        throw new cException(eNORECV);
    if (t<0)
        throw new cException(eNEGTOUT);

    timeoutmsg->setArrivalTime(simTime()+t);
    simulation.msgQueue.insert( timeoutmsg );

    simulation.transferToMain();
    if (stack_cleanup_requested)
        throw new cStackCleanupException();

    cMessage *newmsg = simulation.msgQueue.getFirst();

    if (newmsg==timeoutmsg)  // timeout expired
    {
        take(timeoutmsg);
        return NULL;
    }
    else  // message received OK
    {
        take(cancelEvent(timeoutmsg));
        ev.messageDelivered( newmsg );
        return newmsg;
    }
}

//-------------

void cSimpleModule::activity()
{
    // default thread function
    // the user must redefine this for the module to do anything useful
    throw new cException("Redefine activity() or specify zero stack size to use handleMessage()");
}

void cSimpleModule::handleMessage(cMessage *)
{
    // handleMessage is an alternative to activity()
    // this is the default version
    throw new cException("Redefine handleMessage() or specify non-zero stack size to use activity()");
}

//-------------

void cSimpleModule::pause(const char *phase)
{
    if (!usesactivity)
        throw new cException("pause() is not supported for modules using handleMessage()");

    if (phase)
        phasestr = phase;
    simulation.backtomod = this;
    simulation.transferToMain();
    simulation.backtomod = NULL;
    if (stack_cleanup_requested)
        throw new cStackCleanupException();
}

simtime_t cSimpleModule::simTime() const
{
    return simulation.simTime();
}

void cSimpleModule::endSimulation()
{
    throw new cTerminationException(eENDSIM);
}

void cSimpleModule::breakpoint(const char *label)
{
    ev.breakpointHit(label, this);
}

bool cSimpleModule::snapshot(cObject *object, const char *label)
{
    return simulation.snapshot(object, label);
}

void cSimpleModule::recordScalar(const char *name, double value)
{
    ev.recordScalar(this, name, value);
}

bool cSimpleModule::stackOverflow() const
{
    return coroutine ? coroutine->stackOverflow() : false;
}

unsigned cSimpleModule::stackSize() const
{
    return coroutine ? coroutine->stackSize() : 0;
}

unsigned cSimpleModule::stackUsage() const
{
    return coroutine ? coroutine->stackUsage() : 0;
}


