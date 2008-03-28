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
#include "cmodule.h"
#include "csimplemodule.h"
#include "cgate.h"
#include "cmessage.h"
#include "csimulation.h"
#include "carray.h"
#include "ccomponenttype.h"
#include "cenvir.h"
#include "cexception.h"
#include "cchannel.h"
#include "cproperties.h"
#include "cproperty.h"
#include "stringutil.h"
#include "util.h"

USING_NAMESPACE


// static members:
std::string cModule::lastmodulefullpath;
const cModule *cModule::lastmodulefullpathmod = NULL;


cModule::cModule()
{
    mod_id = -1;
    idx=0; vectsize=-1;
    fullname = NULL;

    prevp = nextp = firstsubmodp = lastsubmodp = NULL;

    numgatedescs = 0;
    gatedescv = NULL;

    // gates and parameter will be added by cModuleType
}

cModule::~cModule()
{
    // notify envir while module object still exists (more or less)
    EVCB.moduleDeleted(this);

    // delete submodules
    for (SubmoduleIterator submod(this); !submod.end(); )
    {
        if (submod() == (cModule *)simulation.activityModule())
        {
            throw cRuntimeError("Cannot delete a compound module from one of its submodules!");
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
    int numgates = gates();
    for (int i=0; i<numgates; i++)
    {
        cGate *g = gate(i);
        if (g && g->toGate() && g->toGate()->fromGate()==g)
           g->disconnect();
        if (g && g->fromGate() && g->fromGate()->toGate()==g)
           g->fromGate()->disconnect();
    }

    // delete all gates
    for (int i=0; i<numgates; i++)
        delete gatev[i];

    // release gatedescs
    for (int i=0; i<numgatedescs; i++)
    {
        cGate::Desc& desc = gatedescv[i];
        if (desc.isInout()) {
            cGate::stringPool.release(cGate::stringPool.peek(opp_concat(desc.namep,"$i")));
            cGate::stringPool.release(cGate::stringPool.peek(opp_concat(desc.namep,"$o")));
        }
        cGate::stringPool.release(desc.namep);
    }

    // deregister ourselves
    if (mod_id!=-1)
        simulation.deregisterModule(this);
    if (parentModule())
        parentModule()->removeSubmodule(this);

    delete [] fullname;
}

void cModule::forEachChild(cVisitor *v)
{
    for (int i=0; i<numparams; i++)
        v->visit(&paramv[i]);

    int numgates = gatev.size();
    for (int i=0; i<numgates; i++)
        if (gatev[i])
            v->visit(gatev[i]);

    cDefaultList::forEachChild(v);
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

cModule *cModule::parentModule() const
{
    return dynamic_cast<cModule *>(owner());
}

void cModule::setName(const char *s)
{
    cOwnedObject::setName(s);

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
        // stop at the toplevel module (don't go up to cSimulation);
        // plus, cache the result, expecting more hits from this module
        if (parentModule()==NULL)
            lastmodulefullpath = fullName();
        else
            lastmodulefullpath = parentModule()->fullPath() + "." + fullName();
        lastmodulefullpathmod = this;
    }
    return lastmodulefullpath;
}

bool cModule::isSimple() const
{
    return dynamic_cast<const cSimpleModule *>(this) != NULL;
}

cGate *cModule::createGateObject(cGate::Desc *desc)
{
    return new cGate(desc);
}

cGate::Desc& cModule::addGateDesc()
{
    // allocate new array
    cGate::Desc *newv = new cGate::Desc[numgatedescs+1];
    memcpy(newv, gatedescv, numgatedescs*sizeof(cGate::Desc));

    // adjust pointers of already existing gates
    int n = gatev.size();
    for (int i=0; i<n; i++)
    {
        cGate *g = gatev[i];
        ASSERT(gatedescv <= g->desc && g->desc < gatedescv+numgatedescs);
        if (g)
            g->desc = newv + (g->desc - gatedescv);
    }

    // install the new array and return its last element
    delete [] gatedescv;
    gatedescv = newv;
    return gatedescv[numgatedescs++];
}

int cModule::findGateDesc(const char *gatename, char& suffix) const
{
    // determine whether gatename contains "$i"/"$o" suffix
    int len = strlen(gatename);
    suffix = (len>2 && gatename[len-2]=='$') ? gatename[len-1] : 0;

    // and search accordingly
    if (!suffix)
    {
        int n = numgatedescs;
        for (int i=0; i<n; i++)
        {
            const cGate::Desc& desc = gatedescv[i];
            if (desc.namep && desc.namep[0]==gatename[0] && strcmp(desc.namep, gatename)==0)
                return i;
        }
    }
    else
    {
        if (suffix!='i' && suffix!='o')
            return -1;  // invalid suffix ==> no such gate

        // ignore suffix during search
        for (int i=0; i<numgatedescs; i++)
        {
            const cGate::Desc& desc = gatedescv[i];
            if (desc.namep && desc.namep[0]==gatename[0] && strncmp(desc.namep, gatename, len-2)==0 && (int)strlen(desc.namep)==len-2)
            {
                if (!desc.isInout())
                    return -1;  // looking for a "foo$i"/"foo$o" gate, but found a non-inout "foo" gate
                return i;
            }
        }
    }
    return -1;
}

const cGate::Desc& cModule::gateDesc(const char *gatename, char& suffix) const
{
    int descId = findGateDesc(gatename, suffix);
    if (descId<0)
        throw cRuntimeError(this, "no `%s' or `%s[]' gate", gatename, gatename);
    return gatedescv[descId];
}

void cModule::addGate(const char *gatename, cGate::Type type, bool isvector)
{
    char suffix;
    if (findGateDesc(gatename, suffix)>=0)
        throw cRuntimeError(this, "addGate(): Gate %s.%s already present", fullPath().c_str(), gatename);
    if (suffix)
        throw cRuntimeError(this, "addGate(): Wrong gate name `%s', must be without `$i'/`$o' suffix", gatename);

    // create desc for new gate (or gate vector)
    cGate::Desc& desc = addGateDesc();
    desc.namep = cGate::stringPool.get(gatename);
    desc.ownerp = this;
    desc.size = isvector ? 0 : -1;

    // allocate two other strings as well, for cGate::name
    if (type==cGate::INOUT)
    {
        cGate::stringPool.get(opp_concat(gatename,"$i"));
        cGate::stringPool.get(opp_concat(gatename,"$o"));
    }

    // now: create gate object, maybe two (name$i and name$o)
    if (type==cGate::INPUT || type==cGate::INOUT)
    {
        if (isvector)
            desc.inGateId = 0;  // must be >=0 to store type=INPUT/INOUT
        else {
            cGate *newgate = createGateObject(&desc);
            newgate->setGateId(desc.inGateId = gatev.size());
            gatev.push_back(newgate);
        }
    }
    if (type==cGate::OUTPUT || type==cGate::INOUT)
    {
        if (isvector)
            desc.outGateId = 0;  // must be >=0 to store type=OUTPUT/INOUT
        else {
            cGate *newgate = createGateObject(&desc);
            newgate->setGateId(desc.outGateId = gatev.size());
            gatev.push_back(newgate);
        }
    }
}

void cModule::setGateSize(const char *gatename, int newsize)
{
    char suffix;
    cGate::Desc& desc = const_cast<cGate::Desc&>(gateDesc(gatename, suffix));
    if (suffix)
        throw cRuntimeError(this, "setGateSize(): wrong gate name `%s', suffix `$i'/`$o' not accepted here", gatename);
    if (desc.isScalar())
        throw cRuntimeError(this, "setGateSize(): gate `%s' is not a vector gate", gatename);
    if (newsize<0)
        throw cRuntimeError(this, "setGateSize(): negative vector size (%d) requested for gate %s[]", newsize, gatename);

    if (desc.isInput())
        desc.inGateId = moveGates(desc.inGateId, desc.size, newsize, &desc);
    if (desc.isOutput())
        desc.outGateId = moveGates(desc.outGateId, desc.size, newsize, &desc);
    desc.size = newsize;
}

int cModule::moveGates(int oldpos, int oldsize, int newsize, cGate::Desc *desc)
{
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
                throw cRuntimeError(this,"setGateSize(): Cannot shrink gate vector, gate %s already connected", gate->fullPath().c_str());
            gatev[oldpos+i] = NULL;
            delete gate;
        }
        // shrink stl container if the gate vector was at its end
        if (oldpos+newsize > (int)gatev.size())
            gatev.resize(oldpos+newsize);

        return oldpos;
    }

    // OK, it'll be expand.
    // find a new id range: newsize empty adjacent slots in the array
    int newpos = 0;
    for (i=0; i < newsize && newpos+i < (int)gatev.size(); i++)
    {
        // if position free, go on to next one
        if (gatev[newpos+i]==NULL)
           continue;

        // also regard old position of this gate vector as unoccupied
        if (newpos+i >= oldpos && newpos+i < oldpos+oldsize)
           continue;

        // position occupied -- must start over from a new position
        newpos += i+1; //FIXME obvious room for improvement: skip gate vectors at once
        i=-1;
    }

    // expand stl container if necessary
    if (newpos+newsize > (int)gatev.size())
        gatev.resize(newpos+newsize);

    // move existing gates from oldpos to newpos.
    if (newpos!=oldpos)
    {
        // Note: it's always OK to begin copying from the beginning, because
        // we either move the vector backwards (newpos<oldpos) or to a completely new,
        // non-overlapping region (newpos>=oldpos+oldsize); the assert() below makes
        // sure this is really the case.
        ASSERT(newpos<oldpos || newpos>=oldpos+oldsize);
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
        gatev[newpos+i] = createGateObject(desc);
    }

    // let all gates know who they are again
    for (i=0; i<newsize; i++)
    {
        cGate *gate = gatev[newpos+i];
        gate->setGateId(newpos+i);
    }
    return newpos;
}

cGate *cModule::getOrCreateFirstUnconnectedGate(const char *gatename, char suffix,
                                                bool inside, bool expand)
{
    // look up gate
    char suffix1;
    cGate::Desc& desc = const_cast<cGate::Desc&>(gateDesc(gatename, suffix1));
    if (!desc.isVector())
        throw cRuntimeError(this,"getOrCreateFirstUnconnectedGate(): gate `%s' is not a vector gate", gatename);
    if (suffix1 && suffix)
        throw cRuntimeError(this,"getOrCreateFirstUnconnectedGate(): gate `%s' AND suffix `%c' given", gatename, suffix);
    suffix = suffix | suffix1;

    // determine whether input or output gates to check
    bool inputside;
    if (!suffix)
    {
        if (desc.isInout())
            throw cRuntimeError(this,"getOrCreateFirstUnconnectedGate(): inout gate specified but no suffix");
        inputside = desc.isInput();
    }
    else
    {
        if (suffix!='i' && suffix!='o')
            throw cRuntimeError(this,"getOrCreateFirstUnconnectedGate(): wrong gate name suffix `%c'", suffix);
        inputside = suffix=='i';
    }

    // find first unconnected gate
    int baseId = inputside ? desc.inGateId : desc.outGateId;
    int oldsize = desc.size;
    if (inside)
    {
        for (int i=baseId; i<baseId+oldsize; i++)
           if (!gatev[i]->isConnectedInside())
               return gatev[i];
    }
    else
    {
        for (int i=baseId; i<baseId+oldsize; i++)
           if (!gatev[i]->isConnectedOutside())
               return gatev[i];
    }

    // no unconnected gate: expand gate vector
    if (expand)
    {
        // expand gatesize by one (code from setGateSize())
        int newsize = oldsize + 1;
        if (desc.isInput())
            desc.inGateId = moveGates(desc.inGateId, desc.size, newsize, &desc);
        if (desc.isOutput())
            desc.outGateId = moveGates(desc.outGateId, desc.size, newsize, &desc);
        desc.size = newsize;
        int newBaseId = inputside ? desc.inGateId : desc.outGateId;
        return gatev[newBaseId + oldsize];
    }

    return NULL;
}

void cModule::getOrCreateFirstUnconnectedGatePair(const char *gatename,
                                                  bool inside, bool expand,
                                                  cGate *&gatein, cGate *&gateout)
{
    // look up gate
    char suffix;
    cGate::Desc& desc = const_cast<cGate::Desc&>(gateDesc(gatename, suffix));
    if (!desc.isVector())
        throw cRuntimeError(this,"getOrCreateFirstUnconnectedGatePair(): gate `%s' is not a vector gate", gatename);
    if (suffix)
        throw cRuntimeError(this,"getOrCreateFirstUnconnectedGatePair(): inout gate expected, without `$i'/`$o' suffix");

    // find first unconnected gate
    int oldsize = desc.size;
    if (inside)
    {
        for (int i=0; i<oldsize; i++)
           if (!gatev[desc.inGateId+i]->isConnectedInside() && !gatev[desc.outGateId+i]->isConnectedInside())
               {gatein = gatev[desc.inGateId+i]; gateout = gatev[desc.outGateId+i]; return;}
    }
    else
    {
        for (int i=0; i<oldsize; i++)
           if (!gatev[desc.inGateId+i]->isConnectedOutside() && !gatev[desc.outGateId+i]->isConnectedOutside())
               {gatein = gatev[desc.inGateId+i]; gateout = gatev[desc.outGateId+i]; return;}
    }

    // no unconnected gate: expand gate vector
    if (expand)
    {
        // expand gatesize by one (code from setGateSize())
        int newsize = oldsize + 1;
        desc.inGateId = moveGates(desc.inGateId, desc.size, newsize, &desc);
        desc.outGateId = moveGates(desc.outGateId, desc.size, newsize, &desc);
        desc.size = newsize;

        gatein = gatev[desc.inGateId+oldsize];
        gateout = gatev[desc.outGateId+oldsize];
        return;
    }

    gatein = gateout = NULL;
}

bool cModule::checkInternalConnections() const
{
    // Note: This routine only checks if all gates are connected or not.
    // It does NOT check where and how they are connected!

    // check this compound module if its inside is connected ok
    for(int j=0; j<gates(); j++)
    {
       const cGate *g = gate(j);
       if (g && g->size()!=0 && !g->isConnectedInside())
            throw cRuntimeError(this,"Gate `%s' is not connected to submodule (or output gate of same module)", g->fullPath().c_str());
    }

    // check submodules
    for (SubmoduleIterator submod(this); !submod.end(); submod++)
    {
       cModule *m = submod();
       for(int j=0; j<m->gates(); j++)
       {
          cGate *g = m->gate(j);
          if (g && g->size()!=0 && !g->isConnectedOutside())
             throw cRuntimeError(this,"Gate `%s' is not connected to sibling or parent module", g->fullPath().c_str());
       }
    }
    return true;
}

int cModule::findSubmodule(const char *submodname, int idx)
{
    for (SubmoduleIterator i(this); !i.end(); i++)
        if (i()->isName(submodname) &&
            ((idx==-1 && !i()->isVector()) || i()->index()==idx)
           )
            return i()->id();
    return -1;
}

cModule *cModule::submodule(const char *submodname, int idx)
{
    for (SubmoduleIterator i(this); !i.end(); i++)
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
                throw cRuntimeError(this,"moduleByRelativePath(): syntax error in path `%s'", path);
            *b='\0';
            modp = modp->submodule(s,atoi(b+1));
        }
    } while ((s=strtok(NULL,"."))!=NULL && modp!=NULL);

    return modp;  // NULL if not found
}

int cModule::findGate(const char *gatename, int index) const
{
    char suffix;
    int descId = findGateDesc(gatename, suffix);
    if (descId<0)
        return -1;  // gatename not found

    const cGate::Desc& desc = gatedescv[descId];
    if (desc.isInout() && !suffix)
        return -1;  // inout gate cannot be referenced without "$i" or "$o" suffix

    if (desc.isScalar())
    {
        // gate is scalar
        if (index!=-1)
            return -1;  // wrong: scalar gate referenced with index
        if (suffix)
            return suffix=='i' ? desc.inGateId : desc.outGateId;
        return desc.inGateId>=0 ? desc.inGateId : desc.outGateId;
    }
    else
    {
        // gate is vector
        if (index<0 || index>=desc.size)
            return -1;  // index not specified (-1) or out of range
        if (suffix)
            return suffix=='i' ? desc.inGateId+index : desc.outGateId+index;
        return desc.isInput() ? desc.inGateId+index : desc.outGateId+index;
    }
}

cGate *cModule::gate(const char *gatename, int index)
{
    int i = findGate(gatename, index);
    if (i==-1)
    {
        // no such gate -- make some extra effort to issue a helpful error message
        std::string fullgatename = index<0 ? gatename : opp_stringf("%s[%d]", gatename, index);
        char suffix;
        int descId = findGateDesc(gatename, suffix);
        if (descId<0)
            throw cRuntimeError(this, "has no gate named `%s'", fullgatename.c_str());
        const cGate::Desc& desc = gatedescv[descId];
        if (index==-1 && desc.isVector())
            throw cRuntimeError(this, "has no gate named `%s' -- "   //FIXME say "attempt to access a vector gate as a scalar gate"
                "use gate(\"%s\", 0) to refer to first gate of gate vector `%s[]', and "
                "occurrences of gate(\"%s\")->size() should be replaced with gateSize(\"%s\")",
                gatename, gatename, gatename, gatename, gatename);
        if (index!=-1 && desc.isScalar())
            throw cRuntimeError(this, "has no gate named `%s' -- use gate(\"%s\") to refer to scalar gate `%s'",
                                fullgatename.c_str(), gatename, gatename);
        if (!suffix && desc.isInout())
            throw cRuntimeError(this, "has no gate named `%s' -- "   //FIXME say "attempt to access a scalar gate as a vector gate"
                "one cannot reference an inout gate as a whole, only its input or output "
                "component, by appending \"$i\" or \"$o\" to the gate name",
                fullgatename.c_str());
        if (desc.isVector() && (index<0 || index>=desc.size))
            throw cRuntimeError(this, "has no gate named `%s' -- vector index out of bounds", fullgatename.c_str());
        // whatever else -- we should never get here
        throw cRuntimeError(this, "has no gate named `%s'", fullgatename.c_str());
    }
    return gate(i);
}

cGate *cModule::gateHalf(const char *gatename, cGate::Type type, int index)
{
    std::string tmp = std::string(gatename) + (type==cGate::INPUT ? "$i" : "$o");
    return gate(tmp.c_str(), index); //FIXME give a more efficient impl, without string operations
}

cGate *cModule::gate(int k)
{
    if (k < 0 || k >= (int)gatev.size())
        throw cRuntimeError(this, "gate id %d out of range", k);
    return gatev[k];
}

bool cModule::hasGate(const char *gatename, int index) const
{
    char suffix;
    int descId = findGateDesc(gatename, suffix);
    if (descId<0)
        return false;
    const cGate::Desc& desc = gatedescv[descId];
    return index==-1 ? true : (index>=0 && index<desc.size);
}

cGate::Type cModule::gateType(const char *gatename) const
{
    char suffix;
    const cGate::Desc& desc = gateDesc(gatename, suffix);
    if (suffix)
        return suffix=='i' ? cGate::INPUT : cGate::OUTPUT;
    return desc.isInput() ? (desc.isOutput() ? cGate::INOUT : cGate::INPUT)
                          : (desc.isOutput() ? cGate::OUTPUT : cGate::NONE);
}

bool cModule::isGateVector(const char *gatename) const
{
    char suffix;
    const cGate::Desc& desc = gateDesc(gatename, suffix);
    return desc.isVector();
}

int cModule::gateSize(const char *gatename) const
{
    char suffix;
    const cGate::Desc& desc = gateDesc(gatename, suffix);
    return desc.isScalar() ? 1 : desc.size;
}

cPar& cModule::ancestorPar(const char *name)
{
    // search parameter in parent modules
    cModule *pmod = this;
    int k;
    while (pmod && (k=pmod->findPar(name))<0)
        pmod = pmod->parentModule();
    if (!pmod)
        throw cRuntimeError(this,"has no ancestor parameter called `%s'",name);
    return pmod->par(k);
}

void cModule::finalizeParameters()
{
    cComponent::finalizeParameters(); // this will read input parameters

    // temporarily switch context
    cContextSwitcher tmp(this);
    cContextTypeSwitcher tmp2(CTX_BUILD);

    // set up gate vectors (their sizes may depend on the parameter settings)
    moduleType()->addGatesTo(this);
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
            throw cRuntimeError(this, "it is not supported to delete module which contains "
                                      "the currently executing simple module");

    delete this;
}

void cModule::changeParentTo(cModule *mod)
{
    if (!mod)
        throw cRuntimeError(this, "changeParentTo(): got NULL pointer");

    // gates must be unconnected to avoid connections break module hierarchy rules
    int numgates = gates();
    cGate *g;
    for (int i=0; i<numgates; i++)
        if (g=gate(i), g && g->isConnectedOutside())
            throw cRuntimeError(this, "changeParentTo(): gates of the module must not be "
                                      "connected (%s is connected now)", g->fullName());

    // cannot insert module under one of its own submodules
    for (cModule *m = mod; m; m = m->parentModule())
        if (m==this)
            throw cRuntimeError(this, "changeParentTo(): cannot move module under one of its own submodules");

    // do it
    cModule *oldparent = parentModule();
    oldparent->removeSubmodule(this);
    mod->insertSubmodule(this);

    // notify environment
    EVCB.moduleReparented(this,oldparent);
}

void cModule::callInitialize()
{
    // Perform stage==0 for channels, then stage==0 for submodules, then
    // stage==1 for channels, stage==1 for modules, etc.
    //
    // Rationale: modules sometimes want to send messages already in stage==0,
    // and channels must be ready for that at that time, i.e. passed at least
    // stage==0.
    //
    cContextTypeSwitcher tmp(CTX_INITIALIZE);
    int stage = 0;
    bool moreChannelStages = true, moreModuleStages = true;
    while (moreChannelStages || moreModuleStages)
    {
        if (moreChannelStages)
            moreChannelStages = initializeChannels(stage);
        if (moreModuleStages)
            moreModuleStages = initializeModules(stage);
        ++stage;
    }
}

bool cModule::initializeChannels(int stage)
{
    if (simulation.contextType()!=CTX_INITIALIZE)
        throw cRuntimeError("internal function initializeChannels() may only be called via callInitialize()");

    // initialize channels directly under this module
    bool moreStages = false;
    for (ChannelIterator chan(this); !chan.end(); chan++)
        if (chan()->initializeChannel(stage))
            moreStages = true;

    // then recursively initialize channels within our submodules too
    for (SubmoduleIterator submod(this); !submod.end(); submod++)
        if (submod()->initializeChannels(stage))
            moreStages = true;

    return moreStages;
}

bool cModule::initializeModules(int stage)
{
    if (simulation.contextType()!=CTX_INITIALIZE)
        throw cRuntimeError("internal function initializeModules() may only be called via callInitialize()");

    // first call initialize(stage) for this module...
    int numStages = numInitStages();
    if (stage < numStages)
    {
        ev << "Initializing module " << fullPath() << ", stage " << stage << "\n";

        // switch context for the duration of the call
        cContextSwitcher tmp(this);
        initialize(stage);
    }

    // then recursively initialize submodules
    bool moreStages = stage < numStages-1;
    for (SubmoduleIterator submod(this); !submod.end(); submod++)
        if (submod()->initializeModules(stage))
            moreStages = true;

    return moreStages;
}

void cModule::callFinish()
{
    // This is the interface for calling finish().

    // first call it for submodules and channels...
    for (ChannelIterator chan(this); !chan.end(); chan++)
        chan()->callFinish();
    for (SubmoduleIterator submod(this); !submod.end(); submod++)
        submod()->callFinish();

    // ...then for this module, in our context: save parameters, then finish()
    cContextSwitcher tmp(this);
    cContextTypeSwitcher tmp2(CTX_FINISH);
    recordParametersAsScalars();
    finish();
}

//----

void cModule::ChannelIterator::init(const cModule *parentmodule)
{
    // loop through the gates of parentmodule and its submodules
    // to fill in the channels[] vector
    bool parent = false;
    channels.clear();
    for (SubmoduleIterator it(parentmodule); !parent; it++)
    {
        const cModule *mod = !it.end() ? it() : (parent=true,parentmodule);

        int n = mod->gates();
        for (int i=0; i<n; i++)
        {
            const cGate *gate = mod->gate(i);
            cGate::Type wantedGateType = parent ? cGate::INPUT : cGate::OUTPUT;
            if (gate && gate->channel() && gate->type()==wantedGateType)
                channels.push_back(gate->channel());
        }
    }

    // reset iterator position too
    k = 0;
}


