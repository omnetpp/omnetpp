//==========================================================================
//   CMODULE.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMODULE_H
#define __CMODULE_H

#include <vector>
#include "ccomponent.h"
#include "globals.h"
#include "cgate.h"
#include "csimulation.h"

#include <time.h>     // time_t, clock_t in cSimulation

class  cMessage;
class  cGate;
class  cModule;
class  cSimulation;
class  cModuleType;


/**
 * Prototype for callback functions that are used to notify graphical user
 * interfaces about display string changes.
 *
 * Args: (sub)module which changed; immediate refresh wanted or not; inspector's data
 *
 * @ingroup EnumsTypes
 */
typedef void (*DisplayStringNotifyFunc)(cModule*,bool,void*);



/**
 * Common base for cSimpleModule and cCompoundModule.
 * cModule provides gates, parameters, RNG mapping, display strings,
 * and a set of virtual methods.
 *
 * For navigating around in the module tree, see:
 * parentModule(), submodule(), cModule::SubmoduleIterator,
 * moduleByRelativePath(), cSimulation::moduleByPath().
 *
 * @ingroup SimCore
 */
class SIM_API cModule : public cComponent //noncopyable
{
    friend class cGate;
    friend class cSimulation;
    friend class cModuleType;
    friend class cChannelType;

  public:
    /**
     * Iterates through submodules of a compound module.
     *
     * Example:
     * \code
     * for (cModule::SubmoduleIterator i(*modp); !i.end(); i++)
     * {
     *     cModule *submodp = i();
     *     ...
     * }
     * \endcode
     */
    class SubmoduleIterator
    {
      private:
        cModule *p;

      public:
        /**
         * Constructor. It takes the parent module on which to iterate.
         */
        SubmoduleIterator(const cModule *m)  {init(m);}

        /**
         * Reinitializes the iterator.
         */
        void init(const cModule *m)  {p = m ? const_cast<cModule *>(m->firstsubmodp) : NULL;}

        /**
         * Returns pointer to the current module. The pointer then
         * may be cast to the appropriate cModule subclass.
         * Returns NULL if the iterator has reached the end of the list.
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

    /**
     * Walks along the channels attached to the gates of a module.
     * Note: these are *not* the same channels whose parentModule() returns
     * the iterated module.
     */
    class ChannelIterator
    {
      private:
        const cModule *module;
        int k;

      public:
        /**
         * Constructor. The iterator will walk on the module passed as argument.
         */
        ChannelIterator(const cModule *m)  {init(m);}

        /**
         * Reinitializes the iterator object.
         */
        void init(const cModule *m);

        /**
         * Returns the current object, or NULL if the iterator is not
         * at a valid position.
         */
        cChannel *operator()()  {return module->gate(k)->channel();}

        /**
         * Returns true if the iterator has reached the end of the module's gates.
         */
        bool end() const   {return k>=module->gates();}

        /**
         * Returns the current object, then moves the iterator to the next item.
         * If the iterator has reached end of the module, nothing happens;
         * you have to call init() again to restart iterating.
         * If gates or channels are added or removed during interation, the behaviour
         * is undefined.
         */
        cChannel *operator++(int);
    };

  public:
    static std::string lastmodulefullpath; // cached result of last fullPath() call
    static const cModule *lastmodulefullpathmod; // module of lastmodulefullpath

  protected:
    mutable char *fullname; // buffer to store full name of object
    int mod_id;             // id (subscript into cSimulation)

    // Note: parent module is stored in ownerp -- a module is always owned by its parent
    // module. If ownerp cannot be cast to a cModule, the module has no parent module
    // (e.g. the system module which is owned by the global object 'simulation').
    cModule *prevp, *nextp; // pointers to sibling submodules
    cModule *firstsubmodp;  // pointer to first submodule
    cModule *lastsubmodp;   // pointer to last submodule (needed for efficient append operation)

    std::vector<cGate*> gatev;  // stores the gates of this module

  protected:
    int idx;               // index if module vector, 0 otherwise
    int vectsize;          // vector size, -1 if not a vector

    cDisplayString *dispstr;   // display string as submodule (icon, etc)
    cDisplayString *bgdispstr; // display string when enclosing module (background color, etc)

  public:
    // internal: used from Tkenv: find out if cGate has a display string.
    // displayString() would create the object immediately which we want to avoid.
    bool hasDisplayString() {return dispstr!=NULL;}
    bool hasBackgroundDisplayString() {return bgdispstr!=NULL;}

  protected:
    // internal: called when a message arrives at a gate which is no further
    // connected (that is, toGate() is NULL)
    virtual void arrived(cMessage *msg,int n,simtime_t t) = 0;

    // internal: sets the module ID. Called as part of the module creation process.
    virtual void setId(int n);

    // internal: sets module index within vector (if module is part of
    // a module vector). Called as part of the module creation process.
    virtual void setIndex(int i, int n);

    // internal: inserts a submodule. Called as part of the module creation process.
    void insertSubmodule(cModule *mod);

    // internal: removes a submodule
    void removeSubmodule(cModule *mod);

    // internal: "virtual ctor" for cGate, because in cPlaceHolderModule
    // we'll need different gate objects
    virtual cGate *createGateObject(const char *gname, char tp);

  protected:
    /** @name Initialization and finish hooks, redefined from cComponent. */  //XXX comment??
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
    //@}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
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
    //@}

    /** @name Redefined cObject functions. */
    //@{

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
     * In compound modules, this method can be called to build submodules
     * and internal connections after module creation. This method is a
     * wrapper around doBuildInside().
     *
     * It does the following:
     *
     *    - 1. checks if module parameters and gates conform to the module interface
         FIXME revise comment
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
     * Redefined from cComponent to return true.
     */
    virtual bool isModule() const  {return true;}

    /**
     * Returns the module containing this module. For the system module,
     * it returns NULL.
     */
    virtual cModule *parentModule() const;

    /**
     * Convenience method: casts the return value of componentType() to cModuleType.
     */
    cModuleType *moduleType() const  {return (cModuleType *)componentType();}

    /**
     * Returns the module ID. It is actually the index of the module
     * in the module vector within the cSimulation simulation object.
     * Module IDs are guaranteed to be unique during a simulation run
     * (that is, IDs of deleted modules are not given out to newly created
     * modules).
     */
    int id() const  {return mod_id;}

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
     * modules it returns 1.
     */
    int size() const  {return vectsize<0?1:vectsize;}
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
     * FIXME clarify - what if it's a vector and I provide no index, or vica versa?
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
//XXX make these methods virtual?
    /**
     * Returns the total size of the gate array. Since the array may contain
     * unused elements, the number of actual gates used might be less.
     */
    virtual int gates() const {return gatev.size();}   //XXX rename to numGates

    /**
     * Returns a gate by its ID. Note that the gate array may contain "holes",
     * that is, this function can return NULL for some IDs in the 0..gates()-1
     * range. NULL is returned for IDs out of range as well.
     */
    virtual cGate *gate(int g);

    /**
     * Returns a gate by its ID. Note that the gate array may contain "holes",
     * that is, this function can return NULL for some IDs in the 0..gates()-1
     * range. NULL is returned for IDs out of range as well.
     */
    const cGate *gate(int g) const {return const_cast<cModule *>(this)->gate(g);}

    /**
     * Looks up a gate by its name and index. Returns NULL if the gate does not exist.
     */
    virtual cGate *gate(const char *gatename, int index=-1);

    /**
     * Looks up a gate by its name and index. Returns NULL if the gate does not exist.
     */
    const cGate *gate(const char *gatename, int index=-1) const {return const_cast<cModule *>(this)->gate(gatename, index);}

    /**
     * Returns the size of the gate vector with the given name. It returns 1 for
     * non-vector gates, and 0 if the gate doesn't exist or the vector has size 0.
     * (Zero-size vectors are represented by a single gate whose size() returns 0.)
     *
     * The gate vector size can also be obtained by calling the cGate::size() method
     * of any gate object in the vector.
     */
    virtual int gateSize(const char *gatename) const;

    /**
     * Returns the ID of the gate specified by name and index.
     * Returns -1 if the gate doesn't exist.
     * FIXME clarify what if it's vector and index is not given, and vica versa
     */
    virtual int findGate(const char *gatename, int index=-1) const;

    /**
     * Checks if a gate exists.
     * FIXME clarify what if it's vector and index is not given, and vica versa
     */
    bool hasGate(const char *gatename, int index=-1) const {return findGate(gatename,index)>=0;}

    /**
     * For compound modules, it checks if all gates are connected inside
     * the module (it returns <tt>true</tt> if they are OK); for simple
     * modules, it returns <tt>true</tt>. This function is usually called from
     * from NEDC-generated code.
     */
    bool checkInternalConnections() const;
    //@}

    /** @name Utilities. */
    //@{
    /**
     * Searches for the parameter in the parent modules, up to the system
     * module. If the parameter is not found, throws cRuntimeError.
     */
    cPar& ancestorPar(const char *parname);
    //@}

    /** @name Public methods for invoking initialize()/finish(), redefined from cComponent.
     * initialize(), numInitStages(), and finish() are themselves also declared in
     * cComponent, and can be redefined in simple modules by the user to perform
     * initialization and finalization (result recording, etc) tasks.
     */
    //@{
    /**
     * Interface for calling initialize() from outside.
     */
    virtual void callInitialize();

    /**
     * Interface for calling initialize() from outside.
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
          //FIXME revise these methods!!!
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
     * When the models is running under Tkenv, it displays the given text
     * in the network graphics, as a bubble above the module's icon.
     */
    void bubble(const char *text);
    //@}
};


/**
 * DEPRECATED -- use cModule::SubmoduleIterator instead.
 */
class SIM_API cSubModIterator : public cModule::SubmoduleIterator
{
  public:
    cSubModIterator(const cModule& m) : cModule::SubmoduleIterator(&m) {}
    void init(const cModule& m) {cModule::SubmoduleIterator::init(&m);}
};

#endif

