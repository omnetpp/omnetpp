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
#include "ccontextswitcher.h"
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
class cNedFileLoader;
class cFingerprintCalculator;
class cModuleType;
class cEnvir;
class cSoftOwner;

SIM_API extern cSoftOwner globalOwningContext; // also in globals.h


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
  private:
    // global variables
    static cSimulation *activeSimulation;
    static cEnvir *activeEnvir;
    static cEnvir *staticEnvir; // the environment to activate when activeSimulation becomes nullptr

    // variables of the module vector
    int size = 0;                       // size of componentv[]
    int delta = 32;                     // if needed, grows by delta
    cComponent **componentv = nullptr;  // vector of modules/channels, componentv[0] is not used for historical reasons
    int lastComponentId = 0;            // index of last used pos. in componentv[]

    // simulation vars
    cEnvir *envir = nullptr;            // the environment that belongs to this simulation object
    cModule *systemModule = nullptr;    // pointer to system (root) module
    cSimpleModule *currentActivityModule = nullptr; // the module currently executing activity() (nullptr if handleMessage() or in main)
    cComponent *contextComponent = nullptr;  // component in context (or nullptr)
    ContextType contextType;            // the innermost context type
    cModuleType *networkType = nullptr; // network type
    cFutureEventSet *fes = nullptr;     // stores future events
    cScheduler *scheduler = nullptr;    // event scheduler
    simtime_t warmupPeriod;             // warm-up period

    ContextType simulationStage;        // simulation stage
    simtime_t currentSimtime;           // simulation time (time of current event)
    eventnumber_t currentEventNumber = 0; // sequence number of current event

    cException *exception;    // helper variable to get exceptions back from activity()
    bool trapOnNextEvent = false;  // when set, next handleMessage or activity() will execute debugger interrupt

    bool parameterMutabilityCheck = true;  // when disabled, module parameters can be set without them being declared @mutable

    cFingerprintCalculator *fingerprint = nullptr; // used for fingerprint calculation

  private:
    // internal
    void checkActive()  {if (getActiveSimulation()!=this) throw cRuntimeError(this, E_WRONGSIM);}

  public:
    // internal
    void setParameterMutabilityCheck(bool b) {parameterMutabilityCheck = b;}
    bool getParameterMutabilityCheck() const {return parameterMutabilityCheck;}

  public:
    /** @name Constructor, destructor. */
    //@{
    /**
     * Constructor. The environment object will be associated with this simulation
     * object, and gets deleted in the simulation object's destructor.
     */
    cSimulation(const char *name, cEnvir *env);

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
    cEnvir *getEnvir() const  {return envir;}
    //@}

    /** @name Accessing modules. */
    //@{

    /**
     * Registers the component in cSimulation and assigns it an ID. It is called
     * internally during component creation. The ID of a deleted component is not
     * issued again to another component, because we want IDs to be unique during
     * the whole simulation.
     */
    int registerComponent(cComponent *component);

    /**
     * Deregisters the component from cSimulation. It is called internally from
     * cComponent's destructor.
     */
    void deregisterComponent(cComponent *component);

    /**
     * Returns the highest used component ID.
     */
    int getLastComponentId() const    {return lastComponentId;}

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
    cModule *getModuleByPath(const char *modulePath) const;

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
    cModule *findModuleByPath(const char *modulePath) const;

    /**
     * Looks up a component (module or channel) by ID. If the ID does not identify
     * a component (e.g. invalid ID or component already deleted), it returns nullptr.
     */
    cComponent *getComponent(int id) const  {return id<0 || id>=size ? nullptr : componentv[id];}

    /**
     * Looks up a module by ID. If the ID does not identify a module (e.g. invalid ID,
     * module already deleted, or object is not a module), it returns nullptr.
     */
    cModule *getModule(int id) const  {return id<0 || id>=size || !componentv[id] ? nullptr : componentv[id]->isModule() ? (cModule *)componentv[id] : nullptr;}

    /**
     * Looks up a channel by ID. If the ID does not identify a channel (e.g. invalid ID,
     * channel already deleted, or object is not a channel), it returns nullptr.
     */
    cChannel *getChannel(int id) const  {return id<0 || id>=size || !componentv[id] ? nullptr : componentv[id]->isChannel() ? (cChannel *)componentv[id] : nullptr;}

    /**
     * Designates the system module, the top-level module in the model.
     */
    void setSystemModule(cModule *module);

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
     * Load all NED files from a NED source folder. This involves visiting
     * each subdirectory, and loading all "*.ned" files from there.
     * The given folder is assumed to be the root of the NED package hierarchy.
     * A list of packages to skip may be specified in the excludedPackages parameter
     * (items must be separated with a semicolon).
     *
     * The function returns the number of NED files loaded.
     *
     * Note: doneLoadingNedFiles() must be called after the last
     * loadNedSourceFolder()/loadNedFile()/loadNedText() call.
     */
    static int loadNedSourceFolder(const char *folderName, const char *excludedPackages="");

    /**
     * Load a single NED file. If the expected package is given (non-nullptr),
     * it should match the package declaration inside the NED file.
     *
     * Note: doneLoadingNedFiles() must be called after the last
     * loadNedSourceFolder()/loadNedFile()/loadNedText() call.
     */
    static void loadNedFile(const char *nedFilename, const char *expectedPackage=nullptr, bool isXML=false);

    /**
     * Parses and loads the NED source code passed in the nedtext argument.
     * The name argument will be used as filename in error messages, and
     * and should be unique among the files loaded. If the expected package
     * is given (non-nullptr), it should match the package declaration inside
     * the NED file.
     *
     * Note: doneLoadingNedFiles() must be called after the last
     * loadNedSourceFolder()/loadNedFile()/loadNedText() call.
     */
    static void loadNedText(const char *name, const char *nedText, const char *expectedPackage=nullptr, bool isXML=false);

    /**
     * To be called after all NED folders / files have been loaded
     * (see loadNedSourceFolder()/loadNedFile()/loadNedText()).
     * Issues errors for components that could not be fully resolved
     * because of missing base types or interfaces.
     */
    static void doneLoadingNedFiles();

    /**
     * Returns the NED package that corresponds to the given folder. Returns ""
     * for the default package, and "-" if the folder is outside all NED folders.
     */
    static std::string getNedPackageForFolder(const char *folder);
    //@}

    /** @name Setting up and finishing a simulation run. */
    //@{

    /**
     * Installs a scheduler object. This method may only be called before or
     * between simulations, when there is no network set up. The cSimulation
     * object will be responsible for deallocating the scheduler object.
     */
    void setScheduler(cScheduler *scheduler);

    /**
     * Returns the scheduler object.
     */
    cScheduler *getScheduler() const  {return scheduler;}

    /**
     * Sets the future event set. This method may only be called before or
     * between simulations, when there is no network set up. The cSimulation
     * object will be responsible for deallocating the FES object.
     */
    void setFES(cFutureEventSet *fes);

    /**
     * Returns the future event set data structure used by the simulation.
     */
    cFutureEventSet *getFES() const  {return fes;}

    /**
     * Sets the simulation stop time be scheduling an appropriate
     * "end-simulation" event. May only be called once per run.
     */
    void setSimulationTimeLimit(simtime_t simTimeLimit);

    /**
     * Builds a new network.
     */
    void setupNetwork(cModuleType *networkType);

    /**
     * Should be called after setupNetwork(), but before the first
     * executeEvent() call. Includes initialization of the modules,
     * that is, invokes callInitialize() on the system module.
     */
    void callInitialize();

    /**
     * Recursively calls finish() on the modules of the network.
     * This method simply invokes callfinish() on the system module.
     */
    void callFinish();

    /**
     * Cleans up the network currently set up. This involves deleting
     * all modules and deleting the messages in the scheduled-event list.
     */
    void deleteNetwork();
    //@}

    /** @name Information about the current simulation run. */
    //@{
    /**
     * Returns the current simulation stage: network building (CTX_BUILD),
     * network initialization (CTX_INIT), simulation execution (CTX_EVENT),
     * simulation finalization (CTX_FINISH), network cleanup (CTX_CLEANUP),
     * or other (CTX_NONE).
     */
    int getSimulationStage() const  {return simulationStage;}

    /**
     * Returns the cModuleType object that was instantiated to set up
     * the current simulation model.
     */
    cModuleType *getNetworkType() const  {return networkType;}

    /**
     * INTERNAL USE ONLY. This method should NEVER be invoked from
     * simulation models, only from scheduler classes subclassed from
     * cScheduler.
     */
    void setSimTime(simtime_t time)  {currentSimtime = time;}

    /**
     * Returns the current simulation time. (It is also available via the
     * global simTime() function.) Between events it returns the time of
     * the last executed event.
     */
    simtime_t_cref getSimTime() const  {return currentSimtime;}

    /**
     * Returns the sequence number of current event. Between events it returns
     * the sequence number of the next event.
     */
    eventnumber_t getEventNumber() const  {return currentEventNumber;}

    /**
     * Returns the length of the initial warm-up period from the configuration.
     * Modules that compute and record scalar results manually (via recordScalar(),
     * recordStatistic(), etc.) should be implemented in a way that they ignore
     * observations generated during the warm-up period. cOutVector objects,
     * and results recorded via the signals mechanism automatically obey
     * the warm-up period and need not be modified. The warm-up period is useful
     * for steady-state simulations.
     */
    simtime_t_cref getWarmupPeriod() const  {return warmupPeriod;}

    /**
     * INTERNAL USE ONLY. Sets the warm-up period.
     */
    void setWarmupPeriod(simtime_t t)  {warmupPeriod = t;}
    //@}

    /** @name Scheduling and simulation execution. */
    //@{
    /**
     * Returns the likely next event in the simulation. Intended to be called
     * from a user interface library for displaying simulation status information.
     * Delegates to cScheduler::guessNextEvent().
     */
    cEvent *guessNextEvent();

    /**
     * Returns the module associated with the likely next event, or nullptr if
     * there is no such module. Based on guessNextEvent(); see further
     * comments there.
     */
    cSimpleModule *guessNextModule();

    /**
     * Returns the simulation time of the likely next event, or -1 if there is
     * none. Based on guessNextEvent(); see further comments there.
     */
    simtime_t guessNextSimtime();

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
    cEvent *takeNextEvent();

    /**
     * Undo for takeNextEvent(); see cScheduler's similar method for details.
     */
    void putBackEvent(cEvent *event);

    /**
     * Executes an event as part of the simulation. Also increments the event
     * number (see getEventNumber()).
     */
    void executeEvent(cEvent *event);

    /**
     * Invoke callRefreshDisplay() on the system module.
     */
    void callRefreshDisplay();

    /**
     * Switches to the given simple module's coroutine. This method is invoked
     * from executeEvent() for activity()-based modules.
     */
    void transferTo(cSimpleModule *module);

    /**
     * Switches to main coroutine.
     */
    void transferToMain();

    /**
     * Inserts the given event into the future events queue while assigning
     * the current event to its scheduling event. Used internally by
     * cSimpleModule::scheduleAt() and various other cSimpleModule methods.
     */
    void insertEvent(cEvent *event);

    /**
     * Sets the component (module or channel) in context. Used internally.
     */
    void setContext(cComponent *component);

    /**
     * Sets the context type for the context module. Used internally.
     */
    void setContextType(ContextType type)  {contextType = type;}

    /**
     * Sets global context. Used internally.
     */
    void setGlobalContext()  {contextComponent=nullptr; cOwnedObject::setOwningContext(&globalOwningContext);}

    /**
     * Returns the module whose activity() method is currently active.
     * Returns nullptr if no module is running, or the current module uses
     * handleMessage().
     */
    cSimpleModule *getActivityModule() const {return currentActivityModule;}

    /**
     * Returns the component (module or channel) currently in context.
     */
    cComponent *getContext() const {return contextComponent;}

    /**
     * Returns one of the CTX_BUILD, CTX_INITIALIZE, CTX_EVENT, CTX_FINISH constants,
     * depending on what the module in context is doing. In case of nested contexts
     * (e.g. when a module is dynamically created, initialized or manually
     * finalized during simulation), the innermost context type is returned.
     * The return value is only valid if getContextModule() != nullptr.
     */
    ContextType getContextType() const {return contextType;}

    /**
     * If the current context is a module, returns its pointer,
     * otherwise returns nullptr.
     */
    cModule *getContextModule() const;

    /**
     * Returns the module currently in context as a simple module.
     * If the module in context is not a simple module, returns nullptr.
     * This is a convenience function which simply calls getContextModule().
     */
    cSimpleModule *getContextSimpleModule() const;

    /**
     * Request the next handleMessage() or activity() to execute a debugger
     * interrupt. (If the program is not run under a debugger, that will
     * usually result in a crash.)
     */
    void requestTrapOnNextEvent() {trapOnNextEvent = true;}

    /**
     * Clear the previous requestTrapOnNextEvent() call.
     */
    void clearTrapOnNextEvent() {trapOnNextEvent = false;}

    /**
     * Returns true if there is a pending request to execute a debugger
     * interrupt on the next event.
     */
    bool isTrapOnNextEventRequested() const {return trapOnNextEvent;}
    //@}

    /** @name Miscellaneous. */
    //@{
    /**
     * This function is guaranteed to return a different integer every time
     * it is called (usually 0, 1, 2, ...). This method works with parallel
     * simulation as well, so it is recommended over incrementing a global
     * variable. Useful for generating unique network addresses, etc.
     */
    unsigned long getUniqueNumber();

    /**
     * Writes a snapshot of the given object and its children to the
     * textual snapshot file.
     * This method is called internally from cSimpleModule's snapshot().
     */
    void snapshot(cObject *obj, const char *label);

    /**
     * Returns the object used for fingerprint calculation. It returns nullptr
     * if no fingerprint is being calculated during this simulation run.
     */
    cFingerprintCalculator *getFingerprintCalculator() {return fingerprint;}

    /**
     * Installs a new fingerprint object, used for fingerprint calculation.
     */
    void setFingerprintCalculator(cFingerprintCalculator *fingerprint);
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
 * @ingroup Misc
 */
inline cSimulation *getSimulation()  {return cSimulation::getActiveSimulation();}

/**
 * @brief Returns the environment object for the currently active simulation.
 * This function never returns nullptr (not even during shutdown).
 *
 * @ingroup Misc
 */
inline cEnvir *getEnvir()  {return cSimulation::getActiveEnvir();}

}  // namespace omnetpp


#endif

