//======================================================================
//
//  CMODULE.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
//   Member functions of
//    cModule         : common base for module classes
//    cCompoundModule : compound module
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
#include "cmodule.h"
#include "csimplemodule.h"
#include "cgate.h"
#include "cmessage.h"
#include "csimul.h"
#include "carray.h"
#include "ctypes.h"
#include "cpar.h"
#include "cenvir.h"
#include "cexception.h"
#include "cdispstr.h"


//=== static members:
bool cModule::pause_in_sendmsg;

// Note: cModule,cSimpleModule,cCompoundModule are left unregistered.
//   One should never create modules by createOne(cSimpleModule) or the like.

//=========================================================================
//=== cModule - member functions

cModule::cModule(const cModule& mod) :
 cDefaultList(),
 gatev(NULL, 0,2),
 paramv(NULL, 0,2)
{
    //take(&gatev);
    //take(&paramv);

    prevp = nextp = firstsubmodp = NULL;
    idx=0; vectsize=-1;
    fullname = NULL;

    dispstr = NULL;
    parentdispstr = NULL;

    setName(mod.name());
    operator=(mod);
}

cModule::cModule(const char *name, cModule *parentmod) :
 cDefaultList(name),
 gatev("gates",0,2),
 paramv("parameters",0,2)
{
    //take(&gatev);
    //take(&paramv);

    idx=0; vectsize=-1;
    fullname = NULL;

    dispstr = NULL;
    parentdispstr = NULL;

    prevp = nextp = firstsubmodp = NULL;
    if (parentmod)
        parentmod->insertSubmodule(this);

    initDisplayStrings();

    // cModuleType::create() call will create gates, params
}

cModule::~cModule()
{
    // notify envir while module object still exists (more or less)
    ev.moduleDeleted(this);

    // delete submodules
    for (cSubModIterator submod(*this); !submod.end(); )
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

        cModule *mod = submod++;
        delete mod;
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

    // deregister ourselves
    simulation.deregisterModule(this);
    if (parentModule())
        parentModule()->removeSubmodule(this);

    //drop(&gatev); FIXME
    //drop(&paramv);

    delete [] fullname;
    delete dispstr;
    delete parentdispstr;

}

cModule& cModule::operator=(const cModule&)
{
    throw new cException(this, eCANTDUP);
}

void cModule::forEach(ForeachFunc do_fn )
{
    if (do_fn(this,true))
    {
       // Note: everything including paramv, gatev and submodules are on the cDefaultList
       for (int i=0; i<defaultListItems(); i++)
            defaultListGet(i)->forEach(do_fn);
    }
    do_fn(this,false);
}

void cModule::setId(int n)
{
    mod_id = n;
}

void cModule::setIndex(int i, int n)
{
    idx = i;
    vectsize = n;
    initDisplayStrings(); // display strings may depend on module index
}

void cModule::initDisplayStrings()
{
    char dispname[128];   // FIXME buffer overflow danger
    if (parentModule())
    {
        sprintf(dispname, "%s.%s",parentModule()->className(),fullName());
        const char *ds = ev.getDisplayString(simulation.runNumber(),dispname);
        if (!ds)
            {delete dispstr; dispstr = NULL;}
        else
            displayString().parse(ds);
    }

    const char *pds = ev.getDisplayString(simulation.runNumber(),className());
    if (!pds)
        {delete parentdispstr; parentdispstr = NULL;}
    else
        displayStringAsParent().parse(pds);
}

void cModule::insertSubmodule(cModule *mod)
{
    // take ownership
    take(mod);

    // find end of submodule list; TBD maybe introduce a lastsubmodp member?
    cModule *lastsubmodp = firstsubmodp;
    if (lastsubmodp)
        while (lastsubmodp->nextp)
            lastsubmodp = lastsubmodp->nextp;

    // append at end of submodule list
    mod->nextp = NULL;
    mod->prevp = lastsubmodp;
    if (mod->prevp)
        mod->prevp->nextp = mod;
    if (!firstsubmodp)
        firstsubmodp = mod;
}

void cModule::removeSubmodule(cModule *mod)
{
    // NOTE: no drop(mod): anyone can take ownership anyway (because we're soft owners)
    // and otherwise it'd cause trouble if mod itself is in context (it'd get inserted
    // on its own DefaultList)

    // remove from submodule list
    if (mod->nextp)
         mod->nextp->prevp = mod->prevp;
    if (mod->prevp)
         mod->prevp->nextp = mod->nextp;
    if (firstsubmodp==mod)
         firstsubmodp = mod->nextp;
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

    // append parent path + "."
    char *buf = buffer;
    if (parentModule())
    {
       parentModule()->fullPath(buf,bufsize);
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

bool cModule::isSimple() const
{
    return dynamic_cast<const cSimpleModule *>(this) != NULL;
}

cGate *cModule::createGateObject(const char *gname, char tp)
{
    return new cGate(gname,tp);
}

cGate *cModule::addGate(const char *gname, char tp)
{
    if (findGate(gname)>=0)
       throw new cException(this, "addGate(): Gate %s.%s already present", fullPath(), gname);

     cGate *newg = createGateObject(gname, tp);
     newg->setOwnerModule(this, gatev.add( newg ));
     return newg;
}

void cModule::setGateSize(const char *gname, int size)
{
    int pos = findGate(gname,-1);
    if (pos<0)
       pos = findGate(gname,0);
    if (pos<0)
       throw new cException(this,"setGateSize(): Gate %s[] not found", gname);
    if (size<0)
       throw new cException(this,"setGateSize(): negative vector size (%d) requested for gate %s[]", size, gname);

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
        cGate *gate = createGateObject(gname, tp);
        gate->setOwnerModule( this, pos+i );
        gate->setIndex( i, size );
        gatev.addAt( pos+i, gate );
    }
}

cPar *cModule::addPar(const char *pname)
{
    int i = findPar(pname);
    if (i!=-1)
       throw new cException(this,"addPar(): Parameter %s.%s already present",fullPath(), pname);

    i = paramv.add( new cModulePar(pname) );
    cModulePar *p = (cModulePar *) &par(i);
    p->setOwnerModule( this );
    p->setInput( true );
    return p;
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
        pmod = pmod->parentModule();
    if (!pmod)
        throw new cException(this,"has no ancestor parameter called `%s'",name);
    return pmod->par(k);
}

int cModule::buildInside()
{
    // temporarily switch context
    cContextSwitcher tmp(this);

    // check parameters and gates  FIXME only if module interface exists?
    cModuleInterface *iface = moduleType()->moduleInterface();
    if (iface)
        iface->checkParametersOf(this);

    // call doBuildInside() in this context
    doBuildInside();

    return 0;
}

void cModule::deleteModule()
{
    // check this module doesn't contain the executing module somehow
    for (cModule *mod = simulation.contextModule(); mod; mod = mod->parentModule())
        if (mod==this)
            throw new cException(this, "it is not supported to delete module which contains "
                                 "the currently executing simple module");

    delete this;
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

bool cModule::callInitialize(int stage)
{
    // This is the interface for calling initialize().

    // first call it for this module...
    int numStages = numInitStages();
    if (stage < numStages)
    {
        // temporarily switch context for the duration of the call
        cContextSwitcher tmp(this);
        initialize(stage);
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

void cModule::callFinish()
{
    // This is the interface for calling finish().

    // first call it for submods...
    for (cSubModIterator submod(*this); !submod.end(); submod++)
    {
        submod()->callFinish();
    }

    // ...then for this module, in our context
    cContextSwitcher tmp(this);
    finish();
}

cDisplayString& cModule::displayString()
{
    if (!dispstr)
    {
        dispstr = new cDisplayString();
        dispstr->setRoleToModule(this);
    }
    return *dispstr;
}

cDisplayString& cModule::displayStringAsParent()
{
    if (!parentdispstr)
    {
        parentdispstr = new cDisplayString();
        parentdispstr->setRoleToModuleAsParent(this);
    }
    return *parentdispstr;
}

// DEPRECATED
void cModule::setDisplayString(const char *s, bool)
{
    displayString().parse(s);
}

// DEPRECATED
void cModule::setDisplayStringAsParent(const char *s, bool)
{
    displayStringAsParent().parse(s);
}

// DEPRECATED
void cModule::setDisplayString(int type, const char *s, bool)
{
    if (type<0 || type>=dispNUMTYPES)
         throw new cException(this,"setDisplayString(): type %d out of range", type );

    if (type==dispENCLOSINGMOD)
         setDisplayStringAsParent(s);
    else // type==dispSUBMOD
         setDisplayString(s);
}

// DEPRECATED
const char *cModule::displayString(int type)
{
    if (type<0 || type>=dispNUMTYPES)
         throw new cException(this,"displayString(): type %d out of range", type );

    if (type==dispENCLOSINGMOD)
         return displayStringAsParent().getString();
    else // type==dispSUBMOD
         return displayString().getString();
}

void cModule::bubble(const char *text)
{
    ev.bubble(this, text);
}

//==========================================================================
//=== cCompoundModule - member functions

cCompoundModule::cCompoundModule(const cCompoundModule& mod) :
  cModule(NULL,NULL)
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

void cCompoundModule::info(char *buf)
{
    sprintf(buf, "id=%d", id());
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

//==========================================================================
//=== little helper functions...

static void _connect(cModule *frm, int frg, cModule *tom, int tog)
{
    cGate *srcgate = frm->gate(frg);
    cGate *destgate = tom->gate(tog);

    if (srcgate->toGate()!=NULL)
       throw new cException( "connect(): gate %s already connected", srcgate->fullPath() );

    if (destgate->fromGate()!=NULL)
       throw new cException( "connect(): gate %s already connected", destgate->fullPath() );

    srcgate->connectTo( destgate );
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



