//=========================================================================
//  CSIMULATION.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Definition of global object:
//    simulation
//
//   Member functions of
//    cSimulation  : holds modules and manages the simulation
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include <cstdio>
#include <climits>
#include <algorithm> // copy_n()
#include "common/stringutil.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cpacket.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/csimulation.h"

#include "ctemporaryowner.h"
#include "omnetpp/cscheduler.h"
#include "omnetpp/ceventheap.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/ccontextswitcher.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cparimpl.h"
#include "omnetpp/cfingerprint.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/ccoroutine.h"
#include "omnetpp/clifecyclelistener.h"
#include "omnetpp/platdep/platmisc.h"  // for DEBUG_TRAP

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

#ifdef WITH_NETBUILDER
#include "sim/netbuilder/cnedloader.h"
#endif

#ifdef DEVELOPER_DEBUG
#include <set>
#endif

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {

using std::ostream;

#ifdef DEVELOPER_DEBUG
extern std::set<cOwnedObject *> objectlist;
void printAllObjects();
#endif

Register_PerRunConfigOption(CFGID_PRINT_INSTANTIATED_NED_TYPES, "print-instantiated-ned-types", CFG_BOOL, "false", "When set to true, the simulation will print the names of all instantiated NED types at the end of the simulation. This is useful for coverage tests.");
Register_PerRunConfigOption(CFGID_PRINT_UNUSED_PARAMETERS, "print-unused-parameters", CFG_BOOL, "false", "When set to true, the simulation will print the names of all parameters that have not been accessed during simulation, i.e. did not have any effect, pointing to possible deficiencies in the model.");


/**
 * Stops the simulation at the time it is scheduled for.
 * Used internally by cSimulation.
 */
class SIM_API cEndSimulationEvent : public cEvent, public noncopyable
{
  public:
    cEndSimulationEvent(const char *name, simtime_t simTimeLimit) : cEvent(name) {
        setArrivalTime(simTimeLimit);
        setSchedulingPriority(SHRT_MAX);  // lowest priority
    }
    virtual cEvent *dup() const override { copyNotSupported(); return nullptr; }
    virtual cObject *getTargetObject() const override { return nullptr; }
    virtual void execute() override { delete this; throw cTerminationException(E_SIMTIME); }
};

/**
 * Internal helper class to collect the list of module types used in a simulation
 * (for coverage tests), and the set of parameters queried by the simulation
 * (for detecting parameters that take no effect).
 */
class cUsageCollector
{
  private:
    bool collectInstantiatedNedTypes = false;
    bool collectParameterAccesses = false;
    std::set<std::string> instantiatedNedTypes;
    // map key: component qualified NED type name, value: set of parameter names instances of that type have / access
    std::map<std::string, std::set<std::string>> allParametersPerComponentType;
    std::map<std::string, std::set<std::string>> accessedParametersPerComponentType;
  public:
    void setPrintInstantiatedNedTypes(bool enabled) {collectInstantiatedNedTypes = enabled;}
    void setPrintUnusedParameters(bool enabled) {collectParameterAccesses = enabled;}

    void componentCreated(cComponent *component);
    void parametersAdded(cComponent *component);
    void parameterAccessed(cPar *par);
    void printResults(std::ostream& out);
};

cSimulation::cSimulation(const char *name, cEnvir *env) : cNamedObject(name, false)
{
    ASSERT(cStaticFlag::insideMain());  // cannot be instantiated as global variable

    envir = env;

    // these are not set inline because the declaring header is included here
    simulationStage = CTX_NONE;
    contextType = CTX_NONE;

    // install default FES
    setFES(new cEventHeap("fes"));

    // install default scheduler
    setScheduler(new cSequentialScheduler());
}

cSimulation::~cSimulation()
{
    if (this == activeSimulation)
        // Note: cannot throw from destructors
        panic(cRuntimeError(this, "Cannot delete the active simulation manager object"));

    deleteNetwork();

    delete envir;
    delete fingerprint;
    delete scheduler;
    dropAndDelete(fes);
    delete usageCollector;
}

void cSimulation::setActiveSimulation(cSimulation *sim)
{
    activeSimulation = sim;
    activeEnvir = sim == nullptr ? staticEnvir : sim->envir;
}

void cSimulation::setStaticEnvir(cEnvir *env)
{
    if (!env)
        throw cRuntimeError("cSimulation::setStaticEnvir(): Argument cannot be nullptr");
    staticEnvir = env;
}

void cSimulation::forEachChild(cVisitor *v)
{
    if (systemModule != nullptr)
        v->visit(systemModule);
    v->visit(fes);
}

std::string cSimulation::getFullPath() const
{
    return getFullName();
}

class cSnapshotWriterVisitor : public cVisitor
{
  protected:
    ostream& os;
    int indentLevel = 0;

  public:
    cSnapshotWriterVisitor(ostream& ostr) : os(ostr) {}

    virtual bool visit(cObject *obj) override {
        std::string indent(2 * indentLevel, ' ');
        os << indent << "<object class=\"" << obj->getClassName() << "\" fullpath=\"" << opp_xmlquote(obj->getFullPath()) << "\">\n";
        os << indent << "  <info>" << opp_xmlquote(obj->str()) << "</info>\n";
        indentLevel++;
        obj->forEachChild(this);
        indentLevel--;
        os << indent << "</object>\n\n";
        if (os.fail())
            return false;
        return true;
    }
};

void cSimulation::snapshot(cObject *object, const char *label)
{
    if (!object)
        throw cRuntimeError("snapshot(): Object pointer is nullptr");

    ostream *osptr = getEnvir()->getStreamForSnapshot();
    if (!osptr)
        throw cRuntimeError("Could not create stream for snapshot");

    ostream& os = *osptr;

    os << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    os << "<snapshot\n";
    os << "    object=\"" << opp_xmlquote(object->getFullPath()) << "\"\n";
    os << "    label=\"" << opp_xmlquote(label ? label : "") << "\"\n";
    os << "    simtime=\"" << opp_xmlquote(SIMTIME_STR(simTime())) << "\"\n";
    os << "    network=\"" << opp_xmlquote(networkType ? networkType->getName() : "") << "\"\n";
    os << "    >\n";

    cSnapshotWriterVisitor v(os);
    v.process(object);

    os << "</snapshot>\n";

    bool success = os.good();
    getEnvir()->releaseStreamForSnapshot(&os);
    if (!success)
        throw cRuntimeError("Could not write snapshot");
}

void cSimulation::setScheduler(cScheduler *sch)
{
    if (systemModule)
        throw cRuntimeError(this, "setScheduler(): Cannot switch schedulers when a network is already set up");
    if (!sch)
        throw cRuntimeError(this, "setScheduler(): New scheduler cannot be nullptr");

    if (scheduler) {
        getEnvir()->removeLifecycleListener(scheduler);
        delete scheduler;
    }

    scheduler = sch;
    scheduler->setSimulation(this);
    getEnvir()->addLifecycleListener(scheduler);
}

void cSimulation::setFES(cFutureEventSet *f)
{
    if (systemModule)
        throw cRuntimeError(this, "setFES(): Cannot switch FES when a network is already set up");
    if (fes && !fes->isEmpty())
        throw cRuntimeError(this, "setFES(): Existing FES is not empty");
    if (!f)
        throw cRuntimeError(this, "setFES(): New FES cannot be nullptr");

    if (fes) {
        drop(fes);
        delete fes;
    }

    fes = f;
    fes->setName("scheduled-events");
    take(fes);
}

void cSimulation::setSimulationTimeLimit(simtime_t simTimeLimit)
{
    getFES()->insert(new cEndSimulationEvent("endsimulation", simTimeLimit));
}

int cSimulation::loadNedSourceFolder(const char *folder, const char *excludedPackages)
{
#ifdef WITH_NETBUILDER
    return cNedLoader::getInstance()->loadNedSourceFolder(folder, excludedPackages);
#else
    throw cRuntimeError("Cannot load NED files from '%s': Simulation kernel was compiled without "
                        "support for dynamic loading of NED files (WITH_NETBUILDER=no)", folder);
#endif
}

void cSimulation::loadNedFile(const char *nedFilename, const char *expectedPackage, bool isXML)
{
#ifdef WITH_NETBUILDER
    cNedLoader::getInstance()->loadNedFile(nedFilename, expectedPackage, isXML);
#else
    throw cRuntimeError("Cannot load '%s': Simulation kernel was compiled without "
                        "support for dynamic loading of NED files (WITH_NETBUILDER=no)", nedFilename);
#endif
}

void cSimulation::loadNedText(const char *name, const char *nedText, const char *expectedPackage, bool isXML)
{
#ifdef WITH_NETBUILDER
    cNedLoader::getInstance()->loadNedText(name, nedText, expectedPackage, isXML);
#else
    throw cRuntimeError("Cannot source NED text: Simulation kernel was compiled without "
                        "support for dynamic loading of NED files (WITH_NETBUILDER=no)");
#endif
}

void cSimulation::doneLoadingNedFiles()
{
#ifdef WITH_NETBUILDER
    cNedLoader::getInstance()->doneLoadingNedFiles();
#endif
}

std::string cSimulation::getNedPackageForFolder(const char *folder)
{
#ifdef WITH_NETBUILDER
    return cNedLoader::getInstance()->getNedPackageForFolder(folder);
#else
    return "-";
#endif
}

int cSimulation::registerComponent(cComponent *component)
{
    ASSERT(component->simulation == nullptr && component->componentId == -1);

    lastComponentId++;

    if (lastComponentId >= size) {
        // vector full, grow by delta
        cComponent **v = new cComponent *[size + delta];
        std::copy_n(componentv, size, v);
        for (int i = size; i < size + delta; i++)
            v[i] = nullptr;
        delete[] componentv;
        componentv = v;
        size += delta;
    }

    int id = lastComponentId;
    componentv[id] = component;
    component->simulation = this;
    component->componentId = id;

    if (usageCollector)
        usageCollector->componentCreated(component);

    return id;
}

void cSimulation::deregisterComponent(cComponent *component)
{
    ASSERT(component->simulation == this && component->componentId != -1);

    int id = component->componentId;
    component->simulation = nullptr;
    component->componentId = -1;
    componentv[id] = nullptr;

    if (component == systemModule) {
        cOwningContextSwitcher tmp(&globalOwningContext);
        drop(systemModule);
        systemModule = nullptr;
    }
}

void cSimulation::setSystemModule(cModule *module)
{
    if (systemModule != nullptr)
        throw cRuntimeError(this, "Cannot set module '%s' as toplevel (system) module: there is already a system module ", module->getFullName());
    if (module->parentModule != nullptr)
        throw cRuntimeError(this, "Cannot set module '%s' as toplevel (system) module: it already has a parent module", module->getFullName());
    if (module->vectorIndex != -1)
        throw cRuntimeError(this, "Cannot set module '%s' as toplevel (system) module: toplevel module cannot be part of a module vector", module->getFullName());

    systemModule = module;
    take(module);
}

void cSimulation::parametersAdded(cComponent *component)
{
    if (usageCollector)
        usageCollector->parametersAdded(component);
}

void cSimulation::parameterAccessed(cPar *par)
{
    if (usageCollector)
        usageCollector->parameterAccessed(par);
}

cModule *cSimulation::getModuleByPath(const char *path) const
{
    cModule *module = findModuleByPath(path);
    if (!module)
        throw cRuntimeError("cSimulation::getModuleByPath(): Module at '%s' not found (Note: operation of getModuleByPath() has changed in OMNeT++ version 6.0, use findModuleByPath() in you want the original behavior)", path);
    return module;
}

cModule *cSimulation::findModuleByPath(const char *path) const
{
    cModule *root = getSystemModule();
    if (!root || !path || !path[0])
        return nullptr;
    if (path[0] == '.' || path[0] == '^')
        throw cRuntimeError(this, "cSimulation::getModuleByPath(): Path cannot be relative, i.e. start with '.' or '^' (path='%s')", path);
    return root->findModuleByPath(path);
}

void cSimulation::setupNetwork(cModuleType *network)
{
#ifdef DEVELOPER_DEBUG
    printf("DEBUG: before setupNetwork: %d objects\n", (int)cOwnedObject::getLiveObjectCount());
    objectlist.clear();
#endif

    checkActive();
    if (!network)
        throw cRuntimeError("setupNetwork(): nullptr received");
    if (!network->isNetwork())
        throw cRuntimeError("Cannot set up network: '%s' is not a network type", network->getFullName());

    // set up usage collector if needed
    delete usageCollector;
    usageCollector = nullptr;

    bool printInstantiatedNedTypes = getEnvir()->getConfig()->getAsBool(CFGID_PRINT_INSTANTIATED_NED_TYPES);
    bool printUnusedParameters = getEnvir()->getConfig()->getAsBool(CFGID_PRINT_UNUSED_PARAMETERS);
    if (printInstantiatedNedTypes || printUnusedParameters) {
        usageCollector = new cUsageCollector();
        usageCollector->setPrintInstantiatedNedTypes(printInstantiatedNedTypes);
        usageCollector->setPrintUnusedParameters(printUnusedParameters);
    }

    // set cNetworkType pointer
    networkType = network;

    // just to be sure
    fes->clear();
    cComponent::clearSignalState();

    simulationStage = CTX_BUILD;

    try {
        // set up the network by instantiating the toplevel module
        cContextTypeSwitcher tmp(CTX_BUILD);
        getEnvir()->notifyLifecycleListeners(LF_PRE_NETWORK_SETUP);
        cModule *module = networkType->create(networkType->getName(), nullptr);
        module->finalizeParameters();
        module->buildInside();
        getEnvir()->notifyLifecycleListeners(LF_POST_NETWORK_SETUP);
    }
    catch (std::exception& e) {
        // Note: no deleteNetwork() call here. We could call it here, but it's
        // dangerous: module destructors may try to delete uninitialized pointers
        // and crash. (Often pointers incorrectly get initialized in initialize()
        // and not in the constructor.)
        throw;
    }
}

void cSimulation::callInitialize()
{
    checkActive();

    // reset counters. Note fes->clear() was already called from setupNetwork()
    currentSimtime = 0;
    currentEventNumber = 0;  // initialize() has event number 0
    trapOnNextEvent = false;
    cMessage::resetMessageCounters();

    simulationStage = CTX_INITIALIZE;

    // prepare simple modules for simulation run:
    //    1. create starter message for all modules,
    //    2. then call initialize() for them (recursively)
    //  This order is important because initialize() functions might contain
    //  send() calls which could otherwise insert msgs BEFORE starter messages
    //  for the destination module and cause trouble in cSimpleMod's activate().
    if (systemModule) {
        cContextSwitcher tmp(systemModule);
        systemModule->scheduleStart(SIMTIME_ZERO);
        getEnvir()->notifyLifecycleListeners(LF_PRE_NETWORK_INITIALIZE);
        systemModule->callInitialize();
        getEnvir()->notifyLifecycleListeners(LF_POST_NETWORK_INITIALIZE);
    }

    simulationStage = CTX_EVENT;
}

void cSimulation::callFinish()
{
    checkActive();

    simulationStage = CTX_FINISH;

    // call user-defined finish() functions for all modules recursively
    if (systemModule) {
        getEnvir()->notifyLifecycleListeners(LF_PRE_NETWORK_FINISH);
        systemModule->callFinish();
        getEnvir()->notifyLifecycleListeners(LF_POST_NETWORK_FINISH);
    }
}

void cSimulation::callRefreshDisplay()
{
    if (systemModule) {
        systemModule->callRefreshDisplay();
        if (fingerprint)
            fingerprint->addVisuals();
    }
}

void cSimulation::deleteNetwork()
{
    if (!systemModule)
        return;  // network already deleted

    if (usageCollector)
        usageCollector->printResults(getEnvir()->getOutputStream());

    if (cSimulation::getActiveSimulation() != this)
        throw cRuntimeError("cSimulation: Cannot invoke deleteNetwork() on an instance that is not the active one (see cSimulation::getActiveSimulation())");  // because cModule::deleteModule() would crash

    if (getContextModule() != nullptr)
        throw cRuntimeError("Attempt to delete network during simulation");

    simulationStage = CTX_CLEANUP;

    getEnvir()->notifyLifecycleListeners(LF_PRE_NETWORK_DELETE);

    // delete all modules recursively
    systemModule->deleteModule();

    // remove stray channel objects (created by cChannelType::create() but not inserted into the network)
    for (int i = 1; i < size; i++) {
        if (componentv[i]) {
            ASSERT(componentv[i]->isChannel() && componentv[i]->getParentModule()==nullptr);
            componentv[i]->setFlag(cComponent::FL_DELETING, true);
            delete componentv[i];
        }
    }

    // and clean up
    delete[] componentv;
    componentv = nullptr;
    size = 0;
    lastComponentId = 0;

    networkType = nullptr;

    for (auto p : componentTypes)
        static_cast<cComponentType *>(p)->clearSharedParImpls();
    cModule::clearNamePools();

    getEnvir()->notifyLifecycleListeners(LF_POST_NETWORK_DELETE);

    // clear remaining messages (module dtors may have cancelled & deleted some of them)
    fes->clear();

    simulationStage = CTX_NONE;

#ifdef DEVELOPER_DEBUG
    printf("DEBUG: after deleteNetwork: %d objects\n", (int)cOwnedObject::getLiveObjectCount());
    printAllObjects();
#endif
}

cEvent *cSimulation::takeNextEvent()
{
    setContextType(CTX_SCHEDULER);

    // determine next event. Normally (with sequential simulation),
    // the scheduler just returns fes->peekFirst().
    cEvent *event = scheduler->takeNextEvent();
    if (!event)
        return nullptr;

    ASSERT(!event->isStale());  // it's the scheduler's task to discard stale events

    return event;
}

void cSimulation::putBackEvent(cEvent *event)
{
    scheduler->putBackEvent(event);
}

cEvent *cSimulation::guessNextEvent()
{
    return scheduler->guessNextEvent();
}

simtime_t cSimulation::guessNextSimtime()
{
    cEvent *event = guessNextEvent();
    return event == nullptr ? -1 : event->getArrivalTime();
}

cSimpleModule *cSimulation::guessNextModule()
{
    cEvent *event = guessNextEvent();
    cMessage *msg = dynamic_cast<cMessage *>(event);
    if (!msg)
        return nullptr;

    // check that destination module still exists and hasn't terminated yet
    if (msg->getArrivalModuleId() == -1)
        return nullptr;
    cComponent *component = componentv[msg->getArrivalModuleId()];
    cSimpleModule *module = dynamic_cast<cSimpleModule *>(component);
    if (!module || module->isTerminated())
        return nullptr;
    return module;
}

void cSimulation::transferTo(cSimpleModule *module)
{
    if (!module)
        throw cRuntimeError("transferTo(): Attempt to transfer to nullptr");

    // switch to activity() of the simple module
    exception = nullptr;
    currentActivityModule = module;
    cCoroutine::switchTo(module->coroutine);

    if (module->hasStackOverflow())
        throw cRuntimeError("Stack violation in module (%s)%s: Module stack too small? "
                            "Try increasing it in the class' Module_Class_Members() or constructor",
                module->getClassName(), module->getFullPath().c_str());

    // if exception occurred in activity(), re-throw it. This allows us to handle
    // handleMessage() and activity() in an uniform way in the upper layer.
    if (exception) {
        cException *e = exception;
        exception = nullptr;

        // ok, so we have an exception *pointer*, but we have to throw further
        // by *value*, and possibly without leaking it. Hence the following magic...
        if (dynamic_cast<cDeleteModuleException *>(e)) {
            cDeleteModuleException e2(*(cDeleteModuleException *)e);
            delete e;
            throw e2;
        }
        else if (dynamic_cast<cTerminationException *>(e)) {
            cTerminationException e2(*(cTerminationException *)e);
            delete e;
            throw e2;
        }
        else if (dynamic_cast<cRuntimeError *>(e)) {
            cRuntimeError e2(*(cRuntimeError *)e);
            delete e;
            throw e2;
        }
        else {
            cException e2(*(cException *)e);
            delete e;
            throw e2;
        }
    }
}


#ifdef NDEBUG
#define DEBUG_TRAP_IF_REQUESTED    /*no-op*/
#else
#define DEBUG_TRAP_IF_REQUESTED    { if (trapOnNextEvent) { trapOnNextEvent = false; if (getEnvir()->ensureDebugger()) DEBUG_TRAP; } }
#endif

void cSimulation::executeEvent(cEvent *event)
{
#ifndef NDEBUG
    checkActive();

    // Sanity check to prevent reentrant event execution - which might
    // happen for example in a faulty cEnvir::pausePoint() implementation.
    static bool inExecuteEvent = false;
    ASSERT(!inExecuteEvent);
    inExecuteEvent = true;
    struct ResetInExecEventFlag {
        ~ResetInExecEventFlag() {
            inExecuteEvent = false;
        }
    } flagResetter;
#endif

    setContextType(CTX_EVENT);

    // increment event count
    currentEventNumber++;

    // advance simulation time
    currentSimtime = event->getArrivalTime();

    // notify the environment about the event (writes eventlog, etc.)
    EVCB.simulationEvent(event);

    // store arrival event number of this message; it is useful input for the
    // sequence chart tool if the message doesn't get immediately deleted or
    // sent out again
    event->setPreviousEventNumber(currentEventNumber);

    // ignore fingerprint of plain events, as they tend to be internal (like cEndSimulationEvent)
    if (getFingerprintCalculator() && event->isMessage())
        getFingerprintCalculator()->addEvent(event);

    try {
        if (!event->isMessage())
            DEBUG_TRAP_IF_REQUESTED;  // ABOUT TO PROCESS THE EVENT YOU REQUESTED TO DEBUG -- SELECT "STEP INTO" IN YOUR DEBUGGER
        event->execute();
    }
    catch (cDeleteModuleException& e) {
        setGlobalContext();
        e.getModuleToDelete()->deleteModule();
    }
    catch (cTerminationException& e) { // NOTE: this helps lldb knowing the exception type_info and it allows skipping the C++ throw exception breakpoint
        // restore global context before throwing the exception further
        setGlobalContext();
        throw e;
    }
    catch (cException&) {
        // restore global context before throwing the exception further
        setGlobalContext();
        throw;
    }
    catch (std::exception& e) {
        // restore global context before throwing the exception further
        // but wrap into a cRuntimeError which captures the module before that
        cRuntimeError e2("%s: %s", opp_typename(typeid(e)), e.what());
        setGlobalContext();
        throw e2;
    }
    setGlobalContext();

    // Note: simulation time (as read via simTime() from modules) will be updated
    // in takeNextEvent(), called right before the next executeEvent().
    // Simtime must NOT be updated here, because it would interfere with parallel
    // simulation (cIdealSimulationProtocol, etc) that relies on simTime()
    // returning the time of the last executed event. If Qtenv wants to display
    // the time of the next event, it should call guessNextSimtime().
}

#undef DEBUG_TRAP_IF_REQUESTED

void cSimulation::transferToMain()
{
    if (currentActivityModule != nullptr) {
        currentActivityModule = nullptr;
        cCoroutine::switchToMain();  // stack switch
    }
}

void cSimulation::setContext(cComponent *p)
{
    contextComponent = p;
    cOwnedObject::setOwningContext(p);
}

cModule *cSimulation::getContextModule() const
{
    // cannot go inline (upward cast would require including cmodule.h in csimulation.h)
    if (!contextComponent || !contextComponent->isModule())
        return nullptr;
    return (cModule *)contextComponent;
}

cSimpleModule *cSimulation::getContextSimpleModule() const
{
    // cannot go inline (upward cast would require including cmodule.h in csimulation.h)
    if (!contextComponent || !contextComponent->isModule() || !((cModule *)contextComponent)->isSimple())
        return nullptr;
    return (cSimpleModule *)contextComponent;
}

unsigned long cSimulation::getUniqueNumber()
{
    return getEnvir()->getUniqueNumber();
}

void cSimulation::setFingerprintCalculator(cFingerprintCalculator *f)
{
    if (fingerprint)
        delete fingerprint;
    fingerprint = f;
}

void cSimulation::insertEvent(cEvent *event)
{
    event->setPreviousEventNumber(currentEventNumber);
    fes->insert(event);
}

//----

void cUsageCollector::componentCreated(cComponent *component)
{
    if (collectInstantiatedNedTypes) {
        std::string typeName = component->getComponentType()->getFullName();
        instantiatedNedTypes.insert(typeName);
    }
}

void cUsageCollector::parametersAdded(cComponent *component)
{
    if (collectParameterAccesses) {
        // We cannot obtain the list of parameters from the component type,
        // so we collect it from the instance the first time the component type
        // is instantiated. We also need to pay attention we do not accidentally
        // register parameter accesses during collection, so this function
        // must be called before parameters are finalized.
        ASSERT(!component->parametersFinalized());
        std::string typeName = component->getComponentType()->getFullName();
        if (allParametersPerComponentType.find(typeName) == allParametersPerComponentType.end()) {
            for (int i = 0; i < component->getNumParams(); i++) {
                cPar& par = component->par(i);
                allParametersPerComponentType[typeName].insert(par.getFullName());
            }
            accessedParametersPerComponentType[typeName]; // make sure it exists
        }
    }
}

void cUsageCollector::parameterAccessed(cPar *par)
{
    if (collectParameterAccesses) {
        // We need to filter out accesses before parameters are finalized (those
        // done by the simulation kernel for technical reasons, during the construction
        // of the network), and those during finish() and network cleanup (for example,
        // recording parameters into the scalar file accesses all parameters, which we
        // want to ignore).
        cComponent *component = par->getOwnerComponent();
        ContextType contextType = getSimulation()->getContextType();
        if (component->parametersFinalized() && contextType != CTX_FINISH && contextType != CTX_CLEANUP) {
            std::string typeName = component->getComponentType()->getFullName();
            accessedParametersPerComponentType[typeName].insert(par->getName());
        }
    }
}

void cUsageCollector::printResults(std::ostream& out)
{
    // each line is prefixed in the output so as to facilitate grepping
    if (collectInstantiatedNedTypes) {
        for (auto& typeName : instantiatedNedTypes)
            out << "instantiated NED type: " << typeName << "\n";
    }

    if (collectParameterAccesses) {
        for (auto &[typeName, accessedParams] : accessedParametersPerComponentType) {
            auto paramNames = allParametersPerComponentType[typeName];
            for (auto& paramName : paramNames)
                if (accessedParams.find(paramName) == accessedParams.end())
                    out << "unused parameter: " << typeName << "." << paramName << "\n";
        }
    }
}

//----


/**
 * A dummy implementation of cEnvir, only provided so that one
 * can use simulation library classes outside simulations, that is,
 * in programs that only link with the simulation library (<i>sim_std</i>)
 * and not with the <i>envir</i>, <i>cmdenv</i>, <i>qtenv</i>,
 * etc. libraries.
 *
 * Many simulation library classes make calls to <i>ev</i> methods,
 * which would crash if <tt>evPtr</tt> was nullptr; one example is
 * cObject's destructor which contains an <tt>getEnvir()->objectDeleted()</tt>.
 * The solution provided here is that <tt>evPtr</tt> is initialized
 * to point to a StaticEnv instance, thus enabling library classes to work.
 *
 * StaticEnv methods either do nothing, or throw an "Unsupported method"
 * exception, so StaticEnv is only useful for the most basic usage scenarios.
 * For anything more complicated, <tt>evPtr</tt> must be set in <tt>main()</tt>
 * to point to a proper cEnvir implementation, like the Cmdenv or
 * Qtenv classes. (The <i>envir</i> library provides a <tt>main()</tt>
 * which does exactly that.)
 *
 * @ingroup Envir
 */
class StaticEnv : public cEnvir
{
  protected:
    void unsupported() const { throw opp_runtime_error("StaticEnv: Unsupported method called"); }
    virtual void alert(const char *msg) override { ::printf("\n<!> %s\n\n", msg); }
    virtual bool askYesNo(const char *msg) override  { unsupported(); return false; }
    virtual std::ostream& getOutputStream() override {return std::cout;}

  public:
    // constructor, destructor
    StaticEnv() {}
    virtual ~StaticEnv() {}

    // eventlog callback interface
    virtual void objectDeleted(cObject *object) override {}
    virtual void simulationEvent(cEvent *event) override  {}
    virtual void messageScheduled(cMessage *msg) override  {}
    virtual void messageCancelled(cMessage *msg) override  {}
    virtual void beginSend(cMessage *msg, const SendOptions& options) override  {}
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result) override  {}
    virtual void messageSendHop(cMessage *msg, cGate *srcGate) override  {}
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, const cChannel::Result& result) override  {}
    virtual void endSend(cMessage *msg) override  {}
    virtual void messageCreated(cMessage *msg) override  {}
    virtual void messageCloned(cMessage *msg, cMessage *clone) override  {}
    virtual void messageDeleted(cMessage *msg) override  {}
    virtual void moduleReparented(cModule *module, cModule *oldparent, int oldId) override  {}
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent) override  {}
    virtual void componentMethodEnd() override  {}
    virtual void moduleCreated(cModule *newmodule) override  {}
    virtual void moduleDeleted(cModule *module) override  {}
    virtual void gateCreated(cGate *newgate) override  {}
    virtual void gateDeleted(cGate *gate) override  {}
    virtual void connectionCreated(cGate *srcgate) override  {}
    virtual void connectionDeleted(cGate *srcgate) override  {}
    virtual void displayStringChanged(cComponent *component) override  {}
    virtual void undisposedObject(cObject *obj) override;
    virtual void log(cLogEntry *entry) override {}

    // configuration, model parameters
    virtual void preconfigure(cComponent *component) override  {}
    virtual void configure(cComponent *component) override {}
    virtual void readParameter(cPar *parameter) override  { unsupported(); }
    virtual bool isModuleLocal(cModule *parentmod, const char *modname, int index) override  { return true; }
    virtual cXMLElement *getXMLDocument(const char *filename, const char *xpath = nullptr) override  { unsupported(); return nullptr; }
    virtual cXMLElement *getParsedXMLString(const char *content, const char *xpath = nullptr) override  { unsupported(); return nullptr; }
    virtual void forgetXMLDocument(const char *filename) override {}
    virtual void forgetParsedXMLString(const char *content) override {}
    virtual void flushXMLDocumentCache() override {}
    virtual void flushXMLParsedContentCache() override {}
    virtual unsigned getExtraStackForEnvir() const override  { return 0; }
    virtual cConfiguration *getConfig() override  { unsupported(); return nullptr; }
    virtual std::string resolveResourcePath(const char *fileName, cComponentType *context) override {return "";}
    virtual bool isGUI() const override  { return false; }
    virtual bool isExpressMode() const override  { return false; }

    // UI functions (see also protected ones)
    virtual void bubble(cComponent *component, const char *text) override  {}
    virtual std::string gets(const char *prompt, const char *defaultreply = nullptr) override  { unsupported(); return ""; }
    virtual cEnvir& flush() { ::fflush(stdout); return *this; }

    // RNGs
    virtual int getNumRNGs() const override { return 0; }
    virtual cRNG *getRNG(int k) override  { unsupported(); return nullptr; }

    // output vectors
    virtual void *registerOutputVector(const char *modulename, const char *vectorname, opp_string_map *attributes=nullptr) override  { return nullptr; }
    virtual void deregisterOutputVector(void *vechandle) override  {}
    virtual bool recordInOutputVector(void *vechandle, simtime_t t, double value) override  { return false; }

    // output scalars
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes = nullptr) override  {}
    virtual void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes = nullptr) override  {}
    virtual void recordParameter(cPar *par) override {}
    virtual void recordComponentType(cComponent *component) override {}

    virtual void addResultRecorders(cComponent *component, simsignal_t signal, const char *statisticName, cProperty *statisticTemplateProperty) override {}

    // snapshot file
    virtual std::ostream *getStreamForSnapshot() override  { unsupported(); return nullptr; }
    virtual void releaseStreamForSnapshot(std::ostream *os) override  { unsupported(); }

    // misc
    virtual int getArgCount() const override  { unsupported(); return 0; }
    virtual char **getArgVector() const override  { unsupported(); return nullptr; }
    virtual int getParsimProcId() const override { return 0; }
    virtual int getParsimNumPartitions() const override { return 1; }
    virtual unsigned long getUniqueNumber() override  { unsupported(); return 0; }
    virtual bool idle() override  { return false; }
    virtual void pausePoint() override {}
    virtual void refOsgNode(osg::Node *scene) override {}
    virtual void unrefOsgNode(osg::Node *scene) override {}
    virtual bool ensureDebugger(cRuntimeError *) override { return false; }

    virtual void getImageSize(const char *imageName, double& outWidth, double& outHeight) override {unsupported();}
    virtual void getTextExtent(const cFigure::Font& font, const char *text, double& outWidth, double& outHeight, double& outAscent) override {unsupported();}
    virtual void appendToImagePath(const char *directory) override {unsupported();}
    virtual void loadImage(const char *fileName, const char *imageName=nullptr) override {unsupported();}
    virtual cFigure::Rectangle getSubmoduleBounds(const cModule *submodule) override {return cFigure::Rectangle(NAN, NAN, NAN, NAN);}
    virtual std::vector<cFigure::Point> getConnectionLine(const cGate *sourceGate) override {return {};}
    virtual double getZoomLevel(const cModule *module) override {return NAN;}
    virtual double getAnimationTime() const override {return 0;}
    virtual double getAnimationSpeed() const override {return 0;}
    virtual double getRemainingAnimationHoldTime() const override {return 0;}
};

void StaticEnv::undisposedObject(cObject *obj)
{
    if (!cStaticFlag::insideMain()) {
        ::printf("<!> WARNING: global object variable (DISCOURAGED) detected: "
                 "(%s)'%s' at %p\n", obj->getClassName(), obj->getFullPath().c_str(), obj);
    }
}

static StaticEnv staticEnv;

// cSimulation's global variables
cEnvir *cSimulation::activeEnvir = &staticEnv;
cEnvir *cSimulation::staticEnvir = &staticEnv;

cSimulation *cSimulation::activeSimulation = nullptr;

}  // namespace omnetpp

