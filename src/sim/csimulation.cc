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
#include <mutex>
#include "common/stringutil.h"
#include "common/stlutil.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cpacket.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/globals.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/ceventlooprunner.h"

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
#include "omnetpp/ccoroutine.h"
#include "omnetpp/cinedloader.h"
#include "omnetpp/clifecyclelistener.h"
#include "omnetpp/crngmanager.h"
#include "omnetpp/cstatisticbuilder.h"
#include "omnetpp/clog.h"
#include "omnetpp/platdep/platmisc.h"  // for DEBUG_TRAP
#include "sim/netbuilder/cnedloader.h"
#include "stopwatch.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#include "omnetpp/cparsimcomm.h"
#include "sim/parsim/cparsimpartition.h"
#include "sim/parsim/cparsimsynchr.h"
#endif

#ifdef WITH_PYTHON
#include "pythonutil.h"
#endif

#ifdef DEVELOPER_DEBUG
#include <set>
#endif

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {

using std::ostream;

#ifdef NDEBUG
#define CHECKSIGNALS_DEFAULT        "false"
#else
#define CHECKSIGNALS_DEFAULT        "true"
#endif

Register_GlobalConfigOption(CFGID_NETWORK, "network", CFG_STRING, nullptr, "The name of the network to be simulated. The package name can be omitted if the ini file is in the same directory as the NED file that contains the network.");
Register_GlobalConfigOption(CFGID_PARALLEL_SIMULATION, "parallel-simulation", CFG_BOOL, "false", "Enables parallel distributed simulation.");
Register_GlobalConfigOption(CFGID_PARSIM_MODE, "parsim-mode", CFG_CUSTOM, "distributed", "If `parallel-simulation=true`: Defines the operation mode of parallel simulation. Possible values: `distributed`, `multithreaded`. In `distributed` mode, simulations of parts of the whole network run in separate processes, which may execute in the same host or different hosts (if the transport layer supports it). In `multithreaded` mode, simulations of parts of the whole network run in separate threads within the same process, which allows utilization of multi-core CPUs and efficient communication. For both modes, `parsim-communications-class` needs to be set appropriately.");
Register_GlobalConfigOption(CFGID_FUTUREEVENTSET_CLASS, "futureeventset-class", CFG_STRING, "omnetpp::cEventHeap", "Part of the Envir plugin mechanism: selects the class for storing the future events in the simulation. The class has to implement the `cFutureEventSet` interface.");
Register_GlobalConfigOption(CFGID_SCHEDULER_CLASS, "scheduler-class", CFG_STRING, "omnetpp::cSequentialScheduler", "Part of the Envir plugin mechanism: selects the scheduler class. This plugin interface allows for implementing real-time, hardware-in-the-loop, distributed and distributed parallel simulation. The class has to implement the `cScheduler` interface.");
Register_GlobalConfigOption(CFGID_FINGERPRINT, "fingerprint", CFG_STRING, nullptr, "The expected fingerprints of the simulation. If you need multiple fingerprints, separate them with commas. When provided, the fingerprints will be calculated from the specified properties of simulation events, messages, and statistics during execution, and checked against the provided values. Fingerprints are suitable for crude regression tests. As fingerprints occasionally differ across platforms, more than one value can be specified for a single fingerprint, separated by spaces, and a match with any of them will be accepted. To obtain a fingerprint, enter a dummy value (such as `0000`), and run the simulation.");
Register_GlobalConfigOption(CFGID_FINGERPRINTER_CLASS, "fingerprintcalculator-class", CFG_STRING, "omnetpp::cSingleFingerprintCalculator", "Part of the Envir plugin mechanism: selects the fingerprint calculator class to be used to calculate the simulation fingerprint. The class has to implement the `cFingerprintCalculator` interface.");
Register_GlobalConfigOption(CFGID_RNGMANAGER_CLASS, "rngmanager-class", CFG_STRING, "omnetpp::cRngManager", "Part of the Envir plugin mechanism: selects the RNG manager class to be used for providing RNGs to modules and channels. The class has to implement the `cIRngManager` interface.");
Register_GlobalConfigOptionU(CFGID_SIM_TIME_LIMIT, "sim-time-limit", "s", nullptr, "Stops the simulation when simulation time reaches the given limit. The default is no limit.");
Register_GlobalConfigOptionU(CFGID_CPU_TIME_LIMIT, "cpu-time-limit", "s", nullptr, "Stops the simulation when CPU usage has reached the given limit. The default is no limit. Note: To reduce per-event overhead, this time limit is only checked every N events (by default, N=1024).");
Register_GlobalConfigOptionU(CFGID_REAL_TIME_LIMIT, "real-time-limit", "s", nullptr, "Stops the simulation after the specified amount of time has elapsed. The default is no limit. Note: To reduce per-event overhead, this time limit is only checked every N events (by default, N=1024).");
Register_GlobalConfigOptionU(CFGID_WARMUP_PERIOD, "warmup-period", "s", nullptr, "Length of the initial warm-up period. When set, results belonging to the first x seconds of the simulation will not be recorded into output vectors, and will not be counted into output scalars (see option `**.result-recording-modes`). This option is useful for steady-state simulations. The default is 0s (no warmup period). Note that models that compute and record scalar results manually (via `recordScalar()`) will not automatically obey this setting.");
Register_GlobalConfigOption(CFGID_CHECK_SIGNALS, "check-signals", CFG_BOOL, CHECKSIGNALS_DEFAULT, "Controls whether the simulation kernel will validate signals emitted by modules and channels against signal declarations (`@signal` properties) in NED files. The default setting depends on the build type: `true` in DEBUG, and `false` in RELEASE mode.");
Register_GlobalConfigOption(CFGID_PARAMETER_MUTABILITY_CHECK, "parameter-mutability-check", CFG_BOOL, "true", "Setting to false will disable errors raised when trying to change the values of module/channel parameters not marked as @mutable. This is primarily a compatibility setting intended to facilitate running simulation models that were not yet annotated with @mutable.");
Register_GlobalConfigOption(CFGID_ALLOW_OBJECT_STEALING_ON_DELETION, "allow-object-stealing-on-deletion", CFG_BOOL, "false", "Setting it to true disables the \"Context component is deleting an object it doesn't own\" error message. This option exists primarily for backward compatibility with pre-6.0 versions that were more permissive during object deletion.");
Register_GlobalConfigOption(CFGID_DEBUG_STATISTICS_RECORDING, "debug-statistics-recording", CFG_BOOL, "false", "Turns on the printing of debugging information related to statistics recording (`@statistic` properties)");
Register_GlobalConfigOption(CFGID_PRINT_UNUSED_CONFIG, "print-unused-config", CFG_BOOL, "true", "Enables listing of unused configuration entries after network setup. Note that the reported entries are not necessarily redundant, e.g. they may be needed by modules created dynamically during simulation. It tries to be smart about which entries to report, e.g. entries overridden from a derived section, likely intentionally, are not reported.");
Register_GlobalConfigOption(CFGID_PRINT_UNUSED_CONFIG_ON_COMPLETION, "print-unused-config-on-completion", CFG_BOOL, "false", "Enables listing of unused configuration entries after the simulation has successfully completed. It tries to be smart about which entries to report, e.g. entries overridden from a derived section, likely intentionally, are not reported.");


#ifdef DEVELOPER_DEBUG
extern std::set<cOwnedObject *> objectlist;
void printAllObjects();
#endif

cSimulation::SharedDataHandles *cSimulation::sharedVariableHandles = nullptr;
cSimulation::SharedDataHandles *cSimulation::sharedCounterHandles = nullptr;

const size_t MAX_NUM_COUNTERS = 1000;  // fixed size to prevent array reallocation -- increase if necessary
const uint64_t CTR_UNINITIALIZED = (uint64_t)-1;

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

cSimulation::cSimulation(const char *name, cEnvir *env, cINedLoader *loader) : cNamedObject(name, false)
{
    // sanity checks regarding library setup
    ASSERT(cStaticFlag::insideMain());  // cannot be instantiated as global variable

    static bool firstInstance = true;
    if (firstInstance) {
        if (CodeFragments::hasItemsOfType(CodeFragments::EARLY_STARTUP))
            throw cRuntimeError("'CodeFragments::executeAll(CodeFragments::EARLY_STARTUP)' must have been called by the time the first cSimulation object is created (it must be called after dynamically loading libraries, too)");
        if (CodeFragments::hasItemsOfType(CodeFragments::STARTUP))
            CodeFragments::executeAll(CodeFragments::STARTUP); // better late than never
        firstInstance = false;
    }

    // member initialization
    nedLoader = loader ? loader : new cNedLoader();
    nedLoaderOwned = (loader == nullptr); // only owned if we created it

    EnvirFactoryFunction creator = envirFactoryFunction.load();
    envir = env ? env : creator ? creator() : nullptr;
    if (!envir)
        throw cRuntimeError("cSimulation constructor: No environment (cEnvir) object passed in, and could not create a default one (missing or incorrect factory function?)");
    envir->setSimulation(this);

    stopwatch = new Stopwatch;

    sharedCounters.resize(MAX_NUM_COUNTERS, CTR_UNINITIALIZED);

    // install default objects
    setFES(new cEventHeap("fes"));
    setScheduler(new cSequentialScheduler());
    setRngManager(new cRngManager());
    setFingerprintCalculator(new cSingleFingerprintCalculator());

    if (getActiveSimulation() == nullptr)
        setActiveSimulation(this);
}

cSimulation::~cSimulation()
{
#ifdef WITH_PYTHON
    for (auto p : componentAccessors)
        Py_DECREF(p.second);
    componentAccessors.clear();
#endif

//    if (this == activeSimulation) {
//         // Note: cannot throw from destructors
//        panic(cRuntimeError(this, "Cannot delete the active simulation manager object, ABORTING").getFormattedMessage().c_str());
//    }

    try {
        deleteNetwork();  // note: requires this being the active simulation
    }
    catch (std::exception&) {
        // ignore (C++ doesn't allow throwing from a destructor)
    }

    auto copy = listeners;
    for (auto& listener : copy)
        listener->listenerRemoved();

    dropAndDelete(fingerprint);
    dropAndDelete(rngManager);
    dropAndDelete(scheduler);
    dropAndDelete(fes);

#ifdef WITH_PARSIM
    delete parsimPartition;
#endif

    if (getActiveSimulation() == this)
        setActiveSimulation(nullptr);

    delete stopwatch;

    delete envir;  // after setActiveSimulation(nullptr), due to objectDeleted() callbacks

    if (nedLoaderOwned)
        delete nedLoader;
}

cSimulation *cSimulation::getActiveSimulation()
{
    // note: the reason this function is not inline is this cppyy bug: https://github.com/wlav/cppyy/issues/60
    return activeSimulation;
}

cEnvir *cSimulation::getActiveEnvir()
{
    // note: the reason this function is not inline is this cppyy bug: https://github.com/wlav/cppyy/issues/60
    return activeEnvir;
}

void cSimulation::setActiveSimulation(cSimulation *sim)
{
    if (sim != activeSimulation && activeSimulation && activeSimulation->stage != STAGE_NONE)
        throw cRuntimeError("cSimulation::setActiveSimulation(): May not be called while the active simulation is doing something (stage!=STAGE_NONE)");
    activeSimulation = sim;
    activeEnvir = sim == nullptr ? staticEnvir : sim->envir;
}

void cSimulation::setStaticEnvir(cEnvir *env)
{
    if (!env)
        throw cRuntimeError("cSimulation::setStaticEnvir(): Argument cannot be nullptr");
    staticEnvir = env;
}

void cSimulation::setEnvirFactoryFunction(EnvirFactoryFunction f)
{
    envirFactoryFunction.store(f);
}

void cSimulation::forEachChild(cVisitor *v)
{
    if (systemModule != nullptr)
        v->visit(systemModule);
    if (fes)
        v->visit(fes);
    if (scheduler)
        v->visit(scheduler);
    if (rngManager)
        v->visit(rngManager);
    if (nedLoader)
        v->visit(nedLoader);
    if (fingerprint)
        v->visit(fingerprint);
}

std::string cSimulation::getFullPath() const
{
    return getFullName();
}

void cSimulation::configure(cConfiguration *cfg, int partitionId)
{
    this->cfg = cfg;

    SimTime::configure(cfg);
    cCoroutine::configure(cfg);

    parsim = cfg->getAsBool(CFGID_PARALLEL_SIMULATION);

#ifndef WITH_PARSIM
    // checks
    if (parsim)
        throw cRuntimeError("Parallel simulation is turned on in the ini file, but OMNeT++ was compiled without parallel simulation support (WITH_PARSIM=no)");
#endif

    // validate parsim-mode here
    const char *parsimMode = cfg->getAsCustom(CFGID_PARSIM_MODE);
    if (parsim && !opp_streq(parsimMode, "multithreaded") && !opp_streq(parsimMode, "distributed"))
        throw cRuntimeError("Invalid value \"%s\" for the 'parsim-mode' configuration option, must be \"multithreaded\" or \"distributed\"", parsimMode);

    // parallel simulation
    if (parsim) {
#ifdef WITH_PARSIM
        delete parsimPartition;
        parsimPartition = new cParsimPartition();
        parsimPartition->configure(this, cfg, partitionId);
#else
        throw cRuntimeError("Parallel simulation is turned on in the ini file, but OMNeT++ was compiled without parallel simulation support (WITH_PARSIM=no)");
#endif
    }

    // install FES
    std::string futureeventsetClass = cfg->getAsString(CFGID_FUTUREEVENTSET_CLASS);
    cFutureEventSet *fes = createByClassName<cFutureEventSet>(futureeventsetClass.c_str(), "FES");
    setFES(fes);
    fes->configure(this, cfg);

    // install scheduler
    if (!parsim) {
        std::string schedulerClass = cfg->getAsString(CFGID_SCHEDULER_CLASS);
        cScheduler *scheduler = createByClassName<cScheduler>(schedulerClass.c_str(), "event scheduler");
        setScheduler(scheduler);
    }

    scheduler->configure(this, cfg);

    // rng manager
    std::string rngManagerClass = cfg->getAsString(CFGID_RNGMANAGER_CLASS);
    cIRngManager *rngManager = createByClassName<cIRngManager>(rngManagerClass.c_str(), "RNG manager");
    setRngManager(rngManager);

    // install fingerprint calculator object
    cFingerprintCalculator *fingerprint = nullptr;
    std::string expectedFingerprints = cfg->getAsString(CFGID_FINGERPRINT);
    if (expectedFingerprints.empty())
        setFingerprintCalculator(nullptr);
    else {
        // create calculator
        std::string fingerprintClass = cfg->getAsString(CFGID_FINGERPRINTER_CLASS);
        fingerprint = createByClassName<cFingerprintCalculator>(fingerprintClass.c_str(), "fingerprint calculator");
        if (expectedFingerprints.find(',') != expectedFingerprints.npos)
            fingerprint = new cMultiFingerprintCalculator(fingerprint);
        setFingerprintCalculator(fingerprint);
        fingerprint->configure(this, cfg, expectedFingerprints.c_str());
    }

    // init nextUniqueNumber
    setUniqueNumberRange(0, 0); // =until it wraps
#ifdef WITH_PARSIM
    if (parsim) {
        uint64_t myRank = parsimPartition->getPartitionId();
        uint64_t range = UINT64_MAX / parsimPartition->getNumPartitions();
        setUniqueNumberRange(myRank * range, (myRank+1) * range);
    }
#endif

    // misc
    simtime_t simtimeLimit = cfg->getAsDouble(CFGID_SIM_TIME_LIMIT, 0);
    setSimulationTimeLimit(simtimeLimit);

    setElapsedTimeLimit(cfg->getAsDouble(CFGID_REAL_TIME_LIMIT, -1));
    setCpuTimeLimit(cfg->getAsDouble(CFGID_CPU_TIME_LIMIT, -1));

    simtime_t warmupPeriod = cfg->getAsDouble(CFGID_WARMUP_PERIOD);
    setWarmupPeriod(warmupPeriod);

    bool checkSignals = cfg->getAsBool(CFGID_CHECK_SIGNALS);
    cComponent::setCheckSignals(checkSignals);

    bool checkParamMutability = cfg->getAsBool(CFGID_PARAMETER_MUTABILITY_CHECK);
    setParameterMutabilityCheck(checkParamMutability);

    bool allowObjectStealing = cfg->getAsBool(CFGID_ALLOW_OBJECT_STEALING_ON_DELETION);
    cSoftOwner::setAllowObjectStealing(allowObjectStealing);

    rngManager->configure(this, cfg, getParsimPartitionId(), getParsimNumPartitions());

    // note: this must come last, as e.g. result manager initializations call cSimulation::isParsimEnabled()
    envir->configure(cfg);
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

    cModuleType *networkType = getNetworkType();

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

void cSimulation::setNedLoader(cINedLoader *loader)
{
    if (getSystemModule() != nullptr)
        throw cRuntimeError(this, "setNedLoader(): Cannot switch NED loaders when a network is already set up");

    if (nedLoaderOwned)
        delete nedLoader;

    nedLoader = loader;
    nedLoaderOwned = false;
}

void cSimulation::setScheduler(cScheduler *sch)
{
    if (systemModule)
        throw cRuntimeError(this, "setScheduler(): Cannot switch schedulers when a network is already set up");
    if (!sch)
        throw cRuntimeError(this, "setScheduler(): New scheduler cannot be nullptr");

    if (scheduler)
        dropAndDelete(scheduler);

    scheduler = sch;
    take(scheduler);
}

void cSimulation::setFES(cFutureEventSet *f)
{
    if (systemModule)
        throw cRuntimeError(this, "setFES(): Cannot switch FES when a network is already set up");
    if (fes && !fes->isEmpty())
        throw cRuntimeError(this, "setFES(): Existing FES is not empty");
    if (!f)
        throw cRuntimeError(this, "setFES(): New FES cannot be nullptr");

    if (fes)
        dropAndDelete(fes);

    fes = f;
    fes->setName("scheduled-events");
    take(fes);
}

void cSimulation::setRngManager(cIRngManager *mgr)
{
    if (systemModule)
        throw cRuntimeError(this, "setRngManager(): Cannot switch RNG managers when a network is already set up");
    if (!mgr)
        throw cRuntimeError(this, "setRngManager(): New RNG manager cannot be nullptr");

    if (rngManager)
        dropAndDelete(rngManager);

    rngManager = mgr;
    take(rngManager);
}

void cSimulation::setSimulationTimeLimit(simtime_t limit)
{
    if (limit != SIMTIME_ZERO && limit < getSimTime())
        throw cRuntimeError(this, "setSimulationTimeLimit(): Requested time limit %s has already passed", limit.str().c_str());

    simTimeLimit = limit;

    if (getSystemModule() != nullptr)
        scheduleEndSimulationEvent();
}

void cSimulation::scheduleEndSimulationEvent()
{
    if (endSimulationEvent) {
        getFES()->remove(endSimulationEvent);
        delete endSimulationEvent;
        endSimulationEvent = nullptr;
    }
    if (simTimeLimit != SIMTIME_ZERO) {
        endSimulationEvent = new cEndSimulationEvent("endSimulation", simTimeLimit);
        getFES()->insert(endSimulationEvent);
    }
}

simtime_t cSimulation::getSimulationTimeLimit() const
{
    return simTimeLimit;
}

int cSimulation::loadNedFolder(const char *folder, const char *excludedPackages)
{
    if (!nedLoader)
        throw cRuntimeError("No NED loader installed");
    return nedLoader->loadNedFolder(folder, excludedPackages);
}

void cSimulation::loadNedFile(const char *nedFilename, const char *expectedPackage, bool isXML)
{
    if (!nedLoader)
        throw cRuntimeError("No NED loader installed");
    nedLoader->loadNedFile(nedFilename, expectedPackage, isXML);
}

void cSimulation::loadNedText(const char *name, const char *nedText, const char *expectedPackage, bool isXML)
{
    if (!nedLoader)
        throw cRuntimeError("No NED loader installed");
    nedLoader->loadNedText(name, nedText, expectedPackage, isXML);
}

void cSimulation::checkLoadedTypes()
{
    if (!nedLoader)
        throw cRuntimeError("No NED loader installed");
    nedLoader->checkLoadedTypes();
}

std::string cSimulation::getNedPackageForFolder(const char *folder)
{
    if (!nedLoader)
        throw cRuntimeError("No NED loader installed");
    return nedLoader->getNedPackageForFolder(folder);
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

const char *cSimulation::getStateName(State state)
{
#define CASE(X) case SIM_ ## X: return #X
    switch (state) {
        CASE(NONETWORK);
        CASE(NETWORKBUILT);
        CASE(INITIALIZED);
        CASE(RUNNING);
        CASE(PAUSED);
        CASE(TERMINATED);
        CASE(FINISHCALLED);
        CASE(ERROR);
        default: return "???";
    }
#undef CASE
}

const char *cSimulation::getStageName(Stage stage)
{
#define CASE(X) case STAGE_ ## X: return #X
    switch (stage) {
        CASE(NONE);
        CASE(BUILD);
        CASE(INITIALIZE);
        CASE(EVENT);
        CASE(REFRESHDISPLAY);
        CASE(FINISH);
        CASE(CLEANUP);
        default: return "???";
    }
#undef CASE
}

void cSimulation::gotoState(State s)
{
    state = s;
}

void cSimulation::setStage(Stage s)
{
    stage = s;
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

cModuleType *cSimulation::resolveNetwork(const char *networkName, const char *baseDirectory)
{
    cModuleType *networkType = nullptr;
    std::string inifilePackage = getNedPackageForFolder(baseDirectory);

    bool hasInifilePackage = !inifilePackage.empty() && strcmp(inifilePackage.c_str(), "-") != 0;
    if (hasInifilePackage)
        networkType = cModuleType::find((inifilePackage+"."+networkName).c_str());
    if (!networkType)
        networkType = cModuleType::find(networkName);
    if (!networkType) {
        if (hasInifilePackage)
            throw cRuntimeError("Network '%s' or '%s' not found, check .ini and .ned files",
                    networkName, (inifilePackage+"."+networkName).c_str());
        else
            throw cRuntimeError("Network '%s' not found, check .ini and .ned files", networkName);
    }
    if (!networkType->isNetwork())
        throw cRuntimeError("Module type '%s' is not a network", networkType->getFullName());
    return networkType;
}

void cSimulation::setupNetwork(cConfiguration *cfg, int partitionId)
{
    cConfiguration *effectiveCfg = cfg ? cfg : this->cfg;
    if (!effectiveCfg)
        throw cRuntimeError("setupNetwork(): No configuration object");

    std::string networkName = effectiveCfg->getAsString(CFGID_NETWORK);
    std::string inifileNetworkDir  = effectiveCfg->getConfigEntry(CFGID_NETWORK->getName()).getBaseDirectory();
    if (networkName.empty())
        throw cRuntimeError("No network specified (missing or empty network= configuration option)");
    cModuleType *networkType = resolveNetwork(networkName.c_str(), inifileNetworkDir.c_str());
    ASSERT(networkType);

    setupNetwork(networkType, cfg, partitionId); // use original arg which can be nullptr, so that the configure() call inside setupNetwork() is not repeated unnecessarily
}

void cSimulation::setupNetwork(cModuleType *networkType, cConfiguration *cfg, int partitionId)
{
#ifdef DEVELOPER_DEBUG
    printf("DEBUG: before setupNetwork: %d objects\n", (int)cOwnedObject::getLiveObjectCount());
    objectlist.clear();
#endif

    checkActive();
    if (state != SIM_NONETWORK)
        throw cRuntimeError("setupNetwork(): A network is already set up");
    ASSERT(systemModule == nullptr);

    if (!networkType)
        throw cRuntimeError("setupNetwork(): networkType cannot be nullptr");
    if (!networkType->isNetwork())
        throw cRuntimeError("Cannot set up network: '%s' is not a network type", networkType->getFullName());

    // just to be sure
    cComponent::clearSignalState();

    if (cfg)
        configure(cfg, partitionId);

    StageSwitcher _(this, STAGE_BUILD);

    try {
        // set up the network by instantiating the toplevel module
        notifyLifecycleListeners(LF_PRE_NETWORK_SETUP, networkType);
        cModule *module = networkType->create(networkType->getName(), this);
        ASSERT(systemModule == module);
        module->finalizeParameters();
        module->buildInside();
        scheduleEndSimulationEvent();
        gotoState(SIM_NETWORKBUILT);
        notifyLifecycleListeners(LF_POST_NETWORK_SETUP);
    }
    catch (std::exception& e) {
        gotoState(SIM_ERROR);
        notifyLifecycleListenersOnErrorThenRethrow(e);
    }

    bool debugStatisticsRecording = getConfig()->getAsBool(CFGID_DEBUG_STATISTICS_RECORDING);
    if (debugStatisticsRecording)
        cStatisticBuilder::dumpResultRecorders(EV_INFO, getSystemModule());

    bool printUnusedConfig = getConfig()->getAsBool(CFGID_PRINT_UNUSED_CONFIG);
    if (printUnusedConfig)
        printUnusedConfigEntriesIfAny(EV_INFO);

    // XML docs loaded for initializing NED parameters of type "xml" in the model are no longer needed
    envir->flushXMLDocumentCache();
    envir->flushXMLParsedContentCache();
}

void cSimulation::callInitialize()
{
    checkActive();

    if (state != SIM_NETWORKBUILT)
        throw cRuntimeError("callInitialize(): A newly set up network is expected");
    ASSERT(systemModule != nullptr);

    // reset counters. Note fes->clear() was already called from setupNetwork()
    currentSimtime = 0;
    currentEventNumber = 0;  // initialize() has event number 0
    trapOnNextEvent = false;
    cMessage::resetMessageCounters();

    cLog::setLoggingEnabled(!envir->isExpressMode());

    StageSwitcher _(this, STAGE_INITIALIZE);

    // Prepare simple modules for simulation run:
    //    1. create starter message for activity() modules,
    //    2. then call initialize() on the module tree
    // This order is important because initialize() functions might contain
    // send() calls which could otherwise insert msgs BEFORE starter messages
    // for the destination module and cause trouble in cSimpleModule's activate().
    try {
        cContextSwitcher tmp(systemModule);
        systemModule->scheduleStart(SIMTIME_ZERO);
        notifyLifecycleListeners(LF_PRE_NETWORK_INITIALIZE);
        systemModule->callInitialize();
        cLogProxy::flushLastLine();
        gotoState(SIM_INITIALIZED);
        notifyLifecycleListeners(LF_POST_NETWORK_INITIALIZE);
    }
    catch (std::exception& e) {
        cLogProxy::flushLastLine();
        gotoState(SIM_ERROR);
        notifyLifecycleListenersOnErrorThenRethrow(e);
    }
}

void cSimulation::callFinish()
{
    checkActive();

    switch (state) {
        case SIM_NONETWORK: throw cRuntimeError("callFinish(): No network set up");
        case SIM_NETWORKBUILT: throw cRuntimeError("callFinish(): Network not yet initialized");
        case SIM_INITIALIZED: break;
        case SIM_PAUSED: break;
        case SIM_RUNNING: throw cRuntimeError("callFinish(): Simulation currently running");
        case SIM_TERMINATED: break;
        case SIM_FINISHCALLED: throw cRuntimeError("callFinish(): Finish already called");
        case SIM_ERROR: throw cRuntimeError("callFinish(): Cannot continue after an error");
    }

    if (state == SIM_INITIALIZED || state == SIM_PAUSED) {
        cTerminationException e("Implicitly terminated by callFinish() call");
        notifyLifecycleListeners(LF_ON_SIMULATION_SUCCESS, &e); // not done before
    }

    cLog::setLoggingEnabled(true);

    StageSwitcher _(this, STAGE_FINISH);

    try {
        notifyLifecycleListeners(LF_PRE_NETWORK_FINISH);
        systemModule->callFinish();
        cLogProxy::flushLastLine();
        gotoState(SIM_FINISHCALLED);
        notifyLifecycleListeners(LF_POST_NETWORK_FINISH);
        onRunEndFired = true;
        notifyLifecycleListeners(LF_ON_RUN_END);
    }
    catch (std::exception& e) {
        cLogProxy::flushLastLine();
        gotoState(SIM_ERROR);
        notifyLifecycleListenersOnErrorThenRethrow(e);
    }

    bool printUnusedConfig = getConfig()->getAsBool(CFGID_PRINT_UNUSED_CONFIG_ON_COMPLETION);
    if (printUnusedConfig)
        printUnusedConfigEntriesIfAny(EV_INFO);

    checkFingerprint();
}

void cSimulation::printUnusedConfigEntriesIfAny(std::ostream& out)
{
    bool postsimulation = (getState() == SIM_FINISHCALLED);
    auto unusedEntries = getConfig()->getUnusedEntries(false, postsimulation);
    if (!unusedEntries.empty()) {
        out << "Note: There were unused entries in the configuration after network setup:\n";
        for (auto entry : unusedEntries)
            out << "  " << entry->str() << std::endl;
    }
}

void cSimulation::checkFingerprint()
{
    if (!fingerprint)
        return;

    if (fingerprint->checkFingerprint())
        getEnvir()->alert(opp_stringf("Fingerprint successfully verified: %s", fingerprint->str().c_str()).c_str());
    else
        throw cRuntimeError("Fingerprint mismatch! calculated: %s, expected: %s", fingerprint->str().c_str(), fingerprint->getExpected().c_str());
}

void cSimulation::callRefreshDisplay()
{
    if (systemModule) {
        StageSwitcher _(this, STAGE_REFRESHDISPLAY);
        systemModule->callRefreshDisplay();
        if (fingerprint)
            fingerprint->addVisuals();
    }
}

bool cSimulation::deleteNetworkOnError(cRuntimeError& primaryError)
{
    try {
        deleteNetwork();
        return true;
    }
    catch (std::exception& e) {
        primaryError.addNestedException(e);
        return false;
    }
}

void cSimulation::deleteNetwork()
{
    if (!systemModule)
        return;  // network already deleted

    if (cSimulation::getActiveSimulation() != this)
        throw cRuntimeError("cSimulation: Cannot invoke deleteNetwork() on an instance that is not the active one (see cSimulation::getActiveSimulation())");  // because cModule::deleteModule() would crash

    if (getContextModule() != nullptr)
        throw cRuntimeError("Attempt to delete network during simulation");

    try {
        if (!onRunEndFired) {
            onRunEndFired = true;
            notifyLifecycleListeners(LF_ON_RUN_END);
        }

        StageSwitcher _(this, STAGE_CLEANUP);
        notifyLifecycleListeners(LF_PRE_NETWORK_DELETE);

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

        // delete "simulation global" variables
        //int i = 0;
        for (auto& item : sharedVariables)
            item.second(); // call stored destructor
        sharedVariables.clear();

        sharedCounters.clear();
        sharedCounters.resize(MAX_NUM_COUNTERS, CTR_UNINITIALIZED);

        // and clean up
        delete[] componentv;
        componentv = nullptr;
        size = 0;
        lastComponentId = 0;
        onRunEndFired = false;
        setTerminationReason(nullptr);

        for (cComponentType *p : nedLoader->getComponentTypes())
            p->clearSharedParImpls();
        cModule::clearNamePools();

        gotoState(SIM_NONETWORK);
        notifyLifecycleListeners(LF_POST_NETWORK_DELETE);

        // clear remaining messages (module dtors may have cancelled & deleted some of them)
        fes->clear();
        endSimulationEvent = nullptr;

        stopwatch->clear();
    }
    catch (std::exception& e) {
        gotoState(SIM_ERROR);
        notifyLifecycleListenersOnErrorThenRethrow(e);
    }

#ifdef DEVELOPER_DEBUG
    printf("DEBUG: after deleteNetwork: %d objects\n", (int)cOwnedObject::getLiveObjectCount());
    printAllObjects();
#endif
}

cModuleType *cSimulation::getNetworkType() const
{
    return systemModule ? systemModule->getModuleType() : nullptr;
}

cEvent *cSimulation::takeNextEvent()
{
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
    ASSERT(state == SIM_RUNNING);  // must be called from run()

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
    if (fingerprint && event->isMessage())
        fingerprint->addEvent(event);

#ifndef NDEBUG
    if (trapOnNextEvent && !event->isMessage())
        DEBUG_TRAP_IF_REQUESTED;  // ABOUT TO PROCESS THE EVENT YOU REQUESTED TO DEBUG -- SELECT "STEP INTO" IN YOUR DEBUGGER
#endif

    try {
        event->execute();
    }
    catch (cDeleteModuleException& e) {
        setGlobalContext();
        e.getModuleToDelete()->deleteModule();
    }
    catch (cException&) {
        // restore global context before throwing the exception further
        setGlobalContext();
        throw;
    }
    catch (std::exception& e) {
        // restore global context before throwing the exception further
        // but wrap into a cRuntimeError which captures the module before that
        cRuntimeError e2(e);
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

class cDefaultEventLoopRunner : public cIEventLoopRunner
{
  public:
    cDefaultEventLoopRunner(cSimulation *simulation) : cIEventLoopRunner(simulation) {}
    virtual void configure(cConfiguration *cfg) override {}
    virtual void runEventLoop() override;
};

void cDefaultEventLoopRunner::runEventLoop()
{
    if (simulation->hasRealTimeLimit()) {
        unsigned int i = 0;
        while (true) {
            cEvent *event = simulation->takeNextEvent();
            if (!event)
                throw cTerminationException("Scheduler interrupted while waiting");
            simulation->executeEvent(event);
            if ((i++ & 1023) == 0)
                simulation->checkRealTimeLimits();
        }
    }
    else {
        while (true) {
            cEvent *event = simulation->takeNextEvent();
            if (!event)
                throw cTerminationException("Scheduler interrupted while waiting");
            simulation->executeEvent(event);
        }
    }
}

bool cSimulation::run(cIEventLoopRunner *runner, bool shouldCallFinish)
{
    ASSERT(stage == STAGE_NONE);  // must not be doing something else

    switch (state) {
        case SIM_NONETWORK: throw cRuntimeError("run(): No network set up");
        case SIM_NETWORKBUILT: callInitialize(); break;
        case SIM_INITIALIZED: case SIM_PAUSED: break;
        case SIM_RUNNING: throw cRuntimeError("run(): Simulation already running");
        case SIM_TERMINATED: case SIM_FINISHCALLED: throw cRuntimeError("run(): Simulation already terminated");
        case SIM_ERROR: throw cRuntimeError("run(): Cannot continue after an error");
    }

    cDefaultEventLoopRunner defaultRunner(this);
    if (runner == nullptr)
        runner = &defaultRunner;

    stopwatch->startClock();

    bool firstRun = (state == SIM_INITIALIZED);

    gotoState(SIM_RUNNING);
    StageSwitcher _(this, STAGE_EVENT);

    try {
        notifyLifecycleListeners(firstRun ? LF_ON_SIMULATION_START : LF_ON_SIMULATION_RESUME);
        runner->runEventLoop();
        stopwatch->stopClock();
        gotoState(SIM_PAUSED);
        notifyLifecycleListeners(LF_ON_SIMULATION_PAUSE);
        return true;
    }
    catch (cTerminationException& e) {
        stopwatch->stopClock();
        gotoState(SIM_TERMINATED);
        setTerminationReason(e.dup());
        notifyLifecycleListeners(LF_ON_SIMULATION_SUCCESS, &e);
        if (shouldCallFinish)
            callFinish();
        return false;
    }
    catch (std::exception& e) {
        stopwatch->stopClock();
        gotoState(SIM_ERROR);
        notifyLifecycleListenersOnErrorThenRethrow(e);
    }
}

void cSimulation::transferToMain()
{
    if (currentActivityModule != nullptr) {
        currentActivityModule = nullptr;
        cCoroutine::switchToMain();  // stack switch
    }
}

void cSimulation::setTerminationReason(cTerminationException *e)
{
    delete terminationReason;
    terminationReason = e;
}

void cSimulation::setElapsedTimeLimit(double seconds)
{
    stopwatch->setRealTimeLimit(seconds);
    stopwatch->resetRealTimeUsage();
}

double cSimulation::getElapsedTimeLimit() const
{
    return stopwatch->getRealTimeLimit();
}

void cSimulation::setCpuTimeLimit(double seconds)
{
    stopwatch->setCPUTimeLimit(seconds);
    stopwatch->resetCPUTimeUsage();
}

double cSimulation::getCpuTimeLimit() const
{
    return stopwatch->getCPUTimeLimit();
}

double cSimulation::getElapsedTime() const
{
    return stopwatch->getElapsedSecs();
}

double cSimulation::getCpuUsageTime() const
{
    return stopwatch->getCPUUsageSecs();
}

void cSimulation::checkRealTimeLimits()
{
    stopwatch->checkTimeLimits();
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

uint64_t cSimulation::getUniqueNumber()
{
    uint64_t ret = nextUniqueNumber++;
    if (nextUniqueNumber == uniqueNumbersEnd)
        throw cRuntimeError("getUniqueNumber(): All values have been consumed");
    return ret;
}


int cSimulation::SharedDataHandles::registerName(const char *name)
{
    auto it = nameToHandle.find(name);
    if (it != nameToHandle.end())
        return it->second;
    else {
        nameToHandle[name] = ++lastHandle;
        return lastHandle;
    }
}

const char *cSimulation::SharedDataHandles::getNameFor(int handle)
{
    // linear search should suffice here (small number of items, non-performance-critical)
    for (auto& pair : nameToHandle)
        if (pair.second == handle)
            return pair.first.c_str();
    return nullptr;
}

static std::recursive_mutex sharedVariablesMutex;

sharedvarhandle_t cSimulation::registerSharedVariableName(const char *name)
{
    std::lock_guard<std::recursive_mutex> lock(sharedVariablesMutex);
    if (sharedVariableHandles == nullptr)
        sharedVariableHandles = new SharedDataHandles();
    return sharedVariableHandles->registerName(name);
}

const char *cSimulation::getSharedVariableName(sharedvarhandle_t handle)
{
    std::lock_guard<std::recursive_mutex> lock(sharedVariablesMutex);
    return sharedVariableHandles ? sharedVariableHandles->getNameFor(handle) : nullptr;
}

static std::recursive_mutex sharedCountersMutex;

sharedcounterhandle_t cSimulation::registerSharedCounterName(const char *name)
{
    std::lock_guard<std::recursive_mutex> lock(sharedCountersMutex);
    if (sharedCounterHandles == nullptr)
        sharedCounterHandles = new SharedDataHandles();
    return sharedCounterHandles->registerName(name);
}

const char *cSimulation::getSharedCounterName(sharedcounterhandle_t handle)
{
    std::lock_guard<std::recursive_mutex> lock(sharedCountersMutex);
    return sharedCounterHandles ? sharedCounterHandles->getNameFor(handle) : nullptr;
}

uint64_t& cSimulation::getSharedCounter(sharedcounterhandle_t handle, uint64_t initialValue)
{
    if (sharedCounters.size() <= handle)
        throw cRuntimeError("getSharedCounter(): invalid handle %lu, or number of shared counters exhausted (you can increase table size in " __FILE__ ")", (unsigned long)handle); // fixed size to prevent array reallocation which would invalidate returned references -- increase if necessary
    uint64_t& counter = sharedCounters[handle];
    if (counter == CTR_UNINITIALIZED)
        counter = initialValue;
    return counter;
}

uint64_t& cSimulation::getSharedCounter(const char *name, uint64_t initialValue)
{
    return getSharedCounter(registerSharedCounterName(name), initialValue);
}

int cSimulation::getParsimPartitionId() const
{
#ifdef WITH_PARSIM
    return parsimPartition? parsimPartition->getPartitionId() : 0;
#else
    return 0;
#endif
}

int cSimulation::getParsimNumPartitions() const
{
#ifdef WITH_PARSIM
    return parsimPartition? parsimPartition->getNumPartitions() : 0;
#else
    return 0;
#endif
}

#ifdef WITH_PYTHON
PyObject *cSimulation::getComponentAccessor(int componentId)
{
    // These have to be initialized lazily, for two reasons:
    //  - So the NEDPATH is properly set up when it's put into PYTHONPATH.
    //  - So we don't do it unnecessarily if the model doesn't make use of it.
    auto it = componentAccessors.find(componentId);
    return it == componentAccessors.end() ? nullptr : it->second;
}

void cSimulation::putComponentAccessor(int componentId, PyObject *accessor)
{
    ASSERT(!common::containsKey(componentAccessors, componentId));
    componentAccessors[componentId] = accessor;
}
#endif

void cSimulation::setFingerprintCalculator(cFingerprintCalculator *f)
{
    if (fingerprint)
        dropAndDelete(fingerprint);
    fingerprint = f;
    if (fingerprint)
        take(fingerprint);
}

void cSimulation::insertEvent(cEvent *event)
{
    event->setPreviousEventNumber(currentEventNumber);
    fes->insert(event);
}

void cSimulation::addLifecycleListener(cISimulationLifecycleListener *listener)
{
    auto it = std::find(listeners.begin(), listeners.end(), listener);
    if (it == listeners.end()) {
        listeners.push_back(listener);
        listener->listenerAdded(this);
    }
}

void cSimulation::removeLifecycleListener(cISimulationLifecycleListener *listener)
{
    auto it = std::find(listeners.begin(), listeners.end(), listener);
    if (it != listeners.end()) {
        listeners.erase(it);
        listener->listenerRemoved();
    }
}

std::vector<cISimulationLifecycleListener*> cSimulation::getLifecycleListeners() const
{
    return listeners;
}

void cSimulation::notifyLifecycleListenersOnErrorThenRethrow(std::exception& exceptionBeingHandled)
{
    if (dynamic_cast<cRuntimeError*>(&exceptionBeingHandled)) {
        cRuntimeError& re = static_cast<cRuntimeError&>(exceptionBeingHandled); // cast
        notifyLifecycleListenersOnError(re);
        throw re;
    }
    else {
        cRuntimeError re(exceptionBeingHandled); // wrap
        notifyLifecycleListenersOnError(re);
        throw re;
    }
}

void cSimulation::notifyLifecycleListenersOnError(cRuntimeError& exceptionBeingHandled)
{
    // To be called while handling an exception. Exceptions during handling will be added to the (parent) exception.
    try {
        notifyLifecycleListeners(LF_ON_SIMULATION_ERROR, &exceptionBeingHandled);
    }
    catch (std::exception& e) {
        exceptionBeingHandled.addNestedException(e);
    }
}

void cSimulation::notifyLifecycleListeners(SimulationLifecycleEventType eventType, cObject *details)
{
    checkActive();

    try {
        // make a copy of the listener list, to avoid problems from listeners getting added/removed during notification
        auto copy = listeners;
        for (auto& listener : copy)
            listener->lifecycleEvent(eventType, details);  // let exceptions through, because errors must cause exitCode!=0
        CodeFragments::executeAll(eventType, false);
    }
    catch (cRuntimeError& e) {
        e.setLifecycleListenerType(eventType);
        gotoState(SIM_ERROR);
        if (eventType != LF_ON_SIMULATION_ERROR) // prevent the same error from occurring again
            notifyLifecycleListenersOnErrorThenRethrow(e);
        throw;
    }
    catch (std::exception& e) {
        gotoState(SIM_ERROR);
        cRuntimeError re(e);
        re.setLifecycleListenerType(eventType);
        if (eventType != LF_ON_SIMULATION_ERROR) // prevent the same error from occurring again
            notifyLifecycleListenersOnErrorThenRethrow(re);
        throw;
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

  public:
    // constructor, destructor
    StaticEnv() {}
    virtual ~StaticEnv() {}
    virtual void configure(cConfiguration *cfg) override {}

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
    virtual void preconfigureComponent(cComponent *component) override  {}
    virtual void configureComponent(cComponent *component) override {}
    virtual void readParameter(cPar *parameter) override  { unsupported(); }
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
    virtual std::string input(const char *prompt, const char *defaultreply = nullptr) override  { unsupported(); return ""; }
    virtual cEnvir& flush() { ::fflush(stdout); return *this; }

    // output vectors
    virtual void *registerOutputVector(const char *modulename, const char *vectorname) override  { return nullptr; }
    virtual void deregisterOutputVector(void *vechandle) override  {}
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value) override  {}
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
    virtual bool idle() override  { return false; }
    virtual void pausePoint() override {}
    virtual void refOsgNode(osg::Node *scene) override {}
    virtual void unrefOsgNode(osg::Node *scene) override {}
    virtual bool ensureDebugger(cRuntimeError *) override { return false; }
    virtual bool shouldDebugNow(cRuntimeError *error) override {return false;}

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

OPP_THREAD_LOCAL StaticEnv staticEnv;

OPP_THREAD_LOCAL cEnvir *cSimulation::activeEnvir = &staticEnv;
OPP_THREAD_LOCAL cEnvir *cSimulation::staticEnvir = &staticEnv;
OPP_THREAD_LOCAL cSimulation *cSimulation::activeSimulation = nullptr;

std::atomic<cSimulation::EnvirFactoryFunction> cSimulation::envirFactoryFunction;

}  // namespace omnetpp

