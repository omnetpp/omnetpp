//==========================================================================
//   CSIMULATION.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cSimulation  : simulation management class; only one instance
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CSIMULATION_H
#define __OMNETPP_CSIMULATION_H

#include "simkerneldefs.h"
#include "simtime_t.h"
#include "cmessageheap.h"
#include "ccomponent.h"
#include "cexception.h"

NAMESPACE_BEGIN

class cEvent;
class cMessage;
class cGate;
class cComponent;
class cModule;
class cChannel;
class cSimpleModule;
class cSimulation;
class cException;
class cScheduler;
class cParsimPartition;
class cNEDFileLoader;
class cHasher;
class cModuleType;
class cEnvir;
class cDefaultList;

SIM_API extern cDefaultList defaultList; // also in globals.h


/**
 * Simulation manager class.  cSimulation is the central class in \opp.
 * It stores the active simulation model, and provides methods for setting up,
 * running and finalizing simulations.
 *
 * Most cSimulation methods are not of interest for simulation model code,
 * they are used internally (e.g. by the user interface libraries (Envir,
 * Cmdenv, Tkenv) to set up and run simulations).
 *
 * Some methods which can be of interest when programming simple modules:
 * getUniqueNumber(), getModuleByPath(), getModule(), snapshot().
 *
 * @ingroup SimCore
 * @ingroup Internals
 */
class SIM_API cSimulation : public cNamedObject, noncopyable
{
    friend class cSimpleModule;
  private:
    // global variables
    static cSimulation *simPtr; // the active cSimulation instance
    static cEnvir *evPtr;       // the active cEnvir instance
    static cEnvir *staticEvPtr; // the environment to activate when simPtr becomes NULL

    // variables of the module vector
    int size;                 // size of vector
    int delta;                // if needed, grows by delta
    cComponent **vect;        // vector of modules/channels, vect[0] is not used
    int last_id;               // index of last used pos. in vect[]
#ifdef USE_OMNETPP4x_FINGERPRINTS
    int lastVersion4ModuleId;   // last used OMNeT++ V4.x compatible module ID
#endif

    // simulation vars
    cEnvir *ownEvPtr;         // the environment that belongs to this simulation object
    cModule *systemmodp;      // pointer to system (root) module
    cSimpleModule *activitymodp; // the module currently executing activity() (NULL if handleMessage() or in main)
    cComponent *contextmodp;  // component in context (or NULL)
    int contexttype;          // the innermost context type (one of CTX_BUILD, CTX_EVENT, CTX_INITIALIZE, CTX_FINISH)
    cModuleType *networktype; // network type
    cScheduler *schedulerp;   // event scheduler
    simtime_t warmup_period;  // warm-up period

    int simulationstage;      // simulation stage (one of CTX_NONE, CTX_BUILD, CTX_EVENT, CTX_INITIALIZE, CTX_FINISH or CTX_CLEANUP)
    simtime_t sim_time;       // simulation time (time of current event)
    eventnumber_t event_num;  // sequence number of current event

    cMessage *msg_for_activity; // helper variable to pass the received message into activity()
    cException *exception;    // helper variable to get exceptions back from activity()
    bool trap_on_next_event;  // when set, next handleMessage or activity() will execute debugger interrupt

    cHasher *hasherp;         // used for fingerprint calculation

  private:
    // internal
    void checkActive()  {if (getActiveSimulation()!=this) throw cRuntimeError(this, E_WRONGSIM);}

    // helper for executeEvent()
    void doMessageEvent(cMessage *msg, cSimpleModule *mod);

  public:
    // internal: FES
    cMessageHeap msgQueue;    // future messages (FES)
    cMessageHeap& getMessageQueue() {return msgQueue;}  // accessor for sim_std.msg

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
    virtual void forEachChild(cVisitor *v);

    /**
     * Redefined. (Reason: a C++ rule that overloaded virtual methods must be redefined together.)
     */
    virtual std::string getFullPath() const;
    //@}

    /** @name Accessing and switching the active simulation object */
    //@{
    /**
     * Returns the active simulation object. May be NULL.
     */
    static cSimulation *getActiveSimulation()  {return simPtr;}

    /**
     * Returns the environment object for the active simulation. Never returns NULL;
     * setActiveSimulation(NULL) will cause a static "do-nothing" instance to step in.
     */
    static cEnvir *getActiveEnvir()  {return evPtr;}

    /**
     * Activate the given simulation object, and its associated environment
     * object. NULL is also accepted; it will cause the static environment
     * object to step in (see getStaticEnvir()).
     */
    static void setActiveSimulation(cSimulation *sim);

    /**
     * Sets the environment object to use when there is no active simulation object.
     * The argument cannot be NULL.
     */
    static void setStaticEnvir(cEnvir *env);

    /**
     * Returns the environment object to use when there is no active simulation object.
     */
    static cEnvir *getStaticEnvir()  {return staticEvPtr;}

    /**
     * Returns the environment object associated with this simulation object.
     */
    cEnvir *getEnvir() const  {return ownEvPtr;}
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
    int getLastComponentId() const    {return last_id;}

    /**
     * Finds a module by its path. The path is a string of module names
     * separated by dots. Inclusion of the toplevel module's name in the
     * path is optional. Returns NULL if the module is not found.
     */
    cModule *getModuleByPath(const char *modulepath) const;

    /**
     * Looks up a component (module or channel) by ID. If the ID does not identify
     * a component (e.g. invalid ID or component already deleted), it returns NULL.
     */
    cComponent *getComponent(int id) const  {return id<0 || id>=size ? NULL : vect[id];}

    /**
     * Looks up a module by ID. If the ID does not identify a module (e.g. invalid ID,
     * module already deleted, or object is not a module), it returns NULL.
     */
    cModule *getModule(int id) const  {return id<0 || id>=size || !vect[id] ? NULL : vect[id]->isModule() ? (cModule *)vect[id] : NULL;}

    /**
     * Looks up a channel by ID. If the ID does not identify a channel (e.g. invalid ID,
     * channel already deleted, or object is not a channel), it returns NULL.
     */
    cChannel *getChannel(int id) const  {return id<0 || id>=size || !vect[id] ? NULL : vect[id]->isChannel() ? (cChannel *)vect[id] : NULL;}

    /**
     * Designates the system module, the top-level module in the model.
     */
    void setSystemModule(cModule *p);

    /**
     * Returns pointer to the system module, the top-level module in the model.
     */
    cModule *getSystemModule() const  {return systemmodp;}
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
     * Returns the number of files loaded.
     *
     * Note: doneLoadingNedFiles() must be called after the last
     * loadNedSourceFolder()/loadNedFile()/loadNedText() call.
     */
    static int loadNedSourceFolder(const char *foldername);

    /**
     * Load a single NED file. If the expected package is given (non-NULL),
     * it should match the package declaration inside the NED file.
     *
     * Note: doneLoadingNedFiles() must be called after the last
     * loadNedSourceFolder()/loadNedFile()/loadNedText() call.
     */
    static void loadNedFile(const char *nedfname, const char *expectedPackage=NULL, bool isXML=false);

    /**
     * Parses and loads the NED source code passed in the nedtext argument.
     * The name argument will be used as filename in error messages, and
     * and should be unique among the files loaded. If the expected package
     * is given (non-NULL), it should match the package declaration inside
     * the NED file.
     *
     * Note: doneLoadingNedFiles() must be called after the last
     * loadNedSourceFolder()/loadNedFile()/loadNedText() call.
     */
    static void loadNedText(const char *name, const char *nedtext, const char *expectedPackage=NULL, bool isXML=false);

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
     * Installs a scheduler object. This may only be called when no
     * network is set up. The cSimulation object will be responsible
     * for deallocating the scheduler object.
     */
    void setScheduler(cScheduler *scheduler);

    /**
     * Returns the scheduler object.
     */
    cScheduler *getScheduler() const  {return schedulerp;}

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
    int getSimulationStage() const  {return simulationstage;}

    /**
     * Returns the cModuleType object that was instantiated to set up
     * the current simulation model.
     */
    cModuleType *getNetworkType() const  {return networktype;}

    /**
     * INTERNAL USE ONLY. This method should NEVER be invoked from
     * simulation models, only from scheduler classes subclassed from
     * cScheduler.
     */
    void setSimTime(simtime_t time)  {sim_time = time;}

    /**
     * Returns the current simulation time. (It is also available via the
     * global simTime() function.) Between events it returns the time of
     * the last executed event.
     */
    simtime_t_cref getSimTime() const  {return sim_time;}

    /**
     * Returns the sequence number of current event. Between events it returns
     * the time of the next executed event.  FIXME this is inconsistent with getSimTime()!!!!! TODO also check what Tkenv and Cmdenv displays!!!!!
     */
    eventnumber_t getEventNumber() const  {return event_num;}

    /**
     * Returns the length of the initial warm-up period from the configuration.
     * Modules that compute and record scalar results manually (via recordScalar(),
     * recordStatistic(), etc.) should be implemented in a way that they ignore
     * observations generated during the warm-up period. cOutVector objects,
     * and results recorded via the signals mechanism automatically obey
     * the warm-up period and need not be modified. The warm-up period is useful
     * for steady-state simulations.
     */
    simtime_t_cref getWarmupPeriod() const  {return warmup_period;}

    /**
     * INTERNAL USE ONLY. Sets the warm-up period.
     */
    void setWarmupPeriod(simtime_t t)  {warmup_period = t;}
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
     * Returns the module associated with the likely next event, or NULL if
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
     * The scheduler function. Returns the next event from the FES, and
     * also advances the simulation time to the time of that event.
     *
     * If there is no more event (FES is empty), it throws cTerminationException.
     *
     * A NULL return value means that there is no error but execution
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
     * Switches to the given simple module's coroutine. This method is invoked
     * from executeEvent() for activity()-based modules.
     */
    void transferTo(cSimpleModule *p);

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
    void setContext(cComponent *p);

    /**
     * Sets the context type (see CTX_xxx constants). Used internally.
     */
    void setContextType(int ctxtype)  {contexttype = ctxtype;}

    /**
     * Sets global context. Used internally.
     */
    void setGlobalContext()  {contextmodp=NULL; cOwnedObject::setDefaultOwner(&defaultList);}

    /**
     * Returns the module whose activity() method is currently active.
     * Returns NULL if no module is running, or the current module uses
     * handleMessage().
     */
    cSimpleModule *getActivityModule() const {return activitymodp;}

    /**
     * Returns the component (module or channel) currently in context.
     */
    cComponent *getContext() const {return contextmodp;}

    /**
     * Returns value only valid if getContextModule()!=NULL. Returns one of:
     * CTX_BUILD, CTX_INITIALIZE, CTX_EVENT, CTX_FINISH, depending on
     * what the module in context is doing. In case of nested contexts
     * (e.g. when a module is dynamically created, initialized or manually
     * finalized during simulation), the innermost context type is returned.
     */
    int getContextType() const {return contexttype;}

    /**
     * If the current context is a module, returns its pointer,
     * otherwise returns NULL.
     */
    cModule *getContextModule() const;

    /**
     * Returns the module currently in context as a simple module.
     * If the module in context is not a simple module, returns NULL.
     * This is a convenience function which simply calls getContextModule().
     */
    cSimpleModule *getContextSimpleModule() const;

    /**
     * Request the next handleMessage() or activity() to execute a debugger
     * interrupt. (If the program is not run under a debugger, that will
     * usually result in a crash.)
     */
    void requestTrapOnNextEvent() {trap_on_next_event = true;}

    /**
     * Clear the previous requestTrapOnNextEvent() call.
     */
    void clearTrapOnNextEvent() {trap_on_next_event = false;}

    /**
     * Returns true if there is a pending request to execute a debugger
     * interrupt on the next event.
     */
    bool isTrapOnNextEventRequested() const {return trap_on_next_event;}
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
    bool snapshot(cObject *obj, const char *label);

    /**
     * Returns the object used for fingerprint calculation. It returns NULL
     * if no fingerprint is being calculated during this simulation run.
     */
    cHasher *getHasher() {return hasherp;}

    /**
     * Installs a new hasher object, used for fingerprint calculation.
     */
    void setHasher(cHasher *hasher);
    //@}
};

/**
 * Returns the current simulation time.
 *
 * @ingroup SimCore
 */
inline simtime_t simTime() {return cSimulation::getActiveSimulation()->getSimTime();}

/**
 * Returns the currently active simulation.
 *
 * @ingroup SimCore
 */
inline cSimulation *getSimulation()  {return cSimulation::getActiveSimulation();}

/**
 * Returns the environment object for the currently active simulation.
 *
 * @ingroup SimCore
 * @ingroup Envir
 */
inline cEnvir *getEnvir()  {return cSimulation::getActiveEnvir();}

// internal helper
inline cRNG *__contextComponentRNG(int k)
{
    cComponent *contextComponent = cSimulation::getActiveSimulation()->getContext();
    return contextComponent ? contextComponent->getRNG(k) : cSimulation::getActiveEnvir()->getRNG(k);
}


NAMESPACE_END


#endif

