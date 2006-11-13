//==========================================================================
//   CMODULE.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cModule        : common base for cCompoundModule and cSimpleModule
//    cCompoundModule: compound module
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMODULE_H
#define __CMODULE_H

#include "defs.h"

#include <time.h>     // time_t, clock_t in cSimulation
#include "cobject.h"
#include "ccoroutine.h"
#include "globals.h"
#include "carray.h"
#include "cqueue.h"
#include "cgate.h"
#include "csimul.h"
#include "cdefaultlist.h"

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
                     cChannelType *linkp,
                     cModule *tom, int tog) _OPPDEPRECATED;

/**
 * DEPRECATED: use cGate::connectTo() instead.
 *
 * Connects two gates.
 */
SIM_API void connect(cModule *frm, int frg,
                     cPar *delayp, cPar *errorp, cPar *dataratep,
                     cModule *tom, int tog) _OPPDEPRECATED;
//@}

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
 * cModule provides gates, parameters, RNG mapping, display strings,
 * and a set of virtual methods.
 *
 * For navigating around in the module tree, see:
 * parentModule(), submodule(), cSubModIterator, moduleByRelativePath(),
 * cSimulation::moduleByPath().
 *
 * @ingroup SimCore
 */
class SIM_API cModule : public cDefaultList
{
    friend class cGate;
    friend class cModulePar; // needs to call handleParameterChange()
    friend class cSimulation;
    friend class cModuleType;
    friend class cSubModIterator;

  public:
    static bool pause_in_sendmsg; // if true, split send() with transferToMain()
    static std::string lastmodulefullpath; // cached result of last fullPath() call
    static const cModule *lastmodulefullpathmod; // module of lastmodulefullpath

  protected:
    mutable char *fullname; // buffer to store full name of object
    cModuleType *mod_type;  // type of this module
    int mod_id;             // id (subscript into cSimulation)

    // Note: parent module is stored in ownerp -- a module is always owned by its parent
    // module. If ownerp cannot be cast to a cModule, the module has no parent module
    // (e.g. the system module which is owned by the global object 'simulation').
    cModule *prevp, *nextp; // pointers to sibling submodules
    cModule *firstsubmodp;  // pointer to first submodule
    cModule *lastsubmodp;   // pointer to last submodule (needed for efficient append operation)

  public:
    // The following members are only made public for use by the inspector
    // classes. Do not use them directly from simple modules.
    cArray gatev;           // vector of gates
    cArray paramv;          // vector of parameters

  protected:
    int  idx;               // index if module vector, 0 otherwise
    int  vectsize;          // vector size, -1 if not a vector

    cDisplayString *dispstr;   // display string as submodule (icon, etc)
    cDisplayString *bgdispstr; // display string when enclosing module (background color, etc)

    bool ev_enabled;        // in Cmdenv this tells if ev<< output if printed for this module

    short rngmapsize;       // size of rngmap array (RNGs>=rngmapsize are mapped one-to-one to physical RNGs)
    int *rngmap;            // maps local RNG numbers (may be NULL if rngmapsize==0)

  public:
    // internal: used from Tkenv: find out if cGate has a display string.
    // displayString() would create the object immediately which we want to avoid.
    bool hasDisplayString() {return dispstr!=NULL;}
    bool hasBackgroundDisplayString() {return bgdispstr!=NULL;}

    // internal: currently used by Cmdenv
    void setEvEnabled(bool e)  {ev_enabled = e;}
    bool isEvEnabled() {return ev_enabled;}

    // internal: invoked from within cEnvir::getRNGMappingFor(mod)
    void setRNGMap(short size, int *map) {rngmapsize=size; rngmap=map;}

  protected:
    // internal: called when a message arrives at a gate which is no further
    // connected (that is, toGate() is NULL)
    virtual void arrived(cMessage *msg,int n,simtime_t t) = 0;

    // internal: sets the module ID. Called as part of the module creation process.
    virtual void setId(int n);

    // internal: sets module index within vector (if module is part of
    // a module vector). Called as part of the module creation process.
    virtual void setIndex(int i, int n);

    // internal: sets associated cModuleType for the module. Called as part of
    // the module creation process.
    virtual void setModuleType(cModuleType *mtype);

    // internal: inserts a submodule. Called as part of the module creation process.
    void insertSubmodule(cModule *mod);

    // internal: removes a submodule
    void removeSubmodule(cModule *mod);

    // internal: "virtual ctor" for cGate, because in cPlaceHolderModule
    // we'll need different gate objects
    virtual cGate *createGateObject(const char *gname, char tp);

  protected:
    /** @name Initialization, finish and parameter change hooks.
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
    virtual int numInitStages() const  {return 1;}

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

    /**
     * This method is called by the simulation kernel to notify the module
     * that the value of an existing module parameter got changed.
     * Redefining this method allows simple modules to be prepared for
     * parameter changes, e.g. by re-reading the value.
     * This default implementation does nothing.
     *
     * To make it easier to write predictable simple modules, the function does
     * NOT get called during initialize() or finish(). If you need
     * notifications within those two functions as well, add the following
     * code to them:
     *
     * <pre>
     * for (int i=0; i<params(); i++)
     *     handleParameterChange(par(i).name());
     * </pre>
     *
     * Also, one must be extremely careful when changing parameters from inside
     * handleParameterChange(), to avoid creating an infinite notification loop.
     */
    virtual void handleParameterChange(const char *parname);
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
     * an ID to the module, inserting it into the global <tt>simulation</tt>
     * object (see cSimulation), etc.).
     */
    cModule();

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
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v);

    /**
     * Sets object's name. Redefined to update the stored fullName string.
     */
    virtual void setName(const char *s);

    /**
     * Returns the full name of the module, which is name() plus the
     * index in square brackets (e.g. "module[4]"). Redefined to add the
     * index.
     */
    virtual const char *fullName() const;

    /**
     * Redefined. (Reason: a C++ rule that overloaded virtual methods must be redefined together.)
     */
    virtual std::string fullPath() const;

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
    cGate *addGate(const char *s, char tp, bool isvector=false);

    /**
     * Sets gate vector size. If the vector size is increased, Ids of existing
     * gates in the vector may change. The function returns the new Id of the
     * first gate in the vector.
     *
     * Note: setGateSize() should not be called when Id change can cause problems:
     * after initialize() of this module has been invoked, or when messages have
     * been sent to this module. In general, setGateSize() should only be used during
     * creation of the module.
     *
     * (Id changes cannot be avoided with the current Id allocation scheme: Ids are
     * guaranteed to be contiguous within a gate vector, and we issue small numbers
     * as Ids, thus if the vector would expand to already issued gate Ids, the
     * whole vector must be moved to a different Id range.)
     */
    int setGateSize(const char *s, int size);

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
     * Convenience function. Returns true this is a simple module
     * (i.e. subclassed from cSimpleModule), false otherwise.
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
    int id() const  {return mod_id;}

    /**
     * Returns the module's parent module. For the system module, it returns
     * NULL.
     */
    cModule *parentModule() const  {return dynamic_cast<cModule *>(owner());}

    /**
     * Returns true if this module is in a module vector.
     */
    bool isVector() const  {return vectsize>=0;}

    /**
     * Returns the index of the module if it is in a module vector, otherwise 0.
     */
    int index() const  {return idx;}

    /**
     * Returns the size of the module vector the module is in. For non-vector
     * gates it returns 1.
     */
    int size() const  {return vectsize<0?1:vectsize;}

    /**
     * Returns the RNG mapped to module local RNG number k. For large k
     * (RNGs above the map size) are mapped one-to-one to physical RNGs,
     * that is, rng(100) returns ev.rng(100) (provided it exists).
     */
    cRNG *rng(int k) const  {return ev.rng(k<rngmapsize ? rngmap[k] : k);}
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
     * Returns a gate by its ID. Note that the gate array may contain "holes",
     * that is, this function can return NULL for some IDs in the 0..gates()-1
     * range. NULL is returned for IDs out of range as well.
     */
    cGate *gate(int g) {return (cGate*)gatev[g];}

    /**
     * Returns a gate by its ID. Note that the gate array may contain "holes",
     * that is, this function can return NULL for some IDs in the 0..gates()-1
     * range. NULL is returned for IDs out of range as well.
     */
    const cGate *gate(int g) const {return (const cGate*)gatev[g];}

    /**
     * Looks up a gate by its name and index. Returns NULL if the gate does
     * not exist.
     */
    cGate *gate(const char *gatename,int sn=-1);

    /**
     * Looks up a gate by its name and index. Returns NULL if the gate does not exist.
     */
    const cGate *gate(const char *gatename,int sn=-1) const;

    /**
     * Returns the size of the gate vector with the given name. It returns 1 for
     * non-vector gates, and 0 if the gate doesn't exist or the vector has size 0.
     * (Zero-size vectors are represented by a single gate whose size() returns 0.)
     *
     * The gate vector size can also be obtained by calling the cGate::size() method
     * of any gate object in the vector.
     */
    int gateSize(const char *gatename) const;

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
     * index p. Throws an error if the parameter does not exist.
     */
    cPar& par(int p);

    /**
     * Returns reference to the module parameter specified with its name.
     * Throws an error if the parameter does not exist.
     */
    cPar& par(const char *parname);

    /**
     * Returns index of the module parameter specified with its name.
     * Returns -1 if the object doesn't exist.
     */
    int findPar(const char *parname) const;

    /**
     * Searches for the parameter in the parent modules, up to the system
     * module. It the parameter is not found, throws cRuntimeError.
     */
    cPar& ancestorPar(const char *parname);

    /**
     * Check if a parameter exists.
     */
    bool hasPar(const char *s) const {return findPar(s)>=0;}
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
    virtual bool callInitialize(int stage);

    /**
     * Interface for calling finish() from outside.
     */
    virtual void callFinish();
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
     * Deletes the module and recursively all its submodules. This method
     * has to be used if a simple module wants to delete itself
     * (<tt>delete this</tt> is not allowed.)
     */
    virtual void deleteModule();

    /**
     * Moves the module under a new parent module. This functionality
     * may be useful for some (rare) mobility scenarios.
     *
     * This function could bypass several rules which are enforced when you
     * build the model using NED, so you must observe the following:
     *
     *  -# you cannot insert the module under one of its own submodules.
     *     This is checked by this function.
     *  -# gates of the module cannot be connected while you move it.
     *     If you moved a module which is connected to its parent module
     *     or to other submodules, you'd create connections that do not obey
     *     the module hierarchy, and this is not permitted. This rule is
     *     also enforced by the implementation of this function.
     *  -# it is recommended that the module name be made unique among the
     *     submodules of its new parent.
     *  -# be aware that if the module is part of a module vector, its
     *     isVector(), index() and size() functions will continue to deliver
     *     the same info -- although other elements of the vector will not
     *     necessarily be present under the same parent module.
     */
    virtual void changeParentTo(cModule *mod);
    //@}

    /** @name Display strings, animation. */
    //@{

    /**
     * Returns the display string which defines presentation when the module
     * is displayed as a submodule in a compound module graphics.
     */
    cDisplayString& displayString();

    /**
     * Returns the display string which is used when this module is a compound module
     * whose internals are being displayed in a window.
     */
    cDisplayString& backgroundDisplayString();

    /**
     * DEPRECATED. Use displayString() and cDisplayString methods instead.
     */
    void setDisplayString(const char *dispstr, bool immediate=true);

    /**
     * DEPRECATED. Use backgroundDisplayString() and cDisplayString methods instead.
     */
    void setBackgroundDisplayString(const char *dispstr, bool immediate=true);

    /**
     * DEPRECATED. Use displayString() or backgroundDisplayString(), plus
     * cDisplayString methods instead.
     */
    const char *displayString(int type);

    /**
     * DEPRECATED. Use displayString() or backgroundDisplayString(), plus
     * cDisplayString methods instead.
     */
    void setDisplayString(int type, const char *dispstr, bool immediate=true);

    /**
     * When the models is running under Tkenv, it displays the given text
     * in the network graphics, as a bubble above the module's icon.
     */
    void bubble(const char *text);
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
    cCompoundModule();

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
    virtual cPolymorphic *dup() const   {return new cCompoundModule(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual std::string info() const;
    //@}

    /** @name Redefined cModule functions. */
    //@{

    /**
     * Calls scheduleStart() recursively for all its (immediate)
     * submodules. This is used with dynamically created modules.
     */
    virtual void scheduleStart(simtime_t t);
    //@}
};

//==========================================================================

/**
 * Iterates through submodules of a compound module.
 *
 * Example:
 * \code
 * for (cSubModIterator i(*modp); !i.end(); i++)
 * {
 *     cModule *submodp = i();
 *     ...
 * }
 * \endcode
 */
class SIM_API cSubModIterator
{
  private:
    cModule *p;

  public:
    /**
     * Constructor. It takes the parent module.
     */
    cSubModIterator(const cModule& h)  {p = &h ? h.firstsubmodp : NULL;}

    /**
     * Reinitializes the iterator.
     */
    void init(const cModule& h)  {p = &h ? h.firstsubmodp : NULL;}

    /**
     * Returns pointer to the current module. The pointer then
     * may be cast to the appropriate cModule subclass.
     * Returns NULL of the iterator has reached the end of the list.
     */
    cModule *operator()() const {return p;}

    /**
     * Returns true if the iterator reached the end of the list.
     */
    bool end() const  {return (bool)(p==NULL);}

    /**
     * Returns the current module, then moves the iterator to the
     * next module. Returns NULL if the iterator has already reached
     * the end of the list.
     */
    cModule *operator++(int)  {if (!p) return NULL; cModule *t=p; p=p->nextp; return t;}
};



#endif

