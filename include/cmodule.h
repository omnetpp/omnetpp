//==========================================================================
//   CMODULE.H  -  header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cModule        : common base for cCompoundModule and cSimpleModule
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

//=== classes mentioned/declared here:
class  cMessage;
class  cGate;
class  cModulePar;
class  cModule;
class  cCompoundModule;
class  cSimulation;
class  cModuleType;

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
    friend class cModuleType;

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
    cHead members;          // list of data members of derived classes

  protected:
    int  idx;               // index if module vector, 0 otherwise
    int  vectsize;          // vector size, -1 if not a vector

    opp_string dispstr;     // display string as submodule
    opp_string parentdispstr; // display string as parent (enclosing) module

    DisplayStringNotifyFunc notify_inspector;
    void *data_for_inspector;

  protected:
    // internal: called when a message arrives at a gate which is no further
    // connected (that is, toGate() is NULL)
    virtual void arrived(cMessage *msg,int n,simtime_t t) = 0;

    // internal: sets the module ID. Called as part of the module creation process.
    virtual void setId(int n);

    // internal: sets associated cModuleType for the module. Called as part of
    // the module creation process.
    virtual void setModuleType(cModuleType *mtype);

    // internal: "virtual ctor" for cGate, because in cPlaceHolderModule
    // we'll need different gate objects
    virtual cGate *createGateObject(const char *gname, char tp);

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
     * Adds a gate. The type character tp can be <tt>'I'</tt> or <tt>'O'</tt> for input
     * and output, respectively.
     */
    cGate *addGate(const char *s, char tp);

    /**
     * Sets gate vector size.
     */
    void setGateSize(const char *s, int size);

    /**
     * Called for modules that are in a module vector, sets the module's index 
     * within the vector. Called as part of the module creation process.
     */
    virtual void setIndex(int i, int n);

    /**
     * Adds a parameter to the module.
     */
    cPar *addPar(const char *s);

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
     * Tells whether this is a simple or a compound module.
     */
    virtual bool isSimple() const;

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
     * Looks up a gate by its name and index. A scalar gate is treated same
     * as a vector gate with size=1. Returns NULL if the gate does not exist.
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

// FIXME remove machine pars from nedc!!!

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

