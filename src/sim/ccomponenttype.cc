//=========================================================================
//  CCOMPONENTTYPE.CC - part of
//
//                    OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
#include <cstring>
#include "common/patternmatcher.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cparimpl.h"
#include "omnetpp/cexception.h"
#include "omnetpp/globals.h"
#include "omnetpp/cdelaychannel.h"
#include "omnetpp/cdataratechannel.h"
#include "omnetpp/cmodelchange.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/cenum.h"
#include "omnetpp/ccanvas.h"
#include "omnetpp/cobjectfactory.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#include "sim/parsim/cplaceholdermod.h"
#endif

using namespace omnetpp::common;

namespace omnetpp {

static const char *getSignalTypeName(SimsignalType type)
{
    switch (type) {
        case SIMSIGNAL_BOOL: return "bool";
        case SIMSIGNAL_LONG: return "long";
        case SIMSIGNAL_ULONG: return "unsigned long";
        case SIMSIGNAL_DOUBLE: return "double";
        case SIMSIGNAL_SIMTIME: return "simtime_t";
        case SIMSIGNAL_STRING: return "string";
        case SIMSIGNAL_OBJECT: return "object";
        default: return nullptr;
    }
};

static SimsignalType getSignalType(const char *name, SimsignalType fallback=SIMSIGNAL_UNDEF)
{
    if (!strcmp(name, "bool")) return SIMSIGNAL_BOOL;
    if (!strcmp(name, "long")) return SIMSIGNAL_LONG;
    if (!strcmp(name, "unsigned long")) return SIMSIGNAL_ULONG;
    if (!strcmp(name, "double")) return SIMSIGNAL_DOUBLE;
    if (!strcmp(name, "simtime_t")) return SIMSIGNAL_SIMTIME;
    if (!strcmp(name, "string")) return SIMSIGNAL_STRING;
    if (!strcmp(name, "object")) return SIMSIGNAL_OBJECT;
    return fallback;
};

//----

cComponentType::cComponentType(const char *qname) : cNoncopyableOwnedObject(qname, false)
{
    // store fully qualified name, and set name to simple (unqualified) name
    qualifiedName = qname;
    const char *lastDot = strrchr(qname, '.');
    setName(!lastDot ? qname : lastDot + 1);
    availabilityTested = available = false;
}

cComponentType::~cComponentType()
{
    for (auto & it : sharedParMap)
        delete it.second;
    for (auto it : sharedParSet)
        delete it;
}

cComponentType *cComponentType::find(const char *qname)
{
    return dynamic_cast<cComponentType *>(componentTypes.getInstance()->lookup(qname));
}

cComponentType *cComponentType::get(const char *qname)
{
    cComponentType *componentType = find(qname);
    if (!componentType) {
        const char *hint = (!qname || !strchr(qname, '.')) ? " (fully qualified type name expected)" : "";
        throw cRuntimeError("NED type \"%s\" not found%s", qname, hint);
    }
    return componentType;
}

cParImpl *cComponentType::getSharedParImpl(const char *key) const
{
    StringToParMap::const_iterator it = sharedParMap.find(key);
    return it == sharedParMap.end() ? nullptr : it->second;
}

void cComponentType::putSharedParImpl(const char *key, cParImpl *value)
{
    ASSERT(sharedParMap.find(key) == sharedParMap.end());  // not yet in there
    value->setIsShared(true);
    sharedParMap[key] = value;
}

// cannot go inline due to declaration order
bool cComponentType::Less::operator()(cParImpl *a, cParImpl *b) const
{
    return a->compare(b) < 0;
}

cParImpl *cComponentType::getSharedParImpl(cParImpl *value) const
{
    ParImplSet::const_iterator it = sharedParSet.find(value);
    return it == sharedParSet.end() ? nullptr : *it;
}

void cComponentType::putSharedParImpl(cParImpl *value)
{
    ASSERT(sharedParSet.find(value) == sharedParSet.end());  // not yet in there
    value->setIsShared(true);
    sharedParSet.insert(value);
}

bool cComponentType::isAvailable()
{
    if (!availabilityTested) {
        const char *className = getImplementationClassName();
        available = classes.getInstance()->lookup(className) != nullptr;
        availabilityTested = true;
    }
    return available;
}

void cComponentType::checkSignal(simsignal_t signalID, SimsignalType type, cObject *obj)
{
    // check that this signal is allowed
    std::map<simsignal_t, SignalDesc>::const_iterator it = signalsSeen.find(signalID);
    if (it == signalsSeen.end()) {
        // ignore built-in signals
        if (signalID == PRE_MODEL_CHANGE || signalID == POST_MODEL_CHANGE)
            return;

        // find @signal property for this signal
        const char *signalName = cComponent::getSignalName(signalID);
        cProperty *prop = getSignalDeclaration(signalName);
        if (!prop)
            throw cRuntimeError("Undeclared signal \"%s\" emitted (@signal missing from NED file?)", signalName);

        // found; extract info from it, and add signal to signalsSeen
        const char *declaredType = prop->getValue("type");
        SignalDesc& desc = signalsSeen[signalID];
        desc.type = !declaredType ? SIMSIGNAL_UNDEF : getSignalType(declaredType, SIMSIGNAL_OBJECT);
        desc.objectType = nullptr;
        desc.isNullable = false;
        if (desc.type == SIMSIGNAL_OBJECT) {
            // if declaredType ends in a question mark, the signal allows nullptr to be emitted as well
            if (declaredType[strlen(declaredType) - 1] == '?') {
                std::string netDeclaredType = std::string(declaredType, strlen(declaredType) - 1);
                desc.objectType = lookupClass(netDeclaredType.c_str());
                desc.isNullable = true;
            }
            else {
                desc.objectType = lookupClass(declaredType);
            }
            if (!desc.objectType)
                throw cRuntimeError("Wrong type \"%s\" in the @signal[%s] property in the \"%s\" NED type, "
                                    "should be one of: long, unsigned long, double, simtime_t, string, or a "
                                    "registered class name optionally followed by a question mark",
                        declaredType, prop->getIndex(), getFullName());
        }
        it = signalsSeen.find(signalID);
    }

    // check data type
    const SignalDesc& desc = it->second;
    if (type == SIMSIGNAL_OBJECT) {
        if (desc.type == SIMSIGNAL_OBJECT) {
            if (desc.objectType && !desc.objectType->isInstance(obj)) {
                cITimestampedValue *timestampedValue = dynamic_cast<cITimestampedValue *>(obj);
                cObject *innerObj;
                if (!timestampedValue) {
                    if (obj)
                        throw cRuntimeError("Signal \"%s\" emitted with wrong class (%s does not subclass from %s as declared)",
                                cComponent::getSignalName(signalID), obj->getClassName(), desc.objectType->getFullName());
                    else if (!desc.isNullable)
                        throw cRuntimeError("Signal \"%s\" emitted a nullptr (specify 'type=%s?' in @signal to allow nullptr)",
                                cComponent::getSignalName(signalID), desc.objectType->getFullName());
                }
                else if (timestampedValue->getValueType(signalID) != SIMSIGNAL_OBJECT)
                    throw cRuntimeError("Signal \"%s\" emitted as timestamped value with wrong type (%s, but object expected)",
                            cComponent::getSignalName(signalID), getSignalTypeName(timestampedValue->getValueType(signalID)));
                else if ((innerObj = timestampedValue->objectValue(signalID)) == nullptr) {
                    if (!desc.isNullable)
                        throw cRuntimeError("Signal \"%s\" emitted as timestamped value with nullptr in it (specify 'type=%s?' in @signal to allow nullptr)",
                                cComponent::getSignalName(signalID), desc.objectType->getFullName());
                }
                else if (!desc.objectType->isInstance(innerObj))
                    throw cRuntimeError("Signal \"%s\" emitted as timestamped value with wrong class in it (%s does not subclass from %s as declared)",
                            cComponent::getSignalName(signalID), innerObj->getClassName(), desc.objectType->getFullName());
            }
        }
        else if (desc.type != SIMSIGNAL_UNDEF) {
            // additionally allow time-stamped value of the appropriate type
            cITimestampedValue *timestampedValue = dynamic_cast<cITimestampedValue *>(obj);
            if (!timestampedValue)
                throw cRuntimeError("Signal \"%s\" emitted with wrong data type (expected=%s, actual=%s)",
                        cComponent::getSignalName(signalID), getSignalTypeName(desc.type), obj ? obj->getClassName() : "nullptr");
            SimsignalType actualType = timestampedValue->getValueType(signalID);
            if (timestampedValue->getValueType(signalID) != desc.type)
                throw cRuntimeError("Signal \"%s\" emitted as timestamped value with wrong data type (expected=%s, actual=%s)",
                        cComponent::getSignalName(signalID), getSignalTypeName(desc.type), getSignalTypeName(actualType));
        }
    }
    else if (type != desc.type && desc.type != SIMSIGNAL_UNDEF) {
        throw cRuntimeError("Signal \"%s\" emitted with wrong data type (expected=%s, actual=%s)",
                cComponent::getSignalName(signalID), getSignalTypeName(desc.type), getSignalTypeName(type));
    }
}

cProperty *cComponentType::getSignalDeclaration(const char *signalName)
{
    std::vector<const char *> declaredSignalNames = getProperties()->getIndicesFor("signal");
    unsigned int i;
    for (i = 0; i < declaredSignalNames.size(); i++) {
        const char *declaredSignalName = declaredSignalNames[i];
        if (declaredSignalName == nullptr)
            continue;  // skip index-less @signal
        if (strcmp(signalName, declaredSignalName) == 0)
            break;
        if (PatternMatcher::containsWildcards(declaredSignalName) &&
            PatternMatcher(declaredSignalName, false, true, true).matches(signalName))
            break;
    }
    bool found = (i != declaredSignalNames.size());
    return found ? getProperties()->get("signal", declaredSignalNames[i]) : nullptr;
}

cObjectFactory *cComponentType::lookupClass(const char *className) const
{
    return cObjectFactory::find(className, getCxxNamespace().c_str(), true);
}

//----

cModuleType::cModuleType(const char *name) : cComponentType(name)
{
}

cModule *cModuleType::create(const char *moduleName, cModule *parentModule)
{
    return create(moduleName, parentModule, -1, 0);
}

cModule *cModuleType::create(const char *moduleName, cModule *parentModule, int vectorSize, int index)
{
    // notify pre-change listeners
    if (parentModule && parentModule->hasListeners(PRE_MODEL_CHANGE)) {
        cPreModuleAddNotification tmp;
        tmp.moduleType = this;
        tmp.moduleName = moduleName;
        tmp.parentModule = parentModule;
        tmp.vectorSize = vectorSize;
        tmp.index = index;
        parentModule->emit(PRE_MODEL_CHANGE, &tmp);
    }

    // set context type to "BUILD"
    cContextTypeSwitcher tmp(CTX_BUILD);

    // Object members of the new module class are collected to tmplist.
    cDefaultList tmpList;
    cDefaultList *oldList = cOwnedObject::getDefaultOwner();
    cOwnedObject::setDefaultOwner(&tmpList);
    cModule *module;
    try {
        // create the new module object
#ifdef WITH_PARSIM
        bool isLocal = getEnvir()->isModuleLocal(parentModule, moduleName, vectorSize < 0 ? -1 : index);
        module = isLocal ? createModuleObject() : new cPlaceholderModule();
#else
        module = createModuleObject();
#endif
    }
    catch (std::exception& e) {
        // restore defaultowner, otherwise it'll remain pointing to a dead object
        cOwnedObject::setDefaultOwner(oldList);
        throw;
    }

    // set up module: set parent, module type, name, vector size
    if (parentModule)
        parentModule->insertSubmodule(module);
    module->setComponentType(this);
    if (vectorSize < 0)
        module->setName(moduleName);
    else
        module->setNameAndIndex(moduleName, index, vectorSize);

    // set system module (must be done before takeAllObjectsFrom(tmplist) because
    // if parentmod==nullptr, mod itself is on tmplist)
    if (!parentModule)
        getSimulation()->setSystemModule(module);

    // put the object members of the new module to their place
    module->takeAllObjectsFrom(tmpList);

    // restore default owner (must precede parameters)
    cOwnedObject::setDefaultOwner(oldList);

    // register with cSimulation
    getSimulation()->registerComponent(module);

    // set up RNG mapping, etc.
    getEnvir()->preconfigure(module);

    // should be called before any gateCreated calls on this module
    EVCB.moduleCreated(module);

    // add parameters and gates to the new module;
    // note: setupGateVectors() will be called from finalizeParameters()
    addParametersAndGatesTo(module);

    // initialize canvas
    if (cCanvas::containsCanvasItems(module->getProperties()))
        module->getCanvas()->addFiguresFrom(module->getProperties());

    // add result recorders, etc
    getEnvir()->configure(module);

    // notify post-change listeners
    if (module->hasListeners(POST_MODEL_CHANGE)) {
        cPostModuleAddNotification tmp;
        tmp.module = module;
        module->emit(POST_MODEL_CHANGE, &tmp);
    }

    // done -- if it's a compound module, buildInside() will do the rest
    return module;
}

cModule *cModuleType::instantiateModuleClass(const char *className)
{
    cObject *obj = cObjectFactory::createOne(className);  // this won't return nullptr
    cModule *module = dynamic_cast<cModule *>(obj);
    if (!module)
        throw cRuntimeError("Incorrect module class %s: Not subclassed from cModule", className);

    // check module object
    if (!module->isModule())
        throw cRuntimeError("Incorrect module class %s: isModule() returns false", className);

    if (isSimple()) {
        if (dynamic_cast<cSimpleModule *>(module) == nullptr)
            throw cRuntimeError("Incorrect simple module class %s: Not subclassed from cSimpleModule", className);
        if (module->isSimple() == false)
            throw cRuntimeError("Incorrect simple module class %s: isSimple() returns false", className);
    }

    return module;
}

cModule *cModuleType::createScheduleInit(const char *moduleName, cModule *parentModule)
{
    if (!parentModule)
        throw cRuntimeError("createScheduleInit(): Parent module pointer cannot be nullptr "
                            "when creating module named '%s' of type %s", moduleName, getFullName());
    cModule *module = create(moduleName, parentModule);
    module->finalizeParameters();
    module->buildInside();
    module->scheduleStart(getSimulation()->getSimTime());
    module->callInitialize();
    return module;
}

cModuleType *cModuleType::find(const char *qname)
{
    return dynamic_cast<cModuleType *>(componentTypes.getInstance()->lookup(qname));
}

cModuleType *cModuleType::get(const char *qname)
{
    cModuleType *p = find(qname);
    if (!p) {
        const char *hint = (!qname || !strchr(qname, '.')) ? " (fully qualified type name expected)" : "";
        throw cRuntimeError("NED module type \"%s\" not found%s", qname, hint);
    }
    return p;
}

//----

cChannelType *cChannelType::idealChannelType;
cChannelType *cChannelType::delayChannelType;
cChannelType *cChannelType::datarateChannelType;

cChannelType::cChannelType(const char *name) : cComponentType(name)
{
}

cChannel *cChannelType::instantiateChannelClass(const char *classname)
{
    cObject *obj = cObjectFactory::createOne(classname);  // this won't return nullptr
    cChannel *channel = dynamic_cast<cChannel *>(obj);
    if (!channel)
        throw cRuntimeError("Class %s is not a channel type", classname);  //FIXME better msg
    return channel;
}

cChannel *cChannelType::create(const char *name)
{
    cContextTypeSwitcher tmp(CTX_BUILD);

    // Object members of the new channel class are collected to tmplist.
    cDefaultList tmplist;
    cDefaultList *oldlist = cOwnedObject::getDefaultOwner();
    cOwnedObject::setDefaultOwner(&tmplist);

    // create channel object
    cChannel *channel;
    try {
        channel = createChannelObject();
    }
    catch (std::exception& e) {
        // restore defaultowner, otherwise it'll remain pointing to a dead object
        cOwnedObject::setDefaultOwner(oldlist);
        throw;
    }

    // determine channel name
    if (!name) {
        cProperty *prop = getProperties()->get("defaultname");
        if (prop)
            name = prop->getValue(cProperty::DEFAULTKEY);
        if (!name)
            name = "channel";
    }

    // set up channel: set name, channel type, etc
    channel->setName(name);
    channel->setComponentType(this);

    // put the object members of the new module to their place
    oldlist->take(channel);
    channel->takeAllObjectsFrom(tmplist);

    // restore default owner
    cOwnedObject::setDefaultOwner(oldlist);

    // register with cSimulation
    getSimulation()->registerComponent(channel);

    // set up RNG mapping, etc.
    getEnvir()->preconfigure(channel);

    // add parameters to the new module
    addParametersTo(channel);

    return channel;
}

cChannelType *cChannelType::find(const char *qname)
{
    return dynamic_cast<cChannelType *>(componentTypes.getInstance()->lookup(qname));
}

cChannelType *cChannelType::get(const char *qname)
{
    cChannelType *p = find(qname);
    if (!p) {
        const char *hint = (!qname || !strchr(qname, '.')) ? " (fully qualified type name expected)" : "";
        throw cRuntimeError("NED channel type \"%s\" not found%s", qname, hint);
    }
    return p;
}

cChannelType *cChannelType::getIdealChannelType()
{
    if (!idealChannelType) {
        idealChannelType = find("ned.IdealChannel");
        ASSERT(idealChannelType);
    }
    return idealChannelType;
}

cChannelType *cChannelType::getDelayChannelType()
{
    if (!delayChannelType) {
        delayChannelType = find("ned.DelayChannel");
        ASSERT(delayChannelType);
    }
    return delayChannelType;
}

cChannelType *cChannelType::getDatarateChannelType()
{
    if (!datarateChannelType) {
        datarateChannelType = find("ned.DatarateChannel");
        ASSERT(datarateChannelType);
    }
    return datarateChannelType;
}

cIdealChannel *cChannelType::createIdealChannel(const char *name)
{
    return cIdealChannel::create(name);
}

cDelayChannel *cChannelType::createDelayChannel(const char *name)
{
    return cDelayChannel::create(name);
}

cDatarateChannel *cChannelType::createDatarateChannel(const char *name)
{
    return cDatarateChannel::create(name);
}

}  // namespace omnetpp

