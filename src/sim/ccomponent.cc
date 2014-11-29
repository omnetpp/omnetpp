//==========================================================================
//   CCOMPONENT.CC  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
#include "ccomponent.h"
#include "ccomponenttype.h"
#include "cmodule.h"
#include "cchannel.h"
#include "cmodelchange.h"
#include "cproperties.h"
#include "cproperty.h"
#include "cpar.h"
#include "cparimpl.h"
#include "crng.h"
#include "cstatistic.h"
#include "cconfiguration.h"
#include "cconfigoption.h"
#include "cdisplaystring.h"
#include "stringutil.h"
#include "cenvir.h"

NAMESPACE_BEGIN


Register_PerObjectConfigOption(CFGID_PARAM_RECORD_AS_SCALAR, "param-record-as-scalar", KIND_PARAMETER, CFG_BOOL, "false", "Applicable to module parameters: specifies whether the module parameter should be recorded into the output scalar file. Set it for parameters whose value you'll need for result analysis.");

#define SIGNALMASK_UNFILLED (~(uint64)0)

cComponent::SignalNameMapping *cComponent::signalNameMapping = NULL;
int cComponent::lastSignalID = -1;

std::vector<uint64> cComponent::signalMasks;
int cComponent::firstFreeSignalMaskBitIndex;

static const int NOTIFICATION_STACK_SIZE = 64;
cIListener **cComponent::notificationStack[NOTIFICATION_STACK_SIZE];
int cComponent::notificationSP = 0;

bool cComponent::checkSignals;

simsignal_t PRE_MODEL_CHANGE = cComponent::registerSignal("PRE_MODEL_CHANGE");
simsignal_t POST_MODEL_CHANGE = cComponent::registerSignal("POST_MODEL_CHANGE");

EXECUTE_ON_SHUTDOWN( cComponent::clearSignalRegistrations() );


cComponent::cComponent(const char *name) : cDefaultList(name)
{
    componenttype = NULL;
    rngmapsize = 0;
    rngmap = 0;

    paramvsize = numparams = 0;
    paramv = NULL;

    dispstr = NULL;
    setEvEnabled(true);

    signalTable = NULL;
    signalHasLocalListeners = signalHasAncestorListeners = 0;
}

cComponent::~cComponent()
{
    ASSERT(signalTable==NULL); // note: releaseLocalListeners() gets called in subclasses, ~cModule and ~cChannel
    delete [] rngmap;
    delete [] paramv;
    delete dispstr;
}

void cComponent::forEachChild(cVisitor *v)
{
    for (int i=0; i<numparams; i++)
        v->visit(&paramv[i]);

    cDefaultList::forEachChild(v);
}

void cComponent::setComponentType(cComponentType *componenttype)
{
    this->componenttype = componenttype;
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

cComponentType *cComponent::getComponentType() const
{
    if (!componenttype)
        throw cRuntimeError(this, "Object has no associated cComponentType (maybe %s should not subclass cModule/cChannel?)", getClassName());
    return componenttype;
}

const char *cComponent::getNedTypeName() const
{
    return getComponentType()->getFullName();
}

void cComponent::reallocParamv(int size)
{
    ASSERT(size>=numparams);
    if (size!=(short)size)
        throw cRuntimeError(this, "reallocParamv(%d): at most %d parameters allowed", size, 0x7fff);
    cPar *newparamv = new cPar[size];
    for (int i=0; i<numparams; i++)
        paramv[i].moveto(newparamv[i]);
    delete [] paramv;
    paramv = newparamv;
    paramvsize = (short)size;
}

void cComponent::addPar(cParImpl *value)
{
    if (parametersFinalized())
        throw cRuntimeError(this, "cannot add parameters at runtime");
    if (findPar(value->getName())>=0)
        throw cRuntimeError(this, "cannot add parameter `%s': already exists", value->getName());
    if (numparams==paramvsize)
        reallocParamv(paramvsize+1);
    paramv[numparams++].init(this, value);
}

cPar& cComponent::par(int k)
{
    if (k<0 || k>=numparams)
        throw cRuntimeError(this, "parameter id %d out of range", k);
    return paramv[k];
}

cPar& cComponent::par(const char *parname)
{
    int k = findPar(parname);
    if (k<0)
        throw cRuntimeError(this, "unknown parameter `%s'", parname);
    return paramv[k];
}

int cComponent::findPar(const char *parname) const
{
    int n = getNumParams();
    for (int i=0; i<n; i++)
        if (paramv[i].isName(parname))
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

    setFlag(FL_PARAMSFINALIZED, true);

    // always store the display string
    EVCB.displayStringChanged(this);
}

bool cComponent::hasDisplayString()
{
    if (dispstr)
        return true;
    if (flags & FL_DISPSTR_CHECKED)
        return flags & FL_DISPSTR_NOTEMPTY;

    // not yet checked: do it now
    cProperties *props = getProperties();
    cProperty *prop = props->get("display");
    const char *propValue = prop ? prop->getValue(cProperty::DEFAULTKEY) : NULL;
    bool result = !opp_isempty(propValue);

    setFlag(FL_DISPSTR_CHECKED, true);
    setFlag(FL_DISPSTR_NOTEMPTY, result);
    return result;
}

cDisplayString& cComponent::getDisplayString()
{
    if (!dispstr)
    {
        // set display string (it may depend on parameter values via "$param" references)
        if (!parametersFinalized())
            throw cRuntimeError(this, "Cannot access display string yet: parameters not yet set up");
        cProperties *props = getProperties();
        cProperty *prop = props->get("display");
        const char *propValue = prop ? prop->getValue(cProperty::DEFAULTKEY) : NULL;
        if (propValue)
            dispstr = new cDisplayString(propValue);
        else
            dispstr = new cDisplayString();
        dispstr->setHostObject(this);
    }
    return *dispstr;
}

void cComponent::setDisplayString(const char *s)
{
    getDisplayString().parse(s);
}

void cComponent::bubble(const char *text)
{
    ev.bubble(this, text);
}

void cComponent::recordParametersAsScalars()
{
    int n = getNumParams();
    for (int i=0; i<n; i++)
    {
        if (ev.getConfig()->getAsBool(par(i).getFullPath().c_str(), CFGID_PARAM_RECORD_AS_SCALAR, false))
        {
            //TODO the following checks should probably produce a WARNING not an error;
            //TODO also, the values should probably be recorded as "param" in the scalar file
            if (par(i).getType() == cPar::BOOL) // note: a bool is not considered to be numeric
                recordScalar(par(i).getName(), par(i).boolValue());
            else if (par(i).isNumeric()) {
                if (par(i).isVolatile() && (par(i).doubleValue()!=par(i).doubleValue() || par(i).doubleValue()!=par(i).doubleValue())) // crude check for random variates
                    throw cRuntimeError(this, "recording volatile parameter `%s' that contains a non-constant value (probably a random variate) as an output scalar -- recorded value is probably just a meaningless random number", par(i).getName());
                recordScalar(par(i).getName(), par(i).doubleValue());
            }
            else
                throw cRuntimeError(this, "cannot record non-numeric parameter `%s' as an output scalar", par(i).getName());
        }
    }
}

void cComponent::recordScalar(const char *name, double value, const char *unit)
{
    if (!unit)
        ev.recordScalar(this, name, value);
    else {
        opp_string_map attributes;
        attributes["unit"] = unit;
        ev.recordScalar(this, name, value, &attributes);
    }
}

void cComponent::recordStatistic(cStatistic *stats, const char *unit)
{
    stats->recordAs(NULL, unit);
}

void cComponent::recordStatistic(const char *name, cStatistic *stats, const char *unit)
{
    stats->recordAs(name, unit);
}

//----

bool cComponent::SignalData::addListener(cIListener *l)
{
    if (findListener(l) != -1)
        return false; // already subscribed

    // reallocate each time (subscribe operations are rare, so we optimize for memory footprint)
    int n = countListeners();
    cIListener **v = new cIListener*[n+2];
    memcpy(v, listeners, n*sizeof(cIListener*));
    v[n] = l;
    v[n+1] = NULL;
    delete [] listeners;
    listeners = v;
    return true;
}

bool cComponent::SignalData::removeListener(cIListener *l)
{
    int k = findListener(l);
    if (k == -1)
        return false; // not there

    // remove listener. note: don't delete listeners[] even if empty,
    // because fire() relies on it not being NULL
    int n = countListeners();
    listeners[k] = listeners[n-1];
    listeners[n-1] = NULL;
    return true;
}

int cComponent::SignalData::countListeners()
{
    if (!listeners)
        return 0;
    int k = 0;
    while (listeners[k])
        k++;
    return k;
}

int cComponent::SignalData::findListener(cIListener *l)
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
    if (signalNameMapping == NULL)
        signalNameMapping = new SignalNameMapping;

    if (signalNameMapping->signalNameToID.find(name) == signalNameMapping->signalNameToID.end())
    {
        // assign ID, register name
        simsignal_t signalID = ++lastSignalID;
        signalNameMapping->signalNameToID[name] = signalID;
        signalNameMapping->signalIDToName[signalID] = name;

        // no signalHasLocalListeners/signalHasAncestorListeners bit index yet
        signalMasks.resize(lastSignalID+1);
        signalMasks[signalID] = SIGNALMASK_UNFILLED;
    }
    return signalNameMapping->signalNameToID[name];
}

const char *cComponent::getSignalName(simsignal_t signalID)
{
    return (signalNameMapping && signalNameMapping->signalIDToName.find(signalID) !=
            signalNameMapping->signalIDToName.end()) ?
                    signalNameMapping->signalIDToName[signalID].c_str() : NULL;
}

void cComponent::clearSignalState()
{
    // note: registered signals remain intact (change in OMNeT++ 4.3)

    // reset mask bit assignments
    firstFreeSignalMaskBitIndex = 0;
    signalMasks.resize(lastSignalID+1);
    for (int i = 0; i < (int)signalMasks.size(); i++)
        signalMasks[i] = SIGNALMASK_UNFILLED;

    // clear notification stack
    notificationSP = 0;
}

void cComponent::clearSignalRegistrations()
{
    delete signalNameMapping;
    signalNameMapping = NULL;
}

cComponent::SignalData *cComponent::findSignalData(simsignal_t signalID) const
{
    // note: we could use std::binary_search() instead of linear search here,
    // but the number of signals that have listeners is likely to be small (<10),
    // so linear search is probably faster.
    if (signalTable)
    {
        int n = signalTable->size();
        for (int i=0; i<n; i++)
            if ((*signalTable)[i].signalID == signalID)
                return &(*signalTable)[i];
    }
    return NULL;
}

cComponent::SignalData *cComponent::findOrCreateSignalData(simsignal_t signalID)
{
    SignalData *data = findSignalData(signalID);
    if (!data)
    {
        if (!signalTable)
            signalTable = new SignalTable;

        // add new entry
        signalTable->push_back(SignalData());
        data = &(*signalTable)[signalTable->size()-1];
        data->signalID = signalID;

        // sort signalTable[] so we can do binary search by signalID
        std::sort(signalTable->begin(), signalTable->end(), SignalData::gt);
        data = findSignalData(signalID); // must find it again because sort() moved it
    }
    return data;
}

void cComponent::checkNotFiring(simsignal_t signalID, cIListener **listenerList)
{
    // Check that the given listener list is not being notified.
    // NOTE: we use the listener list and not SignalData* for a reason:
    // SignalData may get moved as a result of new signals being subscribed
    // (they are stored in an std::vector!), so checking for pointer equality
    // would be no good. Move does not change the listener list pointer.
    if (listenerList)
        for (int i=0; i<notificationSP; i++)
            if (notificationStack[i] == listenerList)
                throw cRuntimeError(this, "subscribe()/unsubscribe() failed: cannot update listener list "
                                          "while its listeners are being notified, signalID=%d", signalID);
}

void cComponent::removeSignalData(simsignal_t signalID)
{
    if (signalTable) {
        // find in signal table
        int i, n = signalTable->size();
        for (i=0; i<n; i++)
            if ((*signalTable)[i].signalID == signalID)
                break;

        // if found, remove
        if (i<n) {
            ASSERT(!(*signalTable)[i].hasListener()); // must not have listeners
            (*signalTable)[i].dispose();
            signalTable->erase(signalTable->begin()+i);
        }
        if (signalTable->empty()) {
            delete signalTable;
            signalTable = NULL;
        }
    }
}

void cComponent::emit(simsignal_t signalID, bool b)
{
    if (checkSignals)
        getComponentType()->checkSignal(signalID, SIMSIGNAL_BOOL);
    fire(this, signalID, getSignalMask(signalID), b);
}

void cComponent::emit(simsignal_t signalID, long l)
{
    if (checkSignals)
        getComponentType()->checkSignal(signalID, SIMSIGNAL_LONG);
    fire(this, signalID, getSignalMask(signalID), l);
}

void cComponent::emit(simsignal_t signalID, unsigned long l)
{
    if (checkSignals)
        getComponentType()->checkSignal(signalID, SIMSIGNAL_ULONG);
    fire(this, signalID, getSignalMask(signalID), l);
}

void cComponent::emit(simsignal_t signalID, double d)
{
    if (checkSignals)
        getComponentType()->checkSignal(signalID, SIMSIGNAL_DOUBLE);
    fire(this, signalID, getSignalMask(signalID), d);
}

void cComponent::emit(simsignal_t signalID, const SimTime& t)
{
    if (checkSignals)
        getComponentType()->checkSignal(signalID, SIMSIGNAL_SIMTIME);
    fire(this, signalID, getSignalMask(signalID), t);
}

void cComponent::emit(simsignal_t signalID, const char *s)
{
    if (checkSignals)
        getComponentType()->checkSignal(signalID, SIMSIGNAL_STRING);
    fire(this, signalID, getSignalMask(signalID), s);
}

void cComponent::emit(simsignal_t signalID, cObject *obj)
{
    if (checkSignals)
        getComponentType()->checkSignal(signalID, SIMSIGNAL_OBJECT, obj);
    fire(this, signalID, getSignalMask(signalID), obj);
}

uint64 cComponent::getSignalMask(simsignal_t signalID)
{
    if (signalID > lastSignalID)
        throw cRuntimeError("emit()/hasListeners(): not a valid signal: signalID=%d", signalID);

    uint64 mask = signalMasks[signalID];

    if (mask == SIGNALMASK_UNFILLED)
    {
        // no bit index assigned to this signal yet; do it
        if (firstFreeSignalMaskBitIndex == 64)
            mask = signalMasks[signalID] = 0; // no more bits
        else
            mask = signalMasks[signalID] = (uint64)1 << firstFreeSignalMaskBitIndex++; // allocate bit index
    }
    return mask;
}

template<typename T>
void cComponent::fire(cComponent *source, simsignal_t signalID, const uint64& mask, T x)
{
    if ((~signalHasLocalListeners & mask) == 0)  // always true for mask==0
    {
        // notify local listeners if there are any
        SignalData *data = findSignalData(signalID);
        if (data)
        {
            cIListener **listeners = data->listeners;
            if (notificationSP >= NOTIFICATION_STACK_SIZE)
                throw cRuntimeError(this, "emit(): recursive notification stack overflow, signalID=%d", signalID);

            int oldNotificationSP = notificationSP;
            try {
                notificationStack[notificationSP++] = listeners; // lock against modification
                for (int i=0; listeners[i]; i++)
                    listeners[i]->receiveSignal(source, signalID, x); // will crash if listener is already deleted
                notificationSP--;
            }
            catch (std::exception& e) {
                notificationSP = oldNotificationSP;
                throw;
            }

        }
    }

    if ((~signalHasAncestorListeners & mask) == 0)  // always true for mask==0
    {
        // notify ancestors recursively
        cModule *parent = getParentModule();
        if (parent)
            parent->fire(source, signalID, mask, x);
    }
}

void cComponent::fireFinish()
{
    if (signalTable)
    {
        int n = signalTable->size();
        for (int i=0; i<n; i++)
            for (cIListener **lp = (*signalTable)[i].listeners; *lp; ++lp)
                (*lp)->finish(this, (*signalTable)[i].signalID);
    }
}

void cComponent::subscribe(simsignal_t signalID, cIListener *listener)
{
    // check that the signal exits
    if (signalID > lastSignalID)
        throw cRuntimeError("subscribe(): not a valid signal: signalID=%d", signalID);

    // add to local listeners
    SignalData *data = findOrCreateSignalData(signalID);
    checkNotFiring(signalID, data->listeners);
    if (!data->addListener(listener))
        throw cRuntimeError(this, "subscribe(): listener already subscribed, signalID=%d (%s)", signalID, getSignalName(signalID));

    uint64 mask = getSignalMask(signalID);
    signalHasLocalListeners |= mask;

    // update hasAncestorListener flag in the whole subtree.
    // Note: if it was true here, it is already true in the whole subtree,
    // so no need to call signalListenerAdded()
    if ((signalHasAncestorListeners & mask) == 0)
    {
        signalListenerAdded(signalID, mask);
        signalHasAncestorListeners &= ~mask; // restore it after signalListenerAdded() set it to true
    }

    listener->subscribecount++;
    listener->subscribedTo(this, signalID);
}

void cComponent::signalListenerAdded(simsignal_t signalID, uint64 mask)
{
    // set the flag recursively in the subtree. If it's already set,
    // no need to recurse because all components must already have it
    // set as well
    if ((signalHasAncestorListeners & mask) == 0)
    {
        signalHasAncestorListeners |= mask;
        if (dynamic_cast<cModule *>(this))
        {
            // Note: because of the dynamic_cast, this part could be moved to cModule
            for (cModule::SubmoduleIterator submod((cModule *)this); !submod.end(); submod++)
                submod()->signalListenerAdded(signalID, mask);
            for (cModule::ChannelIterator chan((cModule *)this); !chan.end(); chan++)
                chan()->signalListenerAdded(signalID, mask);
        }
    }
}

void cComponent::unsubscribe(simsignal_t signalID, cIListener *listener)
{
    // check that the signal exits
    if (signalID > lastSignalID)
        throw cRuntimeError("unsubscribe(): not a valid signal: signalID=%d", signalID);

    // remove from local listeners list
    SignalData *data = findSignalData(signalID);
    if (!data)
        return;
    checkNotFiring(signalID, data->listeners);
    if (!data->removeListener(listener))
        return; // was already removed

    if (!data->hasListener())
    {
        // no local listeners left: remove entry and adjust flags
        removeSignalData(signalID);

        uint64 mask = getSignalMask(signalID);
        signalHasLocalListeners &= ~mask;

        // clear "has ancestor listeners" flags in the whole submodule tree,
        // unless there are still listeners above this module
        if ((signalHasAncestorListeners & mask) == 0)
            signalListenerRemoved(signalID, mask);
    }

    listener->subscribecount--;
    listener->unsubscribedFrom(this, signalID);
}

void cComponent::signalListenerRemoved(simsignal_t signalID, uint64 mask)
{
    // clear the flag recursively in the subtree. If the signal
    // has a listener at a module, leave its subtree alone.
    signalHasAncestorListeners &= ~mask;
    SignalData *data = findSignalData(signalID);
    if (!data || !data->hasListener())
    {
        if (dynamic_cast<cModule *>(this))
        {
            // Note: because of the dynamic_cast, this part could be moved to cModule
            for (cModule::SubmoduleIterator submod((cModule *)this); !submod.end(); submod++)
                submod()->signalListenerRemoved(signalID, mask);
            for (cModule::ChannelIterator chan((cModule *)this); !chan.end(); chan++)
                chan()->signalListenerRemoved(signalID, mask);
        }
    }
}

bool cComponent::isSubscribed(simsignal_t signalID, cIListener *listener) const
{
    SignalData *data = findSignalData(signalID);
    return data && data->findListener(listener) != -1;
}

void cComponent::repairSignalFlags()
{
    // adjusts hasAncestorListeners bits in the component's subtree;
    // to be called when the component's ownership changes
    cModule *p = getParentModule();
    signalHasAncestorListeners = p ? (p->signalHasAncestorListeners | p->signalHasLocalListeners) : 0;

    if (dynamic_cast<cModule *>(this))
    {
        // Note: because of the dynamic_cast, this part could be moved to cModule
        for (cModule::SubmoduleIterator submod((cModule *)this); !submod.end(); submod++)
            submod()->repairSignalFlags();
        for (cModule::ChannelIterator chan((cModule *)this); !chan.end(); chan++)
            chan()->repairSignalFlags();
    }
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
        for (int i=0; i<(int)signalTable->size(); i++)
            result.push_back((*signalTable)[i].signalID);
    return result;
}

std::vector<cIListener*> cComponent::getLocalSignalListeners(simsignal_t signalID) const
{
    std::vector<cIListener*> result;
    SignalData *data = findSignalData(signalID);
    if (data && data->hasListener())
        for (int i=0; data->listeners[i]; i++)
            result.push_back(data->listeners[i]);
    return result;
}

void cComponent::checkLocalSignalConsistency() const
{
    ASSERT((int)signalMasks.size() == lastSignalID+1);
    for (simsignal_t signalID=0; signalID<=lastSignalID; signalID++)
    {
        // determine from the listener lists whether this signal has local and ancestor listeners
        SignalData *data = findSignalData(signalID);
        bool hasLocalListeners = data && data->hasListener();

        bool hasAncestorListeners = false;
        for (cModule *mod = getParentModule(); mod; mod=mod->getParentModule())
        {
            SignalData *data = mod->findSignalData(signalID);
            if (data && data->hasListener())
            {
                hasAncestorListeners = true;
                break;
            }
        }

        // verify the mask bits
        uint64 mask = signalMasks[signalID];
        if (mask == SIGNALMASK_UNFILLED)
        {
            if (hasLocalListeners || hasAncestorListeners)
                throw cRuntimeError(this, "signal has listeners but its signal mask is not yet filled in! signalID=%d", signalID);
        }
        else if (mask != 0)
        {
            if (hasLocalListeners != (bool)(signalHasLocalListeners & mask))
                throw cRuntimeError(this, "hasLocalListeners flag incorrect for signalID=%d", signalID);
            if (hasAncestorListeners != (bool)(signalHasAncestorListeners & mask))
                throw cRuntimeError(this, "signalHasAncestorListeners flag incorrect for signalID=%d", signalID);
        }
    }
}

void cComponent::checkSignalConsistency() const
{
    checkLocalSignalConsistency();
    if (dynamic_cast<const cModule *>(this))
    {
        for (cModule::SubmoduleIterator submod((cModule *)this); !submod.end(); submod++)
            submod()->checkSignalConsistency();
        for (cModule::ChannelIterator chan((cModule *)this); !chan.end(); chan++)
            chan()->checkSignalConsistency();
    }
}

bool cComponent::computeHasListeners(simsignal_t signalID) const
{
    SignalData *data = findSignalData(signalID);
    if (data && data->hasListener())
        return true;
    cModule *parent = getParentModule();
    if (parent)
        return parent->computeHasListeners(signalID);
    return false;
}

void cComponent::releaseLocalListeners()
{
    // note: this may NOT be called from our destructor (only subclasses' destructor),
    // because it would result in a "pure virtual method called" error
    if (signalTable)
    {
        while (signalTable && !signalTable->empty())
        {
            SignalData& signalData = signalTable->front();
            simsignal_t signalID = signalData.signalID;

            // unsubscribe listeners. Note: a "while (signalData.hasListener())"
            // loop would not work, because the last unsubscribe deletes signalData
            // as well. This "unsubscribe n times" method chosen here has problems if new
            // listeners get subscribed inside the unsubscribed() hook though.
            int n = signalData.countListeners();
            for (int i=0; i<n; i++)
                unsubscribe(signalID, signalData.listeners[0]);
        }
        delete signalTable;
        signalTable = NULL;
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
        signalTable = NULL;
    }
    cModule *parent = getParentModule();
    signalHasAncestorListeners = parent ? (parent->signalHasLocalListeners | parent->signalHasAncestorListeners) : 0; // this only works if releaseLocalListeners() is called top-down
*/
}

NAMESPACE_END

