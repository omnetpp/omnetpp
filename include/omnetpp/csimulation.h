//==========================================================================
//   CSIMULATION.H  -  header for
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

#ifndef __OMNETPP_CSIMULATION_H
#define __OMNETPP_CSIMULATION_H

#include "simkerneldefs.h"
#include "simtime_t.h"
#include "ccomponent.h"
#include "cexception.h"

namespace omnetpp {

class cEvent;
class cMessage;
class cGate;
class cComponent;
class cModule;
class cChannel;
class cSimpleModule;
class cSimulation;
class cException;
class cFutureEventSet;
class cScheduler;
class cParsimPartition;
class cFingerprintCalculator;
class cModuleType;
class cEnvir;
class cSoftOwner;
class cINedLoader;
class cIRngManager;
class IRunner;

SIM_API extern OPP_THREAD_LOCAL cSoftOwner globalOwningContext; // also in globals.h


/**
 * @brief Simulation manager class.
 *
 * cSimulation is the central class in \opp. It stores the active simulation
 * model, and provides methods for setting up, running and finalizing simulations.
 *
 * Most cSimulation methods are not of interest for simulation model code,
 * they are used internally (e.g. by the user interface libraries (Envir,
 * Cmdenv, Qtenv) to set up and run simulations).
 *
 * Some methods which can be of interest when programming simple modules:
 * getUniqueNumber(), getModuleByPath(), getModule(), snapshot().
 *
 * @ingroup SimCore
 */
class SIM_API cSimulation : public cNamedObject, noncopyable
{
    friend class cSimpleModule;

  public:
    /**
     * State of the simulation.
     */
    enum State {
        SIM_NONETWORK,
        SIM_NETWORKBUILT,
        SIM_INITIALIZED,
        SIM_RUNNING,
        SIM_PAUSED,
        SIM_TERMINATED,
        SIM_FINISHCALLED,
        SIM_ERROR
    };

    /**
     * What cSimulation is doing currently.
     */
    enum Stage {
        STAGE_NONE,
        STAGE_BUILD,
        STAGE_INITIALIZE,
        STAGE_EVENT,
        STAGE_REFRESHDISPLAY,
        STAGE_BUSY,  //TODO remove?
        STAGE_FINISH,
        STAGE_CLEANUP
    };

  private:
    // global variables
    static OPP_THREAD_LOCAL cSimulation *activeSimulation;
    static OPP_THREAD_LOCAL cEnvir *activeEnvir;
    static OPP_THREAD_LOCAL cEnvir *staticEnvir; // the environment to activate when activeSimulation becomes nullptr

    // variables of the module vector
    int size = 0;                       // size of componentv[]
    int delta = 32;                     // if needed, grows by delta
    cComponent **componentv = nullptr;  // vector of modules/channels, componentv[0] is not used for historical reasons
    int lastComponentId = 0;            // index of last used pos. in componentv[]

    // simulation vars
    cINedLoader *nedLoader = nullptr;   // NED loader/resolver - NOT OWNED
    cIRngManager *rngManager = nullptr; // component-rng mapping
    cEnvir *envir = nullptr;            // the environment that belongs to this simulation object
    cConfiguration *cfg = nullptr;      // the configuration object passed in the last configure() call -- NOT OWNED
    cModule *systemModule = nullptr;    // pointer to system (root) module
    cSimpleModule *currentActivityModule = nullptr; // the module currently executing activity() (nullptr if handleMessage() or in main)
    cComponent *contextComponent = nullptr;  // component in context (or nullptr)
    cFutureEventSet *fes = nullptr;     // stores future events
    cScheduler *scheduler = nullptr;    // event scheduler
    simtime_t warmupPeriod;             // warm-up period

    simtime_t simTimeLimit = 0;         // simulation time limit (0 -> no limit)
    cEvent *endSimulationEvent = nullptr; // only present if simulation time limit is set

    State state = SIM_NONETWORK;        // simulation state
    Stage stage = STAGE_NONE;           // what the simulation is currently doing
    bool onRunEndFired = false;         // whether LF_ON_RUN_END has been fired in this run
    simtime_t currentSimtime;           // simulation time (time of current event)
    eventnumber_t currentEventNumber = 0; // sequence number of current event
    cTerminationException *terminationReason = nullptr; // filled in after the TERMINATED state

    cException *exception;    // helper variable to get exceptions back from activity()
    bool trapOnNextEvent = false;  // when set, next handleMessage or activity() will execute debugger interrupt

    bool parameterMutabilityCheck = true;  // when disabled, module parameters can be set without them being declared @mutable

    cFingerprintCalculator *fingerprint = nullptr; // used for fingerprint calculation

    bool parsim = false;
#ifdef WITH_PARSIM
    cParsimPartition *parsimPartition = nullptr;
#endif

    // Data for getUniqueNumber()
    uint64_t nextUniqueNumber;
    uint64_t uniqueNumbersEnd;

    // Lifecycle listeners
    std::vector<cISimulationLifecycleListener*> listeners;

  private:
    // internal
    void checkActive()  {if (getActiveSimulation()!=this) throw cRuntimeError(this, E_WRONGSIM);}
    void gotoState(State state);
    void setStage(Stage stage);
    void scheduleEndSimulationEvent();
    void notifyLifecycleListeners(SimulationLifecycleEventType eventType, const std::exception& e);
    void notifyLifecycleListeners(SimulationLifecycleEventType eventType, cObject *details=nullptr);
    void setTerminationReason(cTerminationException *e);

    struct StageSwitcher {
        cSimulation *simulation;
        Stage oldStage;
        StageSwitcher(cSimulation *self, Stage newStage) : simulation(self), oldStage(simulation->stage) {simulation->setStage(newStage);}
        ~StageSwitcher() {simulation->setStage(oldStage);}
    };

  public:
    // internal
    void setParameterMutabilityCheck(bool b) {parameterMutabilityCheck = b;}
    bool getParameterMutabilityCheck() const {return parameterMutabilityCheck;}

    // internal
    void setUniqueNumberRange(uint64_t start, uint64_t end) {nextUniqueNumber = start; uniqueNumbersEnd = end;}

  public:
    /** @name Constructor, destructor. */
    //@{
    /**
     * Constructor. The environment object will be associated with this simulation
     * object, and gets deleted in the simulation object's destructor. If no
     * environment object is given, a default one will be constructed.
     */
    cSimulation(const char *name, cEnvir *envir=nullptr);

    /**
     * Destructor.
     */
    virtual ~cSimulation();
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v) override;

    /**
     * Redefined. (Reason: a C++ rule that overloaded virtual methods must be redefined together.)
     */
    virtual std::string getFullPath() const override;
    //@}

    /** @name Accessing and switching the active simulation object */
    //@{

    /**
     * Returns the very simulation instance, i.e. itself.
     */
    virtual cSimulation *getSimulation() const override {return const_cast<cSimulation*>(this);}

    /**
     * Returns the active simulation object. May be nullptr.
     */
    static cSimulation *getActiveSimulation()  {return activeSimulation;}

    /**
     * Returns the environment object for the active simulation. Never returns nullptr;
     * setActiveSimulation(nullptr) will cause a static "do-nothing" instance to step in.
     */
    static cEnvir *getActiveEnvir()  {return activeEnvir;}

    /**
     * Activate the given simulation object, and its associated environment
     * object. nullptr is also accepted; it will cause the static environment
     * object to step in (see getStaticEnvir()).
     */
    static void setActiveSimulation(cSimulation *sim);

    /**
     * Sets the environment object to use when there is no active simulation object.
     * The argument cannot be nullptr.
     */
    static void setStaticEnvir(cEnvir *env);

    /**
     * Returns the environment object to use when there is no active simulation object.
     */
    static cEnvir *getStaticEnvir()  {return staticEnvir;}

    /**
     * Returns the environment object associated with this simulation object.
     */
    cEnvir *getEnvir() const  {return envir;}  // note: intentionally non-virtual

    /**
     * Returns the configuration object passed in the last configure(cConfiguration*) call.
     */
    cConfiguration *getConfig() const {return cfg;}  // note: intentionally not virtual
    //@}

    /** @name Accessing modules and channels. */
    //@{

    /**
     * Registers the component in cSimulation and assigns it an ID. It is called
     * internally during component creation. The ID of a deleted component is not
     * issued again to another component, because we want IDs to be unique during
     * the whole simulation.
     */
    virtual int registerComponent(cComponent *component);

    /**
     * Deregisters the component from cSimulation. It is called internally from
     * cComponent's destructor.
     */
    virtual void deregisterComponent(cComponent *component);

    /**
     * Returns the highest used component ID.
     */
    int getLastComponentId() const {return lastComponentId;}  // note: intentionally non-virtual

    /**
     * Finds a module by its path. The path is a string of module names
     * separated by dots. Inclusion of the toplevel module's name in the
     * path is optional. The toplevel module's name may also be written
     * as "<root>". Note that this method, unlike its counterpart in cModule,
     * does not accept relative paths (those that start with '.' or '^'.)
     *
     * This method never returns nullptr. If the module was not found,
     * an exception is thrown.
     *
     * @see findModuleByPath(), cModule::getModuleByPath()
     */
    virtual cModule *getModuleByPath(const char *modulePath) const;

    /**
     * Finds a module by its path. The path is a string of module names
     * separated by dots. Inclusion of the toplevel module's name in the
     * path is optional. The toplevel module's name may also be written
     * as "<root>". Note that this method, unlike its counterpart in cModule,
     * does not accept relative paths (those that start with '.' or '^'.)
     *
     * If the module was not found or the empty string was given as input,
     * this method returns nullptr.
     *
     * @see getModuleByPath(), cModule::findModuleByPath()
     */
    virtual cModule *findModuleByPath(const char *modulePath) const;

    /**
     * Looks up a component (module or channel) by ID. If the ID does not identify
     * a component (e.g. invalid ID or component already deleted), it returns nullptr.
     */
    cComponent *getComponent(int id) const  {return id < 0 || id >= size ? nullptr : componentv[id];}  // note: intentionally non-virtual

    /**
     * Looks up a module by ID. If the ID does not identify a module (e.g. invalid ID,
     * module already deleted, or object is not a module), it returns nullptr.
     */
    cModule *getModule(int id) const  {return id < 0 || id >= size || !componentv[id] ? nullptr : componentv[id]->isModule() ? (cModule *)componentv[id] : nullptr;}  // note: intentionally non-virtual

    /**
     * Looks up a channel by ID. If the ID does not identify a channel (e.g. invalid ID,
     * channel already deleted, or object is not a channel), it returns nullptr.
     */
    cChannel *getChannel(int id) const  {return id < 0 || id >= size || !componentv[id] ? nullptr : componentv[id]->isChannel() ? (cChannel *)componentv[id] : nullptr;}  // note: intentionally non-virtual

    /**
     * Designates the system module, the top-level module in the model.
     */
    virtual void setSystemModule(cModule *module);

    /**
     * Returns pointer to the system module, the top-level module in the model.
     */
    cModule *getSystemModule() const  {return systemModule;}
    //@}

    /** @name Loading NED files.
     *
     * These functions delegate to the netbuilder part of the simulation kernel,
     * and they are present so that cEnvir and other libs outside the simkernel
     * do not need to directly depend on nedxml or netbuilder classes, and
     * conditional compilation (\#ifdef WITH_NETBUILDER) can be limited to the
     * simkernel.
     */
    //@{

    /**
     * Returns the NED loader associated with this simulation.
     */
    cINedLoader *getNedLoader() const {return nedLoader;}

    /**
     * Shortcut for <tt>getNedLoader()->loadNedSourceFolder(folderName, excludedPackages)</tt>.
     */
    virtual int loadNedSourceFolder(const char *folderName, const char *excludedPackages="");

    /**
     * Shortcut for <tt>getNedLoader()->loadNedFile(nedFilename, expectedPackage, isXML)</tt>.
     */
    virtual void loadNedFile(const char *nedFilename, const char *expectedPackage=nullptr, bool isXML=false);

    /**
     * Shortcut for <tt>getNedLoader()->loadNedText(name, nedText, expectedPackage, isXML)</tt>.
     */
    virtual void loadNedText(const char *name, const char *nedText, const char *expectedPackage=nullptr, bool isXML=false);

    /**
     * Shortcut for <tt>getNedLoader()->checkLoadedTypes()</tt>.
     */
    virtual void checkLoadedTypes();

    /**
     * Shortcut for <tt>getNedLoader()->getNedPackageForFolder(folder)</tt>.
     */
    virtual std::string getNedPackageForFolder(const char *folder);
    //@}

    /** @name Setting up and finishing a simulation run. */
    //@{

    /**
     * Configures this simulation instance and the objects it relies on: the
     * scheduler, the FES, the RNG manager, the fingerprint calculator, etc.
     * The configuration object is remembered, and will also be used for
     * values for unassigned module parameters and additional configuration
     * when a network is set up for simulation (see setupNetwork()).
     */
    virtual void configure(cConfiguration *cfg);

    /**
     * Sets the NED loader associated with this simulation. In order to allow
     * several simulation instances to share the same NED loader, the simulation
     * does NOT become the owner of the NED loader, i.e. the caller needs to
     * arrange it to be deleted when it is no longer used by any simulation.
     */
    virtual void setNedLoader(cINedLoader *loader);

    /**
     * Installs a scheduler object. This method may only be called before or
     * between simulations, when there is no network set up. The cSimulation
     * object will be responsible for deallocating the scheduler object.
     */
    virtual void setScheduler(cScheduler *scheduler);

    /**
     * Returns the scheduler object.
     */
    cScheduler *getScheduler() const  {return scheduler;}  // note: intentionally non-virtual

    /**
     * Sets the future event set. This method may only be called before or
     * between simulations, when there is no network set up. The cSimulation
     * object will be responsible for deallocating the FES object.
     */
    virtual void setFES(cFutureEventSet *fes);

    /**
     * Returns the future event set data structure used by the simulation.
     */
    cFutureEventSet *getFES() const  {return fes;}  // note: intentionally non-virtual

    /**
     * Installs an RNG manager object. This method may only be called before or
     * between simulations, when there is no network set up. The cSimulation
     * object will be responsible for deallocating the object.
     */
    virtual void setRngManager(cIRngManager *rngManager);

    /**
     * Returns the RNG manager that manages the association between the
     * modules/channels of the simulation, and RNGs.
     */
    cIRngManager *getRngManager() const {return rngManager;}  // note: intentionally non-virtual

    /**
     * Installs a new fingerprint object, used for fingerprint calculation.
     */
    virtual void setFingerprintCalculator(cFingerprintCalculator *fingerprint);

    /**
     * Returns the object used for fingerprint calculation. It returns nullptr
     * if no fingerprint is being calculated during this simulation run.
     */
    cFingerprintCalculator *getFingerprintCalculator() {return fingerprint;}  // note: intentionally non-virtual

    /**
     * Sets the simulation stop time be scheduling an appropriate
     * "end-simulation" event. Supply zero to clear an existing simulation
     * time limit.
     */
    virtual void setSimulationTimeLimit(simtime_t simTimeLimit);

    /**
     * Returns the simulation stop time if one has been set, and zero otherwise.
     */
    virtual simtime_t getSimulationTimeLimit() const;

    /**
     * Resolves the module type name. The name may be a fully qualified NED type name,
     * or the package name can be omitted if the ini file is in the same directory as
     * the NED file that contains the network.
     */
    virtual cModuleType *resolveNetwork(const char *networkName, const char *baseDirectory);

    /**
     * Sets up the network specified by the "network" option in the given configuration,
     * or if none is given, in the configuration object previously passed to the
     * configure() method. Values for unassigned module parameters and additional
     * configuration also come from the configuration object.
     */
    virtual void setupNetwork(cConfiguration *cfg=nullptr);

    /**
     * Sets up the given module type as a network to simulate. Values for unassigned
     * module parameters and additional configuration come from the configuration
     * object passed in the second argument, or if none is given, from the configuration
     * object previously passed to the configure() method.
     */
    virtual void setupNetwork(cModuleType *networkType, cConfiguration *cfg=nullptr);

    /**
     * Should be called after setupNetwork(), but before the first
     * executeEvent() call. Includes initialization of the modules,
     * that is, invokes callInitialize() on the system module.
     */
    virtual void callInitialize();

    /**
     * Recursively calls finish() on the modules of the network.
     * This method simply invokes callFinish() on the system module.
     */
    virtual void callFinish();

    /**
     * Cleans up the network currently set up. This involves deleting
     * all modules and deleting the messages in the scheduled-event list.
     */
    virtual void deleteNetwork();
    //@}

    /** @name Information about the current simulation run. */
    //@{
    /**
     * Returns the state of the simulation.
     */
    State getState() const {return state;}  // note: intentionally non-virtual

    /**
     * Returns the current simulation stage.
     */
    Stage getStage() const  {return stage;}  // note: intentionally non-virtual

    /**
     * Returns the textual representation of the given enum value.
     */
    static const char *getStateName(State s);

    /**
     * Returns the textual representation of the given enum value.
     */
    static const char *getStageName(Stage s);

    /**
     * Returns the message that the simulation successfully terminated with.
     */
    cTerminationException *getTerminationReason() {return terminationReason;}

    /**
     * Returns the cModuleType object that was instantiated to set up
     * the current simulation model.
     */
    cModuleType *getNetworkType() const;  // note: intentionally non-virtual

    /**
     * INTERNAL USE ONLY. This method should NEVER be invoked from
     * simulation models, only from scheduler classes subclassed from
     * cScheduler.
     */
    virtual void setSimTime(simtime_t time)  {currentSimtime = time;}

    /**
     * Returns the current simulation time. (It is also available via the
     * global simTime() function.) Between events it returns the time of
     * the last executed event.
     */
    simtime_t_cref getSimTime() const  {return currentSimtime;}  // note: intentionally non-virtual

    /**
     * Returns the sequence number of current event. Between events it returns
     * the sequence number of the next event.
     */
    eventnumber_t getEventNumber() const  {return currentEventNumber;}  // note: intentionally non-virtual

    /**
     * Sets the warm-up period.
     */
    virtual void setWarmupPeriod(simtime_t t)  {warmupPeriod = t;}

    /**
     * Returns the length of the initial warm-up period from the configuration.
     * Modules that compute and record scalar results manually (via recordScalar(),
     * recordStatistic(), etc.) should be implemented in a way that they ignore
     * observations generated during the warm-up period. cOutVector objects,
     * and results recorded via the signals mechanism automatically obey
     * the warm-up period and need not be modified. The warm-up period is useful
     * for steady-state simulations.
     */
    virtual simtime_t_cref getWarmupPeriod() const  {return warmupPeriod;}  // note: intentionally non-virtual
    //@}

    /** @name Scheduling and simulation execution. */
    //@{
    /**
     * Returns the likely next event in the simulation. Intended to be called
     * from a user interface library for displaying simulation status information.
     * Delegates to cScheduler::guessNextEvent().
     */
    virtual cEvent *guessNextEvent();

    /**
     * Returns the module associated with the likely next event, or nullptr if
     * there is no such module. Based on guessNextEvent(); see further
     * comments there.
     */
    virtual cSimpleModule *guessNextModule();

    /**
     * Returns the simulation time of the likely next event, or -1 if there is
     * none. Based on guessNextEvent(); see further comments there.
     */
    virtual simtime_t guessNextSimtime();

    /**
     * The scheduler function. Returns the next event from the FES.
     * If there is no more event (FES is empty), it throws cTerminationException.
     *
     * A nullptr return value means that there is no error but execution
     * was stopped by the user (e.g. with STOP button on the GUI)
     * while the scheduler object was waiting for external synchronization.
     *
     * Delegates to cScheduler::takeNextEvent().
     */
    virtual cEvent *takeNextEvent();

    /**
     * Undo for takeNextEvent(); see cScheduler's similar method for details.
     */
    virtual void putBackEvent(cEvent *event);

    /**
     * Executes an event as part of the simulation. Also increments the event
     * number (see getEventNumber()). This method may not be called directly,
     * only from run() via runners (see cIRunner).
     */
    virtual void executeEvent(cEvent *event);

    /**
     * Runs the simulation with the given runner. The network is initialized
     * if it has not been initialized already. After the simulation completes
     * (provided it is allowed to run through), finish is automatically called
     * if allowed by the 2nd argument.
     *
     * If the runner argument is nullptr, a barebones runner will be used as default.
     *
     * The return value tells whether the simulation can be continued (=true)
     * or has successfully completed (false). In the latter case, the
     * corresponding message can be obtained with getTerminationReason().
     * If there was an error, it manifests as an exception which is allowed
     * to propagate out of this method.
     */
    virtual bool run(IRunner *runner=nullptr, bool shouldCallFinish=true);

    /**
     * Invoke callRefreshDisplay() on the system module.
     */
    virtual void callRefreshDisplay();

    /**
     * Switches to the given simple module's coroutine. This method is invoked
     * from executeEvent() for activity()-based modules.
     */
    virtual void transferTo(cSimpleModule *module);

    /**
     * Switches to main coroutine.
     */
    virtual void transferToMain();

    /**
     * Inserts the given event into the future events queue while assigning
     * the current event to its scheduling event. Used internally by
     * cSimpleModule::scheduleAt() and various other cSimpleModule methods.
     */
    virtual void insertEvent(cEvent *event);

    /**
     * Sets the component (module or channel) in context. Used internally.
     */
    virtual void setContext(cComponent *component);

    /**
     * Sets global context. Used internally.
     */
    virtual void setGlobalContext()  {contextComponent=nullptr; cOwnedObject::setOwningContext(&globalOwningContext);}

    /**
     * Returns the module whose activity() method is currently active.
     * Returns nullptr if no module is running, or the current module uses
     * handleMessage().
     */
    cSimpleModule *getActivityModule() const {return currentActivityModule;}  // note: intentionally non-virtual

    /**
     * Returns the component (module or channel) currently in context.
     */
    cComponent *getContext() const {return contextComponent;}  // note: intentionally non-virtual

    /**
     * If the current context is a module, returns its pointer,
     * otherwise returns nullptr.
     */
    cModule *getContextModule() const;  // note: intentionally non-virtual

    /**
     * Returns the module currently in context as a simple module.
     * If the module in context is not a simple module, returns nullptr.
     * This is a convenience function which simply calls getContextModule().
     */
    cSimpleModule *getContextSimpleModule() const;  // note: intentionally non-virtual

    /**
     * Request the next handleMessage() or activity() to execute a debugger
     * interrupt. (If the program is not run under a debugger, that will
     * usually result in a crash.)
     */
    virtual void requestTrapOnNextEvent() {trapOnNextEvent = true;}

    /**
     * Clear the previous requestTrapOnNextEvent() call.
     */
    virtual void clearTrapOnNextEvent() {trapOnNextEvent = false;}

    /**
     * Returns true if there is a pending request to execute a debugger
     * interrupt on the next event.
     */
    bool isTrapOnNextEventRequested() const {return trapOnNextEvent;}  // note: intentionally non-virtual
    //@}

    /** @name Lifecycle listeners */
    //@{
    /**
     * Adds a listener that will be notified about simulation lifecycle events.
     * It has no effect if the listener is already subscribed.
     * NOTE: The listeners will NOT be deleted when the program exits.
     */
    virtual void addLifecycleListener(cISimulationLifecycleListener *listener);

    /**
     * Removes the given listener. This method has no effect if the listener
     * is not currently subscribed.
     */
    virtual void removeLifecycleListener(cISimulationLifecycleListener *listener);

    /**
     * Returns the list of installed lifecycle listeners.
     */
    virtual std::vector<cISimulationLifecycleListener*> getLifecycleListeners() const;
    //@}

    /** @name Parallel simulation. */
    //@{

    /**
     * Returns true if the current simulation run uses parallel simulation.
     */
    bool isParsimEnabled() const {return parsim;}  // note: intentionally non-virtual

#ifdef WITH_PARSIM
    /**
     * Returns the parsim partition object, or nullptr if parallel simulation
     * is not currently active, i.e. isParsimEnabled() = false.
     */
    cParsimPartition *getParsimPartition() const {return parsimPartition;}
#endif

    /**
     * Returns the partitionID when parallel simulation is active.
     */
    virtual int getParsimProcId() const;

    /**
     * Returns the number of partitions when parallel simulation is active;
     * otherwise it returns 0.
     */
    virtual int getParsimNumPartitions() const;
    //@}

    /** @name Miscellaneous. */
    //@{
    /**
     * This function is guaranteed to return a different integer every time
     * it is called (usually 0, 1, 2, ...). This method works with parallel
     * simulation as well, so it is recommended over incrementing a global
     * variable. Useful for generating unique network addresses, etc.
     */
    virtual uint64_t getUniqueNumber();

    /**
     * Writes a snapshot of the given object and its children to the
     * textual snapshot file.
     * This method is called internally from cSimpleModule's snapshot().
     */
    virtual void snapshot(cObject *obj, const char *label);
    //@}
};

/**
 * @brief Returns the current simulation time.
 *
 * @ingroup SimTime
 */
inline simtime_t simTime() {return cSimulation::getActiveSimulation()->getSimTime();}

/**
 * @brief Returns the currently active simulation, or nullptr if there is none.
 *
 * Deprecated -- use cSimulation::getActiveSimulation() instead.
 *
 * @ingroup Misc
 */
[[deprecated]] inline cSimulation *getSimulation()  {return cSimulation::getActiveSimulation();}

/**
 * @brief Returns the environment object for the currently active simulation.
 * This function never returns nullptr (not even during shutdown).
 *
 * Deprecated -- use cSimulation::getActiveEnvir() instead.
 *
 * @ingroup Misc
 */
[[deprecated]] inline cEnvir *getEnvir()  {return cSimulation::getActiveEnvir();}

}  // namespace omnetpp


#endif

