//======================================================================
//
//  CMODULE.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
//   Member functions of
//    cModule         : common base for module classes
//    cSimpleModule   : base for simple module objects
//    cCompoundModule : compound module
//
//  Author: Andras Varga
//
//======================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include <string.h>          // strcpy
#include <assert.h>
#include "cmodule.h"
#include "cgate.h"
#include "cmessage.h"
#include "csimul.h"
#include "carray.h"
#include "ctypes.h"
#include "cpar.h"
#include "cnetmod.h"
#include "cenvir.h"
#include "cexception.h"


//=== static members:
bool cModule::pause_in_sendmsg;

// Note: cModule,cSimpleModule,cCompoundModule are left unregistered.
//   One should never create modules by createOne(cSimpleModule) or the like.

//=========================================================================
//=== cModule - member functions

cModule::cModule(const cModule& mod) : cObject(),
 gatev(NULL, 0,2),
 paramv(NULL, 0,2),
 machinev(NULL,0,2),
 members(NULL)
{
    take( &members );

    idx=0; vectsize=-1;
    fullname = NULL;

    setName(mod.name());
    operator=(mod);
}

cModule::cModule(const char *name, cModule *parentmod) :
 cObject(name),
 gatev("gates",0,2),
 paramv("parameters",0,2),
 machinev("machines",0,2),
 members("class-data-members")
{
    take( &gatev );
    take( &paramv );
    take( &machinev );
    take( &members );

    parentmodp = parentmod;
    idx=0; vectsize=-1;
    fullname = NULL;

    notify_inspector = NULL;
    data_for_inspector = NULL;

    /* cModuleType::create() call will create gates, params and machines */
}

cModule::~cModule()
{
    delete [] fullname;
}

cModule& cModule::operator=(const cModule&)
{
    throw new cException(this, eCANTDUP);
}

void cModule::forEach(ForeachFunc do_fn )
{
    if (do_fn(this,true))
    {
       paramv.forEach( do_fn );
       gatev.forEach( do_fn );
       members.forEach( do_fn );
       for (cSubModIterator submod(*this); !submod.end(); submod++)
          submod()->forEach( do_fn );
    }
    do_fn(this,false);
}

void cModule::setId( int n )
{
    mod_id = n;
}

void cModule::setIndex(int i, int n)
{
    idx = i;
    vectsize = n;
}

void cModule::setModuleType(cModuleType *mtype)
{
    mod_type = mtype;
}

const char *cModule::fullName() const
{
    // if not in a vector, normal name() will do
    if (!isVector())
       return name();

    // produce index with name here (lazy solution: produce name in each call,
    // instead of overriding both setName() and setIndex()...)
    if (fullname)  delete [] fullname;
    fullname = new char[opp_strlen(name())+10];
    sprintf(fullname, "%s[%d]", name(), index() );
    return fullname;
}

const char *cModule::fullPath() const
{
    return fullPath(fullpathbuf,FULLPATHBUF_SIZE);
}

const char *cModule::fullPath(char *buffer, int bufsize) const
{
    // check we got a decent buffer
    if (!buffer || bufsize<4)
    {
        if (buffer) buffer[0]='\0';
        return "(fullPath(): no or too small buffer)";
    }

    // follows module hierarchy instead of ownership hierarchy
    // append parent path + "."
    char *buf = buffer;
    if (parentmodp!=NULL)
    {
       parentmodp->fullPath(buf,bufsize);
       int len = strlen(buf);
       buf+=len;
       bufsize-=len;
       *buf++ = '.';
       bufsize--;
    }

    // append our own name
    opp_strprettytrunc(buf, fullName(), bufsize-1);
    return buffer;
}

void cModule::addGate(const char *gname, char tp)
{
    if (findGate(gname)>=0)
       throw new cException(this, "addGate(): Gate %s.%s already present", fullPath(), gname);

     cGate *newg = new cGate( gname, tp );
     newg->setOwnerModule(this, gatev.add( newg ));
}

void cModule::setGateSize(const char *gname, int size)
{
    int pos = findGate(gname,-1);
    if (pos<0)
       pos = findGate(gname,0);
    if (pos<0)
       throw new cException(this,"setGateSize(): Gate %s.%s[] not found", fullPath(), gname);

    char tp = gate(pos)->type();
    int oldsize = gate(pos)->size();

    // remove old vector
    int i;
    for (i=0; i<oldsize; i++)
    {
       cGate *g = (cGate *) gatev.remove( pos+i );
       if (g->fromGate() || g->toGate())
          throw new cException(this,"setGateSize(): Too late, gate %s already connected", g->fullPath());
       delete g;
    }

    // find `size' empty adjacent slots in the array
    pos = 0;
    for (i=0; i<size; i++)
       if (gatev.exist(pos+i))
          {pos+=i+1; i=-1;}

    // add new vector starting from pos
    for (i=0; i<size; i++)
    {
        cGate *gate = new cGate( gname, tp );
        gate->setIndex( i, size );
        gatev.addAt( pos+i, gate );
        gate->setOwnerModule( this, pos+i );
    }
}

void cModule::addPar(const char *pname)
{
    int i = findPar(pname);
    if (i!=-1)
       throw new cException(this,"addPar(): Parameter %s.%s already present",fullPath(), pname);

    i = paramv.add( new cModulePar(pname) );
    cModulePar *p = (cModulePar *) &par(i);
    p->setOwnerModule( this );
    p->setInput( true );
}

bool cModule::checkInternalConnections() const
{
    int j;
    // Note: This routine only checks if all gates are connected or not.
    //       Does not NOT check where and how they are connected!

    // check this module compound module if its inside is connected ok
    for(j=0; j<gates(); j++)
    {
       const cGate *g = gate(j);
       if (g && !g->isConnectedInside())
            throw new cException(this,"Gate `%s' is not connected to submodule (or output gate of same module)", g->fullPath());
    }

    // check submodules
    for (cSubModIterator submod(*this); !submod.end(); submod++)
    {
       cModule *m = submod();
       for(j=0; j<m->gates(); j++)
       {
          cGate *g = m->gate(j);
          if (g && !g->isConnectedOutside())
             throw new cException(this,"Gate `%s' is not connected to sibling or parent module", g->fullPath());
       }
    }
    return true;
}

void cModule::addMachinePar(const char *pname)
{
    cPar *par = new cPar(pname);
    *par = "";
    machinev.add( par );
}

void cModule::setMachinePar(const char *pname, const char *value)
{
    int i = machinev.find( pname );
    if (i==-1)
         throw new cException(this,"Machine par `%s' does not exist", pname);

    ((cPar *)machinev[i])->setStringValue(value);
}

int cModule::findSubmodule(const char *submodname, int idx)
{
    for (cSubModIterator i(*this); !i.end(); i++)
        if (i()->isName(submodname) &&
            ((idx==-1 && !i()->isVector()) || i()->index()==idx)
           )
            return i()->id();
    return -1;
}

cModule *cModule::submodule(const char *submodname, int idx)
{
    for (cSubModIterator i(*this); !i.end(); i++)
        if (i()->isName(submodname) &&
            ((idx==-1 && !i()->isVector()) || i()->index()==idx)
           )
            return i();
    return NULL;
}

cModule *cModule::moduleByRelativePath(const char *path)
{
    // start tokenizing the path (path format: "SysModule.DemandGen[2].Source")
    opp_string pathbuf(path);
    char *s = strtok(pathbuf.buffer(),".");

    // search starts from this module
    cModule *modp = this;

    // match components of the path
    do {
        char *b;
        if ((b=strchr(s,'['))==NULL)
            modp = modp->submodule(s);  // no index given
        else
        {
            if (s[strlen(s)-1]!=']')
                throw new cException(this,"moduleByRelativePath(): syntax error in path `%s'", path);
            *b='\0';
            modp = modp->submodule(s,atoi(b+1));
        }
    } while ((s=strtok(NULL,"."))!=NULL && modp!=NULL);

    return modp;  // NULL if not found
}

int cModule::findGate(const char *s, int sn) const
{
    const cGate *g = 0; // initialize g to prevent compiler warnings
    int i = 0, n = gates();
    while (i<n)
    {
       g = gate(i);
       if (!g)
          i++;
       else if (!g->isName(s))
          i+=g->size();
       else
          break;
    }

    if (i>=n)
       return -1;
    else if (sn<0)
       // for sn=-1, we return the 0th gate. This is not very clean but
       // necessary for code like n=gate("out_vector")->size() to work.
       return i;
    else if (sn<g->size())
       // assert may be removed later
       {assert( gate(i+sn)->index()==sn ); return i+sn;}
    else
       return -1;
}

cGate *cModule::gate(const char *s, int sn)
{
    int i = findGate(s,sn);
    if (i==-1)
        return NULL;
    return gate(i);
}

const cGate *cModule::gate(const char *s, int sn) const
{
    int i = findGate(s,sn);
    if (i==-1)
        return NULL;
    return gate(i);
}

int cModule::findPar(const char *s) const
{
    return paramv.find(s);
}

cPar& cModule::par(int pn)
{
    cPar *p = (cPar *)paramv[pn];
    if (!p)
        throw new cException(this,"has no parameter #%d",pn);
    return *p;
}

cPar& cModule::par(const char *s)
{
    cPar *p = (cPar *)paramv.get(s);
    if (!p)
        throw new cException(this,"has no parameter called `%s'",s);
    return *p;
}

cPar& cModule::ancestorPar(const char *name)
{
    // search parameter in parent modules
    cModule *pmod = this;
    int k;
    while (pmod && (k=pmod->findPar(name))<0)
        pmod = pmod->parentmodp;
    if (!pmod)
        throw new cException(this,"has no ancestor parameter called `%s'",name);
    return pmod->par(k);
}

const char *cModule::machinePar(int pn)
{
    cPar *mp = (cPar *)machinev[pn];
    if (!mp)
        return NULL;
    return mp->stringValue();
}

const char *cModule::machinePar(const char *pname)
{
    int i = machinev.find( pname );
    if (i==-1)
         return NULL;
    return ((cPar *)machinev[i])->stringValue();
}

bool cModule::isOnLocalMachine() const
{
    return simulation.netInterface()==NULL || simulation.netInterface()->isLocalMachineIn( machinev );
}

int cModule::buildInside()
{
    // temporarily switch context
    cModule *oldcontext = simulation.contextModule();
    simulation.setContextModule(this);

    // check parameters and gates
    cModuleInterface *iface = moduleType()->moduleInterface();
    iface->checkParametersOf(this);

    // call doBuildInside() in this context
    doBuildInside();

    if (oldcontext)
        simulation.setContextModule( oldcontext );
    else
        simulation.setGlobalContext();

    return 0;
}

void cModule::initialize()
{
    // Called before simulation starts (or usually after dynamic module was created).
    // Should be redefined by user.
}

void cModule::finish()
{
    // Called after end of simulation (and usually before destroying a dynamic module).
    // Should be redefined by user.
}

void cModule::callInitialize()
{
    int stage = 0;
    while (callInitialize(stage))
        ++stage;
}

const char *cModule::displayString()
{
    if ((const char *)dispstr != NULL)
        return dispstr;

    // no display string stored -- try to get it from Envir
    char dispname[128];
    if (!parentModule()) return "";
    sprintf(dispname, "%s.%s",parentModule()->className(),fullName());
    const char *s = ev.getDisplayString(simulation.runNumber(),dispname);
    return s ? s : "";
}

void cModule::setDisplayString(const char *s, bool immediate)
{
    dispstr = s;

    // notify the parent module's inspector
    cModule *p = parentModule();
    if (p && p->notify_inspector) p->notify_inspector(p,immediate,p->data_for_inspector);
}

const char *cModule::displayStringAsParent()
{
    if ((const char *)parentdispstr != NULL)
        return parentdispstr;

    // no display string stored -- try to get it from Envir
    const char *s = ev.getDisplayString(simulation.runNumber(),className());
    return s ? s : "";
}

void cModule::setDisplayStringAsParent(const char *s, bool immediate)
{
    parentdispstr = s;

    // notify inspector
    if (notify_inspector) notify_inspector(this,immediate,data_for_inspector);
}

// DEPRECATED
void cModule::setDisplayString(int type, const char *s, bool immediate)
{
    if (type<0 || type>=dispNUMTYPES)
         throw new cException(this,"setDisplayString(): type %d out of range", type );

    if (type==dispENCLOSINGMOD)
    {
         parentdispstr = s;
         if (notify_inspector) notify_inspector(this,immediate,data_for_inspector);
    }
    else if (type==dispSUBMOD)
    {
         dispstr = s;
         cModule *p = parentModule();
         if (p && p->notify_inspector) p->notify_inspector(this,immediate,p->data_for_inspector);
    }
}

// DEPRECATED
const char *cModule::displayString(int type)
{
    if (type<0 || type>=dispNUMTYPES)
         throw new cException(this,"displayString(): type %d out of range", type );

    if (type==dispSUBMOD && (const char *)dispstr != NULL)
        return dispstr;
    if (type==dispENCLOSINGMOD && (const char *)parentdispstr != NULL)
        return parentdispstr;

    // no hardcoded display string -- try to get it from Envir
    char dispname[128];

    if (type==dispSUBMOD)
    {
        if (!parentModule()) return "";
        sprintf(dispname, "%s.%s",parentModule()->className(),fullName());
    }
    else // type==dispENCLOSINGMOD
    {
        sprintf(dispname, "%s",className());
    }
    const char *s = ev.getDisplayString(simulation.runNumber(),dispname);
    return s ? s : "";
}

void cModule::setDisplayStringNotify(DisplayStringNotifyFunc notify_func, void *data)
{
    notify_inspector = notify_func;
    data_for_inspector = data;
}

//==========================================================================
//=== cSimpleModule - member functions

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
        // simply ignore exception and let module finish
        // smod->state was already set in cSimpleModule::end()
        delete e;
    }
    catch (cTerminationException *e)
    {
        // hand exception to cSimulation::transferTo() and switch back
        simulation.exception = e;
        simulation.exception_type = 1;
        simulation.transferToMain();
    }
    catch (cException *e)
    {
        // hand exception to cSimulation::transferTo() and switch back
        simulation.exception = e;
        simulation.exception_type = 0;
        simulation.transferToMain();
    }
    simulation.transferToMain();
}

cSimpleModule::cSimpleModule(const cSimpleModule& mod) :
  cModule( mod.name(), mod.parentmodp ),
  locals(NULL),
  putAsideQueue( NULL, NULL, false )
{
    take( &locals );
    take( &putAsideQueue );

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
    take( &putAsideQueue );

    // for an activity() module, timeoutmsg will be created in scheduleStart()
    // which must always be called
    timeoutmsg = NULL;

    if (usesactivity)
    {
       // setup coroutine, allocate stack for it
       coroutine = new cCoroutine;
       if (!coroutine->setup(cSimpleModule::activate, this, stksize+ev.extraStackForEnvir()))
           throw new cException("Cannot allocate %d+%d bytes of stack for module `%s' (increase total stack size!)",
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
    // no call to cObject::info()!
    sprintf(buf,"%-20.20s (%s,#%d)", fullName(), className(), id() );
}

void cSimpleModule::forEach(ForeachFunc do_fn)
{
   if (do_fn(this,true))
   {
      paramv.forEach( do_fn );
      gatev.forEach( do_fn );
      locals.forEach( do_fn );
      members.forEach( do_fn );
      putAsideQueue.forEach( do_fn );
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

    p->mod  = this;
    p->prev = NULL;                                 //  insert into
    p->next = heap;                                 //  the list
    heap = p;                                       //  as the first
    if (p->next) p->next->prev = p;                 //  item
    return (void *)(p+1);                  // list structure same as in a cObject-cHead list
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
    timeoutmsg = new cMessage(0,MK_STARTER);
    take( timeoutmsg );

    // initialize message fields
    timeoutmsg->setSentFrom(NULL,-1, 0);
    timeoutmsg->setArrival(this,-1, t);

    // use timeoutmsg as the activation message; insert it into the FES
    simulation.msgQueue.insert( timeoutmsg );
}

void cSimpleModule::deleteModule()
{
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

    // delete module
    //   If a module deletes itself (ie. this module is the running one),
    //   it is somewhat problematic because if we delete it,
    //   we also delete the stack we're currently using.
    //   To solve the problem, we transfer to an external coroutine
    //   called runningmod_deleter which can delete the module without
    //   problem. After that, it'll do transferToMain() so we never
    //   get back here again.

    if (simulation.runningModule()==this && usesActivity())
       cCoroutine::switchTo( &simulation.runningmod_deleter );
    else
       simulation.deleteModule(id());
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
        throw new cException("send()/sendDelayed(): negative delay %g",delay);

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
    if (togate==NULL)
        throw new cException("sendDirect(): destination gate pointer is NULL");
    if (togate->type()=='O')
        throw new cException("sendDirect(): cannot send to an output gate (`%s')",togate->fullPath());

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
    ev.messageSent( msg );
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

void cSimpleModule::syncpoint(simtime_t t, int g)
{
    cGate *gatep = gate(g);
    if (!gatep)
        throw new cException(eNOGATE,g);

    // find route destination
    cGate *destgate = gatep->destinationGate();

    // if dest is not the network interface mod, ignore this call
    // otherwise, send syncpoint to destination segment
    if (destgate->ownerModule()==simulation.netInterface())
        simulation.netInterface()->send_syncpoint(t, destgate->id());
}

void cSimpleModule::syncpoint(simtime_t t, const char *gatename, int sn)
{
    int g = findGate(gatename,sn);
    if (g<0)
        throw new cException(sn<0 ? "syncpoint(): module has no gate `%s'":
                         "syncpoint(): module has no gate `%s[%d]'",gatename,sn);
    syncpoint(t,g);
}

void cSimpleModule::cancelSyncpoint(simtime_t t, int g)
{
    cGate *gatep = gate(g);
    if (!gatep)
        throw new cException(eNOGATE,g);

    // find route destination
    cGate *destgate = gatep->destinationGate();

    // if dest is not the network interface mod, ignore this call
    // otherwise, send syncpoint to destination segment
    if (destgate->ownerModule()==simulation.netInterface())
        simulation.netInterface()->send_cancelsyncpoint(t, destgate->id());
}

void cSimpleModule::cancelSyncpoint(simtime_t t, const char *gatename, int sn)
{
    int g = findGate(gatename,sn);
    if (g<0)
        throw new cException(sn<0 ? "cancelSyncpoint(): module has no gate `%s'":
                         "cancelSyncpoint(): module has no gate `%s[%d]'",gatename,sn);
    cancelSyncpoint(t,g);
}

void cSimpleModule::arrived( cMessage *msg, int ongate, simtime_t t)
{
    if (state==sENDED)
        throw new cException(eMODFIN,fullPath());
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

//==========================================================================
//=== cCompoundModule - member functions

cCompoundModule::cCompoundModule(const cCompoundModule& mod) :
  cModule( mod.name(), mod.parentmodp )
{
    setName(mod.name());
    operator=(mod);
}

cCompoundModule::cCompoundModule(const char *name, cModule *parentmod ) :
  cModule( name, parentmod )
{
}

cCompoundModule::~cCompoundModule()
{
}

void cCompoundModule::info( char *buf )
{
    sprintf(buf,"%-15.15s (%s,#%d)", fullName(), className(), id() );
}

cCompoundModule& cCompoundModule::operator=(const cCompoundModule& mod)
{
    if (this==&mod) return *this;

    cModule::operator=( mod );
    return *this;
}

void cCompoundModule::arrived( cMessage *, int, simtime_t)
{
    throw new cException("Message arrived at COMPOUND module `%s'", fullPath());
}

void cCompoundModule::scheduleStart(simtime_t t)
{
    for (cSubModIterator submod(*this); !submod.end(); submod++)
    {
        submod()->scheduleStart( t );
    }
}

//void cCompoundModule::callInitialize()
//{
//    cModule::callInitialize();
//}

bool cCompoundModule::callInitialize(int stage)
{
    // This is the interface for calling initialize().

    // first call it for this module...
    int numStages = numInitStages();
    if (stage < numStages)
    {
        cModule *oldcontext = simulation.contextModule();
        simulation.setContextModule( this );

        initialize(stage);

        if (oldcontext)
            simulation.setContextModule( oldcontext );
        else
            simulation.setGlobalContext();
    }

    // ...then for submods (meanwhile determine if more stages are needed)
    bool moreStages = stage < numStages-1;
    for (cSubModIterator submod(*this); !submod.end(); submod++)
    {
        if (submod()->callInitialize(stage))
            moreStages = true;
    }

    return moreStages; // return true if there's more stages to do
}


void cCompoundModule::callFinish()
{
    // This is the interface for calling finish().

    // first call it for submods...
    for (cSubModIterator submod(*this); !submod.end(); submod++)
    {
        submod()->callFinish();
    }

    // ...then for this module.
    cModule *oldcontext = simulation.contextModule();
    simulation.setContextModule( this );

    finish();

    if (oldcontext)
        simulation.setContextModule( oldcontext );
    else
        simulation.setGlobalContext();
}

void cCompoundModule::deleteModule()
{
    // delete submodules
    for (cSubModIterator submod(*this); !submod.end(); submod++)
    {
        if (submod() == (cModule *)simulation.runningModule())
        {
            throw new cException("Cannot delete a compound module from one of its submodules!");
            // The reason is that deleteModule() of the currently executing
            // module does not return -- for obvious reasons (we would
            // execute with an already deallocated stack etc).
            // Thus the deletion of the current module would remain unfinished.
            // A solution could be to skip deletion of that very module at first,
            // and delete it only when everything else is deleted.
            // However, this would be clumsy and ugly to implement so
            // I'd rather wait until the real need for it emerges... --VA
        }
        submod()->deleteModule();
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

    // delete module
    simulation.deleteModule( id() );
}

//==========================================================================
//=== cSubModIterator member functions:

cModule *cSubModIterator::operator++(int)
{
    if (end())
        return NULL;

    // linear search among all modules -- should be replaced with something faster
    int lastId = simulation.lastModuleId();
    do
    {
        id++;
        cModule *mod = simulation.module(id);
        if (mod!=NULL && parent==mod->parentModule())
            return mod;
    }
    while (id<=lastId);

    // not found
    id = -1;
    return NULL;
}

//==========================================================================
//=== little helper functions...

static void _connect(cModule *frm, int frg, cModule *tom, int tog)
{
    cGate *srcgate = frm->gate(frg);
    cGate *destgate = tom->gate(tog);

    // cLinkType is stored with the source gate of a connection:
    //srcgate.setLink( li );

    if (srcgate->toGate()!=NULL)
       throw new cException( "connect(): gate %s already connected", srcgate->fullPath() );

    if (destgate->fromGate()!=NULL)
       throw new cException( "connect(): gate %s already connected", destgate->fullPath() );

    srcgate->setTo( destgate );
    destgate->setFrom( srcgate );

    //VZ: NETWORKING EXTENSION OF CONNECT!!
    bool src_local = frm->isOnLocalMachine();
    bool dest_local = tom->isOnLocalMachine();
    cNetMod *netif = simulation.netInterface();

    //?? (*) If the mods were originally a simple mods on other hosts, we should
    //?? temporally connect the presently unused gates, so the checkconnection won't
    //?? complain. At a higher level perhaps, it will be connected to somewhere.
    //
    //if (!frm->isSimple() && !src_local && (srcgate.frommod()==NULL))
    //          srcgate.setFrom(NULL,-2);
    //if (!tom->isSimple() && !tom_local && (destgate.tomod()==NULL))
    //          destgate.setTo(NULL,-2);

    if (!src_local && dest_local)        //VZ
    {
        /* Create network gate that also goes to the 'to' gate (Y-shape) */
        int netg = netif->net_addgate( tom, tog, 'O' );
        netif->outgate(netg)->setTo( destgate );
    }
    if (!dest_local && src_local)        //VZ
    {
        /* Route the placeholder 'to' module's gate to a network gate */
        int netg = netif->net_addgate( tom, tog, 'I' );
        destgate->setTo( netif->gate(netg) );
        netif->ingate(netg)->setFrom( destgate );
        destgate->setLink(NULL);          //To avoid duplicate linking
    }
}


void connect(cModule *frm, int frg,
             cLinkType *linkp,
             cModule *tom, int tog)
{
    _connect( frm, frg, tom, tog);

    cGate *srcgate = frm->gate(frg);
    srcgate->setLink( linkp );
}

void connect(cModule *frm, int frg,
             cPar *delayp, cPar *errorp, cPar *dataratep,
             cModule *tom, int tog)
{
    _connect( frm, frg, tom, tog);

    cGate *srcgate = frm->gate(frg);
    srcgate->setDelay( delayp );
    srcgate->setError( errorp );
    srcgate->setDataRate( dataratep );
}


//--------------------------------------------------------------

void *operator new(size_t m,___nosuchclass *)
{
    return simulation.contextSimpleModule()->memAlloc( m );
}

