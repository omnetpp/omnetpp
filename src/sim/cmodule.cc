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
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include <string.h>          // strcpy
#include <iostream.h>        // ostream
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


//=== static members:
bool cModule::pause_in_sendmsg;

// Note: cModule,cSimpleModule,cCompoundModule are left unregistered.
//   One should never create modules by createOne(cSimpleModule) or the like.

//=========================================================================
//=== cModule - member functions

cModule::cModule(_CONST cModule& mod) : cObject(),
 gatev(NULL, 0,2),
 paramv(NULL, 0,2),
 machinev(NULL,0,2),
 members( NULL, NULL)
{
    take( &members );

    idx=0; vectsize=-1;

    setName(mod.name());
    operator=(mod);
}

cModule::cModule(const char *name, cModule *parentmod) :
 cObject(name, NULL),
 gatev("gates",0,2),
 paramv("parameters",0,2),
 machinev("machines",0,2),
 members( "class-data-members", NULL)
{
    take( &gatev );
    take( &paramv );
    take( &machinev );
    take( &members );

    warn = true;
    parentmodp = parentmod;
    idx=0; vectsize=-1;

    notify_inspector = NULL;
    data_for_inspector = NULL;

    /* cModuleType::create() call will create gates, params and machines */
}

cObject *cModule::dup() _CONST
{
    opp_error(eCANTDUP);
    return NO(cObject);
}

cModule& cModule::operator=(_CONST cModule&)
{
    opp_error(eCANTDUP);
    return *NO(cModule);
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
    static char buf[256];
    if (!isVector())
       return name();
    else
    {
       sprintf(buf, "%s[%d]", name(), index() );
       return buf;
    }
}

const char *cModule::fullPath() const
{
    // follows module hierarchy instead of ownership hierarchy
    static char buf[512]; // should be big enough because there's no check!!

    if (parentmodp==NULL)
       strcpy(buf, fullName());
    else
    {
       const char *p = parentmodp->fullPath();
       if (p!=buf) strcpy(buf,p);
       strcat( buf, "." );
       strcat( buf, fullName() );
    }
    return buf;
}

void cModule::addGate(const char *gname, char tp)
{
    if (findGate(gname)>=0)
       opp_error("addGate(): Gate %s.%s already present",
                fullPath(), gname );
    else
    {
       cGate *newg = new cGate( gname, tp );
       newg->setOwnerModule( this, gatev.add( newg ));
    }
}

void cModule::setGateSize(const char *gname, int size)
{
    int pos = findGate(gname,-1);
    if (pos<0) pos = findGate(gname,0);
    if (pos<0)
    {
       opp_error("setGateSize(): Gate %s.%s[] not found",
                 fullPath(), gname );
    }
    else
    {
       char tp = gate(pos)->type();
       int oldsize = gate(pos)->size();

       // remove old vector
       int i;
       for (i=0; i<oldsize; i++)
       {
          cGate *g = (cGate *) gatev.remove( pos+i );
          if (g->fromGate() || g->toGate())
             opp_error("setGateSize(): Too late, gate %s already connected",
                       g->fullPath());
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
}

void cModule::addPar(const char *pname)
{
    int i = findPar(pname);
    if (i!=-1)
       opp_error("addPar(): Parameter %s.%s already present",
                 fullPath(), pname );
    else
    {
        i = paramv.add( new cModulePar(pname) );
        cModulePar *p = (cModulePar *) &par(i);
        p->setOwnerModule( this );
        p->setInput( true );
    }
}

bool cModule::checkInternalConnections() _CONST
{
    int j;
    // Note: This routine only checks if all gates are connected or not.
    //       Does not NOT check where and how they are connected!

    /* check this module compound module if its inside is connected ok */
    for(j=0; j<gates(); j++)
    {
       _CONST cGate *g = gate(j);
       if (g && ((g->type()=='I' && g->toGate()==NULL) ||
                 (g->type()=='O' && g->fromGate()==NULL))
          )
       {
            opp_error("Gate `%s' is not connected", g->fullPath());
            return false;
       }
    }

    /* check submodules */
    for (cSubModIterator submod(*this); !submod.end(); submod++)
    {
       cModule *m = submod();
       for(j=0; j<m->gates(); j++)
       {
          cGate *g = m->gate(j);
          if (g && !g->isConnected())
          {
             opp_error("Gate `%s' is not connected", g->fullPath());
             return false;
          }
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
         opp_error("(%s)%s: Machine par `%s' does not exist",
                   className(), fullPath(), pname );
    else
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
    opp_string pathbuf = path;
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
            if (s[strlen(s)-1]!=']') {
                opp_error("moduleByRelativePath(): syntax error in path `%s'", path);
                return NULL;
            }
            *b='\0';
            modp = modp->submodule(s,atoi(b+1));
        }
    } while ((s=strtok(NULL,"."))!=NULL && modp!=NULL);

    return modp;  // NULL if not found
}

int cModule::findGate(const char *s, int sn) const
{
    bool w = simulation.warnings();
    simulation.setWarnings( false );

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
    simulation.setWarnings( w );

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
    {
        opp_warning(sn<0 ? "(%s)%s: Gate `%s' not found"
                         : "(%s)%s: Gate `%s[%d]' not found",
                    className(),fullName(), s, sn );
        return NO(cGate);
    }
    else
        return gate(i);
}

const cGate *cModule::gate(const char *s, int sn) const
{
  int i = findGate(s,sn);
  if (i==-1)
    {
      opp_warning(sn<0 ? "(%s)%s: Gate `%s' not found"
                  : "(%s)%s: Gate `%s[%d]' not found",
                  className(),fullName(), s, sn );
      return NO(cGate);
    }
  else
    return gate(i);
}

int cModule::findPar(const char *s) _CONST
{
    return paramv.find(s);
}

cPar& cModule::par(int pn)
{
    return *(cPar *)paramv[pn];
}

cPar& cModule::par(const char *s)
{
    int pn = findPar( s );
    if (pn!=-1)
        return par(pn);
    else
        {opp_warning(eNOPARAM,s);return *NO(cPar);}
}

cPar& cModule::ancestorPar(const char *name)
{
    // search parameter in parent modules
    cModule *pmod = this; // ->parentModule() ?
    int k;
    while (pmod && (k=pmod->findPar(name))<0)
        pmod = pmod->parentmodp;
    if (pmod)
        return pmod->par(k);
    else
    {
         opp_warning("Ancestor parameter `%s' not found for module `%s'",
                     name, fullPath() );
         return *NO(cPar);
    }
}

const char *cModule::machinePar(int pn)
{
    cPar *mp = (cPar *)machinev[pn];
    return (!mp) ? NULL : mp->stringValue();
}

const char *cModule::machinePar(const char *pname)
{
    int i = machinev.find( pname );
    if (i==-1)
         return NULL;
    else
         return ((cPar *)machinev[i])->stringValue();
}

bool cModule::isOnLocalMachine() _CONST
{
    return simulation.netInterface()==NULL || simulation.netInterface()->isLocalMachineIn( machinev );
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

void cModule::setDisplayString(int type, const char *s, bool immediate)
{
    if (type<0 || type>=dispNUMTYPES)
    {
         opp_error("(%s)%s: setDisplayString(): type %d out of range",
                   className(), fullPath(), type );
         return;
    }

    dispstr[type] = s;

    if (type==dispENCLOSINGMOD)
    {
         if (notify_inspector) notify_inspector(this,immediate,data_for_inspector);
    }
    else if (type==dispSUBMOD)
    {
         // notify the parent module's inspector
         cModule *p = parentModule();
         if (p && p->notify_inspector) p->notify_inspector(this,immediate,p->data_for_inspector);
    }
}

const char *cModule::displayString(int type)
{
    if (type<0 || type>=dispNUMTYPES)
    {
         opp_error("(%s)%s: displayString(): type %d out of range",
                   className(), fullPath(), type );
         return NULL;
    }

    if ((const char *)dispstr[type] != NULL)
        return dispstr[type];

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
        opp_error("scheduleStart() should have been called for dynamically created"
                  " module `%s'", smod->fullPath());  // this call won't return

    // give back the message to the module
    starter->setOwner(smod);
    starter->setKind(MK_TIMEOUT);

    // call activity(). At this point, initialize() has already been called
    // from cSimulation::startRun(), or manually in the case of dynamically
    // created modules.
    smod->activity();
    smod->end();
}

cSimpleModule::cSimpleModule(_CONST cSimpleModule& mod) :
  cCoroutine(),
  cModule( mod.name(), mod.parentmodp ),
  locals( NULL, NULL),
  putAsideQueue( NULL, NULL, false )
{
    take( &locals );
    take( &putAsideQueue );

    heap = NULL;
    timeoutmsg = NULL;
    setName(mod.name());
    operator=( mod );
}

cSimpleModule::cSimpleModule(const char *name, cModule *parentmod, unsigned stksize) :
  cCoroutine(),
  cModule( name, parentmod ),
  locals( "local-objects", NULL),
  putAsideQueue( "putaside-queue", cMessage::cmpbydelivtime, false )
{
    state = sREADY;
    heap = NULL;

    usesactivity = (stksize!=0);

    take( &locals );
    take( &putAsideQueue );

    // for an activity() module, timeoutmsg will be created in scheduleStart()
    // which must always be called
    timeoutmsg = NULL;

    if (usesactivity)
    {
       // setup coroutine, allocate stack for it
       if (!cCoroutine::setup(cSimpleModule::activate, this, stksize+ev.extraStackForEnvir()))
           opp_error("Cannot allocate %d+%d bytes of stack for module `%s'",
                             stksize,ev.extraStackForEnvir(),fullPath());
    }
}

cSimpleModule::~cSimpleModule()
{
    // the `members' list should be empty by the time we get here
    // timeoutmsg is deleted from cObject destructor (or by the message queue)

    putAsideQueue.clear();
    locals.destructChildren();
    clearHeap();
}

cSimpleModule& cSimpleModule::operator=(_CONST cSimpleModule& other)
{
    if (this==&other) return *this;

    usesactivity = other.usesactivity;
    cModule::operator=(other);
    cCoroutine::operator=( other );
    return *this;
}

void cSimpleModule::info(char *buf)
{               // no cObject::info() !
    sprintf(buf,"%-20.20s (%s,#%d)", fullName(), className(), id() );
}

void cSimpleModule::forEach( ForeachFunc do_fn )
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

void cSimpleModule::setId( int n )
{
    cModule::setId( n );

    if (timeoutmsg)
        timeoutmsg->tomod = n;
}

void cSimpleModule::end()
{
    state = sENDED;   // leave cleanup to reset() or ~cModule()
    simulation.transferToMain();
}

void cSimpleModule::error(const char *fmt...) const
{
    va_list va;
    va_start(va, fmt);
    char buf[256];
    vsprintf(buf,fmt,va);
    va_end(va);

    opp_error(eUSER,buf);
}

void *cSimpleModule::memAlloc(size_t m)
{
#ifdef __MSDOS__
    if (m > 0xFFF0-sizeof(sBlock)) return NULL;        // too large
#endif
    sBlock *p = (sBlock *)new char[m+sizeof(sBlock)];  // allocate
    if (p) {                                           // if OK,
       p->mod  = this;
       p->prev = NULL;                                 //  insert into
       p->next = heap;                                 //  the list
       heap = p;                                       //  as the first
       if (p->next) p->next->prev = p;                 //  item
       return (void *)(p+1);                  // list structure same as
    } else                                    // in a cObject-cHead list
       return NULL;                                    // alloc failed
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
{                   // free all allocated blocks
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
        {opp_error("scheduleStart(): module `%s' already started",fullPath());return;}

    // create timeoutmsg, used as internal timeout message
    timeoutmsg = new cMessage(0,MK_STARTER);
    take( timeoutmsg );

    // initialize message fields
    timeoutmsg->frommod = -1;
    timeoutmsg->tomod = id();
    timeoutmsg->fromgate = timeoutmsg->togate = -1;
    timeoutmsg->sent = 0.0;
    timeoutmsg->delivd = t;

    // use timeoutmsg as the activation message; insert it into the FES
    simulation.msgQueue.insert( timeoutmsg );
}

void cSimpleModule::deleteModule()
{
    // delete pending messages for this module
    for (cMessageHeapIterator iter(simulation.msgQueue); !iter.end(); iter++)
    {
        cMessage *msg = iter();
        if (msg->tomod == id())
              delete simulation.msgQueue.get( msg );
    }

    // adjust gates that were directed here
    bool w=simulation.warnings(); simulation.setWarnings(false);
    for (int i=0; i<gates(); i++)
    {
        cGate *g = gate(i);
        if (g && g->toGate() && g->toGate()->fromGate()==g)
           g->toGate()->setFrom( NO(cGate) );
        if (g && g->fromGate() && g->fromGate()->toGate()==g)
           g->fromGate()->setTo( NO(cGate) );
    }
    simulation.setWarnings(w);

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
       simulation.del( id() );
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
    {
       opp_error(sn<0 ? "send()/sendDelayed(): module has no gate `%s'":
                        "send()/sendDelayed(): module has no gate `%s[%d]'",gatename,sn);
       return -1;
    }
    return sendDelayed(msg, delay, outgate);
}

int cSimpleModule::sendDelayed(cMessage *msg, double delay, int g)
{
    cGate *outgate = gate(g);
    if (outgate==NULL)
        {opp_error("send()/sendDelayed(): module has no gate #%d",g);return -1;}
    return sendDelayed(msg, delay, outgate);
}

int cSimpleModule::sendDelayed(cMessage *msg, double delay, cGate *outgate)
{
    // error checking:
    if (outgate==NULL)
       {opp_error("send()/sendDelayed(): gate pointer is NULL");return -1;}
    if (outgate->type()=='I')
       {opp_error("send()/sendDelayed(): cannot send via an input gate (`%s')",outgate->name());return -1;}
    if (msg==NULL)
        {opp_error("send()/sendDelayed(): message pointer is NULL");return -1;}
    if (msg->owner()!=&(this->locals))
        {opp_error("send()/sendDelayed(): not owner of message `%s'; owner is `%s'",
         msg->name(),msg->owner()->fullPath());return -1;}
    if (delay<0.0)
        {opp_error("send()/sendDelayed(): negative delay %g",delay);return -1;}

    // when in debugging mode, switch back to main program for a moment
    if (pause_in_sendmsg && usesactivity)
    {
        simulation.backtomod = this;  // Ensure that scheduler will select us
        simulation.transferToMain();  // before all other modules
        simulation.backtomod = NULL;
    }

    // set message parameters and send it
    msg->frommod = id();
    msg->fromgate = outgate->id();
    msg->sent   = simTime()+delay;
    msg->delivd = simTime()+delay;   // then it will grow

    outgate->deliver( msg );
    ev.messageSent( msg );
    return 0;
}

int cSimpleModule::sendDirect(cMessage *msg, double propdelay, cModule *mod, int g)
{
    cGate *togate = mod->gate(g);
    if (togate==NULL)
        {opp_error("sendDirect(): module `%s' has no gate #%d",mod->fullPath(),g);return -1;}

    return sendDirect( msg, propdelay, togate );
}

int cSimpleModule::sendDirect(cMessage *msg, double propdelay,
                              cModule *mod, const char *gatename, int sn)
{
    if (!mod) {opp_error("sendDirect(): module ptr is NULL");return 0;}
    cGate *togate = mod->gate(gatename,sn);
    if (togate==NULL)
    {
        opp_error(sn<0 ? "sendDirect(): module `%s' has no gate `%s'":
                         "sendDirect(): module `%s' has no gate `%s[%d]'",
                         mod->fullPath(),gatename,sn);
        return 0;
    }
    return sendDirect( msg, propdelay, togate );
}

int cSimpleModule::sendDirect(cMessage *msg, double propdelay, cGate *togate)
{
    if (togate==NULL)
        {opp_error("sendDirect(): destination gate pointer is NULL");return -1;}
    if (togate->type()=='O')
        {opp_error("sendDirect(): cannot send to an output gate (`%s')",togate->fullPath());return -1;}

    if (msg==NULL)
        {opp_error("sendDirect(): message pointer is NULL");return -1;}
    if (msg->owner()!=&(this->locals))
        {opp_error("sendDirect(): not owner of message `%s'; owner is `%s'",
         msg->name(),msg->owner()->fullPath());return -1;}

    // to help debugging, switch back to main for a moment
    if (pause_in_sendmsg && usesactivity)
    {
        simulation.backtomod = this;  // Ensure that scheduler will select us
        simulation.transferToMain();  // before all other modules
        simulation.backtomod = NULL;
    }
    msg->frommod = id();
    msg->fromgate = -1;
    msg->sent   = simTime();
    msg->delivd = simTime() + propdelay;

    togate->deliver( msg );
    ev.messageSent( msg );
    return 0;
}

int cSimpleModule::scheduleAt(simtime_t t, cMessage *msg)
{
    if (t<simTime())
        {opp_error(eBACKSCHED);return -1;}
    if (msg==NULL)
        {opp_error("scheduleAt(): message pointer is NULL");return -1;}
    if (msg->owner()!=&this->locals && msg->owner()!=this)
        {opp_error("scheduleAt(): not owner of message `%s'; owner is `%s'",
         msg->name(),msg->owner()->fullPath());return -1;}

    // to help debugging, switch back to main for a moment
    if (pause_in_sendmsg && usesactivity)
    {
        simulation.backtomod = this;  // Ensure that scheduler will
        simulation.transferToMain();  //   select us before all other modules
        simulation.backtomod = NULL;
    }
    msg->frommod = msg->tomod = id();
    msg->fromgate = msg->togate = -1;
    msg->sent   = simTime();
    msg->delivd = t;

    ev.messageSent( msg );
    simulation.msgQueue.insert( msg );
    return 0;
}

cMessage *cSimpleModule::cancelEvent(cMessage *msg)
{
    // make sure we really have the message and it is scheduled
    if (msg==NULL)
        {opp_error("cancelEvent(): message pointer is NULL");return NO(cMessage);}
    if (!msg->isScheduled())
    {
        if (putAsideQueue.contains(msg))
            {opp_error("cancelEvent(): message `%s' is in the put-aside queue",
             msg->name());return NULL;}
        else
            {opp_error("cancelEvent(): message `%s' is currently not scheduled,"
             " owner is `%s'", msg->name(),msg->owner()->fullPath());return NULL;}
    }

    // now remove it from future events and return pointer
    simulation.msgQueue.get( msg );
    return msg;
}

int cSimpleModule::syncpoint(simtime_t t, int g)
{
    cGate *gatep = gate(g);
    if (gatep==NULL) {opp_warning(eNOGATE,g);return -1;}

    // find route destination
    cGate *destgate = gatep->destinationGate();

    // if dest is not the network interface mod, ignore this call
    // otherwise, send syncpoint to destination segment
    if (destgate->ownerModule()==simulation.netInterface())
        simulation.netInterface()->send_syncpoint(t, destgate->id());

    return 0;
}

int cSimpleModule::syncpoint(simtime_t t, const char *gatename, int sn)
{
    int g = findGate(gatename,sn);
    if (g<0)
    {
        opp_error(sn<0 ? "syncpoint(): module has no gate `%s'":
                         "syncpoint(): module has no gate `%s[%d]'",gatename,sn);
        return 0;
    }
    return syncpoint( t, g );
}

int cSimpleModule::cancelSyncpoint(simtime_t t, int g)
{
    cGate *gatep = gate(g);
    if (gatep==NULL) {opp_warning(eNOGATE,g);return -1;}

    // find route destination
    cGate *destgate = gatep->destinationGate();

    // if dest is not the network interface mod, ignore this call
    // otherwise, send syncpoint to destination segment
    if (destgate->ownerModule()==simulation.netInterface())
        simulation.netInterface()->send_cancelsyncpoint(t, destgate->id());

    return 0;
}

int cSimpleModule::cancelSyncpoint(simtime_t t, const char *gatename, int sn)
{
    int g = findGate(gatename,sn);
    if (g<0)
    {
        opp_error(sn<0 ? "cancelSyncpoint(): module has no gate `%s'":
                         "cancelSyncpoint(): module has no gate `%s[%d]'",gatename,sn);
        return 0;
    }
    return cancelSyncpoint( t, g );
}

void cSimpleModule::arrived( cMessage *msg, int ongate )
{
    if (state==sENDED) {opp_error(eMODFIN,fullPath());return;}
    msg->tomod = id();
    msg->togate = ongate;

    simulation.msgQueue.insert( msg );
}

void cSimpleModule::wait(simtime_t t)
{
    if (!usesactivity) {opp_error(eNORECV); return;}
    if (t<0) {opp_error(eNEGTIME); t=0;}

    timeoutmsg->delivd = simTime()+t;
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

bool cSimpleModule::isThereMessage() _CONST
{
    cMessage *msg = simulation.msgQueue.peekFirst();
    return msg!=NULL &&
           msg->arrivalModuleId()==id() &&
           msg->arrivalTime()==simTime();
}

cMessage *cSimpleModule::receiveNew()
{
    if (!usesactivity) {opp_error(eNORECV); return NO(cMessage);}

    simulation.transferToMain();
    cMessage *newmsg = simulation.msgQueue.getFirst();

    ev.messageDelivered( newmsg );

    return newmsg;
}

cMessage *cSimpleModule::receiveNew(simtime_t t)
{
    if (!usesactivity) {opp_error(eNORECV); return NO(cMessage);}
    if (t<0)           {opp_error(eNEGTOUT); t=0;}

    timeoutmsg->delivd = simTime()+t;
    simulation.msgQueue.insert( timeoutmsg );

    simulation.transferToMain();
    cMessage *newmsg = simulation.msgQueue.getFirst();

    if (newmsg==timeoutmsg)  // timeout expired
    {
        take(timeoutmsg);
        return NO(cMessage);
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
    if (!usesactivity)  {opp_error(eNORECV);return NO(cMessage);}
    if (t<0)            {opp_error(eNEGTOUT); t=0;}
    cGate *a = gate(g);
    if (a==NULL)        {opp_error(eNOGATE,g);return NO(cMessage);}
    if (a->type()=='O') {opp_error(eOUTGATE,g);return NO(cMessage);}

    if (t!=MAXTIME)
    {
        timeoutmsg->delivd = simTime()+t;
        simulation.msgQueue.insert( timeoutmsg );
        for(;;)
        {
            simulation.transferToMain();
            cMessage *newmsg = simulation.msgQueue.getFirst();
            if (newmsg==timeoutmsg)  // timeout expired
               {take(timeoutmsg); return NO(cMessage);}
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
    {
        opp_error(sn<0 ? "receiveOn(): module has no gate `%s'":
                         "receiveOn(): module has no gate `%s[%d]'",gatename,sn);
        return NO(cMessage);
    }
    return receiveOn( g, t );
}

//-------------

void cSimpleModule::activity()
{
    // default thread function
    // the user must redefine this for the module to do anything useful
    opp_error("You should redefine activity() to do useful work");
}

void cSimpleModule::handleMessage(cMessage *)
{
    // handleMessage is an alternative to activity()
    // this is the default version
    opp_error("You should redefine handleMessage() if you specify zero stack size");
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

simtime_t cSimpleModule::simTime() _CONST
{
    return simulation.simTime();
}

void cSimpleModule::endSimulation()
{
    opp_terminate(eENDSIM);
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
    simulation.recordScalar(name, value);
}

void cSimpleModule::recordScalar(const char *name, const char *text)
{
    simulation.recordScalar(name, text);
}

void cSimpleModule::recordStats(const char *name, cStatistic *stats)
{
    simulation.recordStats(name, stats);
}

//==========================================================================
//=== cCompoundModule - member functions

cCompoundModule::cCompoundModule(_CONST cCompoundModule& mod) :
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

cCompoundModule& cCompoundModule::operator=(_CONST cCompoundModule& mod)
{
    if (this==&mod) return *this;

    cModule::operator=( mod );
    return *this;
}

void cCompoundModule::arrived( cMessage *, int )
{
    opp_error("Message arrived at COMPOUND module `%s'", fullPath());
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
            opp_error("Cannot delete a compound module from one of its submodules!");
            // The reason is that deleteModule() of the currently executing
            // module does not return -- for obvious reasons (we would
            // execute with an already deallocated stack etc).
            // Thus the deletion of the current module would remain unfinished.
            // A solution could be to skip deletion of that very module at first,
            // and delete it only when everything else is deleted.
            // However, this would be clumsy and ugly to implement so
            // I'd rather wait until the real need for it emerges... --VA
            return;
        }
        submod()->deleteModule();
    }

    // adjust gates that were directed here
    bool w=simulation.warnings(); simulation.setWarnings(false);
    for (int i=0; i<gates(); i++)
    {
            cGate *g = gate(i);
            if (g && g->toGate() && g->toGate()->fromGate()==g)
               g->toGate()->setFrom( NO(cGate) );
            if (g && g->fromGate() && g->fromGate()->toGate()==g)
               g->fromGate()->setTo( NO(cGate) );
    }
    simulation.setWarnings(w);

    // delete module
    simulation.del( id() );
}

//==========================================================================
//=== cSubModIterator member functions:

cModule *cSubModIterator::operator++(int)
{
    if (end()) return NO(cModule);
    int last = simulation.lastModuleIndex();
    do {
       i++;
       cModule *mod = &simulation[i];
       if (mod!=NULL && parent==mod->parentModule())
           return mod;
    } while( i<=last);
    i=-1; // at end
    return NO(cModule);
}

//==========================================================================
//=== little helper functions...

static void _connect(cModule *frm, int frg, cModule *tom, int tog)
{
    cGate *srcgate = frm->gate(frg);
    cGate *destgate = tom->gate(tog);

    // cLinkType is stored with the source gate of a connection:
    //srcgate.setLink( li );

    if( srcgate->toGate()==NULL )
       srcgate->setTo( destgate );
    else
       opp_error( "connect(): gate %s already connected",
                  srcgate->fullPath() );

    if( destgate->fromGate()==NULL )
       destgate->setFrom( srcgate );
    else
       opp_error( "connect(): gate %s already connected",
                  destgate->fullPath() );

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

