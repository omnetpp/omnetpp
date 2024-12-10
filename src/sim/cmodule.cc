//======================================================================
//  CMODULE.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
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
#include "common/stlutil.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/ccontextswitcher.h"
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

Register_PerObjectConfigOption(CFGID_DISPLAY_NAME, "display-name", KIND_MODULE, CFG_STRING, nullptr, "Specifies a display name for the module, which is shown e.g. in Qtenv's graphical module view.");

cModule::GateNamePool cModule::gateNamePool;


cModule::cModule()
{
    flags |= FL_BUILTIN_ANIMATIONS;

    // gates and parameters will be added by cModuleType
}

static void alert(cModule *module, const char *msg)
{
    // called from the destructor.
    // note: C++ forbids throwing in a destructor, and noexcept(false) is not workable
    getEnvir()->alert(cRuntimeError(module, "%s", msg).getFormattedMessage().c_str());
}

cModule::~cModule()
{
    // ensure we are invoked from deleteModule()
    if (componentId !=-1 && (flags&FL_DELETING) == 0) {
        alert(this, "Fatal: Direct deletion of a module is illegal, use deleteModule() instead; ABORTING");
        abort();
    }

    // when control gets here, all submodules, connected gates, and listeners should
    // have been already destroyed by deleteModule(). If one still exists, it can only
    // have been added by a subclass destructor. Warn the user about this.

    if (hasSubmodules())
        alert(this, "~cModule(): module should not have submodules at the time cModule destructor runs");
    if (gateDescArraySize != 0)
        alert(this, "~cModule(): module should not to have gates at the time cModule destructor runs");
    if (signalTable)
        alert(this, "~cModule(): module should not have listeners at the time cModule destructor runs");

    // remove from parent
    if (getParentModule())
        getParentModule()->removeSubmodule(this);

    delete subcomponentData;

    delete canvas;
    delete osgCanvas;

    delete[] fullPath;
}

void cModule::deleteModule()
{
    if (getSystemModule() == this && getSimulation()->getSimulationStage() != CTX_CLEANUP)
        throw cRuntimeError(this, "deleteModule(): It is not allowed to delete the system module during simulation");

    // If a coroutine wants to delete itself (maybe as part of a module subtree),
    // that has to be handled from another coroutine, e.g. from the main one.
    // Control is passed there by throwing an exception that gets transferred
    // to the main coroutine by activate(), and handled in cSimulation.
    cSimpleModule *activeModule = getSimulation()->getActivityModule();
    if (activeModule != nullptr && this->containsModule(activeModule))
        throw cDeleteModuleException(this);

    callPreDelete(this);

    doDeleteModule();
}

void cModule::doDeleteModule()
{
    // notify pre-change listeners
    if (hasListeners(PRE_MODEL_CHANGE)) {
        cPreModuleDeleteNotification tmp;
        tmp.module = this;
        emit(PRE_MODEL_CHANGE, &tmp);
    }

    // notify envir while the module object still exists
    EVCB.moduleDeleted(this);

    // delete external connections
    for (GateIterator it(this); !it.end(); ++it) {
        cGate *gate = *it;
        if (gate->isConnectedOutside())
            gate->doDisconnect(false);
    }

    // delete submodules
    if (subcomponentData != nullptr) {
        auto& scalars = subcomponentData->scalarSubmodules;
        while (!scalars.empty())
            scalars.front()->doDeleteModule();
        for (const auto& vector: subcomponentData->submoduleVectors)
            for (cModule *module: vector.array)
                if (module != nullptr)
                    module->doDeleteModule();
    }

    // delete remaining connections
    for (GateIterator it(this); !it.end(); ++it) {
        cGate *gate = *it;
        if (gate->getNextGate())
            gate->doDisconnect(false);
        if (gate->getPreviousGate())
            gate->getPreviousGate()->doDisconnect(false);
    }

    // delete all gates
    clearGates();

    // releasing listeners must be after deleting submodules etc, because our local listeners
    // may be interested in getting notified about submodule deletions
    releaseLocalListeners();

    // delete module object itself
    cModule *parent = getParentModule();
    if (!parent || !parent->hasListeners(POST_MODEL_CHANGE)) {
        // no listeners, just do it
        setFlag(cComponent::FL_DELETING, true);
        cContextSwitcher tmp(this); // module must be in context so that it is allowed to delete model objects it owns
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
        tmp.vectorSize = isVector() ? getVectorSize() : -1;
        tmp.index = isVector() ? getIndex() : -1;

        {
            setFlag(cComponent::FL_DELETING, true);
            cContextSwitcher tmp2(this); // module must be in context so that it is allowed to delete model objects it owns
            delete this;
        }

        // note: this must be executed in the original context
        parent->emit(POST_MODEL_CHANGE, &tmp);
    }
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
        if (!v->visit(*it))
            return;

    cComponent::forEachChild(v);
}

void cModule::setInitialNameAndIndex(const char *name, int index)
{
    ASSERT(parentModule == nullptr);
    cOwnedObject::setName(name);
    vectorIndex = index;
    updateFullName();
}

std::string cModule::str() const
{
    std::stringstream out;
    out << "id=" << getId();
    return out.str();
}

template<typename T>
typename std::vector<T>::iterator findByName(std::vector<T>& v, const std::string& name) {
    for (auto it = v.begin(); it != v.end(); ++it)
        if (it->name == name)
            return it;
    return v.end();
}

template<typename T>
typename std::vector<T>::const_iterator findByName(const std::vector<T>& v, const std::string& name) {
    std::string namestr = name;
    for (auto it = v.cbegin(); it != v.cend(); ++it)
        if (it->name == namestr)
            return it;
    return v.cend();
}

void cModule::insertSubmodule(cModule *mod)
{
    ASSERT(mod->parentModule == nullptr);

    if (!subcomponentData)
        subcomponentData = new SubcomponentData;

    const char *name = mod->getName();
    int index = mod->vectorIndex;
    if (index == -1) {
        if (hasSubmodule(name))
            throw cRuntimeError("Cannot insert module %s into parent %s: a submodule of the same name already exists", mod->getClassAndFullName().c_str(), getNedTypeAndFullPath().c_str());
        if (hasSubmoduleVector(name))
            throw cRuntimeError("Cannot insert module %s into parent %s: a submodule vector of the same name already exists", mod->getClassAndFullName().c_str(), getNedTypeAndFullPath().c_str());
        subcomponentData->scalarSubmodules.push_back(mod);
    }
    else {
        // add to submodule vectors array (name and index must already be set)
        auto it = findByName(subcomponentData->submoduleVectors, name);
        if (it == subcomponentData->submoduleVectors.end())
            throw cRuntimeError("Cannot insert module %s into parent %s: There is no submodule vector named '%s'", mod->getClassAndFullName().c_str(), getNedTypeAndFullPath().c_str(), name);
        auto& array = it->array;
        if (index < 0 || index >= (int)array.size())
            throw cRuntimeError("Cannot insert module %s into parent %s: index is out of range (vector size is %d)", mod->getClassAndFullName().c_str(), getNedTypeAndFullPath().c_str(), (int)array.size());
        if (array.at(index) != nullptr)
            throw cRuntimeError("Cannot insert module %s into parent %s: vector index already occupied", mod->getClassAndFullName().c_str(), getNedTypeAndFullPath().c_str());
        array[index] = mod;
    }

    subcomponentData->submoduleChangeCount++;

    mod->parentModule = this;
    take(mod);

    mod->invalidateFullPathRec();
}

void cModule::removeSubmodule(cModule *mod)
{
    mod->parentModule = nullptr;
    mod->invalidateFullPathRec();

    // NOTE: no drop(mod): anyone can take ownership anyway (because we're soft owners)
    // and otherwise it'd cause trouble if mod itself is in context (it'd get inserted
    // on its own DefaultList)

    int index = mod->vectorIndex;
    if (index == -1) {
        ASSERT(subcomponentData != nullptr);
        auto& submodules = subcomponentData->scalarSubmodules;
        auto it = std::find(submodules.begin(), submodules.end(), mod);
        ASSERT(it != submodules.end());
        submodules.erase(it);
    }
    else {
        // remove from submodule vectors array
        auto& array = getSubmoduleArray(mod->getName());
        ASSERT(array.at(index) == mod);
        array[index] = nullptr;
    }

    subcomponentData->submoduleChangeCount++;
}

void cModule::insertChannel(cChannel *channel)
{
    // note: no take(channel), as channels are owned by their src gates.

    if (!subcomponentData)
        subcomponentData = new SubcomponentData;

    subcomponentData->channels.push_back(channel);
}

void cModule::removeChannel(cChannel *channel)
{
    ASSERT(subcomponentData != nullptr);
    auto it = find(subcomponentData->channels, channel);
    ASSERT(it != subcomponentData->channels.end());
    subcomponentData->channels.erase(it);
}

void cModule::setName(const char *name)
{
    setNameAndIndex(name, vectorIndex);
}

void cModule::setIndex(int index)
{
    setNameAndIndex(getName(), index);
}

void cModule::setNameAndIndex(const char *name, int index)
{
    cModule *parent = getParentModule();
    if (parent == nullptr && index != -1)
        throw cRuntimeError(this, "Cannot set module index to %d: toplevel module cannot be part of a module vector", index);

    // Note: Re-inserting moves module to new submodule vector or vector index,
    // and also ensures new name doesn't collide with existing module names.
    if (parent)
        parent->removeSubmodule(this);

    std::string origName = getName();
    int origIndex = vectorIndex;

    cOwnedObject::setName(name);
    vectorIndex = index;
    updateFullName();

    if (parent) {
        try {
            parent->insertSubmodule(this);
        }
        catch (std::exception& e) {
            // restore consistent state to prevent failed assertions later
            cOwnedObject::setName(origName.c_str());
            vectorIndex = origIndex;
            updateFullName();
            parent->insertSubmodule(this);
            throw;
        }
    }
}

void cModule::updateFullName()
{
    if (!isVector())
        fullName = nullptr;
    else {
        char buf[128];
        fullName = opp_indexedname(buf, sizeof(buf), getName(), getIndex());
    }

    invalidateFullPathRec();

#ifdef SIMFRONTEND_SUPPORT
    updateLastChangeSerial();
#endif
}

void cModule::reassignModuleIdRec()
{
    int oldId = getId();
    cSimulation *sim = getSimulation();
    sim->deregisterComponent(this);
    sim->registerComponent(this);
    int newId = getId();

    cFutureEventSet *fes = sim->getFES();
    int fesLen = fes->getLength();
    for (int i = 0; i < fesLen; i++) {
        cEvent *event = fes->get(i);
        if (cMessage *msg = dynamic_cast<cMessage *>(event))
            if (msg->getArrivalModuleId() == oldId)
                msg->setArrival(newId, msg->getArrivalGateId());
    }

    for (SubmoduleIterator it(this); !it.end(); ++it)
        (*it)->reassignModuleIdRec();
}

void cModule::invalidateFullPathRec()
{
    delete[] fullPath;
    fullPath = nullptr;

    for (SubmoduleIterator it(this); !it.end(); ++it)
        (*it)->invalidateFullPathRec();
}

const char *cModule::getFullName() const
{
    // if not in a vector, normal getName() will do
    return isVector() ? fullName.c_str() : getName();
}

inline char *strdup2(const char *s) // note: opp_strdup() is not suitable because it turns "" into nullptr which std::string doesn't like
{
    char *p = new char[strlen(s)+1];
    strcpy(p,s);
    return p;
}

std::string cModule::getFullPath() const
{
    if (!fullPath)
        fullPath = getParentModule() == nullptr ? strdup2(getFullName()) : strdup2((getParentModule()->getFullPath() + "." + getFullName()).c_str());
    return fullPath;
}

bool cModule::isSimple() const
{
    return false;
}

std::vector<cModule*>& cModule::getSubmoduleArray(const char *name) const
{
    if (subcomponentData != nullptr) {
        std::string namestr = name;
        for (const auto& v : subcomponentData->submoduleVectors)
            if (v.name == namestr)
                return const_cast<std::vector<cModule*>&>(v.array);
    }
    if (hasSubmodule(name))
        throw cRuntimeError("Module '%s' has no submodule vector named '%s' ('%s' is a scalar submodule)", getFullPath().c_str(), name, name);
    else
        throw cRuntimeError("Module '%s' has no submodule vector named '%s'", getFullPath().c_str(), name);
}

int cModule::getVectorSize() const
{
    if (vectorIndex == -1)
        throw cRuntimeError(this, "getVectorSize(): Module is not member of a submodule vector");

    cModule *parent = getParentModule();
    ASSERT(parent != nullptr);
    auto& array = parent->getSubmoduleArray(getName());
    int size = array.size();
    ASSERT(vectorIndex < size);
    return size;
}

int cModule::getIndex() const
{
    if (vectorIndex == -1)
        throw cRuntimeError(this, "getIndex(): Module is not member of a submodule vector");
    return vectorIndex;
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
        tmp.vectorSize = desc->isVector() ? desc->gateSize() : -1;
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
    gateNamePool.clear();
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
    cGate::Desc *newv = new cGate::Desc[gateDescArraySize + 1];
    std::copy_n(gateDescArray, gateDescArraySize, newv);

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
    auto it = gateNamePool.find(key);
    if (it == gateNamePool.end())
        it = gateNamePool.insert(key).first;
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

bool cModule::hasGates() const
{
    return gateDescArraySize != 0;
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

cGate *cModule::addGate(const char *gatename, cGate::Type type)
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
        tmp.isVector = false;
        emit(PRE_MODEL_CHANGE, &tmp);
    }

    // create desc for new gate (or gate vector)
    cGate::Desc *desc = addGateDesc(gatename, type, false);
    desc->owner = this;

    // if scalar gate, create gate object(s); gate vectors are created with size 0.
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
    if (type == cGate::INOUT)
        newGate = nullptr;

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

    return newGate;
}

void cModule::addGateVector(const char *gatename, cGate::Type type, int size)
{
    char suffix;
    if (findGateDesc(gatename, suffix) >= 0)
        throw cRuntimeError(this, "addGateVector(): Gate '%s' already present", gatename);
    if (suffix)
        throw cRuntimeError(this, "addGateVector(): Wrong gate name '%s', must not contain the '$i' or '$o' suffix", gatename);
    if (size < 0)
        throw cRuntimeError(this, "addGateVector(): Negative vector size (%d) requested for gate %s[]", size, gatename);
    if (size > MAX_VECTORGATESIZE)
        throw cRuntimeError(this, "addGateVector(): Vector size for gate %s[] too large (%d), limit is %d", gatename, size, MAX_VECTORGATESIZE);

    // notify pre-change listeners
    if (hasListeners(PRE_MODEL_CHANGE)) {
        cPreGateAddNotification tmp;
        tmp.module = this;
        tmp.gateName = gatename;
        tmp.gateType = type;
        tmp.isVector = true;
        tmp.size = size;
        emit(PRE_MODEL_CHANGE, &tmp);
    }

    // create desc for new gate (or gate vector)
    cGate::Desc *desc = addGateDesc(gatename, type, true);
    desc->owner = this;
    desc->vectorSize = size;

    // create gates
    if (type != cGate::OUTPUT) {
        desc->input.gatev = new cGate*[size];
        for (int i = 0; i < size; i++) {
            cGate *newGate = createGateObject(cGate::INPUT);
            desc->setInputGate(newGate, i);
            EVCB.gateCreated(newGate);
        }
    }
    if (type != cGate::INPUT) {
        desc->output.gatev = new cGate*[size];
        for (int i = 0; i < size; i++) {
            cGate *newGate = createGateObject(cGate::OUTPUT);
            desc->setOutputGate(newGate, i);
            EVCB.gateCreated(newGate);
        }
    }

    // notify post-change listeners
    if (hasListeners(POST_MODEL_CHANGE)) {
        cPostGateAddNotification tmp;
        tmp.module = this;
        tmp.gateName = gatename;
        emit(POST_MODEL_CHANGE, &tmp);
    }
}

static void reallocGatev(cGate **& v, int oldSize, int newSize)
{
    if (oldSize != newSize) {
        cGate **newv = new cGate *[newSize];
        std::copy_n(v, std::min(oldSize, newSize), newv);
        if (newSize > oldSize)
            std::fill_n(newv + oldSize, newSize - oldSize, nullptr);
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
    if (!desc->isVector())
        throw cRuntimeError(this, "Gate '%s' is not a gate vector", gatename);
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

std::vector<std::string> cModule::getGateNames() const
{
    std::vector<std::string> result;
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

bool cModule::hasGateVector(const char *gatename) const
{
    char suffix;
    int descIndex = findGateDesc(gatename, suffix);
    if (descIndex < 0)
        return false;
    const cGate::Desc *desc = gateDescArray + descIndex;
    return desc->isVector();
}

bool cModule::isGateVector(const char *gatename) const
{
    char dummy;
    const cGate::Desc *desc = gateDesc(gatename, dummy);
    return desc->isVector();
}

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
    less_gatePairConnectedInside(cGate **otherv) : otherv(otherv) { }
    bool operator()(cGate *a, cGate *b)
    {
        return (a && a->isConnectedInside()) > (b && b->isConnectedInside()) &&
               (a && otherv[a->getIndex()]->isConnectedInside()) > (b && otherv[b->getIndex()]->isConnectedInside());
    }
};

struct less_gatePairConnectedOutside
{
    cGate **otherv;
    less_gatePairConnectedOutside(cGate **otherv) : otherv(otherv) { }
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
        setGateSize(desc->name->name.c_str(), oldSize + 1);
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
        setGateSize(desc->name->name.c_str(), oldSize + 1);
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
            if (!gate->isConnectedInside())
                throw cRuntimeError(this, "Gate '%s' is not connected to a submodule (or internally to another gate of the same module)", gate->getFullPath().c_str());
        }
    }

    // check submodules
    for (SubmoduleIterator it(this); !it.end(); ++it) {
        cModule *submodule = *it;
        for (GateIterator git(submodule); !git.end(); ++git) {
            cGate *gate = *git;
            if (!gate->isConnectedOutside() &&
                gate->getProperties()->getAsBool("loose") == false &&
                gate->getProperties()->getAsBool("directIn") == false)
                throw cRuntimeError(this, "Gate '%s' is not connected to sibling or parent module", gate->getFullPath().c_str());
        }
    }
    return true;
}

bool cModule::hasSubmodules() const
{
    if (subcomponentData == nullptr)
        return false;
    if (!subcomponentData->scalarSubmodules.empty())
        return true;
    for (const auto& v: subcomponentData->submoduleVectors)
        for (cModule *m: v.array)
            if (m != nullptr)
                return true;
    return false;
}

bool cModule::hasSubmoduleVector(const char *name) const
{
    if (subcomponentData == nullptr)
        return false;
    const auto& submoduleVectors = subcomponentData->submoduleVectors;
    return findByName(submoduleVectors, name) != submoduleVectors.end();
}

std::vector<std::string> cModule::getSubmoduleVectorNames() const
{
     std::vector<std::string> result;
     if (subcomponentData != nullptr)
         for (const auto& v: subcomponentData->submoduleVectors)
             result.push_back(v.name);
     return result;
}

std::vector<std::string> cModule::getSubmoduleNames() const
{
    std::vector<std::string> result;
    if (subcomponentData != nullptr) {
        for (cModule *m : subcomponentData->scalarSubmodules)
            result.push_back(m->getFullName());
        for (const auto& v: subcomponentData->submoduleVectors)
            result.push_back(v.name);
    }
    return result;
}

int cModule::getSubmoduleVectorSize(const char *name) const
{
    auto& array = getSubmoduleArray(name);
    return array.size();
}

void cModule::addSubmoduleVector(const char *name, int size)
{
    if (hasSubmodule(name) || hasSubmoduleVector(name))
        throw cRuntimeError(this, "Cannot add submodule vector '%s': a submodule or submodule vector of the same name already exists", name);

    if (subcomponentData == nullptr)
        subcomponentData = new SubcomponentData;
    auto& submoduleVectors = subcomponentData->submoduleVectors;
    submoduleVectors.push_back(SubmoduleVector());
    submoduleVectors.back().name = name;
    submoduleVectors.back().array.resize(size);
}

void cModule::deleteSubmoduleVector(const char *name)
{
    if (subcomponentData == nullptr)
        subcomponentData = new SubcomponentData;

    auto& submoduleVectors = subcomponentData->submoduleVectors;
    auto it = findByName(submoduleVectors, name);
    if (it == submoduleVectors.end())
        throw cRuntimeError("Module '%s' has no submodule vector named '%s'", getFullPath().c_str(), name);

    for (cModule *submodule : it->array)
        if (submodule)
            submodule->deleteModule();

    submoduleVectors.erase(it);
}

void cModule::setSubmoduleVectorSize(const char *name, int newSize)
{
    auto& array = getSubmoduleArray(name);
    for (int index = newSize; index < (int)array.size(); index++)
        if (cModule *submodule = array[index])
            throw cRuntimeError("Cannot shrink submodule vector '%s.%s[]' to size %d: Submodule '%s' still exists", getFullPath().c_str(), name, newSize, submodule->getFullName());

    array.resize(newSize, nullptr);
}

cModule *cModule::addSubmodule(cModuleType *type, const char *name, int index)
{
    return type->createScheduleInit(name, this, index);
}

bool cModule::hasSubmodule(const char *name, int index) const
{
    return getSubmodule(name, index) != nullptr;
}

int cModule::findSubmodule(const char *name, int index) const
{
    cModule *submodule = getSubmodule(name, index);
    return submodule != nullptr ? submodule->getId() : -1;
}

cModule *cModule::getSubmodule(const char *name, int index) const
{
    if (subcomponentData == nullptr)
        return nullptr;

    if (index == -1) {
        // scalar
        for (cModule *submodule : subcomponentData->scalarSubmodules)
            if (submodule->isName(name))
                return submodule;
        return nullptr;
    }
    else {
        // vector
        auto it = findByName(subcomponentData->submoduleVectors, name);
        if (it == subcomponentData->submoduleVectors.end())
            return nullptr;
        auto& array = it->array;
        if (index < 0 || index >= (int)array.size())
            return nullptr;
        return array[index];
    }
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

cModule *cModule::doFindModuleByPath(const char *path) const
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
                throw cRuntimeError(this, "find/getModuleByPath(): Wrong path '%s', '" ROOTNAME "' may only occur as the first component", path);
            module = module->getSubmodule(token);
        }
        else {
            if (token[strlen(token)-1] != ']')
                throw cRuntimeError(this, "find/getModuleByPath(): Syntax error (unmatched bracket?) in path '%s'", path);
            int index = atoi(lbracket+1);
            *lbracket = '\0';  // cut off [index]
            if (token[0] == '<' && strcmp(token, ROOTNAME) == 0)
                throw cRuntimeError(this, "find/getModuleByPath(): Wrong path '%s', '" ROOTNAME "' may only occur as the first component", path);
            module = module->getSubmodule(token, index);
        }
        token = nextToken(rest);
        isFirst = false;
    }

    return const_cast<cModule*>(module);
}

bool cModule::containsModule(cModule *module) const
{
    while (module) {
        if (module == this)
            return true;
        module = module->getParentModule();
    }
    return false;
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

    // display names may use (local) parameter values
    cConfiguration *config = getSimulation()->getEnvir()->getConfig();
    std::string displayName = config->getAsString(getFullPath().c_str(), CFGID_DISPLAY_NAME);
    setDisplayName(displayName.empty() ? nullptr : displayName.c_str());

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

    // notify post-change listeners
    if (hasListeners(POST_MODEL_CHANGE)) {
        cPostModuleBuildNotification tmp;
        tmp.module = this;
        emit(POST_MODEL_CHANGE, &tmp);
    }
}

void cModule::doBuildInside()
{
    // ask module type to create submodules and internal connections
    getModuleType()->buildInside(this);
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
    invalidateFullPathRec();

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
    cSimulation *simulation = getSimulation();
    if (simulation->getContextType() != CTX_INITIALIZE)
        throw cRuntimeError("Internal function initializeModules() may only be called via callInitialize()");

    if (stage == 0) {
        // call buildInside() if user has forgotten to do it; this is needed
        // to make dynamic module creation more robust
        if (!buildInsideCalled())
            buildInside();

        emitStatisticInitialValues();
    }

    // call initialize(stage) for this module, provided it has not been been initialized yet
    int numStages = numInitStages();
    int ownId = getId();
    if (stage > lastCompletedInitStage && stage < numStages) {
        try {
            // switch context for the duration of the call
            Enter_Method_Silent("initialize(%d)", stage);
            getEnvir()->componentInitBegin(this, stage);

            // call user code
            initialize(stage);

            // bail out if this module was deleted by user code
            if (simulation->getComponent(ownId) == nullptr)
                return false;

            lastCompletedInitStage = stage;
        }
        catch (cException&) {
            throw;
        }
        catch (std::exception& e) {
            throw cRuntimeError("%s: %s", opp_typename(typeid(e)), e.what());
        }
    }

    // then recursively initialize submodules
    //
    // In theory, ths is simple: just call initializeModules(stage) on all
    // submodules, and report if any of them want more stages. However,
    // submodule insertions/deletions during iteration may happen; in that
    // case, just start over. It is possible because every module knows
    // the number of stages it completed, so duplicate initialization can
    // be prevented.

    bool moreStages = stage < numStages-1;
    for (SubmoduleIterator it(this); !it.end(); /**/) {
        cModule *submodule = *it;

        // initialize subtree
        if (submodule->initializeModules(stage))
            moreStages = true;

        // bail out if this whole module got deleted
        if (simulation->getComponent(ownId) == nullptr)
            return false;

        // start over if some submodule was removed/inserted, otherwise take next one
        if (it.changesDetected())
            it.reset();
        else
            ++it;
    }

    // a few more things to do when initialization is complete
    if (!moreStages) {
        // mark as initialized
        setFlag(FL_INITIALIZED, true);

        // notify listeners when this was the last stage
        if (hasListeners(POST_MODEL_CHANGE)) {
            cPostComponentInitializeNotification tmp;
            tmp.component = this;
            emit(POST_MODEL_CHANGE, &tmp);
        }
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
        recordParameters();
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

void cModule::callPreDelete(cComponent *root)
{
    // This is the interface for calling preDelete().

    // first call it for submodules and channels...
    for (ChannelIterator it(this); !it.end(); ++it)
        (*it)->callPreDelete(root);
    for (SubmoduleIterator it(this); !it.end(); ++it)
        (*it)->callPreDelete(root);

    // ...then for this module, in our context
    cContextSwitcher tmp(this);
    cContextTypeSwitcher tmp2(CTX_CLEANUP);
    try {
        preDelete(root);
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

void cModule::arrived(cMessage *msg, cGate *ongate, const SendOptions& options, simtime_t)
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

void cModule::GateIterator::reset()
{
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

void cModule::SubmoduleIterator::reset()
{
    scalarsSlot = -1;
    vectorsSlot = 0;
    vectorIndex = -1;
    initialModuleChangeCount = parent->subcomponentData ? parent->subcomponentData->submoduleChangeCount : 0;
    currentScalar = currentVector = current = nullptr;

    bumpScalars();
    bumpVectors();
    advance();
}

void cModule::SubmoduleIterator::advance()
{
    if (!parent->subcomponentData)
        return;

    // currentScalar/currentVector pointers may become invalid if modules are being deleted
    if (parent->subcomponentData->submoduleChangeCount != initialModuleChangeCount)
        throw cRuntimeError("SubmoduleIterator: Submodule insertion/deletion detected during enumerating the submodules of %s", parent->getNedTypeAndFullPath().c_str());

    // We keep a "cursor" that enumerates scalars submodules, and another
    // "cursor" that enumerates vector submodules. We merge the two iteration
    // sequences by drawing at each step from the one whose next module
    // has the numerically smaller module ID of the two.
    //
    // The numeric values of module IDs correspond the creation order of modules,
    // and because NED creates a submodule vector and all modules in it together
    // in one go and in increasing index order, the following condition ensures
    // that iteration order will reflect the submodule declaration order in
    // NED files. (The order within subcomponentData's scalarSubmodules and
    // submoduleVectors arrays also reflects creation order.)
    //
    if (currentScalar && currentVector) {
        if (currentScalar->getId() < currentVector->getId()) {
            current = currentScalar;
            bumpScalars();
        }
        else {
            current = currentVector;
            bumpVectors();
        }
    }
    else if (currentScalar) {
        current = currentScalar;
        bumpScalars();
    }
    else if (currentVector) {
        current = currentVector;
        bumpVectors();
    }
    else {
        current = nullptr;
    }
}

void cModule::SubmoduleIterator::bumpScalars()
{
    if (parent->subcomponentData == nullptr) {
        currentScalar = nullptr;
        return;
    }

    const auto& scalars = parent->subcomponentData->scalarSubmodules;
    if (scalarsSlot < (int)scalars.size())
        scalarsSlot++;
    currentScalar = (scalarsSlot < (int)scalars.size()) ? scalars[scalarsSlot] : nullptr;
}

void cModule::SubmoduleIterator::bumpVectors()
{
    if (parent->subcomponentData == nullptr) {
        currentVector = nullptr;
        return;
    }

    const auto& vectors = parent->subcomponentData->submoduleVectors;
    int numVectors = vectors.size();
    if (vectorsSlot >= numVectors) {
        currentVector = nullptr;
        return;
    }

    while (true) {
        vectorIndex++;
        while (vectorIndex >= (int)vectors[vectorsSlot].array.size()) {
            vectorsSlot++;
            vectorIndex = 0;
            if (vectorsSlot >= numVectors) {
                currentVector = nullptr;
                return;
            }
        }
        if (vectors[vectorsSlot].array[vectorIndex] != nullptr) {
            currentVector = vectors[vectorsSlot].array[vectorIndex];
            return;
        }
    }

}

bool cModule::SubmoduleIterator::changesDetected() const
{
    int count = parent->subcomponentData ? parent->subcomponentData->submoduleChangeCount : 0;
    return count != initialModuleChangeCount;
}

//----

void cModule::ChannelIterator::reset()
{
    slot = -1;
    advance();
}

void cModule::ChannelIterator::advance()
{
    const auto data = parent->subcomponentData;
    if (data == nullptr) {
        current = nullptr;
        return;
    }

    if (slot < (int)data->channels.size())
        slot++;
    current = (slot < (int)data->channels.size()) ? data->channels[slot] : nullptr;
}

}  // namespace omnetpp

