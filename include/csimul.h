//==========================================================================
//   CSIMUL.H  -  header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cSimulation  : simulation management class; only one instance
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CSIMUL_H
#define __CSIMUL_H

#include "defs.h"

#include "util.h"
#include "errmsg.h"
#include "chead.h"
#include "cmsgheap.h"
#include "ccoroutine.h"
#include "coutvect.h" //FIXME

//=== classes mentioned:
class  cMessage;
class  cGate;
class  cModulePar;
class  cModule;
class  cSimpleModule;
class  cCompoundModule;
class  cNetMod;
class  cSimulation;
class  cNetworkType;
class  TModInspector;
class  cStatistic;
class  cException;

/**
 * Global simulation instance.
 *
 * @ingroup Internals
 */
SIM_API extern cSimulation simulation;

//==========================================================================

/**
 * Simulation manager class.  cSimulation is the central class in OMNeT++, and
 * it has only one instance, the global variable <tt>simulation</tt>.
 * It holds the modules, it manages setting up a simulation, running and
 * finishing it, etc.
 *
 * cSimulation should normally be of little interest to the simulation
 * programmer. Most of its methods are called by the user interface libraries
 * (Envir, Cmdenv, Tkenv) to set up and run simulations.
 *
 * @ingroup Internals
 */
class SIM_API cSimulation : public cObject
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
    cSimpleModule *runningmodp; // the currently executing module (NULL if in main)
    cModule *contextmodp;     // module in context (or NULL)
    cHead *locallistp;        // owner of newly created objects
    cHead locals;             // "global" local objects list

    cNetMod *netmodp;         // if runs distributed; communications
                              //      (network interface) module

    simtime_t sim_time;       // simulation time (time of current event)
    long event_num;           // sequence number of current event

    int netif_check_freq;     // (distributed execution:) frequency of processing
    int netif_check_cntr;     // msgs from other segments

    cNetworkType *networktype; // ptr to network creator object
    int run_number;            // which simulation run
    cSimpleModule *backtomod;  // used in cSimpleModule::wait/sendmsg
    cCoroutine runningmod_deleter; // used when a simple module deletes itself
    cException *exception;     // helper variable to get exceptions back from activity()
    int exception_type;        // helper variable, also for getting exceptions back from activity()

  public:
    cMessageHeap msgQueue;     // future messages (FES)

  public:
    /** @name Constructor, destructor. */
    //@{

    /**
     * Copy constructor is not supported:  This function
     * gives an error (throws cException) via operator= when called.
     */
    cSimulation(const cSimulation& r);

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
     * Dupping is not implemented for this class. This function
     * gives an error (throws cException) via operator= when called.
     */
    virtual cObject *dup() const  {return new cSimulation(*this);}

    /**
     * Call the passed function for each contained object.
     * See cObject for more details.
     */
    virtual void forEach(ForeachFunc f);

    /**
     * Writes textual information about this object to the stream.
     * See cObject for more details.
     */
    virtual void writeContents(ostream& os);

    /**
     * Redefined. (Reason: a C++ rule that overloaded virtual methods must be redefined together.)
     */
    virtual const char *fullPath() const;

    /**
     * Returns the name of the simulation object, "simulation".
     */
    virtual const char *fullPath(char *buffer, int bufsize) const;

    /**
     * Assignment is not supported by this class: this method throws a cException when called.
     */
    cSimulation& operator=(const cSimulation&)  {copyNotSupported();return *this;}
    //@}

    // Internal: things that cannot be done from the constructor
    // (because it is called before main()).
    void init();

    /** @name Accessing modules. */
    //@{

    /**
     * Adds a new module and return its ID.
     */
    int addModule(cModule *mod);

    /**
     * Deletes a module identified by its ID.
     */
    void deleteModule(int id);

    /**
     * Returns highest used module ID.
     */
    int lastModuleId() const    {return last_id;}

    /**
     * Finds a module by its path.
     */
    cModule *moduleByPath(const char *modulepath) const;

    /**
     * Looks up a module by ID.
     */
    cModule *module(int id) const  {return id>=0 && id<size ? vect[id] : NULL;}

    /**
     * DEPRECATED because it might return null reference; use module(int) instead.
     *
     * Same as module(int), only this returns reference instead of pointer.
     */
    cModule& operator[](int id) const {return id>=0 && id<size ? *vect[id] : *(cModule *)NULL;}

    /**
     * Sets the system module.
     */
    void setSystemModule(cModule *p)  {systemmodp = p;}

    /**
     * Returns pointer to the system module.
     */
    cModule *systemModule() const  {return systemmodp;}
    //@}

    /** @name Setting up and finishing a simulation run. */
    //@{

    /**
     * Sets network interface module.
     */
    void setNetInterface(cNetMod *netif);

    /**
     * Returns network interface module.
     */
    cNetMod *netInterface() const  {return netmodp;}

    /**
     * Builds a new network and initializes it. With distributed simulation,
     * also sets up network on other segments.
     */
    void setupNetwork(cNetworkType *net,int run_num);

    /**
     * Should be called after setupNetwork(), just before the first doOneEvent() call.
     * The initialize() methods of modules are called here.
     * With distributed simulation, also tells other segments that to begin
     * executing the simulation.
     */
    void startRun();

    /**
     * Recursively calls finish() on the modules of the network.
     * This method simply invokes callfinish() on the system module.
     */
    void callFinish();

    /**
     * Should be called at the end of a simulation run.
     * With distributed simulation, this signals other segments that they should stop
     * execution.
     */
    void endRun();

    /**
     * Cleans up the network currently set up. With distributed simulation,
     * also notifies other segments that they should clean up their networks.
     */
    void deleteNetwork();

    /**
     * Used with distributed simulation, sets the frequency of checking
     * messages arriving from other segments. This setting is mostly
     * useful for performance tuning. The meaning of the value is:
     * "check the network interface after every f local events."
     */
    void setNetIfCheckFreq(int f)     {netif_check_freq=f;}
    //@}

    /** @name Information about the current simulation run. */
    //@{

    /**
     * Returns the cNetworkType object that was used to set up
     * the current simulation model.
     */
    cNetworkType *networkType() const     {return networktype;}

    /**
     * Returns the current run number. A run is the execution of a
     * model with a given set of parameter settings. Runs can be defined
     * in omnetpp.ini.
     */
    // FIXME: does run number really belong to the simulation kernel??? why not in Envir?
    int  runNumber() const           {return run_number;}

    /**
     * Returns current simulation time.
     */
    simtime_t simTime() const       {return sim_time;}

    /**
     * Returns sequence number of current event.
     */
    long eventNumber() const         {return event_num;}
    //@}

    /** @name Scheduling and context switching during simulation. */
    //@{

    /**
     * The scheduler function. Returns the module to which the
     * next event (lowest timestamp event in the FES) belongs.
     * If the FES is empty, returns NULL.
     */
    cSimpleModule *selectNextModule();

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
     * Sets the module in context. Used internally.
     */
    void setContextModule(cModule *p);

    /**
     * Sets global context. Used internally.
     */
    void setGlobalContext()  {contextmodp=NULL;locallistp=&locals;}

    /**
     * Sets the 'locals' object for the current context.
     */
    void setLocalList(cHead *p)  {locallistp=p;}

    /**
     * Returns the currently executing simple module.
     */
    cSimpleModule *runningModule() const {return runningmodp;}

    /**
     * Returns the module currently in context.
     */
    cModule *contextModule() const {return contextmodp;}

    /**
     * Returns the module currently in context as a simple module.
     * If the module in context is not a simple module, returns NULL.
     * This is a convenience function which simply calls contextModule().
     */
    cSimpleModule *contextSimpleModule() const;

    /**
     * Returns the currently active 'locals' object. This object is usually the
     * <tt>locals</tt> data member of the module in context, or the global
     * <tt>locals</tt> object if we are in global context. This facility
     * is used internally to manage ownership of user objects created within
     * simple modules.
     */
    cHead *localList() {return locallistp==NULL?(&locals):locallistp;}
    //@}

    /** @name Snapshots. */
    //@{

    /**
     * Writes a snapshot of the given object and its children to the
     * textual snapshot file.
     * This method is called internally from cSimpleModule's snapshot().
     */
    bool snapshot(cObject *obj, const char *label);
    //@}
};

//==========================================================================
//=== operator new used by the NEW() macro:
class ___nosuchclass;
void *operator new(size_t m,___nosuchclass *);

#endif

