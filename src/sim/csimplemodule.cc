//======================================================================
//
//  CSIMPLEMODULE.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
//   Member functions of
//    cSimpleModule   : base for simple module objects
//
//  Author: Andras Varga
//
//======================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include <string.h>          // strcpy
#include <assert.h>
#include "csimplemodule.h"
#include "cgate.h"
#include "cmessage.h"
#include "csimul.h"
#include "carray.h"
#include "ctypes.h"
#include "cpar.h"
#include "cenvir.h"
#include "cexception.h"


void cSimpleModule::activate(void *p)
{
    cSimpleModule *smod = (cSimpleModule *)p;

    // The starter message should be the same as the timeoutmsg member of
    // cSimpleModule. If not, then something is wrong...
    cMessage *starter = simulation.msgQueue.getFirst();
    if (starter!=smod->timeoutmsg)
    {
        // hand exception to cSimulation::transferTo() and switch back
        simulation.exception = new cException("scheduleStart() should have been called for dynamically created module `%s'", smod->fullPath());
        simulation.exception_type = 0;
        simulation.transferToMain();
    }

    // give back the message to the module
    starter->setOwner(smod);
    starter->setKind(MK_TIMEOUT);
    char buf[24];
    sprintf(buf,"timeout-%d", smod->id());
    starter->setName(buf);

    // call activity(). At this point, initialize() has already been called
    // from cSimulation::startRun(), or manually in the case of dynamically
    // created modules.
    try
    {
        smod->activity();
        smod->state = sENDED;
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
    simulation.transferToMain();
}

cSimpleModule::cSimpleModule(const cSimpleModule& mod) :
  cModule( mod.name(), mod.parentmodp ),
  locals(NULL),
  putAsideQueue( NULL, NULL, false )
{
    take( &locals );
    //take( &putAsideQueue );
    putAsideQueue.setOwner(NULL); // hide deprecated putAsideQueue from object tree

    heap = NULL;
    timeoutmsg = NULL;
    coroutine = NULL;
    setName(mod.name());
    operator=( mod );
}

cSimpleModule::cSimpleModule(const char *name, cModule *parentmod, unsigned stksize) :
  cModule( name, parentmod ),
  locals( "local-objects"),
  putAsideQueue( "putaside-queue", cMessage::cmpbydelivtime, false )
{
    state = sREADY;
    heap = NULL;
    coroutine = NULL;

    usesactivity = (stksize!=0);

    take( &locals );
    //take( &putAsideQueue );
    putAsideQueue.setOwner(NULL); // hide deprecated putAsideQueue from object tree

    // for an activity() module, timeoutmsg will be created in scheduleStart()
    // which must always be called
    timeoutmsg = NULL;

    if (usesactivity)
    {
       // setup coroutine, allocate stack for it
       coroutine = new cCoroutine;
       if (!coroutine->setup(cSimpleModule::activate, this, stksize+ev.extraStackForEnvir()))
           throw new cException("Cannot create coroutine with %d+%d bytes of stack space for module `%s'",
                             stksize,ev.extraStackForEnvir(),fullPath());
    }
}

cSimpleModule::~cSimpleModule()
{
    // the `members' list should be empty by the time we get here
    // timeoutmsg is deleted from cObject destructor (or by the message queue)

    // delete putaside queue
    putAsideQueue.clear();

    // clean up user's objects: dispose of all objects in 'locals' list.
    discardLocals();

    clearHeap();
    delete coroutine;
}

cSimpleModule& cSimpleModule::operator=(const cSimpleModule& other)
{
    if (this==&other) return *this;

    usesactivity = other.usesactivity;
    cModule::operator=(other);
    coroutine = new cCoroutine();
    if (!coroutine->setup(cSimpleModule::activate, this, other.coroutine->stackSize()))
        throw new cException("Cannot allocate stack for module `%s' (increase total stack size!)",
                  ev.extraStackForEnvir(),fullPath());
    return *this;
}

void cSimpleModule::info(char *buf)
{
    sprintf(buf, "id=%d", id());
}

void cSimpleModule::forEach(ForeachFunc do_fn)
{
   if (do_fn(this,true))
   {
      paramv.forEach( do_fn );
      gatev.forEach( do_fn );
      locals.forEach( do_fn );
      members.forEach( do_fn );
      //putAsideQueue.forEach( do_fn );
      for (cSubModIterator submod(*this); !submod.end(); submod++)
         submod()->forEach( do_fn );
   }
   do_fn(this,false);
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

void cSimpleModule::discardLocals()
{
    // Called when cleaning up a simple module after simulation. Its job is
    // to delete objects (now garbage) owned by the simple module, and destruct
    // (via calling dtor) objects left on the coroutine stack.
    //
    // Actual method of "disposing of" an object depends on the storage class of the object:
    // - dynamic (allocated on the heap): operator delete
    // - auto (allocated on the stack, i.e. it is a local variable of activity()): direct destructor call
    // - static (global variable): setOwner(NULL) which makes the object join its default owner
    //
    while (cIterator(locals)())
    {
       cObject *obj = cIterator(locals)();
       stor = obj->storage();
       if (stor == 'D')
          delete obj;
       else if (stor == 'A')
          obj->destruct();
       else  /* stor == 'S' */
          obj->setOwner( NULL );
    }
}

void *cSimpleModule::memAlloc(size_t m)
{
    sBlock *p = (sBlock *)new char[m+sizeof(sBlock)];  // allocate
    if (!p)
        throw new cException("memAlloc(): could not allocate %u bytes", m);

    // insert into the list as first item (list structure is the same as
    // in a cObject-cHead list)
    p->mod  = this;
    p->prev = NULL;
    p->next = heap;
    heap = p;
    if (p->next) p->next->prev = p;
    return (void *)(p+1);
}

void cSimpleModule::memFree(void *&p)
{
    if (!p) return;                    // if existing block,
    sBlock *q = (sBlock *)p - 1;
    if (q->next)                       // remove it from list
           q->next->prev = q->prev;
    if (q->prev)
           q->prev->next = q->next;
    else
           q->mod->heap = q->next;

    delete q;                          // delete pointer and
    p = NULL;                          //  NULL it as an extra service
}

void cSimpleModule::clearHeap()
{
    // free all allocated blocks
    while (heap)
    {
        sBlock *p = heap->next;
        delete heap;
        heap = p;
    }
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
        throw new cException("scheduleStart(): module `%s' already started",fullPath());

    // create timeoutmsg, used as internal timeout message
    char buf[24];
    sprintf(buf,"starter-%d", id());
    timeoutmsg = new cMessage(buf,MK_STARTER);
    take( timeoutmsg );

    // initialize message fields
    timeoutmsg->setSentFrom(NULL,-1, 0);
    timeoutmsg->setArrival(this,-1, t);

    // use timeoutmsg as the activation message; insert it into the FES
    simulation.msgQueue.insert( timeoutmsg );
}

void cSimpleModule::deleteModule()
{
    // we have to be explicitly prepared that we might be called from a different
    // simple module (ie. that this!=contextModule())!

    // delete pending messages for this module
    for (cMessageHeapIterator iter(simulation.msgQueue); !iter.end(); iter++)
    {
        cMessage *msg = iter();
        if (msg->arrivalModuleId() == id())
            delete simulation.msgQueue.get( msg );
    }

    // adjust gates that were directed here
    for (int i=0; i<gates(); i++)
    {
        cGate *g = gate(i);
        if (g && g->toGate() && g->toGate()->fromGate()==g)
            g->toGate()->setFrom( NULL );
        if (g && g->fromGate() && g->fromGate()->toGate()==g)
            g->fromGate()->setTo( NULL );
    }

    if (simulation.contextModule()!=this)
    {
        // we're being deleted from another module: just finish job and return
        simulation.deleteModule( id() );
    }
    else
    {
        // we're inside the currently executing module: get outta here quickly,
        // and leave simulation.deleteModule(id()) to whoever catches the exception
        throw new cEndModuleException(true);
    }
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
    if (msg->owner() && msg->owner()!=&(this->locals))
        throw new cException("send()/sendDelayed(): not owner of message `%s'; owner is `%s'",
                             msg->name(),msg->owner()->fullPath());
    if (delay<0.0)
        throw new cException("sendDelayed(): negative delay %g",delay);

    // when in debugging mode, switch back to main program for a moment
    if (pause_in_sendmsg && usesactivity)
    {
        simulation.backtomod = this;  // Ensure that scheduler will select us
        simulation.transferToMain();  // before all other modules
        simulation.backtomod = NULL;
    }

    // set message parameters and send it
    msg->setSentFrom(this, outgate->id(), simTime()+delay);

    outgate->deliver(msg, simTime()+delay);
    ev.messageSent( msg );
    return 0;
}

int cSimpleModule::sendDirect(cMessage *msg, double propdelay, cModule *mod, int g)
{
    cGate *togate = mod->gate(g);
    if (togate==NULL)
        throw new cException("sendDirect(): module `%s' has no gate #%d",mod->fullPath(),g);

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
                         mod->fullPath(),gatename,sn);
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
                             " (``from'' side of dest. gate `%s' should NOT be connected)",togate->fullPath());

    if (msg==NULL)
        throw new cException("sendDirect(): message pointer is NULL");
    if (msg->owner() && msg->owner()!=&(this->locals))
        throw new cException("sendDirect(): not owner of message `%s'; owner is `%s'", msg->name(),msg->owner()->fullPath());

    // to help debugging, switch back to main for a moment
    if (pause_in_sendmsg && usesactivity)
    {
        simulation.backtomod = this;  // Ensure that scheduler will select us
        simulation.transferToMain();  // before all other modules
        simulation.backtomod = NULL;
    }

    // set message parameters and send it
    msg->setSentFrom(this, -1, simTime());
    togate->deliver( msg, simTime()+propdelay);
    ev.messageSent(msg, togate);
    return 0;
}

int cSimpleModule::scheduleAt(simtime_t t, cMessage *msg)
{
    if (t<simTime())
        throw new cException(eBACKSCHED);
    if (msg==NULL)
        throw new cException("scheduleAt(): message pointer is NULL");
    if (msg->owner() && msg->owner()!=&(this->locals) && msg->owner()!=this)
        throw new cException("scheduleAt(): not owner of message `%s'; owner is `%s'",msg->name(),msg->owner()->fullPath());

    // to help debugging, switch back to main for a moment
    if (pause_in_sendmsg && usesactivity)
    {
        simulation.backtomod = this;  // Ensure that scheduler will
        simulation.transferToMain();  //   select us before all other modules
        simulation.backtomod = NULL;
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
    if (!msg->isScheduled())
    {
        if (putAsideQueue.contains(msg))
            throw new cException("cancelEvent(): message `%s' is in the put-aside queue",msg->name());
        else
            throw new cException("cancelEvent(): message `%s' is currently not scheduled",msg->name());
        return NULL;
    }

    // now remove it from future events and return pointer
    simulation.msgQueue.get( msg );
    return msg;
}

void cSimpleModule::arrived( cMessage *msg, int ongate, simtime_t t)
{
    if (state==sENDED)
        throw new cException(eMODFIN,fullPath());
    if (t<simTime())
        throw new cException("causality violation: message `%s' arrival time %s at module `%s' "
                             "is earlier than current simulation time",
                             msg->name(), simtimeToStr(t), fullPath());
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

    for(;;)
    {
        simulation.transferToMain();
        cMessage *newmsg = simulation.msgQueue.getFirst();

        if (newmsg==timeoutmsg)
            break;
        else
        {
            ev.messageDelivered( newmsg );
            putAsideQueue.insert( newmsg );
        }
    }
    take(timeoutmsg);
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
        cMessage *newmsg = simulation.msgQueue.getFirst();

        if (newmsg==timeoutmsg)
            break;
        else
        {
            ev.messageDelivered( newmsg );
            queue->insert( newmsg );
        }
    }
    take(timeoutmsg);
}

//-------------

bool cSimpleModule::isThereMessage() const
{
    cMessage *msg = simulation.msgQueue.peekFirst();
    return msg!=NULL &&
           msg->arrivalModuleId()==id() &&
           msg->arrivalTime()==simTime();
}

cMessage *cSimpleModule::receiveNew()
{
    if (!usesactivity)
        throw new cException(eNORECV);

    simulation.transferToMain();
    cMessage *newmsg = simulation.msgQueue.getFirst();

    ev.messageDelivered( newmsg );

    return newmsg;
}

cMessage *cSimpleModule::receiveNew(simtime_t t)
{
    if (!usesactivity)
        throw new cException(eNORECV);
    if (t<0)
        throw new cException(eNEGTOUT);

    timeoutmsg->setArrivalTime(simTime()+t);
    simulation.msgQueue.insert( timeoutmsg );

    simulation.transferToMain();
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

cMessage *cSimpleModule::receiveNewOn(int g, simtime_t t)
{
    if (!usesactivity)
        throw new cException(eNORECV);
    if (t<0)
        throw new cException(eNEGTOUT);
    cGate *a = gate(g);
    if (a==NULL)
        throw new cException(eNOGATE,g);
    if (a->type()=='O')
        throw new cException(eOUTGATE,g);

    if (t!=MAXTIME)
    {
        timeoutmsg->setArrivalTime(simTime()+t);
        simulation.msgQueue.insert( timeoutmsg );
        for(;;)
        {
            simulation.transferToMain();
            cMessage *newmsg = simulation.msgQueue.getFirst();
            if (newmsg==timeoutmsg)  // timeout expired
               {take(timeoutmsg); return NULL;}
            else
            {
               ev.messageDelivered( newmsg );
               if (newmsg->arrivedOn(g))  // OK!
                   {take(cancelEvent(timeoutmsg)); return newmsg;}
               else   // not good --> put-aside queue
                   putAsideQueue.insert( newmsg );
            }
        }
    }
    else
    {
        for(;;)
        {
            simulation.transferToMain();
            cMessage *newmsg = simulation.msgQueue.getFirst();
            ev.messageDelivered( newmsg );
            if (newmsg->arrivedOn(g))
                return newmsg;
            else
                putAsideQueue.insert( newmsg );
        }
    }
}

cMessage *cSimpleModule::receiveNewOn(const char *gatename, int sn, simtime_t t)
{
    return receiveNewOn( findGate(gatename,sn), t );
}

//-------------

cMessage *cSimpleModule::receive()
{
    if (!putAsideQueue.empty())
        return (cMessage *)putAsideQueue.pop();
    else
        return receiveNew();
}

cMessage *cSimpleModule::receive(simtime_t t)
{
    if (!putAsideQueue.empty())
        return (cMessage *)putAsideQueue.pop();
    else
        return receiveNew( t );
}

cMessage *cSimpleModule::receiveOn(int g, simtime_t t)
{
    // first try to get it from the put-aside queue
    for( cQueueIterator qiter( putAsideQueue, 0 ); !qiter.end(); qiter--)
    {
        cMessage *msg = (cMessage *)qiter();
        if (msg->arrivedOn(g))
            return (cMessage *)putAsideQueue.remove( msg );
    }
    // ok, get it from the message queue
    return receiveNewOn( g, t );
}

cMessage *cSimpleModule::receiveOn(const char *gatename, int sn, simtime_t t)
{
    int g = findGate(gatename,sn);
    if (g<0)
        throw new cException(sn<0 ? "receiveOn(): module has no gate `%s'":
                         "receiveOn(): module has no gate `%s[%d]'",gatename,sn);
    return receiveOn( g, t );
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

//void cSimpleModule::callInitialize()
//{
//    cModule::callInitialize();
//}

bool cSimpleModule::callInitialize(int stage)
{
    int numStages = numInitStages();
    if (stage < numStages)
    {
        // switch to the module's context for the duration of the initialize() call.
        cModule *oldcontext = simulation.contextModule();
        simulation.setContextModule( this );

        initialize( stage );

        if (oldcontext)
            simulation.setContextModule( oldcontext );
        else
            simulation.setGlobalContext();
    }
    return stage < numStages-1;  // return true if there's more stages to do
}

void cSimpleModule::callFinish()
{
    // This is the interface for calling finish().
    // We switch to the module's context for the duration of the call.

    cModule *oldcontext = simulation.contextModule();
    simulation.setContextModule( this );

    finish();

    if (oldcontext)
        simulation.setContextModule( oldcontext );
    else
        simulation.setGlobalContext();
}

void cSimpleModule::pause(const char *phase)
{
    if (!usesactivity)
        throw new cException("pause() is not supported for modules using handleMessage()");

    if (phase)
        phasestr = phase;
    simulation.backtomod = this;
    simulation.transferToMain();
    simulation.backtomod = NULL;
}

//void cSimpleModule::realtimewait(double secs)
//{
//    if (simulation.rtwait_modp!=NULL)
//       opp_error("realtimewait(): one realtimewait() already pending in %s",
//                   simulation.rtwait_modp->fullPath());
//    else
//    {
//       simulation.rtwait_modp = this;
//       simulation.rtwait_from = clock();
//       simulation.rtwait_ticks = (clock_t)(secs*CLK_TCK);
//       simulation.transferToMain();
//       if (simulation.rtwait_modp!=NULL)
//          opp_error("received a message during a realtimewait() call");
//    }
//}

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

void cSimpleModule::recordScalar(const char *name, const char *text)
{
    ev.recordScalar(this, name, text);
}

void cSimpleModule::recordStats(const char *name, cStatistic *stats)
{
    ev.recordScalar(this, name, stats);
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

//--------------------------------------------------------------

void *operator new(size_t m,___nosuchclass *)
{
    return simulation.contextSimpleModule()->memAlloc( m );
}

