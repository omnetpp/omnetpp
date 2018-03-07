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
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>  // sprintf
#include <cstring>  // strcpy
#include <algorithm>
#include "common/stringutil.h"
#include "omnetpp/cmodule.h"

#include "omnetpp/csimplemodule.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/carray.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/ccanvas.h"
#include "omnetpp/cosgcanvas.h"
#include "omnetpp/simutil.h"
#include "omnetpp/cmodelchange.h"

using namespace omnetpp::common;

namespace omnetpp {

Register_Class(cModule);


// static members:
std::string cModule::lastModuleFullPath;
const cModule *cModule::lastModuleFullPathModule = nullptr;

#ifdef NDEBUG
bool cModule::cacheFullPath = false; // in release mode keep memory usage low
#else
bool cModule::cacheFullPath = true;  // fullpath is useful during debugging
#endif

cModule::cModule()
{
    vectorIndex = 0;
    vectorSize = -1;
    fullPath = nullptr;
    fullName = nullptr;

    prevSibling = nextSibling = firstSubmodule = lastSubmodule = nullptr;

    gateDescArraySize = 0;
    gateDescArray = nullptr;
#ifdef USE_OMNETPP4x_FINGERPRINTS
    version4ModuleId = -1;
#endif

    canvas = nullptr;
    osgCanvas = nullptr;
    flags |= FL_BUILTIN_ANIMATIONS;

    // gates and parameters will be added by cModuleType
}

cModule::~cModule()
{
    // release listeners in the whole subtree first. This mostly eliminates
    // the need for explicit unsubscribe() calls in module destructors.
    releaseListeners();

    // notify envir while module object still exists (more or less)
    EVCB.moduleDeleted(this);

    // delete submodules
    for (SubmoduleIterator it(this); !it.end(); ) {
        cModule *submodule = *it;
        ++it;
        if (submodule == (cModule *)getSimulation()->getContextModule()) {
            // NOTE: subclass destructors will not be called, but the simulation will stop anyway
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
        submodule->deleteModule();
    }

    // adjust gates that were directed here
    for (GateIterator it(this); !it.end(); ++it) {
        cGate *gate = *it;
        if (gate->getNextGate() && gate->getNextGate()->getPreviousGate() == gate)
            gate->disconnect();
        if (gate->getPreviousGate() && gate->getPreviousGate()->getNextGate() == gate)
            gate->getPreviousGate()->disconnect();
    }

    // delete all gates
    clearGates();

    // deregister ourselves
    if (getParentModule())
        getParentModule()->removeSubmodule(this);

    delete canvas;
    delete osgCanvas;

    delete[] fullName;
    delete[] fullPath;
}

void cModule::releaseListeners()
{
    releaseLocalListeners();
    for (ChannelIterator it(this); !it.end(); ++it)
        (*it)->releaseLocalListeners();
    for (SubmoduleIterator it(this); !it.end(); ++it)
        (*it)->releaseListeners();
}

void cModule::forEachChild(cVisitor *v)
{
    for (GateIterator it(this); !it.end(); ++it)
        v->visit(*it);

    cComponent::forEachChild(v);
}

void cModule::setNameAndIndex(const char *s, int i, int n)
{
    // a two-in-one function, so that we don't end up calling updateFullPath() twice
    cOwnedObject::setName(s);
    vectorIndex = i;
    vectorSize = n;
    updateFullName();
}

std::string cModule::str() const
{
    std::stringstream out;
    out << "id=" << getId();
    return out.str();
}

void cModule::insertSubmodule(cModule *mod)
{
    // take ownership
    take(mod);

    // append at end of submodule list
    mod->nextSibling = nullptr;
    mod->prevSibling = lastSubmodule;
    if (mod->prevSibling)
        mod->prevSibling->nextSibling = mod;
    if (!firstSubmodule)
        firstSubmodule = mod;
    lastSubmodule = mod;

    // cached module getFullPath() possibly became invalid
    lastModuleFullPathModule = nullptr;
}

void cModule::removeSubmodule(cModule *mod)
{
    // NOTE: no drop(mod): anyone can take ownership anyway (because we're soft owners)
    // and otherwise it'd cause trouble if mod itself is in context (it'd get inserted
    // on its own DefaultList)

    // remove from submodule list
    if (mod->nextSibling)
        mod->nextSibling->prevSibling = mod->prevSibling;
    if (mod->prevSibling)
        mod->prevSibling->nextSibling = mod->nextSibling;
    if (firstSubmodule == mod)
        firstSubmodule = mod->nextSibling;
    if (lastSubmodule == mod)
        lastSubmodule = mod->prevSibling;

    // this is not strictly needed but makes it cleaner
    mod->prevSibling = mod->nextSibling = nullptr;

    // cached module getFullPath() possibly became invalid
    lastModuleFullPathModule = nullptr;
}

cModule *cModule::getParentModule() const
{
    return dynamic_cast<cModule *>(getOwner());
}

void cModule::setName(const char *s)
{
    cOwnedObject::setName(s);
    updateFullName();
}

void cModule::updateFullName()
{
    if (fullName) {
        delete[] fullName;
        fullName = nullptr;
    }
    if (isVector()) {
        fullName = new char[omnetpp::opp_strlen(getName()) + 10];
        strcpy(fullName, getName());
        opp_appendindex(fullName, getIndex());
    }

    if (lastModuleFullPathModule == this)
        lastModuleFullPathModule = nullptr;  // invalidate

    if (cacheFullPath)
        updateFullPathRec();

#ifdef SIMFRONTEND_SUPPORT
    updateLastChangeSerial();
#endif
}

void cModule::reassignModuleIdRec()
{
    int oldId = getId();
    getSimulation()->deregisterComponent(this);
    getSimulation()->registerComponent(this);
    int newId = getId();

    cFutureEventSet *fes = getSimulation()->getFES();
    int fesLen = fes->getLength();
    for (int i = 0; i < fesLen; i++) {
        cEvent *event = fes->get(i);
        if (cMessage *msg = dynamic_cast<cMessage *>(event))
            if (msg->getArrivalModuleId() == oldId)
                msg->setArrival(newId, msg->getArrivalGateId());
    }

    for (cModule *child = firstSubmodule; child; child = child->nextSibling)
        child->reassignModuleIdRec();
}

void cModule::updateFullPathRec()
{
    delete[] fullPath;
    fullPath = nullptr;  // for the next getFullPath() call
    fullPath = omnetpp::opp_strdup(getFullPath().c_str());

    for (cModule *child = firstSubmodule; child; child = child->nextSibling)
        child->updateFullPathRec();
}

const char *cModule::getFullName() const
{
    // if not in a vector, normal getName() will do
    return isVector() ? fullName : getName();
}

std::string cModule::getFullPath() const
{
    // use cached value if filled in
    if (fullPath)
        return fullPath;

    if (lastModuleFullPathModule != this) {
        // stop at the toplevel module (don't go up to cSimulation);
        // plus, cache the result, expecting more hits from this module
        if (getParentModule() == nullptr)
            lastModuleFullPath = getFullName();
        else
            lastModuleFullPath = getParentModule()->getFullPath() + "." + getFullName();
        lastModuleFullPathModule = this;
    }
    return lastModuleFullPath;
}

bool cModule::isSimple() const
{
    return dynamic_cast<const cSimpleModule *>(this) != nullptr;
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
    if (gate) {
        if (checkConnected && (gate->getPreviousGate() || gate->getNextGate()))
            throw cRuntimeError(this, "Cannot delete gate '%s', it is still connected", gate->getFullName());
        EVCB.gateDeleted(gate);
        delete gate;
    }
}

void cModule::disposeGateDesc(cGate::Desc *desc, bool checkConnected)
{
    if (!desc->name)
        return;  // already deleted

    // notify pre-change listeners
    if (hasListeners(PRE_MODEL_CHANGE)) {
        cPreGateDeleteNotification tmp;
        tmp.module = this;
        tmp.gateName = desc->name->name.c_str();
        emit(PRE_MODEL_CHANGE, &tmp);
    }

    // do it
    if (!desc->isVector()) {
        disposeGateObject(desc->input.gate, checkConnected);
        disposeGateObject(desc->output.gate, checkConnected);
    }
    else {
        for (int j = 0; j < desc->gateSize(); j++) {
            if (desc->input.gatev)
                disposeGateObject(desc->input.gatev[j], checkConnected);
            if (desc->output.gatev)
                disposeGateObject(desc->output.gatev[j], checkConnected);
        }
        delete[] desc->input.gatev;
        delete[] desc->output.gatev;
    }
    const char *gatename = desc->name->name.c_str();
    cGate::Type gatetype = desc->getType();
    desc->name = nullptr;  // mark as deleted, but leave shared Name struct in the pool
#ifdef SIMFRONTEND_SUPPORT
    updateLastChangeSerial();
#endif

    // notify post-change listeners
    if (hasListeners(POST_MODEL_CHANGE)) {
        cPostGateDeleteNotification tmp;
        tmp.module = this;
        tmp.gateName = gatename;  // points into namePool
        tmp.gateType = gatetype;
        tmp.isVector = desc->isVector();  // desc still exists, only namep was nullptr'd
        tmp.vectorSize = desc->gateSize();
        emit(POST_MODEL_CHANGE, &tmp);
    }
}

void cModule::clearGates()
{
    for (int i = 0; i < gateDescArraySize; i++)
        disposeGateDesc(gateDescArray + i, false);
    delete[] gateDescArray;
    gateDescArray = nullptr;
    gateDescArraySize = 0;
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
        ASSERT(gateDescArray <= gate->desc && gate->desc < gateDescArray + gateDescArraySize);
        gate->desc = newvec + (gate->desc - gateDescArray);
    }
}

cGate::Desc *cModule::addGateDesc(const char *gatename, cGate::Type type, bool isVector)
{
    // check limits
    if (isVector) {
        if (gateDescArraySize >= MAX_VECTORGATES)
            throw cRuntimeError(this, "Cannot add gate '%s[]': Too many vector gates (limit is %d)", gatename, MAX_VECTORGATES);
    }
    else {
        if (gateDescArraySize >= MAX_SCALARGATES)
            throw cRuntimeError(this, "Cannot add gate '%s': Too many scalar gates (limit is %d)", gatename, MAX_SCALARGATES);
    }

    // allocate new array
    //TODO preallocate a larger descv[] in advance?
    cGate::Desc *newv = new cGate::Desc[gateDescArraySize+1];
    memcpy(newv, gateDescArray, gateDescArraySize * sizeof(cGate::Desc));

    // adjust desc pointers in already existing gates
    for (int i = 0; i < gateDescArraySize; i++) {
        cGate::Desc *desc = gateDescArray + i;
        if (desc->name) {
            if (!desc->isVector()) {
                adjustGateDesc(desc->input.gate, newv);
                adjustGateDesc(desc->output.gate, newv);
            }
            else {
                for (int j = 0; j < desc->gateSize(); j++) {
                    if (desc->input.gatev)
                        adjustGateDesc(desc->input.gatev[j], newv);
                    if (desc->output.gatev)
                        adjustGateDesc(desc->output.gatev[j], newv);
                }
            }
        }
    }

    // install the new array and get its last element
    delete[] gateDescArray;
    gateDescArray = newv;
    cGate::Desc *newDesc = gateDescArray + gateDescArraySize++;

    // configure this gatedesc with name and type
    cGate::Name key(gatename, type);
    NamePool::iterator it = namePool.find(key);
    if (it == namePool.end())
        it = namePool.insert(key).first;
    newDesc->name = const_cast<cGate::Name *>(&(*it));
    newDesc->vectorSize = isVector ? 0 : -1;
    return newDesc;
}

int cModule::findGateDesc(const char *gatename, char& suffix) const
{
    // determine whether gatename contains "$i"/"$o" suffix
    int len = strlen(gatename);
    suffix = (len > 2 && gatename[len-2] == '$') ? gatename[len-1] : 0;
    if (suffix && suffix != 'i' && suffix != 'o')
        return -1;  // invalid suffix ==> no such gate

    // and search accordingly
    switch (suffix) {
        case '\0':
            for (int i = 0; i < gateDescArraySize; i++) {
                const cGate::Desc *desc = gateDescArray + i;
                if (desc->name && strcmp(desc->name->name.c_str(), gatename) == 0)
                    return i;
            }
            break;

        case 'i':
            for (int i = 0; i < gateDescArraySize; i++) {
                const cGate::Desc *desc = gateDescArray + i;
                if (desc->name && strcmp(desc->name->namei.c_str(), gatename) == 0)
                    return i;
            }
            break;

        case 'o':
            for (int i = 0; i < gateDescArraySize; i++) {
                const cGate::Desc *desc = gateDescArray + i;
                if (desc->name && strcmp(desc->name->nameo.c_str(), gatename) == 0)
                    return i;
            }
            break;
    }
    return -1;
}

cGate::Desc *cModule::gateDesc(const char *gatename, char& suffix) const
{
    int descIndex = findGateDesc(gatename, suffix);
    if (descIndex < 0)
        throw cRuntimeError(this, "No such gate or gate vector: '%s'", gatename);
    return gateDescArray + descIndex;
}

#define ENSURE(x)  if (!(x)) throw cRuntimeError(this, E_GATEID, id)

cGate *cModule::gate(int id)
{
    // To make sense of the following code, see comment in cgate.cc,
    // titled "Interpretation of gate Ids".
    unsigned int h = id & GATEID_HMASK;
    if (h == 0) {
        // scalar gate
        unsigned int descIndex = id >> 1;
        ENSURE(descIndex < (unsigned int)gateDescArraySize);
        cGate::Desc *desc = gateDescArray + descIndex;
        ENSURE(desc->name);  // not deleted
        ENSURE(!desc->isVector());
        ENSURE((id & 1) == 0 ? desc->getType() != cGate::OUTPUT : desc->getType() != cGate::INPUT);
        cGate *g = (id & 1) == 0 ? desc->input.gate : desc->output.gate;
        ENSURE((id & 1) == (g->pos & 1));
        return g;
    }
    else {
        // vector gate
        unsigned int descIndex = (h >> GATEID_LBITS)-1;
        ENSURE(descIndex < (unsigned int)gateDescArraySize);
        cGate::Desc *desc = gateDescArray + descIndex;
        ENSURE(desc->name);  // not deleted
        ENSURE(desc->isVector());
        bool isOutput = id & (1 << (GATEID_LBITS-1));  // L's MSB
        ENSURE(isOutput ? desc->getType() != cGate::INPUT : desc->getType() != cGate::OUTPUT);
        unsigned int index = id & (GATEID_LMASK >> 1);
        if (index >= (unsigned int)desc->gateSize()) {
            // try to issue a useful error message if gate was likely produced as baseId+index
            if (index < 100000)
                throw cRuntimeError(this, "Invalid gate Id %d: Size of '%s[]' is only %d, so index %d (deduced from the Id) is out of bounds",
                        id, desc->nameFor(isOutput ? cGate::OUTPUT : cGate::INPUT), desc->gateSize(), index);
            else
                throw cRuntimeError(this, E_GATEID, id);  // id probably just plain garbage
        }
        return isOutput ? desc->output.gatev[index] : desc->input.gatev[index];
    }
}

#undef ENSURE

cGate *cModule::addGate(const char *gatename, cGate::Type type, bool isVector)
{
    char suffix;
    if (findGateDesc(gatename, suffix) >= 0)
        throw cRuntimeError(this, "addGate(): Gate '%s' already present", gatename);
    if (suffix)
        throw cRuntimeError(this, "addGate(): Wrong gate name '%s', must not contain the '$i' or '$o' suffix", gatename);

    // notify pre-change listeners
    if (hasListeners(PRE_MODEL_CHANGE)) {
        cPreGateAddNotification tmp;
        tmp.module = this;
        tmp.gateName = gatename;
        tmp.gateType = type;
        tmp.isVector = isVector;
        emit(PRE_MODEL_CHANGE, &tmp);
    }

    // create desc for new gate (or gate vector)
    cGate::Desc *desc = addGateDesc(gatename, type, isVector);
    desc->owner = this;

    // if scalar gate, create gate object(s); gate vectors are created with size 0.
    cGate *result = nullptr;
    if (!isVector) {
        cGate *newGate;
        if (type != cGate::OUTPUT) {  // that is, INPUT or INOUT
            newGate = createGateObject(cGate::INPUT);
            desc->setInputGate(newGate);
            EVCB.gateCreated(newGate);
        }
        if (type != cGate::INPUT) {  // that is, OUTPUT or INOUT
            newGate = createGateObject(cGate::OUTPUT);
            desc->setOutputGate(newGate);
            EVCB.gateCreated(newGate);
        }
        if (type != cGate::INOUT)
            result = newGate;
    }

#ifdef SIMFRONTEND_SUPPORT
    updateLastChangeSerial();
#endif

    // notify post-change listeners
    if (hasListeners(POST_MODEL_CHANGE)) {
        cPostGateAddNotification tmp;
        tmp.module = this;
        tmp.gateName = gatename;
        emit(POST_MODEL_CHANGE, &tmp);
    }

    return result;
}

static void reallocGatev(cGate **& v, int oldSize, int newSize)
{
    if (oldSize != newSize) {
        cGate **newv = new cGate *[newSize];
        memcpy(newv, v, (oldSize < newSize ? oldSize : newSize) * sizeof(cGate *));
        if (newSize > oldSize)
            memset(newv + oldSize, 0, (newSize - oldSize) * sizeof(cGate *));
        delete[] v;
        v = newv;
    }
}

void cModule::setGateSize(const char *gatename, int newSize)
{
    char suffix;
    int descIndex = findGateDesc(gatename, suffix);
    if (descIndex < 0)
        throw cRuntimeError(this, "No '%s' or '%s[]' gate", gatename, gatename);
    if (suffix)
        throw cRuntimeError(this, "setGateSize(): Wrong gate name '%s', suffix '$i'/'$o' not accepted here", gatename);
    cGate::Desc *desc = gateDescArray + descIndex;
    if (!desc->isVector())
        throw cRuntimeError(this, "setGateSize(): Gate '%s' is not a vector gate", gatename);
    if (newSize < 0)
        throw cRuntimeError(this, "setGateSize(): Negative vector size (%d) requested for gate %s[]", newSize, gatename);
    if (newSize > MAX_VECTORGATESIZE)
        throw cRuntimeError(this, "setGateSize(): Vector size for gate %s[] too large (%d), limit is %d", gatename, newSize, MAX_VECTORGATESIZE);

    // notify pre-change listeners
    if (hasListeners(PRE_MODEL_CHANGE)) {
        cPreGateVectorResizeNotification tmp;
        tmp.module = this;
        tmp.gateName = gatename;
        tmp.newSize = newSize;
        emit(PRE_MODEL_CHANGE, &tmp);
    }

    int oldSize = desc->vectorSize;
    cGate::Type type = desc->getType();

    // we need to allocate more (to have good gate++ performance) but we
    // don't want to store the capacity -- so we'll always calculate the
    // capacity from the current size (by rounding it up to the nearest
    // multiple of 2, 4, 16, 64).
    int oldCapacity = cGate::Desc::capacityFor(oldSize);
    int newCapacity = cGate::Desc::capacityFor(newSize);

    // shrink?
    if (newSize < oldSize) {
        // remove excess gates
        for (int i = oldSize-1; i >= newSize; i--) {
            // check & notify
            if (type != cGate::OUTPUT) {
                cGate *gate = desc->input.gatev[i];
                if (gate->getPreviousGate() || gate->getNextGate())
                    throw cRuntimeError(this, "setGateSize(): Cannot shrink gate vector %s[] to size %d, gate %s still connected", gatename, newSize, gate->getFullPath().c_str());
                EVCB.gateDeleted(gate);
            }
            if (type != cGate::INPUT) {
                cGate *gate = desc->output.gatev[i];
                if (gate->getPreviousGate() || gate->getNextGate())
                    throw cRuntimeError(this, "setGateSize(): Cannot shrink gate vector %s[] to size %d, gate %s still connected", gatename, newSize, gate->getFullPath().c_str());
                EVCB.gateDeleted(gate);
            }

            // actually delete
            desc->vectorSize = i;
            if (type != cGate::OUTPUT) {
                delete desc->input.gatev[i];
                desc->input.gatev[i] = nullptr;
            }
            if (type != cGate::INPUT) {
                delete desc->output.gatev[i];
                desc->output.gatev[i] = nullptr;
            }
        }

        // shrink container
        if (type != cGate::OUTPUT)
            reallocGatev(desc->input.gatev, oldCapacity, newCapacity);
        if (type != cGate::INPUT)
            reallocGatev(desc->output.gatev, oldCapacity, newCapacity);
        desc->vectorSize = newSize;
    }

    // expand?
    if (newSize > oldSize) {
        // expand container (slots newSize..newCapacity will stay unused nullptr for now)
        if (type != cGate::OUTPUT)
            reallocGatev(desc->input.gatev, oldCapacity, newCapacity);
        if (type != cGate::INPUT)
            reallocGatev(desc->output.gatev, oldCapacity, newCapacity);

        // set new size beforehand, because EVCB.gateCreate() calls getId()
        // which assumes that gate->index < gateSize.
        desc->vectorSize = newSize;

        // and create the additional gates
        for (int i = oldSize; i < newSize; i++) {
            if (type != cGate::OUTPUT) {
                cGate *newGate = createGateObject(cGate::INPUT);
                desc->setInputGate(newGate, i);
                EVCB.gateCreated(newGate);
            }
            if (type != cGate::INPUT) {
                cGate *newGate = createGateObject(cGate::OUTPUT);
                desc->setOutputGate(newGate, i);
                EVCB.gateCreated(newGate);
            }
        }
    }

#ifdef SIMFRONTEND_SUPPORT
    if (newSize != oldSize)
        updateLastChangeSerial();
#endif

    // notify post-change listeners
    if (hasListeners(POST_MODEL_CHANGE)) {
        cPostGateVectorResizeNotification tmp;
        tmp.module = this;
        tmp.gateName = gatename;
        tmp.oldSize = oldSize;
        emit(POST_MODEL_CHANGE, &tmp);
    }
}

int cModule::gateSize(const char *gatename) const
{
    char dummy;
    const cGate::Desc *desc = gateDesc(gatename, dummy);
    return desc->gateSize();
}

int cModule::gateBaseId(const char *gatename) const
{
    char suffix;
    int descIndex = findGateDesc(gatename, suffix);
    if (descIndex < 0)
        throw cRuntimeError(this, "gateBaseId(): No such gate or gate vector: '%s'", gatename);
    const cGate::Desc *desc = gateDescArray + descIndex;
    if (desc->getType() == cGate::INOUT && !suffix)
        throw cRuntimeError(this, "gateBaseId(): Inout gate '%s' cannot be referenced without $i/$o suffix", gatename);
    bool isInput = (suffix == 'i' || desc->getType() == cGate::INPUT);

    // To make sense of the following code, see comment in cgate.cc,
    // titled "Interpretation of gate Ids".
    int id;
    if (!desc->isVector())
        id = (descIndex << 1) | (isInput ? 0 : 1);
    else
        id = ((descIndex+1) << GATEID_LBITS) | ((isInput ? 0 : 1) << (GATEID_LBITS-1));
    return id;
}

cGate *cModule::gate(const char *gatename, int index)
{
    char suffix;
    const cGate::Desc *desc = gateDesc(gatename, suffix);
    if (desc->getType() == cGate::INOUT && !suffix)
        throw cRuntimeError(this, "Inout gate '%s' cannot be referenced without $i/$o suffix", gatename);
    bool isInput = (suffix == 'i' || desc->getType() == cGate::INPUT);

    if (!desc->isVector()) {
        // gate is scalar
        if (index != -1)
            throw cRuntimeError(this, "Scalar gate '%s' referenced with index", gatename);
        return isInput ? desc->input.gate : desc->output.gate;
    }
    else {
        // gate is vector
        if (index < 0)
            throw cRuntimeError(this, "%s when accessing vector gate '%s'", (index == -1 ? "No gate index specified" : "Negative gate index specified"), gatename);
        if (index >= desc->vectorSize)
            throw cRuntimeError(this, "Gate index %d out of range when accessing vector gate '%s[]' with size %d", index, gatename, desc->vectorSize);
        return isInput ? desc->input.gatev[index] : desc->output.gatev[index];
    }
}

int cModule::findGate(const char *gatename, int index) const
{
    char suffix;
    int descIndex = findGateDesc(gatename, suffix);
    if (descIndex < 0)
        return -1;  // no such gate name
    const cGate::Desc *desc = gateDescArray + descIndex;
    if (desc->getType() == cGate::INOUT && !suffix)
        return -1;  // inout gate cannot be referenced without "$i" or "$o" suffix
    bool isInput = (suffix == 'i' || desc->getType() == cGate::INPUT);

    if (!desc->isVector()) {
        // gate is scalar
        if (index != -1)
            return -1;  // wrong: scalar gate referenced with index
        return isInput ? desc->input.gate->getId() : desc->output.gate->getId();
    }
    else {
        // gate is vector
        if (index < 0 || index >= desc->vectorSize)
            return -1;  // index not specified (-1) or out of range
        return isInput ? desc->input.gatev[index]->getId() : desc->output.gatev[index]->getId();
    }
}

cGate *cModule::gateHalf(const char *gatename, cGate::Type type, int index)
{
    char dummy;
    const cGate::Desc *desc = gateDesc(gatename, dummy);
    const char *nameWithSuffix = (type == cGate::INPUT) ? desc->name->namei.c_str() : desc->name->nameo.c_str();
    return gate(nameWithSuffix, index);
}

bool cModule::hasGate(const char *gatename, int index) const
{
    char suffix;
    int descIndex = findGateDesc(gatename, suffix);
    if (descIndex < 0)
        return false;
    const cGate::Desc *desc = gateDescArray + descIndex;
    return index == -1 ? true : (index >= 0 && index < desc->vectorSize);
}

void cModule::deleteGate(const char *gatename)
{
    char suffix;
    cGate::Desc *desc = gateDesc(gatename, suffix);
    if (suffix)
        throw cRuntimeError(this, "Cannot delete one half of an inout gate: '%s'", gatename);
    disposeGateDesc(desc, true);
}

std::vector<const char *> cModule::getGateNames() const
{
    std::vector<const char *> result;
    for (int i = 0; i < gateDescArraySize; i++)
        if (gateDescArray[i].name)
            result.push_back(gateDescArray[i].name->name.c_str());

    return result;
}

cGate::Type cModule::gateType(const char *gatename) const
{
    char suffix;
    const cGate::Desc *desc = gateDesc(gatename, suffix);
    if (suffix)
        return suffix == 'i' ? cGate::INPUT : cGate::OUTPUT;
    else
        return desc->name->type;
}

bool cModule::isGateVector(const char *gatename) const
{
    char dummy;
    const cGate::Desc *desc = gateDesc(gatename, dummy);
    return desc->isVector();
}

//XXX test code:
//    bool operator()(cGate *a, cGate *b) {
//        printf("   comparing %s, %s ==> ", (a?a->getFullName():nullptr), (b?b->getFullName():nullptr) );
//        bool x = (a && a->isConnectedInside()) > (b && b->isConnectedInside());
//        printf("%d > %d : ", (a && a->isConnectedInside()), (b && b->isConnectedInside()));
//        printf("%d\n", x);
//        return x;
//    }

struct less_gateConnectedInside
{
    bool operator()(cGate *a, cGate *b) { return (a && a->isConnectedInside()) > (b && b->isConnectedInside()); }
};

struct less_gateConnectedOutside
{
    bool operator()(cGate *a, cGate *b) { return (a && a->isConnectedOutside()) > (b && b->isConnectedOutside()); }
};

struct less_gatePairConnectedInside
{
    cGate **otherv;
    less_gatePairConnectedInside(cGate **otherv) { this->otherv = otherv; }
    bool operator()(cGate *a, cGate *b)
    {
        return (a && a->isConnectedInside()) > (b && b->isConnectedInside()) &&
               (a && otherv[a->getIndex()]->isConnectedInside()) > (b && otherv[b->getIndex()]->isConnectedInside());
    }
};

struct less_gatePairConnectedOutside
{
    cGate **otherv;
    less_gatePairConnectedOutside(cGate **otherv) { this->otherv = otherv; }
    bool operator()(cGate *a, cGate *b)
    {
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
        throw cRuntimeError(this, "getOrCreateFirstUnconnectedGate(): Gate '%s' is not a vector gate", gatename);
    if (suffix1 && suffix)
        throw cRuntimeError(this, "getOrCreateFirstUnconnectedGate(): Gate '%s' AND suffix '%c' given", gatename, suffix);
    suffix = suffix | suffix1;

    // determine whether input or output gates to check
    bool inputSide;
    if (!suffix) {
        if (desc->getType() == cGate::INOUT)
            throw cRuntimeError(this, "getOrCreateFirstUnconnectedGate(): Inout gate specified but no suffix");
        inputSide = desc->getType() == cGate::INPUT;
    }
    else {
        if (suffix != 'i' && suffix != 'o')
            throw cRuntimeError(this, "getOrCreateFirstUnconnectedGate(): Wrong gate name suffix '%c'", suffix);
        inputSide = suffix == 'i';
    }

    // gate array we'll be looking at
    cGate **gatev = inputSide ? desc->input.gatev : desc->output.gatev;
    int oldSize = desc->vectorSize;

    // since gates get connected from the beginning of the vector, we can do
    // binary search for the first unconnected gate. In the (rare) case when
    // gates are not connected in order (i.e. some high gate indices get
    // connected before lower ones), binary search may not be able to find the
    // "holes" (unconnected gates) and we expand the gate unnecessarily.
    // Note: nullptr is used as a synonym for "any unconnected gate" (see less_* struct).
    cGate **it = inside ?
        std::lower_bound(gatev, gatev + oldSize, (cGate *)nullptr, less_gateConnectedInside()) :
        std::lower_bound(gatev, gatev + oldSize, (cGate *)nullptr, less_gateConnectedOutside());
    if (it != gatev + oldSize)
        return *it;

    // no unconnected gate: expand gate vector
    if (expand) {
        setGateSize(desc->name->name.c_str(), oldSize+1);  //FIXME spare extra name lookup!!!
        return inputSide ? desc->input.gatev[oldSize] : desc->output.gatev[oldSize];
    }
    else {
        // gate is not allowed to expand, so let's try harder to find an unconnected gate
        // (in case the binary search missed it)
        for (int i = 0; i < oldSize; i++)
            if (inside ? !gatev[i]->isConnectedInside() : !gatev[i]->isConnectedOutside())
                return gatev[i];

        return nullptr;  // sorry
    }
}

void cModule::getOrCreateFirstUnconnectedGatePair(const char *gatename,
        bool inside, bool expand,
        cGate *& gatein, cGate *& gateout)
{
    // look up gate
    char suffix;
    cGate::Desc *desc = const_cast<cGate::Desc *>(gateDesc(gatename, suffix));
    if (!desc->isVector())
        throw cRuntimeError(this, "getOrCreateFirstUnconnectedGatePair(): Gate '%s' is not a vector gate", gatename);
    if (suffix)
        throw cRuntimeError(this, "getOrCreateFirstUnconnectedGatePair(): Inout gate expected, without '$i'/'$o' suffix");

    // the gate arrays we'll work with
    int oldSize = desc->vectorSize;
    cGate **inputgatev = desc->input.gatev;
    cGate **outputgatev = desc->output.gatev;

    // binary search for the first unconnected gate -- see explanation in method above
    cGate **it = inside ?
        std::lower_bound(inputgatev, inputgatev + oldSize, (cGate *)nullptr, less_gatePairConnectedInside(outputgatev)) :
        std::lower_bound(inputgatev, inputgatev + oldSize, (cGate *)nullptr, less_gatePairConnectedOutside(outputgatev));
    if (it != inputgatev + oldSize) {
        gatein = *it;
        gateout = outputgatev[gatein->getIndex()];
        return;
    }

    // no unconnected gate: expand gate vector
    if (expand) {
        setGateSize(desc->name->name.c_str(), oldSize+1);  //FIXME spare extra name lookup!!!
        gatein = desc->input.gatev[oldSize];
        gateout = desc->output.gatev[oldSize];
        return;
    }
    else {
        // gate is not allowed to expand, so let's try harder to find an unconnected gate
        // (in case the binary search missed it)
        for (int i = 0; i < oldSize; i++)
            if (inside ? !inputgatev[i]->isConnectedInside() : !inputgatev[i]->isConnectedOutside())
                if (inside ? !outputgatev[i]->isConnectedInside() : !outputgatev[i]->isConnectedOutside()) {
                    gatein = inputgatev[i];
                    gateout = outputgatev[i];
                    return;
                }

        gatein = gateout = nullptr;  // sorry
    }
}

int cModule::gateCount() const
{
    int n = 0;
    for (int i = 0; i < gateDescArraySize; i++) {
        cGate::Desc *desc = gateDescArray + i;
        if (desc->name) {
            if (!desc->isVector())
                n += (desc->getType() == cGate::INOUT) ? 2 : 1;
            else
                n += (desc->getType() == cGate::INOUT) ? 2 * desc->vectorSize : desc->vectorSize;
        }
    }
    return n;
}

cGate *cModule::gateByOrdinal(int k) const
{
    GateIterator it(this);
    for (int i = 0; i < k; i++)
        ++it;  //TODO make more efficient
    return it.end() ? nullptr : *it;
}

bool cModule::checkInternalConnections() const
{
    // Note: This routine only checks if all gates are connected or not.
    // It does NOT check where and how they are connected!
    // To allow a gate go unconnected, annotate it with @loose or @directIn.

    // check this compound module if its inside is connected ok
    // Note: checking of the inner side of compound module gates
    // cannot be turned off with @loose
    if (!isSimple()) {
        for (GateIterator it(this); !it.end(); ++it) {
            cGate *gate = *it;
            if (gate->size() != 0 && !gate->isConnectedInside())
                throw cRuntimeError(this, "Gate '%s' is not connected to a submodule (or internally to another gate of the same module)", gate->getFullPath().c_str());
        }
    }

    // check submodules
    for (SubmoduleIterator it(this); !it.end(); ++it) {
        cModule *submodule = *it;
        for (GateIterator git(submodule); !git.end(); ++git) {
            cGate *gate = *git;
            if (gate->size() != 0 && !gate->isConnectedOutside() &&
                gate->getProperties()->getAsBool("loose") == false &&
                gate->getProperties()->getAsBool("directIn") == false)
                throw cRuntimeError(this, "Gate '%s' is not connected to sibling or parent module", gate->getFullPath().c_str());
        }
    }
    return true;
}

int cModule::findSubmodule(const char *name, int index) const
{
    for (SubmoduleIterator it(this); !it.end(); ++it) {
        cModule *submodule = *it;
        if (submodule->isName(name) && ((index == -1 && !submodule->isVector()) || submodule->getIndex() == index))
            return submodule->getId();
    }

    return -1;
}

cModule *cModule::getSubmodule(const char *name, int index) const
{
    for (SubmoduleIterator it(this); !it.end(); ++it) {
        cModule *submodule = *it;
        if (submodule->isName(name) && ((index == -1 && !submodule->isVector()) || submodule->getIndex() == index))
            return submodule;
    }

    return nullptr;
}

inline char *nextToken(char *& rest)
{
    if (!rest)
        return nullptr;
    char *token = rest;
    rest = strchr(rest, '.');
    if (rest)
        *rest++ = '\0';
    return token;
}

#define ROOTNAME "<root>"

cModule *cModule::getModuleByPath(const char *path) const
{
    if (!path || !path[0])
        return nullptr;

    // determine starting point
    bool isRelative = (path[0] == '.' || path[0] == '^');
    const cModule *module = isRelative ? this : getSimulation()->getSystemModule();
    const char *pathWithoutFirstDot = (path[0] == '.') ? path+1 : path;

    // match components of the path
    opp_string pathbuf(pathWithoutFirstDot);
    char *rest = pathbuf.buffer();
    char *token = nextToken(rest);
    bool isFirst = true;
    while (token && module) {
        char *lbracket;
        if (!token[0])
            ;  /*skip empty path component*/
        else if (!isRelative && isFirst && (module->isName(token) || strcmp(token, ROOTNAME)==0))
            /*ignore network name*/;
        else if (token[0] == '^' && token[1] == '\0')
            module = module->getParentModule();  // if module is the root, we'll return nullptr
        else if ((lbracket = strchr(token, '[')) == nullptr) {
            if (token[0] == '<' && strcmp(token, ROOTNAME) == 0)
                throw cRuntimeError(this, "getModuleByPath(): Wrong path '%s', '" ROOTNAME "' may only occur as the first component", path);
            module = module->getSubmodule(token);
        }
        else {
            if (token[strlen(token)-1] != ']')
                throw cRuntimeError(this, "getModuleByPath(): Syntax error (unmatched bracket?) in path '%s'", path);
            int index = atoi(lbracket+1);
            *lbracket = '\0';  // cut off [index]
            if (token[0] == '<' && strcmp(token, ROOTNAME) == 0)
                throw cRuntimeError(this, "getModuleByPath(): Wrong path '%s', '" ROOTNAME "' may only occur as the first component", path);
            module = module->getSubmodule(token, index);
        }
        token = nextToken(rest);
        isFirst = false;
    }

    return const_cast<cModule*>(module);  // nullptr if not found
}

cPar& cModule::getAncestorPar(const char *name)
{
    // search parameter in parent modules
    cModule *module = this;
    int k;
    while (module && (k = module->findPar(name)) < 0)
        module = module->getParentModule();
    if (!module)
        throw cRuntimeError(this, "Has no ancestor parameter called '%s'", name);
    return module->par(k);
}

void cModule::finalizeParameters()
{
    // temporarily switch context
    cContextSwitcher tmp(this);
    cContextTypeSwitcher tmp2(CTX_BUILD);

    cComponent::finalizeParameters();  // this will read input parameters

    // set up gate vectors (their sizes may depend on the parameter settings)
    getModuleType()->setupGateVectors(this);
}

void cModule::scheduleStart(simtime_t t)
{
    for (SubmoduleIterator it(this); !it.end(); ++it)
        (*it)->scheduleStart(t);
}

void cModule::buildInside()
{
    if (buildInsideCalled())
        throw cRuntimeError(this, "buildInside() already called for this module");

    // call finalizeParameters() if user has forgotten to do it;
    // this is needed to make dynamic module creation more robust
    if (!parametersFinalized())
        finalizeParameters();

    // temporarily switch context
    cContextSwitcher tmp(this);
    cContextTypeSwitcher tmp2(CTX_BUILD);

    // call doBuildInside() in this context
    doBuildInside();

    setFlag(FL_BUILDINSIDE_CALLED, true);

    // configure @statistics here, as they may refer to submodules' signals
    addResultRecorders();
}

void cModule::doBuildInside()
{
    // ask module type to create submodules and internal connections
    getModuleType()->buildInside(this);
}

void cModule::deleteModule()
{
    // check this module doesn't contain the executing module somehow
    for (cModule *module = getSimulation()->getContextModule(); module; module = module->getParentModule())
        if (module == this)
            throw cRuntimeError(this, "It is not supported to delete a module that contains "
                                      "the currently executing simple module");


    // notify pre-change listeners
    if (hasListeners(PRE_MODEL_CHANGE)) {
        cPreModuleDeleteNotification tmp;
        tmp.module = this;
        emit(PRE_MODEL_CHANGE, &tmp);
    }

    cModule *parent = getParentModule();
    if (!parent || !parent->hasListeners(POST_MODEL_CHANGE)) {
        // no listeners, just do it
        delete this;
    }
    else {
        // need to fill in notification data before deleting the module
        cPostModuleDeleteNotification tmp;
        tmp.module = this;
        tmp.moduleId = getId();
        tmp.moduleType = getModuleType();
        std::string tmpname = getName();
        tmp.moduleName = tmpname.c_str();
        tmp.parentModule = getParentModule();
        tmp.vectorSize = getVectorSize();
        tmp.index = getIndex();

        delete this;

        parent->emit(POST_MODEL_CHANGE, &tmp);
    }
}

void cModule::changeParentTo(cModule *module)
{
    if (!module)
        throw cRuntimeError(this, "changeParentTo(): Got nullptr");

    // gates must be unconnected to avoid connections breaking module hierarchy rules
    for (GateIterator it(this); !it.end(); ++it)
        if ((*it)->isConnectedOutside())
            throw cRuntimeError(this, "changeParentTo(): Gates of the module must not be "
                                      "connected (%s is connected now)", (*it)->getFullName());


    // cannot insert module under one of its own submodules
    for (cModule *m = module; m; m = m->getParentModule())
        if (m == this)
            throw cRuntimeError(this, "changeParentTo(): Cannot move module under one of its own submodules");


    // notify pre-change listeners
    if (hasListeners(PRE_MODEL_CHANGE)) {
        cPreModuleReparentNotification tmp;
        tmp.module = this;
        tmp.newParentModule = module;
        module->emit(PRE_MODEL_CHANGE, &tmp);
    }

    // do it
    cModule *oldparent = getParentModule();
    oldparent->removeSubmodule(this);
    module->insertSubmodule(this);
    int oldId = getId();
    reassignModuleIdRec();
    if (cacheFullPath)
        updateFullPathRec();

    // notify environment
    EVCB.moduleReparented(this, oldparent, oldId);
#ifdef SIMFRONTEND_SUPPORT
    updateLastChangeSerial();
#endif

    // notify post-change listeners
    if (hasListeners(POST_MODEL_CHANGE)) {
        cPostModuleReparentNotification tmp;
        tmp.module = this;
        tmp.oldParentModule = oldparent;
        module->emit(POST_MODEL_CHANGE, &tmp);
    }
}

cCanvas *cModule::getCanvas() const
{
    if (!canvas) {
        canvas = new cCanvas("canvas");
        canvas->addFigure(new cGroupFigure("submodules"));
        const_cast<cModule*>(this)->take(canvas);
    }
    return canvas;
}

cOsgCanvas *cModule::getOsgCanvas() const
{
    if (!osgCanvas) {
        osgCanvas = new cOsgCanvas("osgCanvas");
        const_cast<cModule*>(this)->take(osgCanvas);
    }
    return osgCanvas;
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
    while (moreChannelStages || moreModuleStages) {
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
    if (getSimulation()->getContextType() != CTX_INITIALIZE)
        throw cRuntimeError("Internal function initializeChannels() may only be called via callInitialize()");

    // initialize channels directly under this module
    bool moreStages = false;
    for (ChannelIterator it(this); !it.end(); ++it)
        if ((*it)->initializeChannel(stage))
            moreStages = true;


    // then recursively initialize channels within our submodules too
    for (SubmoduleIterator it(this); !it.end(); ++it)
        if ((*it)->initializeChannels(stage))
            moreStages = true;


    return moreStages;
}

bool cModule::initializeModules(int stage)
{
    if (getSimulation()->getContextType() != CTX_INITIALIZE)
        throw cRuntimeError("Internal function initializeModules() may only be called via callInitialize()");

    if (stage == 0) {
        if (initialized())
            throw cRuntimeError(this, "initialize() already called for this module");

        // call buildInside() if user has forgotten to do it; this is needed
        // to make dynamic module creation more robust
        if (!buildInsideCalled())
            buildInside();
    }

    // first call initialize(stage) for this module...
    int numStages = numInitStages();
    if (stage < numStages) {
        // switch context for the duration of the call
        Enter_Method_Silent("initialize(%d)", stage);
        getEnvir()->componentInitBegin(this, stage);
        try {
            initialize(stage);
        }
        catch (cException&) {
            throw;
        }
        catch (std::exception& e) {
            throw cRuntimeError("%s: %s", opp_typename(typeid(e)), e.what());
        }
    }

    // then recursively initialize submodules
    bool moreStages = stage < numStages-1;
    for (SubmoduleIterator it(this); !it.end(); ++it)
        if ((*it)->initializeModules(stage))
            moreStages = true;


    // as a last step, call handleParameterChange() to notify the component about
    // parameter changes that occured during initialization phase
    if (!moreStages) {
        // a module is initialized when all init stages have been completed
        // (both its own and on all its submodules)
        setFlag(FL_INITIALIZED, true);
        handleParameterChange(nullptr);
    }

    return moreStages;
}

void cModule::callFinish()
{
    // This is the interface for calling finish().

    // first call it for submodules and channels...
    for (ChannelIterator it(this); !it.end(); ++it)
        (*it)->callFinish();
    for (SubmoduleIterator it(this); !it.end(); ++it)
        (*it)->callFinish();

    // ...then for this module, in our context: save parameters, then finish()
    cContextSwitcher tmp(this);
    cContextTypeSwitcher tmp2(CTX_FINISH);
    try {
        recordParametersAsScalars();
        //Enter_Method_Silent("finish()");
        finish();
        fireFinish();
    }
    catch (cException&) {
        throw;
    }
    catch (std::exception& e) {
        throw cRuntimeError("%s: %s", opp_typename(typeid(e)), e.what());
    }
}

void cModule::callRefreshDisplay()
{
    // This is the interface for calling refreshDisplay().

    // first call it for submodules and channels...
    for (ChannelIterator it(this); !it.end(); ++it)
        (*it)->callRefreshDisplay();
    for (SubmoduleIterator it(this); !it.end(); ++it)
        (*it)->callRefreshDisplay();

    // ...then for this module, in our context: save parameters, then finish()
    cContextSwitcher tmp(this);
    cContextTypeSwitcher tmp2(CTX_REFRESHDISPLAY);
    try {
        refreshDisplay();
   }
    catch (cException&) {
        throw;
    }
    catch (std::exception& e) {
        throw cRuntimeError("%s: %s", opp_typename(typeid(e)), e.what());
    }
}

void cModule::arrived(cMessage *msg, cGate *ongate, simtime_t)
{
    // by default, cModule acts as compound module (cSimpleModule overrides this)
    throw cRuntimeError("Gate '%s' of compound module (%s)%s is not connected on the %s, "
                        "upon arrival of message (%s)%s",
            ongate->getFullName(),
            getClassName(), getFullPath().c_str(),
            (ongate->isConnectedOutside() ? "inside" : "outside"),
            msg->getClassName(), msg->getName());
}

//----

void cModule::GateIterator::init(const cModule *module)
{
    this->module = module;
    descIndex = 0;
    isOutput = false;
    index = 0;

    while (!end() && current() == nullptr)
        bump();
}

void cModule::GateIterator::bump()
{
    cGate::Desc *desc = module->gateDescArray + descIndex;

    if (desc->name) {
        if (isOutput == false && desc->getType() == cGate::OUTPUT) {
            isOutput = true;
            return;
        }

        if (desc->isVector()) {
            if (index < desc->vectorSize-1) {
                index++;
                return;
            }
            index = 0;
        }
        if (isOutput == false && desc->getType() != cGate::INPUT) {
            isOutput = true;
            return;
        }
    }
    if (descIndex < module->gateDescArraySize) {
        descIndex++;
        isOutput = false;
        index = 0;
    }
}

bool cModule::GateIterator::end() const
{
    return descIndex >= module->gateDescArraySize;
}

cGate *cModule::GateIterator::current() const
{
    if (descIndex >= module->gateDescArraySize)
        return nullptr;
    cGate::Desc *desc = module->gateDescArray + descIndex;
    if (!desc->name)
        return nullptr;  // deleted gate
    if (isOutput == false && desc->getType() == cGate::OUTPUT)
        return nullptr;  // isOutput still incorrect
    if (!desc->isVector())
        return isOutput ? desc->output.gate : desc->input.gate;
    else if (desc->vectorSize == 0)
        return nullptr;
    else
        return isOutput ? desc->output.gatev[index] : desc->input.gatev[index];
}

void cModule::GateIterator::advance()
{
    do {
        bump();
    } while (!end() && current() == nullptr);
}

//----

cModule::ChannelIterator::ChannelIterator(const cModule *parentModule)
{
    init(parentModule);
}

void cModule::ChannelIterator::init(const cModule *parentModule)
{
    // loop through the gates of parentModule and its submodules
    // to fill in the channels[] vector
    bool atParent = false;
    channels.clear();
    for (SubmoduleIterator it(parentModule); !atParent; ++it) {
        const cModule *module = !it.end() ? *it : (atParent = true, parentModule);

        for (GateIterator gateIt(module); !gateIt.end(); ++gateIt) {
            const cGate *gate = *gateIt;
            cGate::Type wantedGateType = atParent ? cGate::INPUT : cGate::OUTPUT;
            if (gate && gate->getChannel() && gate->getType() == wantedGateType)
                channels.push_back(gate->getChannel());
        }
    }

    // reset iterator position too
    k = 0;
}

}  // namespace omnetpp

