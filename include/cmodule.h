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
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMODULE_H
#define __CMODULE_H

#include <time.h>     // time_t, clock_t in cSimulation
#include "cobject.h"
#include "ccor.h"
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

//=== display string selector
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
 * Connects two gates.
 */
SIM_API void connect(cModule *frm, int frg,
                     cLinkType *linkp,
                     cModule *tom, int tog);

/**
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
    bool warn;              // warnings on/off

    int  idx;               // index if module vector, 0 otherwise
    int  vectsize;          // vector size, -1 if not a vector

    opp_string dispstr[dispNUMTYPES]; // see setDisplayString(..) etc.

    DisplayStringNotifyFunc notify_inspector;
    void *data_for_inspector;

  protected:
    // internal use
    virtual void arrived(cMessage *msg,int n) = 0;

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
     * Multi-stage initialization hook. This default implementation does
     * single-stage init, that is, calls initialize() if stage is 0.
     */
    virtual void initialize(int stage) {if(stage==0) initialize();}

    /**
     * Multi-stage initialization hook, should be redefined to return the
     * number of initialization stages required. This default implementation
     * does single-stage init, that is, returns 1.
     */
    virtual int  numInitStages() _CONST  {return 1;}

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
    cModule(_CONST cModule& mod);

    /**
     * Constructor.
     */
    cModule(const char *name, cModule *parentmod);

    /**
     * Destructor.
     */
    virtual ~cModule() {}

    /**
     * Assignment operator. The name member doesn't get copied;
     * see cObject's operator=() for more details.
     */
    cModule& operator=(_CONST cModule& mod);
    //@}

    /** @name Redefined cObject functions. */
    //@{

    /**
     * Returns pointer to a string containing the class name, "cModule".
     */
    virtual const char *className() const {return "cModule";}

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() _CONST;

    /**
     * Call the passed function for each contained object.
     * See cObject for more details.
     */
    virtual void forEach(ForeachFunc f);

    /**
     * Returns the name of the inspector factory class associated with this class.
     * See cObject for more details.
     */
    virtual const char *inspectorFactoryName() const {return "cModuleIFC";}

    /**
     * Returns full name of the module in a static buffer, in the form
     * <tt>"name"</tt> or <tt>"name[index]"</tt>.
     */
    virtual const char *fullName() const;

    /**
     * Returns the full path name of the module. Example: "mynet.node[12].gen".
     * Redefined in order to hide the global cSimulation instance in the
     * path name (it would add "simulation." to the beginning of the full path).
     */
    virtual const char *fullPath() const;
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
    void addGate(const char *s, char tp);

    /**
     * Sets gate vector size.
     */
    void setGateSize(const char *s, int size);

    /**
     * Adds a parameter to the module.
     */
    void addPar(const char *s);

    /**
     * Adds a machine parameter to the module.
     */
    void addMachinePar(const char *pnam);

    /**
     * Sets value of a machine parameter.
     */
    void setMachinePar(const char *pnam, const char *val);

    /**
     * Builds submodules and internal connections. This default implementation
     * does nothing.
     */
    virtual void buildInside() {}
    //@}

    /** @name Information about the module itself. */
    //@{

    /**
     * Returns this is a simple or compound module. Pure virtual function,
     * redefined in <tt>cSimpleModule</tt> to return true and in <tt>cCompoundModule</tt>
     * to return false.
     */
    virtual bool isSimple() _CONST = 0;

    /**
     * Returns the cModuleType object associated with this module type.
     */
    cModuleType *moduleType() _CONST  {return mod_type;}

    /**
     * Returns the module ID. It is actually the index of the module
     * in the module vector within the cSimulation simulation object.
     * Module IDs are guaranteed to be unique during a simulation run
     * (that is, IDs of deleted modules are not given out to newly created
     * modules).
     */
    int id() _CONST               {return mod_id;}

    /**
     * Returns the module's parent module. For the system module, it returns
     * <tt>NULL</tt>.
     */
    cModule *parentModule() _CONST     {return parentmodp;}

    /**
     * Used with parallel execution: determines if the module is on the
     * local machine. See the user manual for more info.
     */
    bool isOnLocalMachine() _CONST;

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
     * Returns total number of module gates.
     */
    int gates() const {return gatev.items();}

    /**
     * Return a gate by its ID. Issues a warning if gate does not exist.
     */
    cGate *gate(int g) {return (cGate*)gatev[g];}

    /**
     * Return a gate by its ID. Issues a warning if gate does not exist.
     */
    const cGate *gate(int g) const {return (const cGate*)gatev[g];}

    /**
     * Look up a gate by its name and index.
     * Issues a warning if gate was not found.
     */
    cGate *gate(const char *gatename,int sn=-1);

    /**
     * Look up a gate by its name and index.
     * Issues a warning if gate was not found.
     */
    const cGate *gate(const char *gatename,int sn=-1) const;

    /**
     * Return the ID of the gate specified by name and index.
     * Returns -1 if the gate doesn't exist.
     */
    int findGate(const char *gatename, int sn=-1) const;

    /**
     * Check if a gate exists.
     */
    bool hasGate(const char *gatename, int sn=-1) const {return findGate(gatename,sn)>=0;}

    /**
     * For compound modules, it checks if all gates are connected inside
     * the module (it returns <tt>true</tt> if they are OK); for simple
     * modules, it returns <tt>true</tt>. This function is usually called from
     * from NEDC-generated code.
     */
    bool checkInternalConnections() _CONST;
    //@}

    /** @name Parameters. */
    //@{

    /**
     * Returns total number of the module's parameters.
     */
    int params() _CONST {return paramv.items();}

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
    int findPar(const char *parname) _CONST;

    /**
     * Searches for the parameter in the parent modules, up to the system
     * module. It the parameter is not found, an error message is generated.
     */
    cPar& ancestorPar(const char *parname);

    /**
     * Check if a parameter exists.
     */
    bool hasPar(const char *s) _CONST {return findPar(s)>=0;}
    //@}

    /** @name Machine parameters (used by parallel execution). */
    //@{

    /**
     * Returns the number of machine parameters.
     */
    int machinePars() _CONST  {return machinev.items();}    // NET

    /**
     * Returns the value of the ith machine parameter.
     */
    const char *machinePar(int i);                   // NET

    /**
     * Returns a machine parameter by its name.
     */
    const char *machinePar(const char *machinename); // NET
    //@}

    /** @name Interface for calling <tt>initialize()</tt>/<tt>finish()</tt>.
     * Those functions may not be called directly, only via
     * <tt>callInitialize()</tt> and <tt>callFinish()</tt> provided here.
     */
    //@{

    /**
     * Interface for calling <tt>initialize()</tt> from outside.
     * Implements full multi-stage init for this module and its submodules.
     */
    virtual void callInitialize();

    /**
     * Interface for calling <tt>initialize()</tt> from outside. It does a single stage
     * of initialization, and returns <tt>true</tt> if more stages are required.
     */
    virtual bool callInitialize(int stage) = 0;

    /**
     * Interface for calling <tt>finish()</tt> from outside.
     */
    virtual void callFinish() = 0;
    //@}

    /** @name Dynamic module creation. */
    //@{

    /**
     * Pure virtual function; it is redefined in both <tt>cCompoundModule</tt>
     * and <tt>cSimpleModule</tt>. It creates starting message for dynamically
     * created module (or recursively for its submodules). See the user
     * manual for explanation how to use dynamically created modules.
     */
    virtual void scheduleStart(simtime_t t) = 0;

    /**
     * Deletes a dynamically created module and recursively all its submodules.
     * This is a pure virtual function; it is redefined in both <tt>cCompoundModule</tt>
     * and <tt>cSimpleModule</tt>.
     */
    virtual void deleteModule() = 0;
    //@}

    /** @name Enable/disable warnings */
    //@{

    /**
     * Warning messages can be enabled/disabled individually for each
     * module. This function returns the warning status for this module:
     * <tt>true</tt>=enabled, <tt>false</tt>=disabled.
     */
    bool warnings() _CONST       {return warn;}

    /**
     * Enables or disables warnings for this module: <tt>true</tt>=enable, <tt>false</tt>=disable.
     */
    void setWarnings(bool wr)  {warn=wr;}
    //@}

    /** @name Display strings. */
    //@{
    // FIXME: split to 2x2 functions, and get rid of 'type' parameter!

    /**
     * Change the display string for this module. The type can be
     * dispSUBMOD ("as submodule") or dispENCLOSINGMOD ("as enclosing module").
     * The immediate flag selects whether the change should become effective
     * right now or later (after finishing the current event); this
     *
     */
    void setDisplayString(int type, const char *dispstr, bool immediate=true);

    /**
     * Returns the display string for this module. The type can be
     * dispSUBMOD ("as submodule") or dispENCLOSINGMOD ("as enclosing module").
     */
    const char *displayString(int type);

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
 * <tt>cSimpleModule</tt> is a base class for all simple module classes.
 * Most important, <tt>cSimpleModule</tt> has the virtual member functions
 * <tt>activity()</tt> and <tt>handleMessage()</tt>, one of which has to be redefined in
 * the user's simple modules.
 *
 * All basic functions associated with the simulation such as sending
 * and receiving messages are implemented as <tt>cSimpleModule</tt>'s member
 * functions.
 *
 * The <tt>activity()</tt> functions run as coroutines during simulation.
 * Coroutines are brought to <tt>cSimpleModule</tt> from the <tt>cCoroutine</tt> base class.
 *
 * @ingroup SimCore
 */
class SIM_API cSimpleModule : public cCoroutine, public cModule
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

  private:
    // internal use
    static void activate(void *p);

  protected:
    // internal use
    virtual void arrived(cMessage *msg,int n);

  public:
    cHead locals;           // list of local variables of module function
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
     * This default implementation issues an error message.
     */
    virtual void activity();

    /**
     * Should be redefined to contain the module's message handling function.
     * This default implementation issues an error message.
     */
    virtual void handleMessage(cMessage *msg);
    //@}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cSimpleModule(_CONST cSimpleModule& mod);

    /**
     * Constructor.
     */
    cSimpleModule(const char *name, cModule *parentmod, unsigned stk);

    /**
     * Destructor.
     */
    virtual ~cSimpleModule();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cSimpleModule& operator=(_CONST cSimpleModule& mod);
    //@}

    /** @name Redefined cObject functions. */
    //@{

    /**
     * Returns pointer to a string containing the class name, "cSimpleModule".
     */
    virtual const char *className() const {return "cSimpleModule";}

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() _CONST  {return new cSimpleModule(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Returns the name of the inspector factory class associated with this class.
     * See cObject for more details.
     */
    virtual const char *inspectorFactoryName() const {return "cSimpleModuleIFC";}

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
    virtual bool isSimple() _CONST {return true;}

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
     * Deletes a dynamically created module.
     */
    virtual void deleteModule();
    //@}

    /** @name Information about the module. */
    //@{

    /**
     * Returns event handling scheme.
     */
    bool usesActivity() _CONST  {return usesactivity;}
    //@}

    /** @name Simulation time. */
    //@{

    /**
     * Returns the current simulation time (that is, the arrival time
     * of the last message returned by a receiveNew() call).
     */
    simtime_t simTime() _CONST;   // cannot make inline because of declaration order!
    //@}

    /** @name Debugging aids. */
    //@{

    /**
     * Sets the phase string.
     * The string can be displayed in user interfaces which support tracing
     * / debugging (currently only Tkenv) and the string can contain
     * information that tells the user what the module is currently doing.
     * The module creates its own copy of the string.
     */
    void setPhase(const char *phase)  {phasestr=phase;}

    /**
     * Returns pointer to the current phase string.
     */
    const char *phase() _CONST  {return correct(phasestr);}

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
     * Send a message directly to another module (to an input gate).
     */
    int sendDirect(cMessage *msg, double delay, cGate *inputgate);
    //@}

    /** @name Self-messages. */
    //@{

    /**
     * Schedule a self-message. Receive() will return the message at
     * t simulation time.
     */
    int scheduleAt(simtime_t t, cMessage *msg);

    /**
     * Removes the given message from the message queue. The message
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
    int syncpoint(simtime_t t, int gateid);

    /**
     * Used with parallel execution: synchronize with receiver segment.
     * Blocks receiver at t until a message arrives.
     */
    int syncpoint(simtime_t t, const char *gatename, int sn=-1);

    /**
     * Used with parallel execution: cancel a synchronization point set by a
     * syncpoint() call.
     */
    int cancelSyncpoint(simtime_t t, int gateid);

    /**
     * Used with parallel execution: cancel a synchronization point set by a
     * syncpoint() call.
     */
    int cancelSyncpoint(simtime_t t, const char *gatename, int sn=-1);
    //@}

    /** @name Receiving messages.
     *
     * These functions can only be used with activity(), but not with
     * handleMessage().
     */
    //@{

    /**
     * Tells if the next message in the event queue is for the same module
     * and has the same arrival time. (Returns true only if
     * two or more messages arrived to the module at the same time.)
     */
    bool isThereMessage() _CONST;

    /**
     * Receive a message from the put-aside queue or the FES.
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
     */
    cMessage *receiveOn(const char *gatename, int sn=-1, simtime_t timeout=MAXTIME);

    /**
     * Same as the previous function except that the gate must be specified
     * with its index in the gate array. Using this function instead
     * the previous one may speed up the simulation if the function is
     * called frequently.
     */
    cMessage *receiveOn(int gateid, simtime_t timeout=MAXTIME);

    /**
     * Remove the next message from the event queue and return a pointer
     * to it. Ignores put-aside queue.
     */
    cMessage *receiveNew();

    /**
     * Removes the next message from the event queue and returns a pointer
     * to it. Ignores put-aside queue. If there is no message in the event
     * queue, the function waits with t timeout until a message will be
     * available. If the timeout expires and there is still no message
     * in the queue, the function returns NULL.
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
     */
    cMessage *receiveNewOn(const char *gatename, int sn=-1, simtime_t timeout=MAXTIME);

    /**
     * Same as the previous function except that the gate must be specified
     * with its index in the gate array. Using this function instead
     * the previous one may speed up the simulation if the function is
     * called frequently.
     */
    cMessage *receiveNewOn(int gateid, simtime_t timeout=MAXTIME);
    //@}

    /** @name Waiting. */
    //@{

    /**
     * Wait for the given interval. This function can only be used with
     * activity(), but not with handleMessage(). The messages received
     * meanwhile are inserted into the put-aside queue.
     */
    void wait(simtime_t time);
    //@}

    /** @name Stopping the module or the simulation. */
    //@{

    /**
     * Ends the run of the simple module. The simulation is not stopped
     * (unless this is the last running module.)
     */
    void end();

    /**
     * Causes the whole simulation to stop.
     */
    void endSimulation();

    /**
     * Issue an error message.
     */
    void error(const char *fmt,...) const;
    //@}

    //// access putaside-queue -- soon!
    //virtual cQueue& putAsideQueue();

    /** @name Statistics collection */
    //@{

    /**
     * Record a double into the scalar result file.
     */
    void recordScalar(const char *name, double value);

    /**
     * Record a string into the scalar result file.
     */
    void recordScalar(const char *name, const char *text);

    /**
     * Record a statistics object into the scalar result file.
     */
    void recordStats(const char *name, cStatistic *stats);
    //@}

    /** @name Heap allocation. */
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
    void *memAlloc(size_t m);          // allocate m bytes

    /**
     * Frees a memory block reserved with the malloc() described
     * above and NULLs the pointer.
     */
    void memFree(void *&p);            // free block & NULL pointer
    //@}

    // INTERNAL: free module's local allocations
    void clearHeap();

    /**
     * FIXME: Return module state.
     */
    int moduleState() _CONST {return state;}
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
    virtual void arrived(cMessage *msg,int n);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cCompoundModule(_CONST cCompoundModule& mod);

    /**
     * Constructor.
     */
    cCompoundModule(const char *name, cModule *parentmod);

    /**
     * Destructor.
     */
    virtual ~cCompoundModule();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cCompoundModule& operator=(_CONST cCompoundModule& mod);
    //@}

    /** @name Redefined cObject functions. */
    //@{

    /**
     * Returns pointer to a string containing the class name, "cCompoundModule".
     */
    virtual const char *className() const {return "cCompoundModule";}

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() _CONST   {return new cCompoundModule(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Returns the name of the inspector factory class associated with this class.
     * See cObject for more details.
     */
    virtual const char *inspectorFactoryName() const {return "cCompoundModuleIFC";}
    //@}

    /** @name Redefined cModule functions. */
    //@{

    /**
     * Redefined cModule method. Returns false.
     */
    virtual bool isSimple() _CONST  {return false;}

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
    _CONST cModule *parent;
    int i;

  public:
    /**
     * Constructor. It takes the parent module.
     */
    cSubModIterator(_CONST cModule& p)  {parent=&p;i=0;operator++(0);}

    /**
     * Reinitializes the iterator.
     */
    void init(_CONST cModule& p)        {parent=&p;i=0;operator++(0);}

    /**
     * Obsolete.
     */
    // FIXME: should really return NULL and not crash!
    cModule& operator[](int)     {return simulation[i];}

    /**
     * Returns pointer to the current module. The pointer then
     * may be cast to the appropriate cModule subclass.
     * Returns NULL of the iterator has reached the end of the list.
     */
    // FIXME: should really return NULL and not crash!
    cModule *operator()()        {return &simulation[i];}

    /**
     * Returns true of the iterator has reached the end of the list.
     */
    bool end() _CONST                   {return (i==-1);}

    /**
     * Returns the current module, then moves the iterator to the
     * next module. Returns NULL if the iterator has already reached
     * the end of the list.
     */
    cModule *operator++(int);    // sets i to -1 if end was reached
};

#endif

