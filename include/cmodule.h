//==========================================================================
//   CMODULE.H  -  header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cModule        : common base for cCompoundModule and cSimpleModule
//    cSimpleModule  : base for simple module objects
//    cCompoundModule: compound module
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMODULE_H
#define __CMODULE_H

#include "defs.h"

#include <time.h>     // time_t, clock_t in cSimulation
#include "cobject.h"
#include "ccoroutine.h"
#include "chead.h"
#include "carray.h"
#include "cqueue.h"
#include "cgate.h"
#include "csimul.h"

//=== module state codes
enum {
       sENDED,    // module terminated
       sREADY     // module is active
};

//=== display string selector (DEPRECATED)
enum {
       dispSUBMOD=0,        // display string: "as submodule"
       dispENCLOSINGMOD=1,  // display string: "as enclosing module"
       dispNUMTYPES         // this one must always be the last element
};

//=== dynamic allocation in module functions
#define opp_new             new((___nosuchclass *)NULL)
#define opp_delete          memFree((void *&)x)

//=== classes mentioned/declared here:
class  cMessage;
class  cGate;
class  cModulePar;
class  cModule;
class  cSimpleModule;
class  cCompoundModule;
class  cNetMod;
class  cSimulation;
class  cNetworkType;

/**
 * @name Connecting gates.
 * @ingroup Functions
 */
//@{

/**
 * DEPRECATED: use cGate::connectTo() instead.
 *
 * Connects two gates.
 */
SIM_API void connect(cModule *frm, int frg,
                     cLinkType *linkp,
                     cModule *tom, int tog);

/**
 * DEPRECATED: use cGate::connectTo() instead.
 *
 * Connects two gates.
 */
SIM_API void connect(cModule *frm, int frg,
                     cPar *delayp, cPar *errorp, cPar *dataratep,
                     cModule *tom, int tog);
//@}


//=== operator new used by the NEW() macro:
class ___nosuchclass;
void *operator new(size_t m,___nosuchclass *);


/**
 * Prototype for callback functions that are used to notify graphical user
 * interfaces about display string changes.
 *
 * Args: (sub)module which changed; immediate refresh wanted or not; inspector's data
 *
 * @ingroup EnumsTypes
 */
typedef void (*DisplayStringNotifyFunc)(cModule*,bool,void*);

//==========================================================================

/**
 * Common base for cSimpleModule and cCompoundModule.
 * Provides gates, parameters, error handling, etc.
 *
 * NOTE: dup() cannot be use with modules.
 * Use <tt>mod->moduleType()->create()</tt>/<tt>buildInside()</tt> instead.
 *
 * @ingroup SimCore
 */
class SIM_API cModule : public cObject
{
    friend class cGate;
    friend class cSimulation;

  public:
    static bool pause_in_sendmsg; // if true, split send() with transferToMain()

  protected:
    mutable char *fullname; // buffer to store full name of object
    cModuleType *mod_type;  // type of this module
    int mod_id;             // id (subscript into cSimulation)
    cModule *parentmodp;    // pointer to parent module

  public:
    // The following members are only made public for use by the inspector
    // classes. Do not use them directly from simple modules.
    cArray gatev;           // vector of gates
    cArray paramv;          // vector of parameters
    cArray machinev;        // NET: list of machines --VA
    cHead members;          // list of data members of derived classes

  protected:
    int  idx;               // index if module vector, 0 otherwise
    int  vectsize;          // vector size, -1 if not a vector

    opp_string dispstr;     // display string as submodule
    opp_string parentdispstr; // display string as parent (enclosing) module

    DisplayStringNotifyFunc notify_inspector;
    void *data_for_inspector;

  protected:
    // internal use
    virtual void arrived(cMessage *msg,int n,simtime_t t) = 0;

  protected:
    /** @name Initialization and finish hooks.
     *
     * Initialize and finish functions should (may) be provided by the user,
     * to perform special tasks at the beginning and the end of the simulation.
     * The functions are made protected because they are supposed
     * to be called only via callInitialize() and callFinish().
     *
     * The initialization process was designed to support multi-stage
     * initialization of compound modules (i.e. initialization in several
     * 'waves'). (Calling the initialize() function of a simple module is
     * hence a special case). The initialization process is performed
     * on a module like this. First, the number of necessary initialization
     * stages is determined by calling numInitStages(), then initialize(stage)
     * is called with <tt>0,1,...numstages-1</tt> as argument. The default
     * implementation of numInitStages() and initialize(stage) provided here
     * defaults to single-stage initialization, that is, numInitStages()
     * returns 1 and initialize(stage) simply calls initialize() if stage is 0.
     *
     * All initialization and finish functions are redefined in cCompoundModule
     * to recursively traverse all submodules.
     */
    //@{

    /**
     * Should be refined in subclasses representing compound modules
     * to build submodule and internal connections of this module.
     * This default implementation does nothing.
     *
     * This method should not be called directly, only via buildInside();
     * this method is declared protected to enforce this.
     *
     * @see buildInside()
     */
    virtual void doBuildInside() {}

    /**
     * Multi-stage initialization hook. This default implementation does
     * single-stage init, that is, calls initialize() if stage is 0.
     */
    virtual void initialize(int stage) {if(stage==0) initialize();}

    /**
     * Multi-stage initialization hook, should be redefined to return the
     * number of initialization stages required. This default implementation
     * does single-stage init, that is, returns 1.
     */
    virtual int  numInitStages() const  {return 1;}

    /**
     * Single-stage initialization hook. This default implementation
     * does nothing.
     */
    virtual void initialize();

    /**
     * Finish hook. finish() is called after end of simulation, if it
     * terminated without error. This default implementation does nothing.
     */
    virtual void finish();
    //@}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cModule(const cModule& mod);

    /**
     * Constructor. Note that module objects should not be created directly,
     * only via their cModuleType objects. cModuleType::create() will do
     * all housekeeping tasks associated with module creation (assigning
     * an ID to the module, inserting it into the global <code>simulation</code>
     * object (see cSimulation), etc.).
     */
    cModule(const char *name, cModule *parentmod);

    /**
     * Destructor.
     */
    virtual ~cModule();

    /**
     * Assignment operator. The name member doesn't get copied;
     * see cObject's operator=() for more details.
     */
    cModule& operator=(const cModule& mod);
    //@}

    /** @name Redefined cObject functions. */
    //@{

    /* No dup() because this is an abstract class. */

    /**
     * Call the passed function for each contained object.
     * See cObject for more details.
     */
    virtual void forEach(ForeachFunc f);

    /**
     * Returns the full name of the module, which is name() plus the
     * index in square brackets (e.g. "module[4]"). Redefined to add the
     * index.
     */
    virtual const char *fullName() const;

    /**
     * Redefined. (Reason: a C++ rule that overloaded virtual methods must be redefined together.)
     */
    virtual const char *fullPath() const;

    /**
     * Returns the full path name of the module. Example: <tt>"mynet.node[12].gen"</tt>.
     * The original fullPath() was redefined in order to hide the global cSimulation
     * instance in the path name (it would add "simulation." to the beginning of the full path).
     */
    virtual const char *fullPath(char *buffer, int bufsize) const;
    //@}

    /** @name Setting up the module. */
    //@{

    /**
     * Sets the module ID. Called internally as part of the module creation process.
     */
    // FIXME why not private? cSimulation is friend anyway.
    virtual void setId(int n);

    /**
     * Sets module index within vector (if module is part of
     * a module vector). Called internally as part of the module creation process.
     */
    void setIndex(int i, int n);

    /**
     * Sets associated cModuleType for the module. Called internally
     * as part of the module creation process.
     */
    // FIXME why not private..?
    void setModuleType(cModuleType *mtype);

    /**
     * Adds a gate. The type character tp can be <tt>'I'</tt> or <tt>'O'</tt> for input
     * and output, respectively.
     */
    cGate *addGate(const char *s, char tp);

    /**
     * Sets gate vector size.
     */
    void setGateSize(const char *s, int size);

    /**
     * Adds a parameter to the module.
     */
    cPar *addPar(const char *s);

    /**
     * Adds a machine parameter to the module.
     */
    void addMachinePar(const char *pnam);

    /**
     * Sets value of a machine parameter.
     */
    void setMachinePar(const char *pnam, const char *val);

    /**
     * In compound modules, this method can be called to build submodules
     * and internal connections after module creation. This method is a
     * wrapper around doBuildInside().
     *
     * It does the following:
     *
     *    - 1. checks if module parameters and gates conform to the module interface
     *    (cModuleInterface object, generated from NED declaration of module)
     *
     *    - 2. calls doBuildInside(), switching to the context of this module
     *    for the duration of the call (using simulation.setContextModule()).
     *
     * Note: semantic has changed -- in OMNeT++ 2.2 and earlier versions,
     * doBuildInside() did not exist, its role was fulfilled by this method.
     * After 2.2, the return value of this method was changed from void
     * to int deliberately to cause compile error in older code, in order to
     * call attention to the semantics change. (Returned value can be ignored.)
     *
     * @see doBuildInside()
     */
    virtual int buildInside();
    //@}

    /** @name Information about the module itself. */
    //@{

    /**
     * Returns this is a simple or compound module. Pure virtual function,
     * redefined in cSimpleModule to return true and in cCompoundModule
     * to return false.
     */
    virtual bool isSimple() const = 0;

    /**
     * Returns the cModuleType object associated with this module type.
     */
    cModuleType *moduleType() const  {return mod_type;}

    /**
     * Returns the module ID. It is actually the index of the module
     * in the module vector within the cSimulation simulation object.
     * Module IDs are guaranteed to be unique during a simulation run
     * (that is, IDs of deleted modules are not given out to newly created
     * modules).
     */
    int id() const               {return mod_id;}

    /**
     * Returns the module's parent module. For the system module, it returns
     * <tt>NULL</tt>.
     */
    cModule *parentModule() const     {return parentmodp;}

    /**
     * Used with parallel execution: determines if the module is on the
     * local machine. See the user manual for more info.
     */
    bool isOnLocalMachine() const;

    /**
     * Returns true if this module is in a module vector.
     */
    bool isVector() const       {return vectsize>=0;}

    /**
     * Returns the index of the module if it is in a module vector, otherwise 0.
     */
    int index() const           {return idx;}

    /**
     * Returns the size of the module vector the module is in, or 1.
     */
    int size() const            {return vectsize<0?1:vectsize;}
    //@}

    /** @name Submodule access. */
    //@{

    /**
     * Finds an immediate submodule with the given name and (optional)
     * index, and returns its module ID. If the submodule was not found,
     * returns -1.
     */
    int findSubmodule(const char *submodname, int idx=-1);

    /**
     * Finds an immediate submodule with the given name and (optional)
     * index, and returns its pointer. If the submodule was not found,
     * returns NULL.
     */
    cModule *submodule(const char *submodname, int idx=-1);

    /**
     * Finds a submodule potentially several levels deeper, given with
     * its relative path from this module. (The path is a string of module
     * full names separated by dots). If the submodule was not found,
     * returns <tt>NULL</tt>.
     */
    cModule *moduleByRelativePath(const char *path);
    //@}

    /** @name Gates. */
    //@{

    /**
     * Returns the total size of the gate array. Since the array may contain
     * unused elements, the number of actual gates used might be less.
     */
    int gates() const {return gatev.items();}

    /**
     * Returns a gate by its ID. Returns NULL if the gate does not exist.
     */
    cGate *gate(int g) {return (cGate*)gatev[g];}

    /**
     * Returns a gate by its ID. Returns NULL if the gate does not exist.
     */
    const cGate *gate(int g) const {return (const cGate*)gatev[g];}

    /**
     * Looks up a gate by its name and index. Returns NULL if the gate does not exist.
     */
    cGate *gate(const char *gatename,int sn=-1);

    /**
     * Looks up a gate by its name and index. Returns NULL if the gate does not exist.
     */
    const cGate *gate(const char *gatename,int sn=-1) const;

    /**
     * Returns the ID of the gate specified by name and index.
     * Returns -1 if the gate doesn't exist.
     */
    int findGate(const char *gatename, int sn=-1) const;

    /**
     * Checks if a gate exists.
     */
    bool hasGate(const char *gatename, int sn=-1) const {return findGate(gatename,sn)>=0;}

    /**
     * For compound modules, it checks if all gates are connected inside
     * the module (it returns <tt>true</tt> if they are OK); for simple
     * modules, it returns <tt>true</tt>. This function is usually called from
     * from NEDC-generated code.
     */
    bool checkInternalConnections() const;
    //@}

    /** @name Parameters. */
    //@{

    /**
     * Returns total number of the module's parameters.
     */
    int params() const {return paramv.items();}

    /**
     * Returns reference to the module parameter identified with its
     * index p. Returns <tt>*NULL</tt> if the object doesn't exist.
     */
    cPar& par(int p);

    /**
     * Returns reference to the module parameter specified with its name.
     * Returns <tt>*NULL</tt> if the object doesn't exist.
     */
    cPar& par(const char *parname);

    /**
     * Returns index of the module parameter specified with its name.
     * Returns -1 if the object doesn't exist.
     */
    int findPar(const char *parname) const;

    /**
     * Searches for the parameter in the parent modules, up to the system
     * module. It the parameter is not found, throws cException.
     */
    cPar& ancestorPar(const char *parname);

    /**
     * Check if a parameter exists.
     */
    bool hasPar(const char *s) const {return findPar(s)>=0;}
    //@}

    /** @name Machine parameters (used by parallel execution). */
    //@{

    /**
     * Returns the number of machine parameters.
     */
    int machinePars() const  {return machinev.items();}    // NET

    /**
     * Returns the value of the ith machine parameter.
     */
    const char *machinePar(int i);                   // NET

    /**
     * Returns a machine parameter by its name.
     */
    const char *machinePar(const char *machinename); // NET
    //@}

    /** @name Interface for calling initialize()/finish().
     * Those functions may not be called directly, only via
     * callInitialize() and callFinish() provided here.
     */
    //@{

    /**
     * Interface for calling initialize() from outside.
     * Implements full multi-stage init for this module and its submodules.
     */
    virtual void callInitialize();

    /**
     * Interface for calling initialize() from outside. It does a single stage
     * of initialization, and returns <tt>true</tt> if more stages are required.
     */
    virtual bool callInitialize(int stage) = 0;

    /**
     * Interface for calling finish() from outside.
     */
    virtual void callFinish() = 0;
    //@}

    /** @name Dynamic module creation. */
    //@{

    /**
     * Pure virtual function; it is redefined in both cCompoundModule
     * and cSimpleModule. It creates starting message for dynamically
     * created module (or recursively for its submodules). See the user
     * manual for explanation how to use dynamically created modules.
     */
    virtual void scheduleStart(simtime_t t) = 0;

    /**
     * Deletes a dynamically created module and recursively all its submodules.
     * This is a pure virtual function; it is redefined in both cCompoundModule
     * and cSimpleModule.
     */
    virtual void deleteModule() = 0;
    //@}

    /** @name Display strings. */
    //@{

    /**
     * Returns the first display string.  This display string
     * defines presentation when the module is displayed as a submodule
     * in a compound module graphics.
     */
    const char *displayString();

    /**
     * Sets the first display string for this module. This display string
     * defines presentation when the module is displayed as a submodule
     * in a compound module graphics.
     *
     * The immediate flag selects whether the change should become effective
     * right now or later (after finishing the current event).
     *
     * If several display string changes are going to be done within one event,
     * then immediate=false is useful because it reduces the number of necessary
     * redraws. Immediate=false also uses less stack. But its drawback is that
     * a setDisplayString() followed by a send() would actually be displayed
     * in reverse order (message animation first), because message animations
     * are performed immediately (actually within the send() call).
     */
    void setDisplayString(const char *dispstr, bool immediate=true);

    /**
     * Returns the second display string. This display string
     * is used when this module is a compound module whose internals are
     * being displayed in a window.
     */
    const char *displayStringAsParent();

    /**
     * Sets the second display string for this module. This display string
     * is used when this module is a compound module whose internals are
     * being displayed in a window.
     *
     * The immediate flag selects whether the change should become effective
     * right now or later (after finishing the current event).
     *
     * If several display string changes are going to be done within one event,
     * then immediate=false is useful because it reduces the number of necessary
     * redraws. Immediate=false also uses less stack. But its drawback is that
     * a setDisplayString() followed by a send() would actually be displayed
     * in reverse order (message animation first), because message animations
     * are performed immediately (actually within the send() call).
     */
    void setDisplayStringAsParent(const char *dispstr, bool immediate=true);

    /**
     * DEPRECATED. Use displayString() (without argument) or
     * displayStringAsLocal() instead.
     */
    const char *displayString(int type);

    /**
     * DEPRECATED. Use setDisplayString(dispstr,immediate) (without the type
     * argument) or displayStringAsLocal(dispstr,immediate) instead.
     */
    void setDisplayString(int type, const char *dispstr, bool immediate=true);

    /**
     * Registers a notification function to be called when the display string
     * changes. This function is used by graphical user interfaces (e.g. Tkenv)
     * to get notified when the network graphics needs redraw due to a display
     * string change.
     */
    void setDisplayStringNotify(DisplayStringNotifyFunc notify_func, void *data);
    //@}
};

//==========================================================================

//
// internal struct: block on module function's heap
//
struct sBlock
{
    sBlock *next;
    sBlock *prev;
    cSimpleModule *mod;
};

//--------------------------------------------------------------------------

/**
 * cSimpleModule is the base class for all simple module classes.
 * cSimpleModule, although stuffed with simulation-related functionality,
 * doesn't do anything useful by itself: one has to redefine
 * one or more virtual member functions to make it do useful work:
 *
 *    - void initialize()
 *    - void activity()
 *    - void handleMessage(cMessage *msg)
 *    - void finish()
 *
 * initialize() is called after OMNeT++ created the module.
 *
 * One has to redefine either activity() or handleMessage() to contain
 * the internal logic of the module. activity() and handleMessage() implement
 * different event processing strategies: activity() is a coroutine-based
 * solution which implements the process-interaction approach (coroutines are
 * non-preemptive [cooperative] threads). handleMessage() is a method that is
 * called by the simulation kernel when the module receives a message.
 *
 * The finish() functions are called when the simulation terminates
 * successfully. Typical use of finish() is recording statistics
 * collected during simulation.
 *
 * The activity() functions run as coroutines during simulation.
 * Coroutines are brought to cSimpleModule from the cCoroutine base class.
 *
 * @ingroup SimCore
 */
class SIM_API cSimpleModule : public cModule
{
    friend class cModule;
    friend class cSimulation;
    friend class TSimpleModInspector;
  private:
    bool usesactivity;      // uses activity() or handleMessage()
    int state;              // ended/ready/waiting for msg
    opp_string phasestr;    // a 'phase' string
    sBlock *heap;           // head of modfunc's heap list
    cMessage *timeoutmsg;   // msg used in wait() and receive() with timeout
    cCoroutine *coroutine;

  private:
    // internal use
    static void activate(void *p);
    void discardLocals();

  protected:
    // internal use
    virtual void arrived(cMessage *msg,int n,simtime_t t);

  public:
    cHead locals;           // list of local variables of module function

    /**
     * DEPRECATED. putAsideQueue will be removed at some point in the
     * future, and this will affect the message receiving functions.
     * Details below.
     *
     * putAsideQueue is used implicitly by the methods wait(), receiveOn()
     * and receiveNewOn() to insert messages that arrive during the wait
     * period or not on the specified gate. The receive() functions
     * looked first on the putAsideQueue and only then at the future events.
     *
     * As practice has shown, the purpose of the putAsideQueue was very
     * often misunderstood, and its implicit use by wait() and the message
     * receiving functions was highly error-prone. It will be removed
     * at some point in the future. Be prepared: use wait() and receiveOn()
     * if you do not expect other messages to arrive at the module;
     * use waitAndEnqueue() where you mean it; do not use the receiveNew..()
     * functions as they will be made redundant when putAsideQueue goes away.
     */
    cQueue putAsideQueue;   // put-aside queue

  protected:
    /** @name Hooks for defining module behavior.
     *
     * Exactly one of activity() and handleMessage() must be redefined
     * by the user to add functionality to the simple module. See the
     * manual for detailed guidance on how use to these two methods.
     *
     * These methods are made protected because they shouldn't be called
     * directly from outside.
     */
    //@{

    /**
     * Should be redefined to contain the module activity function.
     * This default implementation issues an error message (throws cException).
     */
    virtual void activity();

    /**
     * Should be redefined to contain the module's message handling function.
     * This default implementation issues an error message (throws cException).
     */
    virtual void handleMessage(cMessage *msg);
    //@}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cSimpleModule(const cSimpleModule& mod);

    /**
     * Constructor. Note that module objects should not be created directly,
     * only via their cModuleType objects. See cModule constructor for more info.
     */
    cSimpleModule(const char *name, cModule *parentmod, unsigned stk);

    /**
     * Destructor.
     */
    virtual ~cSimpleModule();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cSimpleModule& operator=(const cSimpleModule& mod);
    //@}

    /** @name Redefined cObject functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() const  {return new cSimpleModule(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Call the passed function for each contained object.
     * See cObject for more details.
     */
    virtual void forEach(ForeachFunc f);
    //@}

    /** @name Redefined cModule functions. */
    //@{

    /**
     * FIXME: redefined cModule functions:
     */
    virtual void setId(int n);

    /**
     * Returns true.
     */
    virtual bool isSimple() const {return true;}

    /**
     * Calls initialize(int stage) in the context of this module.
     * It does a single stage of initialization, and returns <tt>true</tt>
     * if more stages are required.
     */
    virtual bool callInitialize(int stage);

    /**
     * Calls finish() in the context of this module.
     */
    virtual void callFinish();

    /**
     * Creates a starting message for the module.
     */
    virtual void scheduleStart(simtime_t t);

    /**
     * Deletes a dynamically created module. A running module can also
     * delete itself.
     */
    virtual void deleteModule();
    //@}

    /** @name Information about the module. */
    //@{

    /**
     * Returns event handling scheme.
     */
    bool usesActivity() const  {return usesactivity;}
    //@}

    /** @name Simulation time. */
    //@{

    /**
     * Returns the current simulation time (that is, the arrival time
     * of the last message returned by a receiveNew() call).
     */
    simtime_t simTime() const;   // cannot make inline because of declaration order!
    //@}

    /** @name Debugging aids. */
    //@{

    /**
     * DEPRECATED.
     * Sets the phase string which can be displayed as some kind of status.
     * The module creates its own copy of the string.
     */
    void setPhase(const char *phase)  {phasestr=phase;}

    /**
     * DEPRECATED.
     * Returns pointer to the current phase string.
     */
    const char *phase() const  {return correct(phasestr);}

    /**
     * To be called from module functions. Outputs textual information
     * about all objects of the simulation (including the objects created
     * in module functions by the user!) into the snapshot file. The
     * output is detailed enough to be used for debugging the simulation:
     * by regularly calling snapshot(), one can trace how the
     * values of variables, objects changed over the simulation. The
     * arguments: label is a string that will appear in the
     * output file; obj is the object whose inside is of interest.
     * By default, the whole simulation (all modules etc) will be written
     * out.
     *
     * Tkenv also supports making snapshots manually, from menu.
     *
     * See also class cWatch and the WATCH() macro.
     */
    bool snapshot(cObject *obj=&simulation, const char *label=NULL); // write snapshot file

    /**
     * Specifies a breakpoint. During simulation, if execution gets to
     * a breakpoint() call (and breakpoints are active etc.),
     * the simulation will be stopped, if the user interface can handle
     * breakpoints.
     */
    void breakpoint(const char *label);     // user breakpoint

    /**
     * Sets phase string and temporarily yield control to the user interface.
     * If the user interface supports step-by-step execution, one can
     * stop execution at each receive() call of the module function
     * and examine the objects, variables, etc. If the state of simulation
     * between receive() calls is also of interest, one can
     * use pause() calls. The string argument (if given) sets
     * the phase string, so pause("here") is the same
     * as setPhase("here"); pause().
     */
    void pause(const char *phase=NULL);
    //@}

    /** @name Message sending. */
    //@{

    /**
     * Sends a message through the gate given with its ID.
     */
    int send(cMessage *msg, int gateid);   // send a message thru gate id

    /**
     * Sends a message through the gate given with its name and index
     * (if multiple gate).
     */
    int send(cMessage *msg, const char *gatename, int sn=-1); // s:gate name, sn:index

    /**
     * Sends a message through the gate given with its pointer.
     */
    int send(cMessage *msg, cGate *outputgate);

    /**
     * Delayed sending. Sends a message through the gate given with
     * its index as if it was sent delay seconds later.
     */
    int sendDelayed(cMessage *msg, double delay, int gateid);

    /**
     * Delayed sending. Sends a message through the gate given with
     * its name and index (if multiple gate) as if it was sent delay
     * seconds later.
     */
    int sendDelayed(cMessage *msg, double delay, const char *gatename, int sn=-1);

    /**
     * Sends a message through the gate given with its pointer as if
     * it was sent delay seconds later.
     */
    int sendDelayed(cMessage *msg, double delay, cGate *outputgate);

    /**
     * Send a message directly to another module (to an input gate).
     */
    int sendDirect(cMessage *msg, double delay, cModule *mod, int inputgateid);

    /**
     * Send a message directly to another module (to an input gate).
     */
    int sendDirect(cMessage *msg, double delay, cModule *mod, const char *inputgatename, int sn=-1);

    /**
     * Sends a message directly to another module (to an input gate).
     */
    int sendDirect(cMessage *msg, double delay, cGate *inputgate);
    //@}

    /** @name Self-messages. */
    //@{

    /**
     * Schedules a self-message. Receive() will return the message at
     * t simulation time.
     */
    int scheduleAt(simtime_t t, cMessage *msg);

    /**
     * Removes the given message from the future events. The message
     * needs to have been sent using the scheduleAt() function.
     * This function can be used to cancel a timer implemented with scheduleAt().
     */
    cMessage *cancelEvent(cMessage *msg);
    //@}

    /** @name Parallel simulation. */
    //@{

    /**
     * Used with parallel execution: synchronize with receiver segment.
     * Blocks receiver at t until a message arrives.
     */
    void syncpoint(simtime_t t, int gateid);

    /**
     * Used with parallel execution: synchronize with receiver segment.
     * Blocks receiver at t until a message arrives.
     */
    void syncpoint(simtime_t t, const char *gatename, int sn=-1);

    /**
     * Used with parallel execution: cancel a synchronization point set by a
     * syncpoint() call.
     */
    void cancelSyncpoint(simtime_t t, int gateid);

    /**
     * Used with parallel execution: cancel a synchronization point set by a
     * syncpoint() call.
     */
    void cancelSyncpoint(simtime_t t, const char *gatename, int sn=-1);
    //@}

    /** @name Receiving messages.
     *
     * These functions can only be used with activity(), but not with
     * handleMessage().
     */
    //@{

    /**
     * DEPRECATED.
     *
     * Tells if the next message in the event queue is for the same module
     * and has the same arrival time. (Returns true only if
     * two or more messages arrived to the module at the same time.)
     */
    bool isThereMessage() const;

    /**
     * Receives a message from the put-aside queue or the FES.
     */
    cMessage *receive();

    /**
     * Returns the first message from the put-aside queue or, if it is
     * empty, calls receiveNew() to return a message from the
     * event queue with the given timeout. Note that the arrival time
     * of the message returned by receive() can be earlier than
     * the current simulation time.
     */
    cMessage *receive(simtime_t timeout);

    /**
     * Scans the put-aside queue for the first message that has arrived
     * on the gate specified with its name and index. If there is no
     * such message in the put-aside queue, calls receiveNew()
     * to return a message from the event queue with the given timeout.
     * Note that the arrival time of the message returned by receive()
     * can be earlier than the current simulation time.
     *
     * <b>IMPORTANT</b>: The put-aside queue has been deprecated,
     * and the semantics of this function will be changed in the future.
     * It will be intended for use only if you do not expect other messages
     * to arrive at the module on other gates than the specified one.
     * To assert this, it will throw an exception if an inappropriate message
     * arrives, as it will probably signal a logic error in the model.
     * On the other hand, if you <i>do</i> expect to receive other messages
     * during the call, you should not use receiveOn() but implement
     * similar functionality using a loop (for() or while()) and receive(),
     * which will make your intent more conspicuous to the reader of your
     * source code.
     * See the API-doc on putAsideQueue for more explanation.
     */
    cMessage *receiveOn(const char *gatename, int sn=-1, simtime_t timeout=MAXTIME);

    /**
     * Same as the previous function except that the gate must be specified
     * with its index in the gate array. Using this function instead
     * the previous one may speed up the simulation if the function is
     * called frequently.
     *
     * <b>IMPORTANT</b>: The put-aside queue has been deprecated,
     * and the semantics of this function will be changed in the future.
     * It will be intended for use only if you do not expect other messages
     * to arrive at the module on other gates than the specified one.
     * To assert this, it will throw an exception if an inappropriate message
     * arrives, as it will probably signal a logic error in the model.
     * On the other hand, if you <i>do</i> expect to receive other messages
     * during the call, you should not use receiveOn() but implement
     * similar functionality using a loop (for() or while()) and receive(),
     * which will make your intent more conspicuous to the reader of your
     * source code.
     * See the API-doc on putAsideQueue for more explanation.
     */
    cMessage *receiveOn(int gateid, simtime_t timeout=MAXTIME);

    /**
     * Remove the next message from the event queue and return a pointer
     * to it. Ignores put-aside queue.
     *
     * <b>IMPORTANT</b>: This function should not be used. As putAsideQueue is
     * deprecated, it will be removed at some point in the future.
     * This means that then the receive() method will act exactly as this one,
     * and this method will no longer be needed.
     * See the API-doc on putAsideQueue, wait() and waitAndEnqueue() for more explanation.
     */
    cMessage *receiveNew();

    /**
     * Removes the next message from the event queue and returns a pointer
     * to it. Ignores put-aside queue. If there is no message in the event
     * queue, the function waits with t timeout until a message will be
     * available. If the timeout expires and there is still no message
     * in the queue, the function returns NULL.
     *
     * <b>IMPORTANT</b>: This function should not be used. As putAsideQueue is
     * deprecated, it will be removed at some point in the future.
     * This means that then the receive() method will act exactly as this one,
     * and this method will no longer be needed.
     * See the API-doc on putAsideQueue, wait() and waitAndEnqueue() for more explanation.
     */
    cMessage *receiveNew(simtime_t timeout);

    /**
     * The same as receiveNew(), except that it returns the
     * next message that arrives on the gate specified with its name
     * and index. All messages received meanwhile are inserted into the
     * put-aside queue. If the timeout expires and there is still no
     * such message in the queue, the function returns NULL.
     *
     * In order to process messages that may have been put in the put-aside
     * queue, the user is expected to call receive() or receiveOn(),
     * or to examine the put-aside queue directly sometime.
     *
     * <b>IMPORTANT</b>: This function should not be used. As putAsideQueue is
     * deprecated, it will be removed at some point in the future.
     * This means that then the receiveOn() method will act exactly as this one,
     * and this method will no longer be needed.
     * See the API-doc on putAsideQueue, wait() and waitAndEnqueue() for more explanation.
     */
    cMessage *receiveNewOn(const char *gatename, int sn=-1, simtime_t timeout=MAXTIME);

    /**
     * Same as the previous function except that the gate must be specified
     * with its index in the gate array. Using this function instead
     * the previous one may speed up the simulation if the function is
     * called frequently.
     *
     * <b>IMPORTANT</b>: This function should not be used. As putAsideQueue is
     * deprecated, it will be removed at some point in the future.
     * This means that then the receiveOn() method will act exactly as this one,
     * and this method will no longer be needed.
     * See the API-doc on putAsideQueue, wait() and waitAndEnqueue() for more explanation.
     */
    cMessage *receiveNewOn(int gateid, simtime_t timeout=MAXTIME);
    //@}

    /** @name Waiting. */
    //@{

    /**
     * Waits for the given interval. (Some other simulators call this
     * functionality hold()). The messages received meanwhile are inserted
     * into the put-aside queue, but if you expect to receive messages
     * during the call, you should use the waitAndEnqueue() method
     * instead (see the following note).
     *
     * This function can only be used with activity(), but not with
     * handleMessage().
     *
     * <b>IMPORTANT</b>: The put-aside queue has been deprecated,
     * and the semantics of this function will be changed in the future.
     * It will be intended for use only if you do not expect other messages
     * to arrive at the module during the wait period. To assert this,
     * it will throw an exception if a message arrives during the wait,
     * as it will probably signal a logic error in the model.
     * On the other hand, if you <i>do</i> expect to receive messages
     * during the call, you should use waitAndEnqueue(), which makes
     * this assumption much more conspicuous to the reader of your source
     * code than the old wait() method with its implicit putAsideQueue
     * would. See the API-doc on putAsideQueue for more explanation.
     */
    void wait(simtime_t time);

    /**
     * Waits for the given interval. The messages received during the wait
     * period are inserted into the queue passed as argument.
     *
     * This function can only be used with activity(), but not with
     * handleMessage().
     */
    void waitAndEnqueue(simtime_t time, cQueue *queue);
    //@}

    /** @name Stopping the module or the simulation. */
    //@{

    /**
     * Ends the run of the simple module. The simulation is not stopped
     * (unless this is the last running module.) The implementation simply
     * throws a cEndModuleException.
     *
     * Note that end() does NOT delete the module; its state is simply
     * set to Ended and it won't take part in further simulation.
     * If you also want to delete the module, use deleteModule(); however,
     * this implies that the module's finish() method won't be called
     * at the end of the simulation.
     */
    void end();

    /**
     * Causes the whole simulation to stop. The implementation simply
     * throws a cTerminationException.
     */
    void endSimulation();

    /**
     * DEPRECATED. Equivalent to <code>throw new cException(
     * ...<i>same argument list</i>...)</code>.
     */
    void error(const char *fmt,...) const;
    //@}

    //// access putaside-queue -- soon!
    //virtual cQueue& putAsideQueue();

    /** @name Statistics collection */
    //@{

    /**
     * Records a double into the scalar result file.
     */
    void recordScalar(const char *name, double value);

    /**
     * Records a string into the scalar result file.
     */
    void recordScalar(const char *name, const char *text);

    /**
     * Records a statistics object into the scalar result file.
     */
    void recordStats(const char *name, cStatistic *stats);
    //@}

    /** Coroutine stack info. Useful only if module uses activity(). */
    //@{

    /**
     * Returns true if there was a stack overflow during execution of the
     * coroutine. (Not implemented for every coroutine package - see cCoroutine
     * documentation for more info.) If the module uses handleMessage(),
     * this method always returns false.
     *
     * @see cCoroutine
     */
    virtual bool stackOverflow() const;

    /**
     * Returns the stack size of the coroutine. If the module uses handleMessage(),
     * this method always returns 0.
     */
    virtual unsigned stackSize() const;

    /**
     * Returns the amount of stack actually used by the coroutine.
     * (Not implemented for every coroutine package - see cCoroutine
     * documentation for more info.) If the module uses handleMessage(),
     * this method always returns 0.
     *
     * @see cCoroutine
     */
    virtual unsigned stackUsage() const;
    //@}

    /** @name Miscellaneous. */
    //@{

    /**
     * Dynamic memory allocation. This function should be used instead
     * of the global ::malloc() from inside the module function
     * (activity()), if deallocation by the simple module constructor
     * is not provided.
     *
     * Dynamic allocations are discouraged in general unless you put
     * the pointer into the class declaration of the simple module class
     * and provide a proper destructor. Or, you can use container classes
     * (cArray, cQueue)!
     */
    void *memAlloc(size_t m);

    /**
     * Frees a memory block reserved with the malloc() described
     * above and NULLs the pointer.
     */
    void memFree(void *&p);

    // INTERNAL: free module's local allocations
    void clearHeap();

    /**
     * Returns module state.
     */
    int moduleState() const {return state;}
    //@}
};

//==========================================================================

/**
 * Represents a compound module in the simulation.
 *
 * NOTE: dup() cannot be used. Use moduleType()->create() instead.
 *
 * @ingroup SimCore
 */
class SIM_API cCompoundModule : public cModule
{
    friend class TCompoundModInspector;

  protected:
    // internal use
    virtual void arrived(cMessage *msg,int n,simtime_t t);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cCompoundModule(const cCompoundModule& mod);

    /**
     * Constructor. Note that module objects should not be created directly,
     * only via their cModuleType objects. See cModule constructor for more info.
     */
    cCompoundModule(const char *name, cModule *parentmod);

    /**
     * Destructor.
     */
    virtual ~cCompoundModule();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cCompoundModule& operator=(const cCompoundModule& mod);
    //@}

    /** @name Redefined cObject functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() const   {return new cCompoundModule(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);
    //@}

    /** @name Redefined cModule functions. */
    //@{

    /**
     * Redefined cModule method. Returns false.
     */
    virtual bool isSimple() const  {return false;}

    /**
     * Redefined cModule method.
     * Calls initialize() first for this module first, then recursively
     * for all its submodules.
     */
    virtual bool callInitialize(int stage);

    /**
     * Redefined cModule method. Calls finish() first for submodules
     * recursively, then for this module.
     */
    virtual void callFinish();

    /**
     * Calls scheduleStart() recursively for all its (immediate)
     * submodules. This is used with dynamically created modules.
     */
    virtual void scheduleStart(simtime_t t);

    /**
     * Calls deleteModule() for all its submodules and then
     * deletes itself.
     */
    virtual void deleteModule();
    //@}
};

//==========================================================================

/**
 * Iterates through submodules of a compound module.
 */
class SIM_API cSubModIterator
{
  private:
    const cModule *parent;
    int id;

  public:
    /**
     * Constructor. It takes the parent module.
     */
    cSubModIterator(const cModule& p)  {parent=&p;id=0;operator++(0);}

    /**
     * Reinitializes the iterator.
     */
    void init(const cModule& p)  {parent=&p;id=0;operator++(0);}

    /**
     * DEPRECATED because it might return null reference; use operator() instead.
     */
    cModule& operator[](int)  {return id==-1 ? *(cModule*)NULL : *(simulation.module(id));}

    /**
     * Returns pointer to the current module. The pointer then
     * may be cast to the appropriate cModule subclass.
     * Returns NULL of the iterator has reached the end of the list.
     */
    cModule *operator()()  {return id==-1 ? NULL : simulation.module(id);}

    /**
     * Returns true of the iterator has reached the end of the list.
     */
    bool end() const  {return (id==-1);}

    /**
     * Returns the current module, then moves the iterator to the
     * next module. Returns NULL if the iterator has already reached
     * the end of the list.
     */
    cModule *operator++(int);    // sets id to -1 if end was reached
};

#endif

