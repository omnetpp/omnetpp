//==========================================================================
//   CCOMPONENT.CC  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
#include "common/stringutil.h"
#include "common/stlutil.h"
#include "omnetpp/ccomponent.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/ccontextswitcher.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cmodelchange.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/cpar.h"
#include "omnetpp/cparimpl.h"
#include "omnetpp/crng.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/cstatisticbuilder.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cresultrecorder.h"
#include "omnetpp/cresultfilter.h"

using namespace omnetpp::common;

namespace omnetpp {

Register_PerObjectConfigOption(CFGID_DISPLAY_STRING, "display-string", KIND_COMPONENT, CFG_STRING, nullptr, "Additional display string for the module/channel; it will be merged into the display string given via `@display` properties, and override its content.");
Register_PerObjectConfigOption(CFGID_PARAM_RECORD_AS_SCALAR, "param-record-as-scalar", KIND_PARAMETER, CFG_BOOL, "false", "Applicable to module parameters: specifies whether the module parameter should be recorded into the output scalar file. Set it for parameters whose value you will need for result analysis.");

cComponent::SignalNameMapping *cComponent::signalNameMapping = nullptr;
int cComponent::lastSignalID = -1;

static const int NOTIFICATION_STACK_SIZE = 64;
cIListener **cComponent::notificationStack[NOTIFICATION_STACK_SIZE];
int cComponent::notificationSP = 0;

bool cComponent::checkSignals;

simsignal_t PRE_MODEL_CHANGE = cComponent::registerSignal("PRE_MODEL_CHANGE");
simsignal_t POST_MODEL_CHANGE = cComponent::registerSignal("POST_MODEL_CHANGE");

EXECUTE_ON_SHUTDOWN(cComponent::clearSignalRegistrations());

std::vector<int> cComponent::signalListenerCounts;

// Calling registerSignal in static initializers of runtime loaded dynamic
// libraries would cause an assertion failure without this:
EXECUTE_ON_STARTUP(cComponent::clearSignalState());

std::vector<cComponent::ResultRecorderList*> cComponent::cachedResultRecorderLists;

EXECUTE_ON_SHUTDOWN(cComponent::invalidateCachedResultRecorderLists())


cComponent::cComponent(const char *name) : cSoftOwner(name)
{
    componentType = nullptr;
    simulation = nullptr;
    componentId = -1;
    rngMapSize = 0;
    rngMap = nullptr;
    lastCompletedInitStage = -1;

    parArraySize = numPars = 0;
    parArray = nullptr;

    displayString = nullptr;

    signalTable = nullptr;

    setNamePooling(true);
    setLogLevel(LOGLEVEL_TRACE);
}

// cannot throw in destructors, so just print to stderr instead
#define ASSERT_SOFT(expr) \
  ((void) ((expr) ? 0 : fprintf(stderr, "ASSERT: Condition '%s' does not hold in function '%s' at %s:%d", #expr, __FUNCTION__, __FILE__, __LINE__), 0))

cComponent::~cComponent()
{
    if (componentId != -1)
        simulation->deregisterComponent(this);

    ASSERT_DTOR(signalTable == nullptr);  // note: releaseLocalListeners() gets called in subclasses, ~cModule and ~cChannel

    delete[] rngMap;
    delete[] parArray;
    delete displayString;

    if (selfPointers) {
        for (void **pptr : *selfPointers)
            *pptr = nullptr;
        delete selfPointers;
    }

    reportUndisposed();
    removeFromOwnershipTree();
}

void cComponent::forEachChild(cVisitor *v)
{
    for (int i = 0; i < numPars; i++)
        v->visit(&parArray[i]);

    if (signalTable) {
        int n = signalTable->size();
        for (int i = 0; i < n; i++) {
            cIListener **listeners = (*signalTable)[i].listeners;
            for (int j = 0; listeners[j]; j++)
                if (cObject *listenerObj = dynamic_cast<cObject*>(listeners[j]))
                    // Filter the objects we recurse into, to avoid infinite recursion and double-visiting:
                    // 1. avoid self-visiting (module hooked on itself as listener)
                    // 2. only visit owner-less objects (objects owned by others can cause infinite recursion
                    // as they are visited from their parents too; objects owned by this module may already
                    // be visited from this module via another place.)
                    if (listenerObj != this && listenerObj->getOwner() == nullptr)
                        if (!v->visit(listenerObj))
                            return;
        }
    }

    cSoftOwner::forEachChild(v);
}

void cComponent::setComponentType(cComponentType *componenttype)
{
    this->componentType = componenttype;
}

void cComponent::initialize()
{
    // Called before simulation starts (or usually after dynamic module was created).
    // Should be redefined by user.
}

void cComponent::finish()
{
    // Called at the end of a successful simulation (and usually before destroying a dynamic module).
    // Should be redefined by user.
}

void cComponent::handleParameterChange(const char *)
{
    // Called when a module parameter changes.
    // Can be redefined by the user.
}

void cComponent::refreshDisplay() const
{
    // Called by graphical user interfaces when the display needs to be refreshed.
    // Can be redefined by the user.
}

void cComponent::preDelete(cComponent *root)
{
}

void cComponent::setDisplayName(const char *name)
{
    displayName = name;
}

cComponentType *cComponent::getComponentType() const
{
    if (!componentType)
        throw cRuntimeError(this, "Object has no associated cComponentType (maybe %s is not derived from cModule/cChannel?)", getClassName());
    return componentType;
}

const char *cComponent::getNedTypeName() const
{
    return getComponentType()->getFullName();
}

std::string cComponent::getNedTypeAndFullName() const
{
    std::stringstream os;
    os << "(" << getNedTypeName() << ")" << getFullName();
    return os.str();
}

std::string cComponent::getNedTypeAndFullPath() const
{
    std::stringstream os;
    os << "(" << getNedTypeName() << ")" << getFullPath();
    return os.str();
}

cModule *cComponent::getSystemModule() const
{
    return simulation->getSystemModule();
}

cModule *cComponent::getModuleByPath(const char *path) const
{
    cModule *module = findModuleByPath(path);
    if (!module)
        throw cRuntimeError(this, "getModuleByPath(): Module '%s' not found (Note: Operation of getModuleByPath() has changed in OMNeT++ version 6.0, use findModuleByPath() if you want the original behavior)", path);
    return module;
}

cModule *cComponent::findModuleByPath(const char *path) const
{
    return doFindModuleByPath(path);
}

cRNG *cComponent::getRNG(int k) const
{
    return getEnvir()->getRNG(k < rngMapSize ? rngMap[k] : k);
}

void cComponent::setLogLevel(LogLevel logLevel)
{
    ASSERT(0 <= logLevel && logLevel <= 7);
    flags &= ~(0x7 << FL_LOGLEVEL_SHIFT);
    flags |= logLevel << FL_LOGLEVEL_SHIFT;
}

void cComponent::addResultRecorders()
{
    cStatisticBuilder(getEnvir()->getConfig()).addResultRecorders(this);
}

void cComponent::emitStatisticInitialValues()
{
    if (signalTable) {
        int n = signalTable->size();
        for (int i = 0; i < n; i++)
            for (cIListener **lp = (*signalTable)[i].listeners; *lp; ++lp)
                if (auto rl = dynamic_cast<cResultListener *>(*lp))
                    rl->callEmitInitialValue();
    }
}

void cComponent::reallocParamv(int size)
{
    ASSERT(size >= numPars);
    if (size != (short)size)
        throw cRuntimeError(this, "reallocParamv(%d): At most %d parameters allowed", size, 0x7fff);
    cPar *newparamv = new cPar[size];
    for (int i = 0; i < numPars; i++)
        parArray[i].moveto(newparamv[i]);
    delete[] parArray;
    parArray = newparamv;
    parArraySize = (short)size;
}

void cComponent::addPar(cParImpl *value)
{
    if (parametersFinalized())
        throw cRuntimeError(this, "Cannot add parameters at runtime");
    if (findPar(value->getName()) >= 0)
        throw cRuntimeError(this, "Cannot add parameter '%s': It already exists", value->getName());
    if (numPars == parArraySize)
        reallocParamv(parArraySize+1);
    parArray[numPars++].init(this, value);
}

cPar& cComponent::par(int k)
{
    if (k < 0 || k >= numPars)
        throw cRuntimeError(this, "Parameter id %d out of range", k);
    cPar& p = parArray[k];
    simulation->parameterAccessed(&p);
    return p;
}

cPar& cComponent::par(const char *parName)
{
    int k = findPar(parName);
    if (k < 0)
        throw cRuntimeError(this, "Unknown parameter '%s'", parName);
    cPar& p = parArray[k];
    simulation->parameterAccessed(&p);
    return p;
}

int cComponent::findPar(const char *parName) const
{
    int n = getNumParams();
    for (int i = 0; i < n; i++)
        if (parArray[i].isName(parName))
            return i;
    return -1;
}

void cComponent::finalizeParameters()
{
    if (parametersFinalized())
        throw cRuntimeError(this, "finalizeParameters() already called for this module or channel");

    // temporarily switch context
    cContextSwitcher tmp(this);
    cContextTypeSwitcher tmp2(CTX_BUILD);

    getComponentType()->applyPatternAssignments(this);

    // read parameters from that are still not set;
    // we need two stages (read+finalize) because of possible cross-parameter references
    int n = getNumParams();
    for (int i = 0; i < n; i++)
        par(i).read();
    for (int i = 0; i < n; i++)
        par(i).finalize();

    simulation->parametersAdded(this);

    setFlag(FL_PARAMSFINALIZED, true);

    // take display-string config option into account
    std::string dispstr = getSimulation()->getEnvir()->getConfig()->getAsString(getFullPath().c_str(), CFGID_DISPLAY_STRING);
    if (!dispstr.empty())
        getDisplayString().updateWith(dispstr.c_str());

    // always store the display string
    EVCB.displayStringChanged(this);
}

bool cComponent::hasGUI() const
{
    return simulation->getEnvir()->isGUI();
}

bool cComponent::hasDisplayString()
{
    if (displayString)
        return true;
    if (flags & FL_DISPSTR_CHECKED)
        return flags & FL_DISPSTR_NOTEMPTY;

    // not yet checked: do it now
    cProperties *props = getProperties();
    cProperty *prop = props->get("display");
    const char *propValue = prop ? prop->getValue(cProperty::DEFAULTKEY) : nullptr;
    bool result = !opp_isempty(propValue);

    setFlag(FL_DISPSTR_CHECKED, true);
    setFlag(FL_DISPSTR_NOTEMPTY, result);
    return result;
}

cDisplayString& cComponent::getDisplayString() const
{
    if (!displayString) {
        // note: do not throw exceptions
        cProperties *props = getProperties();
        cProperty *prop = props->get("display");
        const char *propValue = prop ? prop->getValue(cProperty::DEFAULTKEY) : nullptr;
        if (propValue)
            displayString = new cDisplayString(propValue);
        else
            displayString = new cDisplayString();
        displayString->setHostObject(const_cast<cComponent*>(this));
    }
    return *displayString;
}

void cComponent::setDisplayString(const char *s)
{
    getDisplayString().parse(s);
}

void cComponent::bubble(const char *text) const
{
    getEnvir()->bubble(const_cast<cComponent*>(this), text);
}

std::string cComponent::resolveResourcePath(const char *fileName) const
{
    return getEnvir()->resolveResourcePath(fileName, getComponentType());
}

void cComponent::recordParameters()
{
    int n = getNumParams();
    cEnvir *envir = getEnvir();
    cConfiguration *config = envir->getConfig();

    envir->recordComponentType(this);

    for (int i = 0; i < n; i++) {
        cPar *p = &par(i);
        envir->recordParameter(p); // enablement is handled inside

        // legacy recording
        if (config->getAsBool(p->getFullPath().c_str(), CFGID_PARAM_RECORD_AS_SCALAR, false))
            recordParameterAsScalar(p);
    }
}

void cComponent::recordParameterAsScalar(cPar *par)
{
    switch(par->getType()) {
    case cPar::BOOL:
        recordScalar(par->getName(), par->boolValue());
        break;
    case cPar::INT:
        if (par->isVolatile() && (par->intValue() != par->intValue() || par->intValue() != par->intValue()))  // crude check for random variates
            throw cRuntimeError(this, "Refusing to record volatile parameter '%s' that contains a non-constant value (probably a random variate) as an output scalar: Recorded value would likely be a meaningless random number", par->getName());
        recordScalar(par->getName(), par->intValue());
        break;
    case cPar::DOUBLE:
        if (par->isVolatile() && (par->doubleValue() != par->doubleValue() || par->doubleValue() != par->doubleValue()))  // crude check for random variates
            throw cRuntimeError(this, "Refusing to record volatile parameter '%s' that contains a non-constant value (probably a random variate) as an output scalar: Recorded value would likely be a meaningless random number", par->getName());
        recordScalar(par->getName(), par->doubleValue());
        break;
    default:
        throw cRuntimeError(this, "Cannot record non-numeric parameter '%s' as an output scalar", par->getName());
    }
}

void cComponent::recordScalar(const char *name, double value, const char *unit)
{
    if (!unit)
        getEnvir()->recordScalar(this, name, value);
    else {
        opp_string_map attributes;
        attributes["unit"] = unit;
        getEnvir()->recordScalar(this, name, value, &attributes);
    }
}

void cComponent::recordStatistic(cStatistic *stats, const char *unit)
{
    stats->recordAs(nullptr, unit);
}

void cComponent::recordStatistic(const char *name, cStatistic *stats, const char *unit)
{
    stats->recordAs(name, unit);
}

//----

const char *cComponent::SignalListenerList::getSignalName() const
{
    return cComponent::getSignalName(signalID);
}

std::string cComponent::SignalListenerList::str() const
{
    std::stringstream out;
    out << "'" << getSignalName() << "': " << countListeners() << " listeners";
    return out.str();
}

bool cComponent::SignalListenerList::addListener(cIListener *l)
{
    if (findListener(l) != -1)
        return false;  // already subscribed

    // reallocate each time (subscribe operations are rare, so we optimize for memory footprint)
    int n = countListeners();
    cIListener **v = new cIListener *[n + 2];
    std::copy_n(listeners, n, v);
    v[n] = l;
    v[n+1] = nullptr;
    delete[] listeners;
    listeners = v;
    return true;
}

bool cComponent::SignalListenerList::removeListener(cIListener *l)
{
    int k = findListener(l);
    if (k == -1)
        return false;  // not there
    while (listeners[k]) {
        listeners[k] = listeners[k+1];
        k++;
    }
    return true;
}

int cComponent::SignalListenerList::countListeners() const
{
    if (!listeners)
        return 0;
    int k = 0;
    while (listeners[k])
        k++;
    return k;
}

int cComponent::SignalListenerList::findListener(cIListener *l) const
{
    if (!listeners)
        return -1;
    for (int k = 0; listeners[k]; k++)
        if (listeners[k] == l)
            return k;
    return -1;
}

simsignal_t cComponent::registerSignal(const char *name)
{
    if (signalNameMapping == nullptr)
        signalNameMapping = new SignalNameMapping;

    std::map<std::string,simsignal_t>::iterator it = signalNameMapping->signalNameToID.find(name);
    if (it == signalNameMapping->signalNameToID.end()) {
        // assign ID, register name
        simsignal_t signalID = ++lastSignalID;
        signalNameMapping->signalNameToID[name] = signalID;
        signalNameMapping->signalIDToName[signalID] = name;
        if (cStaticFlag::insideMain()) { // otherwise signalListenerCount[] may not have been initialized by C++ yet
            signalListenerCounts.push_back(0);
            ASSERT((int)signalListenerCounts.size() == lastSignalID+1);
        }
        return signalID;
    }
    else {
        return it->second;
    }
}

const char *cComponent::getSignalName(simsignal_t signalID)
{
    if (!signalNameMapping)
        return nullptr;
    std::map<simsignal_t,std::string>::iterator it = signalNameMapping->signalIDToName.find(signalID);
    return it != signalNameMapping->signalIDToName.end() ? it->second.c_str() : nullptr;
}

void cComponent::clearSignalState()
{
    // note: registered signals remain intact

    // reset listener counts
    signalListenerCounts.resize(lastSignalID+1);
    for (int & listenerCount : signalListenerCounts)
        listenerCount = 0;

    // clear notification stack
    notificationSP = 0;
}

void cComponent::clearSignalRegistrations()
{
    delete signalNameMapping;
    signalNameMapping = nullptr;
}

cComponent::SignalListenerList *cComponent::findListenerList(simsignal_t signalID) const
{
    // note: we could use std::binary_search() instead of linear search here,
    // but the number of signals that have listeners is likely to be small (<10),
    // so linear search is probably faster.
    if (signalTable) {
        int n = signalTable->size();
        for (int i = 0; i < n; i++)
            if ((*signalTable)[i].signalID == signalID)
                return &(*signalTable)[i];
    }
    return nullptr;
}

cComponent::SignalListenerList *cComponent::findOrCreateListenerList(simsignal_t signalID)
{
    SignalListenerList *listenerList = findListenerList(signalID);
    if (!listenerList) {
        if (!signalTable)
            signalTable = new SignalTable;

        // add new entry
        signalTable->push_back(SignalListenerList());
        listenerList = &(*signalTable)[signalTable->size()-1];
        listenerList->signalID = signalID;

        // sort signalTable[] so we can do binary search by signalID
        std::sort(signalTable->begin(), signalTable->end(), SignalListenerList::gt);
        listenerList = findListenerList(signalID);  // must find it again because sort() moved it
    }
    return listenerList;
}

void cComponent::throwInvalidSignalID(simsignal_t signalID) const
{
    throw cRuntimeError(this, "Invalid signal signalID=%d", signalID);
}

void cComponent::checkNotFiring(simsignal_t signalID, cIListener **listenerList)
{
    // Check that the given listener list is not being notified.
    // NOTE: we use the listener list and not ListenerList* for a reason:
    // ListenerList may get moved as a result of new signals being subscribed
    // (they are stored in an std::vector!), so checking for pointer equality
    // would be no good. Move does not change the listener list pointer.
    if (listenerList)
        for (int i = 0; i < notificationSP; i++)
            if (notificationStack[i] == listenerList)
                throw cRuntimeError(this, "subscribe()/unsubscribe() failed: Cannot update listener list "
                                          "while its listeners are being notified, signalID=%d", signalID);
}

void cComponent::removeListenerList(simsignal_t signalID)
{
    if (signalTable) {
        // find in signal table
        int i, n = signalTable->size();
        for (i = 0; i < n; i++)
            if ((*signalTable)[i].signalID == signalID)
                break;

        // if found, remove
        if (i < n) {
            ASSERT(!(*signalTable)[i].hasListener());  // must not have listeners
            (*signalTable)[i].dispose();
            signalTable->erase(signalTable->begin() + i);
        }
        if (signalTable->empty()) {
            delete signalTable;
            signalTable = nullptr;
        }
    }
}

bool cComponent::hasListeners(simsignal_t signalID) const
{
    // check local listeners
    if (signalTable) {
        int n = signalTable->size();
        for (int i = 0; i < n; i++)
            if ((*signalTable)[i].signalID == signalID)
                return true;
    }

    // if not found, check ancestors
    cModule *parent = getParentModule();
    return parent ? parent->hasListeners(signalID) : false;
}

void cComponent::emit(simsignal_t signalID, bool b, cObject *details)
{
    if (checkSignals)
        getComponentType()->checkSignal(signalID, SIMSIGNAL_BOOL);
    if (mayHaveListeners(signalID))
        fire(this, signalID, b, details);
}

void cComponent::doEmit(simsignal_t signalID, intval_t i, cObject *details)
{
    if (checkSignals)
        getComponentType()->checkSignal(signalID, SIMSIGNAL_INT);
    if (mayHaveListeners(signalID))
        fire(this, signalID, i, details);
}

void cComponent::doEmit(simsignal_t signalID, uintval_t i, cObject *details)
{
    if (checkSignals)
        getComponentType()->checkSignal(signalID, SIMSIGNAL_UINT);
    if (mayHaveListeners(signalID))
        fire(this, signalID, i, details);
}

void cComponent::emit(simsignal_t signalID, double d, cObject *details)
{
    if (checkSignals)
        getComponentType()->checkSignal(signalID, SIMSIGNAL_DOUBLE);
    if (mayHaveListeners(signalID))
        fire(this, signalID, d, details);
}

void cComponent::emit(simsignal_t signalID, const SimTime& t, cObject *details)
{
    if (checkSignals)
        getComponentType()->checkSignal(signalID, SIMSIGNAL_SIMTIME);
    if (mayHaveListeners(signalID))
        fire(this, signalID, t, details);
}

void cComponent::emit(simsignal_t signalID, const char *s, cObject *details)
{
    if (s == nullptr)
        throw cRuntimeError(this, "emit(): Emitting nullptr as string (const char *) signal value is not allowed, signalID=%d", signalID);
    if (checkSignals)
        getComponentType()->checkSignal(signalID, SIMSIGNAL_STRING);
    if (mayHaveListeners(signalID))
        fire(this, signalID, s, details);
}

void cComponent::emit(simsignal_t signalID, cObject *obj, cObject *details)
{
    if (checkSignals)
        getComponentType()->checkSignal(signalID, SIMSIGNAL_OBJECT, obj);
    if (mayHaveListeners(signalID))
        fire(this, signalID, obj, details);
}

template<typename T>
void cComponent::fire(cComponent *source, simsignal_t signalID, T x, cObject *details)
{
    // notify local listeners if there are any
    SignalListenerList *listenerList = findListenerList(signalID);
    if (listenerList) {
        cIListener **listeners = listenerList->listeners;
        if (notificationSP >= NOTIFICATION_STACK_SIZE)
            throw cRuntimeError(this, "emit(): Recursive notification stack overflow, signalID=%d", signalID);

        int oldNotificationSP = notificationSP;
        try {
            notificationStack[notificationSP++] = listeners;  // lock against modification
            for (int i = 0; listeners[i]; i++)
                listeners[i]->receiveSignal(source, signalID, x, details);  // will crash if listener is already deleted
            notificationSP--;
        }
        catch (std::exception& e) {
            notificationSP = oldNotificationSP;
            throw;
        }
    }

    // notify ancestors recursively
    cModule *parent = getParentModule();
    if (parent)
        parent->fire(source, signalID, x, details);
}

void cComponent::fireFinish()
{
    if (signalTable) {
        int n = signalTable->size();
        for (int i = 0; i < n; i++)
            for (cIListener **lp = (*signalTable)[i].listeners; *lp; ++lp)
                (*lp)->finish(this, (*signalTable)[i].signalID);
    }
}

void cComponent::subscribe(simsignal_t signalID, cIListener *listener)
{
    // check that the signal exits
    if (signalID > lastSignalID)
        throw cRuntimeError("subscribe(): Not a valid signal: SignalID=%d", signalID);

    // add to local listeners
    SignalListenerList *listenerList = findOrCreateListenerList(signalID);
    checkNotFiring(signalID, listenerList->listeners);
    if (!listenerList->addListener(listener))
        throw cRuntimeError(this, "subscribe(): Listener already subscribed at this component to signal '%s' (id=%d)", getSignalName(signalID), signalID);
    signalListenerCounts[signalID]++;
    listener->subscriptions.push_back(std::pair<cComponent*,simsignal_t>(this,signalID));
    listener->subscribedTo(this, signalID);
}

void cComponent::unsubscribe(simsignal_t signalID, cIListener *listener)
{
    // check that the signal exits
    if (signalID > lastSignalID)
        throw cRuntimeError("unsubscribe(): Not a valid signal: SignalID=%d", signalID);

    // remove from local listeners list
    SignalListenerList *listenerList = findListenerList(signalID);
    if (!listenerList)
        return;
    checkNotFiring(signalID, listenerList->listeners);
    if (!listenerList->removeListener(listener))
        return;  // was already removed

    if (!listenerList->hasListener())
        removeListenerList(signalID);

    signalListenerCounts[signalID]--;
    ASSERT(signalListenerCounts[signalID] >= 0);
    auto subscription = std::pair<cComponent*,simsignal_t>(this,signalID);
    ASSERT(contains(listener->subscriptions, subscription));
    remove(listener->subscriptions, subscription);
    listener->unsubscribedFrom(this, signalID);
}

bool cComponent::isSubscribed(simsignal_t signalID, cIListener *listener) const
{
    SignalListenerList *listenerList = findListenerList(signalID);
    return listenerList && listenerList->findListener(listener) != -1;
}

void cComponent::subscribe(const char *signalName, cIListener *listener)
{
    subscribe(registerSignal(signalName), listener);
}

bool cComponent::isSubscribed(const char *signalName, cIListener *listener) const
{
    return isSubscribed(registerSignal(signalName), listener);
}

void cComponent::unsubscribe(const char *signalName, cIListener *listener)
{
    unsubscribe(registerSignal(signalName), listener);
}

std::vector<simsignal_t> cComponent::getLocalListenedSignals() const
{
    std::vector<simsignal_t> result;
    if (signalTable)
        for (auto & listenerList : *signalTable)
            result.push_back(listenerList.signalID);
    return result;
}

std::vector<cIListener *> cComponent::getLocalSignalListeners(simsignal_t signalID) const
{
    std::vector<cIListener *> result;
    SignalListenerList *listenerList = findListenerList(signalID);
    if (listenerList && listenerList->hasListener())
        for (int i = 0; listenerList->listeners[i]; i++)
            result.push_back(listenerList->listeners[i]);
    return result;
}

void cComponent::releaseLocalListeners()
{
    // note: this may NOT be called from our destructor (only subclasses' destructor),
    // because it would result in a "pure virtual method called" error
    if (signalTable) {
        while (signalTable && !signalTable->empty()) {
            SignalListenerList& listenerList = signalTable->front();
            simsignal_t signalID = listenerList.signalID;

            // unsubscribe listeners. Note: a "while (listenerList.hasListener())"
            // loop would not work, because the last unsubscribe deletes listenerList
            // as well. This "unsubscribe n times" method chosen here has problems if new
            // listeners get subscribed inside the unsubscribed() hook though.
            int n = listenerList.countListeners();
            for (int i = 0; i < n; i++)
                unsubscribe(signalID, listenerList.listeners[0]);
        }
        delete signalTable;
        signalTable = nullptr;
    }

/*
    // this is a faster version, but since listener lists and flags are only
    // updated at the end, hasListeners(), isSubscribed() etc. may give
    // strange results when invoked within from unsubscribedFrom().
    if (signalTable)
    {
        // fire unsubscribedFrom() on listeners
        for (int i = 0; i < (int)signalTable->size(); i++) {
            for (cIListener **lp = (*signalTable)[i].listeners; *lp; ++lp) {
                (*lp)->subscribecount--;
                (*lp)->unsubscribedFrom(this, (*signalTable)[i].signalID);
            }
        }
        signalHasLocalListeners = 0;
        delete signalTable;
        signalTable = nullptr;
    }
    cModule *parent = getParentModule();
    signalHasAncestorListeners = parent ? (parent->signalHasLocalListeners | parent->signalHasAncestorListeners) : 0; // this only works if releaseLocalListeners() is called top-down
 */
}

const std::vector<cResultRecorder*>& cComponent::getResultRecorders() const
{
    // return cached copy if exists
    for (auto & cachedResultRecorderList : cachedResultRecorderLists)
        if (cachedResultRecorderList->component == this)
            return cachedResultRecorderList->recorders;

    // not found, create and cache it
    ResultRecorderList *recorderList =  new ResultRecorderList;
    recorderList->component = this;
    collectResultRecorders(recorderList->recorders);
    cachedResultRecorderLists.push_back(recorderList);
    return recorderList->recorders;
}

static void collectResultRecordersRec(std::vector<cResultRecorder*>& result, cIListener *listener)
{
    if (cResultRecorder *recorder = dynamic_cast<cResultRecorder*>(listener))
        result.push_back(recorder);
    else if (cResultFilter *filter = dynamic_cast<cResultFilter*>(listener)) {
        int n = filter->getNumDelegates();
        for (int i = 0; i < n; i++)
            collectResultRecordersRec(result, filter->getDelegate(i));
    }
}

void cComponent::collectResultRecorders(std::vector<cResultRecorder*>& result) const
{
    if (signalTable) {
        int n = signalTable->size();
        for (int i = 0; i < n; i++) {
            cIListener **listeners = (*signalTable)[i].listeners;
            for (int j = 0; listeners[j]; j++)
                collectResultRecordersRec(result, listeners[j]);
        }
    }
}

void cComponent::invalidateCachedResultRecorderLists()
{
    int n = cachedResultRecorderLists.size();
    for (int i = 0; i < n; i++)
        delete cachedResultRecorderLists[i];
    cachedResultRecorderLists.clear();
}


}  // namespace omnetpp

