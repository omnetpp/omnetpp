//======================================================================
//  CMODULE.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//======================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

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
#include "csimulation.h"
#include "carray.h"
#include "ccomponenttype.h"
#include "cenvir.h"
#include "cexception.h"
#include "cdisplaystring.h"
#include "util.h"

// static members:
std::string cModule::lastmodulefullpath;
const cModule *cModule::lastmodulefullpathmod = NULL;


cModule::cModule()
{
    mod_id = -1;
    idx=0; vectsize=-1;
    fullname = NULL;

    rngmap = NULL;
    rngmapsize = 0;

    dispstr = NULL;
    bgdispstr = NULL;

    prevp = nextp = firstsubmodp = lastsubmodp = NULL;

    ev_enabled = true; // may get overridden from cEnvir::moduleCreated() hook

    // gates and parameter will be added by cModuleType
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
            throw new cRuntimeError("Cannot delete a compound module from one of its submodules!");
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
           g->disconnect();
        if (g && g->fromGate() && g->fromGate()->toGate()==g)
           g->fromGate()->disconnect();
    }

    // delete all gates
    for (int i=0; i<gates(); i++)
        delete gatev[i];

    // deregister ourselves
    if (mod_id!=-1)
        simulation.deregisterModule(this);
    if (parentModule())
        parentModule()->removeSubmodule(this);

    delete [] rngmap;

    delete [] fullname;
    delete dispstr;
    delete bgdispstr;
}

void cModule::forEachChild(cVisitor *v)
{
    // Note: everything including gatev and submodules are on the cDefaultList
    for (int i=0; i<defaultListItems(); i++)
         v->visit(defaultListGet(i));
}

void cModule::setId(int n)
{
    mod_id = n;
}

void cModule::setIndex(int i, int n)
{
    idx = i;
    vectsize = n;

    // update fullname
    if (fullname)  delete [] fullname;
    fullname = NULL;
    if (isVector())
    {
        fullname = new char[opp_strlen(name())+10];
        strcpy(fullname, name());
        opp_appendindex(fullname, index());
    }
}

void cModule::insertSubmodule(cModule *mod)
{
    // take ownership
    take(mod);

    // append at end of submodule list
    mod->nextp = NULL;
    mod->prevp = lastsubmodp;
    if (mod->prevp)
        mod->prevp->nextp = mod;
    if (!firstsubmodp)
        firstsubmodp = mod;
    lastsubmodp = mod;

    // cached module fullPath() possibly became invalid
    lastmodulefullpathmod = NULL;
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
    if (lastsubmodp==mod)
         lastsubmodp = mod->prevp;

    // this is not strictly needed but makes it cleaner
    mod->prevp = mod->nextp = NULL;

    // cached module fullPath() possibly became invalid
    lastmodulefullpathmod = NULL;
}

void cModule::setName(const char *s)
{
    cObject::setName(s);

    // update fullname
    if (isVector())
    {
        if (fullname)  delete [] fullname;
        fullname = new char[opp_strlen(name())+10];
        sprintf(fullname, "%s[%d]", name(), index());
    }
}

const char *cModule::fullName() const
{
    // if not in a vector, normal name() will do
    return isVector() ? fullname : name();
}

std::string cModule::fullPath() const
{
    if (lastmodulefullpathmod!=this)
    {
        // cache the result
        lastmodulefullpath = fullPath(fullpathbuf,MAX_OBJECTFULLPATH);
        lastmodulefullpathmod = this;
    }
    return lastmodulefullpath;
}

const char *cModule::fullPath(char *buffer, int bufsize) const
{
    // check we got a decent buffer
    if (!buffer || bufsize<4)
    {
        if (buffer) buffer[0]='\0';
        return "(fullPath(): no buffer or buffer too small)";
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

cGate *cModule::addGate(const char *gname, char tp, bool isvector)
{
    if (findGate(gname)>=0)
       throw new cRuntimeError(this, "addGate(): Gate %s.%s already present", fullPath().c_str(), gname);

    cGate *newg = createGateObject(gname, tp);
    take(newg);
    gatev.push_back(newg);
    if (isvector)
        newg->setIndex(0,0);
    return newg;
}

int cModule::setGateSize(const char *gname, int newsize)
{
    int oldpos = findGate(gname,-1);
    if (oldpos<0)
       oldpos = findGate(gname,0);
    if (oldpos<0)
        throw new cRuntimeError(this,"setGateSize(): Gate %s[] not found", gname);
    if (newsize<0)
        throw new cRuntimeError(this,"setGateSize(): negative vector size (%d) requested for gate %s[]", newsize, gname);

    char tp = gate(oldpos)->type();
    int oldsize = gate(oldpos)->size();
    if (oldsize==newsize)
        return oldpos;
    if (oldsize==0)
        oldsize = 1;  // a zero-size vector is actually a single gate with vecsize=0

    // first see if gate vector has to be shrunk
    int i;
    if (newsize<=oldsize)
    {
        // shrink to zero vector size?
        bool zerosize = false;
        if (newsize==0)
        {
            // we need one gate to represent a zero-size vector
            zerosize = true;
            newsize = 1;
        }
        // simply remove excess gates
        for (i=newsize; i<oldsize; i++)
        {
            cGate *gate = gatev[oldpos+i];
            if (gate->fromGate() || gate->toGate())
                throw new cRuntimeError(this,"setGateSize(): Cannot shrink gate vector, gate %s already connected", gate->fullPath().c_str());
            gatev[oldpos+i] = NULL;
            delete gate;
        }
        // shrink stl container if the gate vector was at its end
        if (oldpos+newsize > gatev.size())
            gatev.resize(oldpos+newsize);

        // and tell remaining gates the new vector size
        for (i=0; i<newsize; i++)
        {
            cGate *gate = gatev[oldpos+i];
            gate->setIndex(i, zerosize ? 0 : newsize);
        }
        return oldpos;
    }

    // OK, it'll be expand.
    // find a new id range: newsize empty adjacent slots in the array
    int newpos = 0;
    for (i=0; i<newsize && newpos+i<gatev.size(); i++)
    {
        // if position free, go on to next one
        if (gatev[newpos+i]==NULL)
           continue;

        // also regard old position of this gate vector as unoccupied
        if (newpos+i>=oldpos && newpos+i<oldpos+oldsize)
           continue;

        // position occupied -- must start over from a new position
        newpos += i+1; //FIXME obvious room for improvement: skip gate vectors at once
        i=-1;
    }

    // expand stl container if necessary
    if (newpos+newsize > gatev.size())
        gatev.resize(newpos+newsize);

    // move existing gates from oldpos to newpos.
    if (newpos!=oldpos)
    {
        // Note: it's always OK to begin copying from the beginning, because
        // we either move the vector backwards (newpos<oldpos) or to a completely new,
        // non-overlapping region (newpos>=oldpos+oldsize); the assert() below makes
        // sure this is really the case.
        assert(newpos<oldpos || newpos>=oldpos+oldsize);
        for (i=0; i<oldsize; i++)
        {
            cGate *gate = gatev[oldpos+i];
            gatev[oldpos+i] = NULL;
            gatev[newpos+i] = gate;
        }
    }

    // and create additional gates
    for (i=oldsize; i<newsize; i++)
    {
        cGate *gate = createGateObject(gname, tp);
        gatev[newpos+i] = gate;
    }

    // let all gates know who they are again
    for (i=0; i<newsize; i++)
    {
        cGate *gate = gatev[newpos+i];
        gate->setOwnerModule(this, newpos+i);
        gate->setIndex(i, newsize);
    }
    return newpos;
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
       if (g && g->size()!=0 && !g->isConnectedInside())
            throw new cRuntimeError(this,"Gate `%s' is not connected to submodule (or output gate of same module)", g->fullPath().c_str());
    }

    // check submodules
    for (cSubModIterator submod(*this); !submod.end(); submod++)
    {
       cModule *m = submod();
       for(j=0; j<m->gates(); j++)
       {
          cGate *g = m->gate(j);
          if (g && g->size()!=0 && !g->isConnectedOutside())
             throw new cRuntimeError(this,"Gate `%s' is not connected to sibling or parent module", g->fullPath().c_str());
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
                throw new cRuntimeError(this,"moduleByRelativePath(): syntax error in path `%s'", path);
            *b='\0';
            modp = modp->submodule(s,atoi(b+1));
        }
    } while ((s=strtok(NULL,"."))!=NULL && modp!=NULL);

    return modp;  // NULL if not found
}

int cModule::findGate(const char *s, int index) const
{
    const cGate *g = 0; // initialize g to prevent compiler warnings
    int i = 0, n = gates();
    while (i<n)
    {
       g = gate(i);
       if (!g)
          i++;
       else if (!g->isName(s))
          i += g->size()==0 ? 1 : g->size();
       else
          break;
    }

    if (i>=n)
       return -1;
    else if (index<0)
       // for index=-1, we return the 0th gate. This is not very clean but
       // necessary for code like n=gate("out_vector")->size() to work.
       return i;
    else if (index<g->size())
       // assert may be removed later
       {assert( gate(i+index)->index()==index ); return i+index;}
    else
       return -1;
}

cGate *cModule::gate(const char *s, int index)
{
    int i = findGate(s,index);
    if (i==-1)
        return NULL;
    return gate(i);
}

cGate *cModule::gate(int k)
{
    if (k<0 || k>=gatev.size())
        throw new cRuntimeError(this, "gate id %d out of range", k);
    return gatev[k];
}

int cModule::gateSize(const char *gatename) const
{
    int i = findGate(gatename);
    if (i==-1)
        return 0;
    return gate(i)->size();
}

cPar& cModule::ancestorPar(const char *name)
{
    // search parameter in parent modules
    cModule *pmod = this;
    int k;
    while (pmod && (k=pmod->findPar(name))<0)
        pmod = pmod->parentModule();
    if (!pmod)
        throw new cRuntimeError(this,"has no ancestor parameter called `%s'",name);
    return pmod->par(k);
}

int cModule::buildInside()
{
    // temporarily switch context
    cContextSwitcher tmp(this);
    cContextTypeSwitcher tmp2(CTX_BUILD);

/*FIXME replace this with new code!!!!
    // check parameters and gates  FIXME only if module interface exists?
    cNEDDeclaration *iface = moduleType()->moduleNEDDeclaration();
    if (iface)
        iface->checkParametersOf(this);
*/

    // call doBuildInside() in this context
    doBuildInside();

    return 0;
}

void cModule::deleteModule()
{
    // check this module doesn't contain the executing module somehow
    for (cModule *mod = simulation.contextModule(); mod; mod = mod->parentModule())
        if (mod==this)
            throw new cRuntimeError(this, "it is not supported to delete module which contains "
                                    "the currently executing simple module");

    delete this;
}

void cModule::changeParentTo(cModule *mod)
{
    if (!mod)
        throw new cRuntimeError(this, "changeParentTo(): got NULL pointer");

    // gates must be unconnected to avoid connections break module hierarchy rules
    int numgates = gates();
    cGate *g;
    for (int i=0; i<numgates; i++)
        if (g=gate(i), g && g->isConnectedOutside())
            throw new cRuntimeError(this, "changeParentTo(): gates of the module must not be "
                                    "connected (%s is connected now)", g->fullName());

    // cannot insert module under one of its own submodules
    for (cModule *m = mod; m; m = m->parentModule())
        if (m==this)
            throw new cRuntimeError(this, "changeParentTo(): cannot move module under one of its own submodules");

    // do it
    cModule *oldparent = parentModule();
    oldparent->removeSubmodule(this);
    mod->insertSubmodule(this);

    // notify environment
    ev.moduleReparented(this,oldparent);
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

void cModule::handleParameterChange(const char *)
{
    // Called when a module parameter changes.
    // Can be redefined by user.
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
        cContextTypeSwitcher tmp2(CTX_INITIALIZE);
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
    cContextTypeSwitcher tmp2(CTX_FINISH);
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

cDisplayString& cModule::backgroundDisplayString()
{
    if (!bgdispstr)
    {
        bgdispstr = new cDisplayString();
        bgdispstr->setRoleToModuleBackground(this);
    }
    return *bgdispstr;
}

// DEPRECATED
void cModule::setDisplayString(const char *s, bool)
{
    displayString().parse(s);
}

// DEPRECATED
void cModule::setBackgroundDisplayString(const char *s, bool)
{
    backgroundDisplayString().parse(s);
}

void cModule::bubble(const char *text)
{
    ev.bubble(this, text);
}




