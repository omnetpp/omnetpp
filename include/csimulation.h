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
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CSIMULATION_H
#define __CSIMULATION_H

#include "simkerneldefs.h"
#include "util.h"
#include "globals.h"
#include "cmessageheap.h"
#include "ccoroutine.h"

NAMESPACE_BEGIN

//=== classes mentioned:
class  cMessage;
class  cGate;
class  cModule;
class  cSimpleModule;
class  cCompoundModule;
class  cSimulation;
class  cException;
class  cScheduler;
class  cParsimPartition;
class  cNEDFileLoader;
class  cHasher;


/**
 * Global simulation instance.
 *
 * @ingroup Internals
 */
SIM_API extern cSimulation simulation;


/**
 * Simulation manager class.  cSimulation is the central class in \opp, and
 * it has only one instance, the global variable <tt>simulation</tt>.
 * It holds the modules, manages setting up a simulation, running and
 * finishing it, etc.
 *
 * Most cSimulation methods are used internally (e.g. by the user interface
 * libraries (Envir, Cmdenv, Tkenv) to set up and run simulations).
 *
 * Some methods which can be of interest when programming simple modules:
 * getUniqueNumber(), moduleByPath(), module(), snapshot().
 *
 * @ingroup SimCore
 * @ingroup Internals
 */
class SIM_API cSimulation : public cNoncopyableOwnedObject
{
    friend class cSimpleModule;

  private:
    // variables of the module vector
    int size;                 // size of vector
    int delta;                // if needed, grows by delta
    cModule **vect;           // vector of modules, vect[0] is not used
    int last_id;              // index of last used pos. in vect[]

    // simulation global vars
    cModule *systemmodp;      // pointer to system module
    cSimpleModule *activitymodp; // the module currently executing activity() (NULL if handleMessage() or in main)
    cComponent *contextmodp;  // component in context (or NULL)
    int contexttype;          // CTX_BUILD, CTX_EVENT, CTX_INITIALIZE or CTX_FINISH
    cModuleType *networktype; // network type
    cScheduler *schedulerp;   // event scheduler

    simtime_t sim_time;       // simulation time (time of current event)
    long event_num;           // sequence number of current event

    cMessage *msg_for_activity; // helper variable to pass the received message into activity()
    cException *exception;    // helper variable to get exceptions back from activity()

    cHasher *hasherp;         // used for fingerprint calculation

  public:
    // internal: FES
    cMessageHeap msgQueue;     // future messages (FES)
    cMessageHeap& messageQueue() {return msgQueue;}  // accessor for sim_std.msg

    // internal: things that cannot be done from the constructor of global object
    void init();

    // internal: complements init().
    void shutdown();

  public:
    /** @name Constructor, destructor. */
    //@{
    /**
     * Constructor.
     */
    explicit cSimulation(const char *name);

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
    virtual std::string fullPath() const;
    //@}

    /** @name Accessing modules. */
    //@{

    /**
     * Registers the module in cSimulation and assigns a module Id. It is called
     * internally during module creation. The Id of a deleted module is not
     * issued again to another module, because we want module Ids to be
     * unique during the whole simulation.
     */
    int registerModule(cModule *mod);

    /**
     * Deregisters the module from cSimulation. It is called internally from cModule
     * destructor.
     */
    void deregisterModule(cModule *mod);

    /**
     * Returns highest used module ID.
     */
    int lastModuleId() const    {return last_id;}

    /**
     * Finds a module by its path. Inclusion of the name of the toplevel module
     * in the path is optional. Returns NULL if not found.
     */
    cModule *moduleByPath(const char *modulepath) const;

    /**
     * Looks up a module by ID. If the module doesn't exist, returns NULL.
     */
    cModule *module(int id) const  {return id>=0 && id<size ? vect[id] : NULL;}

    /**
     * DEPRECATED because it might return null reference; use module(int) instead.
     *
     * Same as module(int), only this returns reference instead of pointer.
     */
    _OPPDEPRECATED cModule& operator[](int id) const  {return id>=0 && id<size ? *vect[id] : *(cModule *)NULL;}

    /**
     * Designates the system module, the top-level module in the model.
     */
    void setSystemModule(cModule *p);

    /**
     * Returns pointer to the system module, the top-level module in the model.
     */
    cModule *systemModule() const  {return systemmodp;}
    //@}

    /** @name Setting up and finishing a simulation run. */
    //@{

    /**
     * Installs a scheduler object. It doesn't need to be deleted in the
     * destructor, that will be done externally.
     */
    void setScheduler(cScheduler *sched);

    /**
     * Returns the scheduler object.
     */
    cScheduler *scheduler() const  {return schedulerp;}

    /**
     * Load all NED files from a NED source folder. This involves visiting
     * each subdirectory, and loading all "*.ned" files from there.
     * The given folder is assumed to be the root of the NED package hierarchy.
     * The return value is the number of NED files loaded.
     *
     * These functions delegate to the netbuilder part of the simulation kernel,
     * and they are present so that cEnvir and other libs outside the simkernel
     * don't need to directly depend on nedxml or netbuilder classes, and
     * conditional compilation (#ifdef WITH_NETBUILDER) can be limited to the
     * simkernel.
     */
    int loadNedSourceFolder(const char *folder);

    /**
     * Should be called after the last loadNedSourceFolder() call. This method
     * warns if there are NED components which could not be fully resolved
     * due to missing super types or interfaces.
     */
    void doneLoadingNedFiles();

    /**
     * For loading additional NED files after doneLoadingNedFiles().
     * This method resolves dependencies (base types, etc) immediately,
     * and throws an error if something is missing. (So this method is
     * not useful if two or more NED files mutually depend on each other.)
     * If the expected package is given (non-NULL), it should match the
     * package declaration inside the NED file.
     *
     * These functions delegate to the netbuilder part of the simulation kernel,
     * and they are present so that cEnvir and other libs outside the simkernel
     * don't need to directly depend on nedxml or netbuilder classes, and
     * conditional compilation (#ifdef WITH_NETBUILDER) can be limited to the
     * simkernel.
     */
    void loadNedFile(const char *nedfile, const char *expectedPackage=NULL, bool isXML=false);

    /**
     * Returns the NED package that corresponds to the given folder. Returns ""
     * for the default package, and "-" if the folder is outside all NED folders.
     */
    std::string getNedPackageForFolder(const char *folder) const;

    /**
     * Builds a new network.
     */
    void setupNetwork(cModuleType *networkType);

    /**
     * Should be called after setupNetwork(), but before the first
     * doOneEvent() call. Includes initialization of the modules,
     * that is, invokes callInitialize() on the system module.
     */
    void startRun();

    /**
     * Recursively calls finish() on the modules of the network.
     * This method simply invokes callfinish() on the system module.
     */
    void callFinish();

    /**
     * Should be called at the end of a simulation run.
     */
    void endRun();

    /**
     * Cleans up the network currently set up. This involves deleting
     * all modules and deleting the messages in the scheduled-event list.
     */
    void deleteNetwork();
    //@}

    /** @name Information about the current simulation run. */
    //@{
    /**
     * Returns the cModuleType object that was instantiated to set up
     * the current simulation model.
     */
    cModuleType *networkType() const  {return networktype;}

    /**
     * WARNING: INTERNAL USE ONLY. This method should NEVER be invoked from
     * simulation models, only from scheduler classes subclassed from
     * cScheduler.
     */
    void setSimTime(simtime_t time)  {sim_time = time;}

    /**
     * Returns current simulation time.
     */
    simtime_t simTime() const  {return sim_time;}

    /**
     * Returns sequence number of current event.
     */
    long eventNumber() const  {return event_num;}
    //@}

    /** @name Scheduling and context switching during simulation. */
    //@{

    /**
     * The scheduler function. Returns the module to which the
     * next event (lowest timestamp event in the FES) belongs.
     *
     * If there's no more event (FES is empty), it throws cTerminationException.
     *
     * A NULL return value means that there's no error but execution
     * was stopped by the user (e.g. with STOP button on the GUI)
     * while selectNextModule() --or rather, the installed cScheduler object--
     * was waiting for external synchronization.
     */
    cSimpleModule *selectNextModule();

    /**
     * To be called between events from the environment of the simulation
     * (e.g. from Tkenv), this function returns a pointer to the event
     * at the head of the FES. It is only guaranteed to be the next event
     * with sequential simulation; with parallel, distributed or real-time
     * simulation there might be another event coming from other processes
     * with a yet smaller timestamp.
     *
     * This method is careful not to change anything. It never throws
     * an exception, and especially, it does NOT invoke the scheduler
     * (see cScheduler) because e.g. its parallel simulation incarnations
     * might do subtle things to keep events synchronized in various
     * partitions of the parallel simulation.
     */
    cMessage *guessNextEvent();

    /**
     * To be called between events from the environment of the simulation
     * (e.g. from Tkenv), this function returns the module associated
     * with the event at the head of the FES. It returns NULL if the
     * FES is empty, there is no module associated with the event, or
     * the module has already finished.
     *
     * Based on guessNextEvent(); see further comments there.
     */
    cSimpleModule *guessNextModule();

    /**
     * To be called between events from the environment of the simulation
     * (e.g. Tkenv), this function returns the simulation time of the event
     * at the head of the FES. In contrast, simTime() returns the time of the
     * last executed (or currently executing) event. Returns a negative value
     * if the FES is empty.
     *
     * Based on guessNextEvent(); see further comments there.
     */
    simtime_t guessNextSimtime();

//FIXME change these functions to work with events instead of modules?

    /**
     * Executes one event. The argument should be the module
     * returned by selectNextModule(); that is, the module
     * to which the next event (lowest timestamp event in
     * the FES) belongs. Also increments the event number
     * (returned by eventNumber()).
     */
    void doOneEvent(cSimpleModule *m);

    /**
     * Switches to simple module's coroutine. This method is invoked
     * from doOneEvent() for activity()-based modules.
     */
    void transferTo(cSimpleModule *p);

    /**
     * Switches to main coroutine.
     */
    void transferToMain();

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
    cSimpleModule *activityModule() const {return activitymodp;}

    /**
     * Returns the component (module or channel) currently in context.
     */
    cComponent *context() const {return contextmodp;}

    /**      XXX refine comment: maybe valid as well if it's a channel in context?
     * Returns value only valid if contextModule()!=NULL. Returns one of:
     * CTX_BUILD, CTX_INITIALIZE, CTX_EVENT, CTX_FINISH depending on
     * what the module in context is doing.
     */
    int contextType() const {return contexttype;}

    /**
     * If the current context is a module, returns its pointer,
     * otherwise returns NULL.
     */
    cModule *contextModule() const;

    /**
     * Returns the module currently in context as a simple module.
     * If the module in context is not a simple module, returns NULL.
     * This is a convenience function which simply calls contextModule().
     */
    cSimpleModule *contextSimpleModule() const;
    //@}

    /** @name Miscellaneous. */
    //@{
    /**
     * This function is guaranteed to return a different integer every time
     * it is called (usually 0, 1, 2, ...). This method is recommended over
     * incrementing a global variable because it works also with distributed
     * execution. Useful for generating unique network addresses, etc.
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
    cHasher *hasher() {return hasherp;}

    /**
     * Installs a new hasher object, used for fingerprint calculation.
     */
    void setHasher(cHasher *hasher);
    //@}

    /**
     * Inserts the given message into the future events queue while assigning
     * the current event to its scheduling event.   FIXME move it into the proper function group
     */
    void insertMsg(cMessage *msg);
};

NAMESPACE_END


#endif

