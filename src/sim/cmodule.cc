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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include <string.h>          // strcpy
#include <algorithm>
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
#include "simutil.h"

USING_NAMESPACE

//XXX rewrite to use Desc& not Desc* ?
//XXX clean up gate(int id)
//XXX revise "FIXME" error messages

//TODO consider:
//  call cGate::clearFullnamePool() between events, and add to doc:
//  "lifetime of the string returned by getFullName() is the simulation event"?


// static members:
std::string cModule::lastmodulefullpath;
const cModule *cModule::lastmodulefullpathmod = NULL;


cModule::cModule()
{
    mod_id = -1;
    idx = 0;
    vectsize = -1;
    fullname = NULL;

    prevp = nextp = firstsubmodp = lastsubmodp = NULL;

    descvSize = 0;
    descv = NULL;

    // gates and parameters will be added by cModuleType
}

cModule::~cModule()
{
    // notify envir while module object still exists (more or less)
    EVCB.moduleDeleted(this);

    // delete submodules
    for (SubmoduleIterator submod(this); !submod.end(); )
    {
        if (submod() == (cModule *)simulation.getActivityModule())
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
    for (GateIterator i(this); !i.end(); i++)
    {
        cGate *gate = i();
        if (gate->getToGate() && gate->getToGate()->getFromGate()==gate)
           gate->disconnect();
        if (gate->getFromGate() && gate->getFromGate()->getToGate()==gate)
           gate->getFromGate()->disconnect();
    }

    // delete all gates
    clearGates();

    // deregister ourselves
    if (mod_id!=-1)
        simulation.deregisterModule(this);
    if (getParentModule())
        getParentModule()->removeSubmodule(this);

    delete [] fullname;
}

void cModule::forEachChild(cVisitor *v)
{
    for (int i=0; i<numparams; i++)
        v->visit(&paramv[i]);

    for (GateIterator i(this); !i.end(); i++)
        v->visit(i());

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
    if (fullname)
    {
        delete [] fullname;
        fullname = NULL;
    }
    if (isVector())
    {
        fullname = new char[opp_strlen(getName())+10];
        strcpy(fullname, getName());
        opp_appendindex(fullname, getIndex());
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

    // cached module getFullPath() possibly became invalid
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

    // cached module getFullPath() possibly became invalid
    lastmodulefullpathmod = NULL;
}

cModule *cModule::getParentModule() const
{
    return dynamic_cast<cModule *>(getOwner());
}

void cModule::setName(const char *s)
{
    cOwnedObject::setName(s);

    // update fullname
    if (isVector())
    {
        if (fullname)  delete [] fullname;
        fullname = new char[opp_strlen(getName())+10];
        sprintf(fullname, "%s[%d]", getName(), getIndex());
    }
}

const char *cModule::getFullName() const
{
    // if not in a vector, normal getName() will do
    return isVector() ? fullname : getName();
}

std::string cModule::getFullPath() const
{
    if (lastmodulefullpathmod!=this)
    {
        // stop at the toplevel getModule(don't go up to cSimulation);
        // plus, cache the result, expecting more hits from this module
        if (getParentModule()==NULL)
            lastmodulefullpath = getFullName();
        else
            lastmodulefullpath = getParentModule()->getFullPath() + "." + getFullName();
        lastmodulefullpathmod = this;
    }
    return lastmodulefullpath;
}

bool cModule::isSimple() const
{
    return dynamic_cast<const cSimpleModule *>(this) != NULL;
}

cProperties *cModule::getProperties() const
{
    cModule *parent = getParentModule();
    cComponentType *type = getComponentType();
    cProperties *props;
    if (parent)
        props = parent->getComponentType()->getSubmoduleProperties(getName(), type->getFullName());
    else
        props = type->getProperties();
    return props;
}

cGate *cModule::createGateObject(cGate::Type)
{
    return new cGate();
}

cModule::NamePool cModule::namePool;

void cModule::disposeGateObject(cGate *gate, bool checkConnected)
{
    if (gate)
    {
        if (checkConnected && (gate->getFromGate() || gate->getToGate()))
            throw cRuntimeError(this, "Cannot delete gate `%s', it is still connected", gate->getFullName());
        EVCB.gateDeleted(gate);
        delete gate;
    }
}

void cModule::disposeGateDesc(cGate::Desc *desc, bool checkConnected)
{
    if (desc->namep)  // is alive
    {
        if (!desc->isVector()) {
            disposeGateObject(desc->inputgate, checkConnected);
            disposeGateObject(desc->outputgate, checkConnected);
        }
        else {
            for (int j=0; j<desc->gateSize(); j++) {
                if (desc->inputgatev)
                    disposeGateObject(desc->inputgatev[j], checkConnected);
                if (desc->outputgatev)
                    disposeGateObject(desc->outputgatev[j], checkConnected);
            }
            delete [] desc->inputgatev;
            delete [] desc->outputgatev;
        }
        desc->namep = NULL; // leave shared Name struct in the pool
    }
}

void cModule::clearGates()
{
    for (int i=0; i<descvSize; i++)
        disposeGateDesc(descv + i, false);
    delete [] descv;
    descv = NULL;
    descvSize = 0;
}

void cModule::clearNamePools()
{
    namePool.clear();
    cGate::clearFullnamePool();
}

void cModule::adjustGateDesc(cGate *gate, cGate::Desc *newvec)
{
    if (gate) {
        // the "desc" pointer in each gate needs to be updated when descv[] gets reallocated
        ASSERT(descv <= gate->desc && gate->desc < descv+descvSize);
        gate->desc = newvec + (gate->desc - descv);
    }
}

cGate::Desc *cModule::addGateDesc(const char *gatename, cGate::Type type, bool isVector)
{
    // check limits
    if (isVector) {
        if (descvSize >= MAX_VECTORGATES)
            throw cRuntimeError(this, "cannot add gate `%s[]': too many vector gates (limit is %d)", gatename, MAX_VECTORGATES);
    }
    else {
        if (descvSize >= MAX_SCALARGATES)
            throw cRuntimeError(this, "cannot add gate `%s': too many scalar gates (limit is %d)", gatename, MAX_SCALARGATES);
    }

    // allocate new array
    //TODO preallocate a larger descv[] in advance?
    cGate::Desc *newv = new cGate::Desc[descvSize+1];
    memcpy(newv, descv, descvSize*sizeof(cGate::Desc));

    // adjust desc pointers in already existing gates
    for (int i=0; i<descvSize; i++)
    {
        cGate::Desc *desc = descv + i;
        if (desc->namep) {
            if (!desc->isVector()) {
                adjustGateDesc(desc->inputgate, newv);
                adjustGateDesc(desc->outputgate, newv);
            }
            else {
                for (int j=0; j<desc->gateSize(); j++) {
                    if (desc->inputgatev)
                        adjustGateDesc(desc->inputgatev[j], newv);
                    if (desc->outputgatev)
                        adjustGateDesc(desc->outputgatev[j], newv);
                }
            }
        }
    }

    // install the new array and get its last element
    delete [] descv;
    descv = newv;
    cGate::Desc *newDesc = descv + descvSize++;

    // configure this gatedesc with name and type
    cGate::Name key(gatename, type);
    NamePool::iterator it = namePool.find(key);
    if (it==namePool.end())
        it = namePool.insert(key).first;
    newDesc->namep = const_cast<cGate::Name*>(&(*it));
    newDesc->size = isVector ? 0 : -1;
    return newDesc;
}

int cModule::findGateDesc(const char *gatename, char& suffix) const
{
    // determine whether gatename contains "$i"/"$o" suffix
    int len = strlen(gatename);
    suffix = (len>2 && gatename[len-2]=='$') ? gatename[len-1] : 0;
    if (suffix && suffix!='i' && suffix!='o')
        return -1;  // invalid suffix ==> no such gate

    // and search accordingly
    switch (suffix) {
        case '\0':
            for (int i=0; i<descvSize; i++) {
                const cGate::Desc *desc = descv + i;
                if (desc->namep && strcmp(desc->namep->name.c_str(), gatename)==0)
                    return i;
            }
            break;
        case 'i':
            for (int i=0; i<descvSize; i++) {
                const cGate::Desc *desc = descv + i;
                if (desc->namep && strcmp(desc->namep->namei.c_str(), gatename)==0)
                    return i;
            }
            break;
        case 'o':
            for (int i=0; i<descvSize; i++) {
                const cGate::Desc *desc = descv + i;
                if (desc->namep && strcmp(desc->namep->nameo.c_str(), gatename)==0)
                    return i;
            }
            break;
    }
    return -1;
}

cGate::Desc *cModule::gateDesc(const char *gatename, char& suffix) const
{
    int descIndex = findGateDesc(gatename, suffix);
    if (descIndex<0)
        throw cRuntimeError(this, "no such gate: `%s'", gatename);
    return descv + descIndex;
}

cGate *cModule::gate(int id)
{
    unsigned int h = id & GATEID_HMASK;
    if (h==0) {
        unsigned int descIndex = id>>1;
        if (descIndex >= (unsigned int) descvSize)
            return NULL; //XXX as assert?
        cGate::Desc *desc = descv + descIndex;
        ASSERT(desc->namep); // not deleted
        ASSERT(!desc->isVector()); //FIXME too many asserts here?
        ASSERT((id&1)==0 ? desc->getType()!=cGate::OUTPUT : desc->getType()!=cGate::INPUT);
        //return (id&1)==0 ? desc->inputgate : desc->outputgate;
        cGate *g = (id&1)==0 ? desc->inputgate : desc->outputgate;
        ASSERT((id&1)==(g->pos&1));
        return g;
    }
    else {
        unsigned int descIndex = (h>>GATEID_LBITS)-1;
        if (descIndex >= (unsigned int) descvSize)
            return NULL;  //FIXME error/assert instead
        cGate::Desc *desc = descv + descIndex;
        ASSERT(desc->namep); // not deleted
        unsigned int index = id & (GATEID_LMASK>>1);
        //FIXME assert isvector, type, etc
        if (index>=(unsigned int)desc->gateSize())
            return NULL;  //FIXME error/assert instead
        bool isOutput = id & (1<<(GATEID_LBITS-1));  // L's MSB
        return isOutput ? desc->outputgatev[index] : desc->inputgatev[index];
    }
}

void cModule::addGate(const char *gatename, cGate::Type type, bool isVector)
{
    char suffix;
    if (findGateDesc(gatename, suffix)>=0)
        throw cRuntimeError(this, "addGate(): Gate `%s' already present", gatename);
    if (suffix)
        throw cRuntimeError(this, "addGate(): Wrong gate name `%s', must be without `$i'/`$o' suffix", gatename);

    // create desc for new gate (or gate vector)
    cGate::Desc *desc = addGateDesc(gatename, type, isVector);
    desc->ownerp = this;

    // if scalar gate, create gate object(s); gate vectors are created with size 0.
    if (!isVector)
    {
        if (type!=cGate::OUTPUT)
        {
            cGate *newGate = createGateObject(type);
            desc->setInputGate(newGate);
            EVCB.gateCreated(newGate);
        }
        if (type!=cGate::INPUT)
        {
            cGate *newGate = createGateObject(type);
            desc->setOutputGate(newGate);
            EVCB.gateCreated(newGate);
        }
    }
}

static void reallocGatev(cGate **&v, int oldSize, int newSize)
{
    if (oldSize != newSize) {
        cGate **newv = new cGate*[newSize];
        memcpy(newv, v, (oldSize<newSize?oldSize:newSize)*sizeof(cGate*));
        if (newSize>oldSize)
            memset(newv+oldSize, 0, (newSize-oldSize)*sizeof(cGate*));
        delete [] v;
        v = newv;
    }
}

void cModule::setGateSize(const char *gatename, int newSize)
{
    char suffix;
    int descIndex = findGateDesc(gatename, suffix);
    if (descIndex<0)
        throw cRuntimeError(this, "no `%s' or `%s[]' gate", gatename, gatename);
    if (suffix)
        throw cRuntimeError(this, "setGateSize(): wrong gate name `%s', suffix `$i'/`$o' not accepted here", gatename);
    cGate::Desc *desc = descv + descIndex;
    if (!desc->isVector())
        throw cRuntimeError(this, "setGateSize(): gate `%s' is not a vector gate", gatename);
    if (newSize<0)
        throw cRuntimeError(this, "setGateSize(): negative vector size (%d) requested for gate %s[]", newSize, gatename);
    if (newSize>MAX_VECTORGATESIZE)
        throw cRuntimeError(this, "setGateSize(): vector size for gate %s[] too large (%d), limit is %d", gatename, newSize, MAX_VECTORGATESIZE);
    int oldSize = desc->size;
    cGate::Type type = desc->getType();

    // we need to allocate more (to have good gate++ performance) but we
    // don't want to store the capacity -- so we'll always calculated the
    // capacity from the current size (by rounding it up to the nearest
    // multiple of 2, 4, 16, 64.
    int oldCapacity = cGate::Desc::capacityFor(oldSize);
    int newCapacity = cGate::Desc::capacityFor(newSize);

    // shrink?
    if (newSize < oldSize)
    {
        // remove excess gates
        for (int i=oldSize-1; i>=newSize; i--)
        {
            // check & notify
            if (type!=cGate::OUTPUT) {
                cGate *gate = desc->inputgatev[i];
                if (gate->getFromGate() || gate->getToGate())
                    throw cRuntimeError(this,"setGateSize(): Cannot shrink gate vector, gate %s already connected", gate->getFullPath().c_str());
                EVCB.gateDeleted(gate);
            }
            if (type!=cGate::INPUT) {
                cGate *gate = desc->outputgatev[i];
                if (gate->getFromGate() || gate->getToGate())
                    throw cRuntimeError(this,"setGateSize(): Cannot shrink gate vector, gate %s already connected", gate->getFullPath().c_str());
                EVCB.gateDeleted(gate);
            }

            // actually delete
            desc->size = i;
            if (type!=cGate::OUTPUT) {
                delete desc->inputgatev[i];
                desc->inputgatev[i] = NULL;
            }
            if (type!=cGate::INPUT) {
                delete desc->outputgatev[i];
                desc->outputgatev[i] = NULL;
            }
        }

        // shrink container
        if (type!=cGate::OUTPUT)
            reallocGatev(desc->inputgatev, oldCapacity, newCapacity);
        if (type!=cGate::INPUT)
            reallocGatev(desc->outputgatev, oldCapacity, newCapacity);
        desc->size = newSize;
    }

    // expand?
    if (newSize > oldSize)
    {
        // expand container (slots newSize..newCapacity will stay unused NULL for now)
        if (type!=cGate::OUTPUT)
            reallocGatev(desc->inputgatev, oldCapacity, newCapacity);
        if (type!=cGate::INPUT)
            reallocGatev(desc->outputgatev, oldCapacity, newCapacity);

        // set new size beforehand, because EVCB.gateCreate() calls getId()
        // which assumes that gate->index < gateSize.
        desc->size = newSize;

        // and create the additional gates
        for (int i=oldSize; i<newSize; i++)
        {
            if (type!=cGate::OUTPUT) {
                cGate *newGate = createGateObject(type);
                desc->setInputGate(newGate, i);
                EVCB.gateCreated(newGate);
            }
            if (type!=cGate::INPUT) {
                cGate *newGate = createGateObject(type);
                desc->setOutputGate(newGate, i);
                EVCB.gateCreated(newGate);
            }
        }
    }
}

int cModule::gateSize(const char *gatename) const
{
    char dummy;
    const cGate::Desc *desc = gateDesc(gatename, dummy);
    return desc->gateSize();
}

cGate *cModule::gate(const char *gatename, int index)
{
    char suffix;
    const cGate::Desc *desc = gateDesc(gatename, suffix);
    if (desc->getType()==cGate::INOUT && !suffix)
        throw cRuntimeError(this, "Inout gate `%s' cannot be referenced without $i/$o suffix", gatename);
    bool isInput = (suffix=='i' || desc->getType()==cGate::INPUT);

    if (!desc->isVector())
    {
        // gate is scalar
        if (index!=-1)
            throw cRuntimeError(this, "Scalar gate `%s' referenced with index", gatename);
        return isInput ? desc->inputgate : desc->outputgate;
    }
    else
    {
        // gate is vector
        if (index<0)
            throw cRuntimeError(this, "%s when accessing vector gate `%s", (index==-1?"No gate index specified":"Negative gate index specified"), gatename);
        if (index>=desc->size)
            throw cRuntimeError(this, "Gate index %d out of range when accessing vector gate `%s[]' (size %d)", index, gatename, desc->size);
        return isInput ? desc->inputgatev[index] : desc->outputgatev[index];
    }
}

int cModule::findGate(const char *gatename, int index) const
{
    char suffix;
    int descIndex = findGateDesc(gatename, suffix);
    if (descIndex<0)
        return -1;  // no such gate name
    const cGate::Desc *desc = descv + descIndex;
    if (desc->getType()==cGate::INOUT && !suffix)
        return -1;  // inout gate cannot be referenced without "$i" or "$o" suffix
    bool isInput = (suffix=='i' || desc->getType()==cGate::INPUT);

    if (!desc->isVector())
    {
        // gate is scalar
        if (index!=-1)
            return -1;  // wrong: scalar gate referenced with index
        return isInput ? desc->inputgate->getId() : desc->outputgate->getId();
    }
    else
    {
        // gate is vector
        if (index<0 || index>=desc->size)
            return -1;  // index not specified (-1) or out of range
        return isInput ? desc->inputgatev[index]->getId() : desc->outputgatev[index]->getId();
    }
}

cGate *cModule::gateHalf(const char *gatename, cGate::Type type, int index)
{
    char dummy;
    const cGate::Desc *desc = gateDesc(gatename, dummy);
    const char *nameWithSuffix = (type==cGate::INPUT) ? desc->namep->namei.c_str() : desc->namep->nameo.c_str();
    return gate(nameWithSuffix, index);
}

bool cModule::hasGate(const char *gatename, int index) const
{
    char suffix;
    int descIndex = findGateDesc(gatename, suffix);
    if (descIndex<0)
        return false;
    const cGate::Desc *desc = descv + descIndex;
    return index==-1 ? true : (index>=0 && index<desc->size);
}

void cModule::deleteGate(const char *gatename)
{
    char suffix;
    cGate::Desc *desc = gateDesc(gatename, suffix);
    if (suffix)
        throw cRuntimeError(this, "Cannot delete one half of an inout gate: `%s'", gatename);
    disposeGateDesc(desc, true);
}

std::vector<const char *> cModule::getGateNames() const
{
    std::vector<const char *> result;
    for (int i=0; i<descvSize; i++)
        if (descv[i].namep)
            result.push_back(descv[i].namep->name.c_str());
    return result;
}

cGate::Type cModule::gateType(const char *gatename) const
{
    char suffix;
    const cGate::Desc *desc = gateDesc(gatename, suffix);
    if (suffix)
        return suffix=='i' ? cGate::INPUT : cGate::OUTPUT;
    else
        return desc->namep->type;
}

bool cModule::isGateVector(const char *gatename) const
{
    char dummy;
    const cGate::Desc *desc = gateDesc(gatename, dummy);
    return desc->isVector();
}

//XXX test code:
//    bool operator()(cGate *a, cGate *b) {
//        printf("   comparing %s, %s ==> ", (a?a->getFullName():NULL), (b?b->getFullName():NULL) );
//        bool x = (a && a->isConnectedInside()) > (b && b->isConnectedInside());
//        printf("%d > %d : ", (a && a->isConnectedInside()), (b && b->isConnectedInside()));
//        printf("%d\n", x);
//        return x;
//    }

struct less_gateConnectedInside {
    bool operator()(cGate *a, cGate *b) {return (a && a->isConnectedInside()) > (b && b->isConnectedInside());}
};

struct less_gateConnectedOutside {
    bool operator()(cGate *a, cGate *b) {return (a && a->isConnectedOutside()) > (b && b->isConnectedOutside());}
};

struct less_gatePairConnectedInside {
    cGate **otherv;
    less_gatePairConnectedInside(cGate **otherv) {this->otherv = otherv;}
    bool operator()(cGate *a, cGate *b) {
        return (a && a->isConnectedInside()) > (b && b->isConnectedInside()) &&
               (a && otherv[a->getIndex()]->isConnectedInside()) > (b && otherv[b->getIndex()]->isConnectedInside());
    }
};

struct less_gatePairConnectedOutside {
    cGate **otherv;
    less_gatePairConnectedOutside(cGate **otherv) {this->otherv = otherv;}
    bool operator()(cGate *a, cGate *b) {
        return (a && a->isConnectedOutside()) > (b && b->isConnectedOutside()) &&
               (a && otherv[a->getIndex()]->isConnectedOutside()) > (b && otherv[b->getIndex()]->isConnectedOutside());
    }
};


cGate *cModule::getOrCreateFirstUnconnectedGate(const char *gatename, char suffix,
                                                bool inside, bool expand)
{
    // look up gate
    char suffix1;
    cGate::Desc *desc = const_cast<cGate::Desc *>(gateDesc(gatename, suffix1));
    if (!desc->isVector())
        throw cRuntimeError(this,"getOrCreateFirstUnconnectedGate(): gate `%s' is not a vector gate", gatename);
    if (suffix1 && suffix)
        throw cRuntimeError(this,"getOrCreateFirstUnconnectedGate(): gate `%s' AND suffix `%c' given", gatename, suffix);
    suffix = suffix | suffix1;

    // determine whether input or output gates to check
    bool inputSide;
    if (!suffix)
    {
        if (desc->getType()==cGate::INOUT)
            throw cRuntimeError(this,"getOrCreateFirstUnconnectedGate(): inout gate specified but no suffix");
        inputSide = desc->getType()==cGate::INPUT;
    }
    else
    {
        if (suffix!='i' && suffix!='o')
            throw cRuntimeError(this,"getOrCreateFirstUnconnectedGate(): wrong gate name suffix `%c'", suffix);
        inputSide = suffix=='i';
    }

    // gate array we'll be looking at
    cGate **gatev = inputSide ? desc->inputgatev : desc->outputgatev;
    int oldSize = desc->size;

    // since gates get connected from the beginning of the vector, we can do
    // binary search for the first unconnected gate. In the (rare) case when
    // gates are not connected in order (i.e. some high gate indices get
    // connected before lower ones), binary search may not be able to find the
    // "holes" (unconnected gates) and we expand the gate unnecessarily.
    // Note: NULL is used as a synonym for "any unconnected gate" (see less_* struct).
    cGate **it = inside ?
        std::lower_bound(gatev, gatev+oldSize, (cGate *)NULL, less_gateConnectedInside()) :
        std::lower_bound(gatev, gatev+oldSize, (cGate *)NULL, less_gateConnectedOutside());
    if (it != gatev+oldSize)
        return *it;

    // no unconnected gate: expand gate vector
    if (expand)
    {
        setGateSize(desc->namep->name.c_str(), oldSize+1);   //FIXME spare extra name lookup!!!
        return inputSide ? desc->inputgatev[oldSize] : desc->outputgatev[oldSize];
    }
    else
    {
        // gate is not allowed to expand, so let's try harder to find an unconnected gate
        // (in case the binary search missed it)
        for (int i=0; i<oldSize; i++)
            if (inside ? !gatev[i]->isConnectedInside() : !gatev[i]->isConnectedOutside())
                return gatev[i];
        return NULL; // sorry
    }
}

void cModule::getOrCreateFirstUnconnectedGatePair(const char *gatename,
                                                  bool inside, bool expand,
                                                  cGate *&gatein, cGate *&gateout)
{
    // look up gate
    char suffix;
    cGate::Desc *desc = const_cast<cGate::Desc *>(gateDesc(gatename, suffix));
    if (!desc->isVector())
        throw cRuntimeError(this,"getOrCreateFirstUnconnectedGatePair(): gate `%s' is not a vector gate", gatename);
    if (suffix)
        throw cRuntimeError(this,"getOrCreateFirstUnconnectedGatePair(): inout gate expected, without `$i'/`$o' suffix");

    // the gate arrays we'll work with
    int oldSize = desc->size;
    cGate **inputgatev = desc->inputgatev;
    cGate **outputgatev = desc->outputgatev;

    // binary search for the first unconnected gate -- see explanation in method above
    cGate **it = inside ?
        std::lower_bound(inputgatev, inputgatev+oldSize, (cGate *)NULL, less_gatePairConnectedInside(outputgatev)) :
        std::lower_bound(inputgatev, inputgatev+oldSize, (cGate *)NULL, less_gatePairConnectedOutside(outputgatev));
    if (it != inputgatev+oldSize) {
        gatein = *it;
        gateout = outputgatev[gatein->getIndex()];
        return;
    }

    // no unconnected gate: expand gate vector
    if (expand)
    {
        setGateSize(desc->namep->name.c_str(), oldSize+1); //FIXME spare extra name lookup!!!
        gatein = desc->inputgatev[oldSize];
        gateout = desc->outputgatev[oldSize];
        return;
    }
    else
    {
        // gate is not allowed to expand, so let's try harder to find an unconnected gate
        // (in case the binary search missed it)
        for (int i=0; i<oldSize; i++)
            if (inside ? !inputgatev[i]->isConnectedInside() : !inputgatev[i]->isConnectedOutside())
                if (inside ? !outputgatev[i]->isConnectedInside() : !outputgatev[i]->isConnectedOutside())
                    {gatein = inputgatev[i]; gateout = outputgatev[i]; return;}
        gatein = gateout = NULL; // sorry
    }
}

int cModule::gateCount() const
{
    int n = 0;
    for (int i=0; i<descvSize; i++)
    {
        cGate::Desc *desc = descv + i;
        if (desc->namep) {
            if (!desc->isVector())
                n += (desc->getType()==cGate::INOUT) ? 2 : 1;
            else
                n += (desc->getType()==cGate::INOUT) ? 2*desc->size : desc->size;
        }
    }
    return n;
}

cGate *cModule::gateByOrdinal(int k) const
{
    GateIterator it(this);
    it += k;
    return it.end() ? NULL : it();
}

bool cModule::checkInternalConnections() const
{
    // Note: This routine only checks if all gates are connected or not.
    // It does NOT check where and how they are connected!

    // check this compound module if its inside is connected ok
    for (GateIterator i(this); !i.end(); i++)
    {
       cGate *g = i();
       if (g->size()!=0 && !g->isConnectedInside())
            throw cRuntimeError(this,"Gate `%s' is not connected to getSubmodule(or output gate of same module)", g->getFullPath().c_str());
    }

    // check submodules
    for (SubmoduleIterator submod(this); !submod.end(); submod++)
    {
        cModule *m = submod();
        for (GateIterator i(m); !i.end(); i++)
        {
            cGate *g = i();
            if (g->size()!=0 && !g->isConnectedOutside())
                throw cRuntimeError(this,"Gate `%s' is not connected to sibling or parent module", g->getFullPath().c_str());
        }
    }
    return true;
}

int cModule::findSubmodule(const char *submodname, int idx)
{
    for (SubmoduleIterator i(this); !i.end(); i++)
        if (i()->isName(submodname) &&
            ((idx==-1 && !i()->isVector()) || i()->getIndex()==idx)
           )
            return i()->getId();
    return -1;
}

cModule *cModule::getSubmodule(const char *submodname, int idx)
{
    for (SubmoduleIterator i(this); !i.end(); i++)
        if (i()->isName(submodname) &&
            ((idx==-1 && !i()->isVector()) || i()->getIndex()==idx)
           )
            return i();
    return NULL;
}

cModule *cModule::getModuleByRelativePath(const char *path)
{
    // start tokenizing the path
    opp_string pathbuf(path);
    char *s = strtok(pathbuf.buffer(),".");

    // search starts from this module
    cModule *modp = this;

    // match components of the path
    do {
        char *b;
        if ((b=strchr(s,'['))==NULL)
            modp = modp->getSubmodule(s);  // no index given
        else
        {
            if (s[strlen(s)-1]!=']')
                throw cRuntimeError(this,"getModuleByRelativePath(): syntax error in path `%s'", path);
            *b='\0';
            modp = modp->getSubmodule(s,atoi(b+1));
        }
    } while ((s=strtok(NULL,"."))!=NULL && modp!=NULL);

    return modp;  // NULL if not found
}

cPar& cModule::getAncestorPar(const char *name)
{
    // search parameter in parent modules
    cModule *pmod = this;
    int k;
    while (pmod && (k=pmod->findPar(name))<0)
        pmod = pmod->getParentModule();
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
    getModuleType()->addGatesTo(this);
}

int cModule::buildInside()
{
    // temporarily switch context
    cContextSwitcher tmp(this);
    cContextTypeSwitcher tmp2(CTX_BUILD);

    // call doBuildInside() in this context
    doBuildInside();

    return 0;
}

void cModule::deleteModule()
{
    // check this module doesn't contain the executing module somehow
    for (cModule *mod = simulation.getContextModule(); mod; mod = mod->getParentModule())
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
    cGate *g;
    for (GateIterator i(this); !i.end(); i++)
        if (g=i(), g->isConnectedOutside())
            throw cRuntimeError(this, "changeParentTo(): gates of the module must not be "
                                      "connected (%s is connected now)", g->getFullName());

    // cannot insert module under one of its own submodules
    for (cModule *m = mod; m; m = m->getParentModule())
        if (m==this)
            throw cRuntimeError(this, "changeParentTo(): cannot move module under one of its own submodules");

    // do it
    cModule *oldparent = getParentModule();
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

bool cModule::callInitialize(int stage)
{
    cContextTypeSwitcher tmp(CTX_INITIALIZE);
    bool moreChannelStages = initializeChannels(stage);
    bool moreModuleStages = initializeModules(stage);
    return moreChannelStages || moreModuleStages;
}

bool cModule::initializeChannels(int stage)
{
    if (simulation.getContextType()!=CTX_INITIALIZE)
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
    if (simulation.getContextType()!=CTX_INITIALIZE)
        throw cRuntimeError("internal function initializeModules() may only be called via callInitialize()");

    // first call initialize(stage) for this module...
    int numStages = numInitStages();
    if (stage < numStages)
    {
        // switch context for the duration of the call
        Enter_Method("initialize");
        ev << "Initializing module " << getFullPath() << ", stage " << stage << "\n";
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

void cModule::GateIterator::init(const cModule *module)
{
    this->module = module;
    descIndex = 0;
    isOutput = false;
    index = 0;

    while (!end() && current()==NULL)
        advance();
}

void cModule::GateIterator::advance()
{
    cGate::Desc *desc = module->descv + descIndex;

    if (desc->namep) {
        if (isOutput==false && desc->getType()==cGate::OUTPUT) {
            isOutput = true;
            return;
        }

        if (desc->isVector()) {
            if (index < desc->size-1) {
                index++;
                return;
            }
            index = 0;
        }
        if (isOutput==false && desc->getType()!=cGate::INPUT) {
            isOutput = true;
            return;
        }
    }
    if (descIndex < module->descvSize) {
        descIndex++;
        isOutput = false;
        index = 0;
    }
}

bool cModule::GateIterator::end() const
{
    return descIndex >= module->descvSize;
}

cGate *cModule::GateIterator::current() const
{
    if (descIndex >= module->descvSize)
        return NULL;
    cGate::Desc *desc = module->descv + descIndex;
    if (!desc->namep)
        return NULL; // deleted gate
    if (isOutput==false && desc->getType()==cGate::OUTPUT)
        return NULL; // isOutput still incorrect
    if (!desc->isVector())
        return isOutput ? desc->outputgate : desc->inputgate;
    else if (desc->size==0)
        return NULL;
    else
        return isOutput ? desc->outputgatev[index] : desc->inputgatev[index];
}

cGate *cModule::GateIterator::operator++(int)
{
    if (end())
        return NULL;
    cGate *gate = NULL;
    do {
        advance();
    } while (!end() && (gate=current())==NULL);
    return gate;
}

cGate *cModule::GateIterator::operator+=(int k)
{
    //FIXME this is the primitive solution. We could do better, like skip gate vectors at once, etc
    for (int i=0; i<k; i++)
        (*this)++;
    return (*this)();
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

        for (GateIterator i(mod); !i.end(); i++)
        {
            const cGate *gate = i();
            cGate::Type wantedGateType = parent ? cGate::INPUT : cGate::OUTPUT;
            if (gate && gate->getChannel() && gate->getType()==wantedGateType)
                channels.push_back(gate->getChannel());
        }
    }

    // reset iterator position too
    k = 0;
}


