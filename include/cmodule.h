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

NAMESPACE_BEGIN


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
class SIM_API cModule : public cComponent //implies noncopyable
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
     * for (cModule::SubmoduleIterator i(modp); !i.end(); i++)
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
     * Walks along the channels inside a module, that is, the channels
     * among the module and its submodules. This is the same set of channels
     * whose parentModule() would return the iterated module.
     */
    class ChannelIterator
    {
      private:
        std::vector<cChannel *> channels;
        int k;

      public:
        /**
         * Constructor. The iterator will walk on the module passed as argument.
         */
        ChannelIterator(const cModule *parentmodule) {init(parentmodule);}

        /**
         * Reinitializes the iterator object.
         */
        void init(const cModule *parentmodule);

        /**
         * Returns the current object, or NULL if the iterator is not
         * at a valid position.
         */
        cChannel *operator()() {return k < (int)channels.size() ? channels[k] : NULL;}

        /**
         * Returns true if the iterator has reached the end.
         */
        bool end() const {return k == (int)channels.size();}

        /**
         * Returns the current object, then moves the iterator to the next item.
         * If the iterator has reached end, nothing happens; you have to call
         * init() again to restart iterating. If modules, gates or channels
         * are added or removed during interation, the behaviour is undefined.
         */
        cChannel *operator++(int) {return end() ? NULL : channels[k++];}
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

  protected:
    int numgatedescs;       // size of the gatedescv array
    cGate::Desc *gatedescv; // array with one element per gate or gate vector
    std::vector<cGate*> gatev;  // stores the gates themselves

  protected:
    int idx;      // index if module vector, 0 otherwise
    int vectsize; // vector size, -1 if not a vector

    cDisplayString *dispstr; // display string (created on demand)

  public:
    // internal: used from Tkenv: find out if cGate has a display string.
    // displayString() would create the object immediately which we want to avoid.
    bool hasDisplayString() {return dispstr!=NULL;}

  protected:
    // internal: called from callInitialize(). Does one stage for this submodule
    // tree, and returns true if there's more stages to do
    virtual bool initializeModules(int stage);

    // internal: called from callInitialize(). Does one stage for channels in this
    // submodule tree, and returns true if there's more stages to do
    virtual bool initializeChannels(int stage);

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
    virtual cGate *createGateObject(cGate::Desc *desc);

    // internal: add a new gatedesc by expanding gatedescv[]
    cGate::Desc& addGateDesc();

    // internal: finds a gate descriptor with the given name in gatedescv[];
    // ignores (but gives back) potential "$i"/"$o" suffix in gatename
    int findGateDesc(const char *gatename, char& suffix) const;

    // internal: like findGateDesc(), but throws an error if the gate does not exist
    const cGate::Desc& gateDesc(const char *gatename, char& suffix) const;

    // internal: resize a gate vector, possibly moving it in gatev[]. Returns newpos.
    int moveGates(int oldpos, int oldsize, int newsize, cGate::Desc *desc);

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

    /** @name Redefined cObject member functions. */
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
     * Returns the full path name of the module. Example: <tt>"net.node[12].gen"</tt>.
     * The original fullPath() was redefined in order to hide the global cSimulation
     * instance from the path name.
     */
    virtual std::string fullPath() const;
    //@}

    /** @name Setting up the module. */
    //@{

    /**
     * Adds a gate or gate vector to the module. Gate vectors are created with
     * zero size. When the creation of a (scalar) gate of type cGate::INOUT is
     * requested, actually two gate objects will be created, "gatename$i" and
     * "gatename$o". The specified gatename must not contain a "$i" or "$o"
     * suffix itself.
     */
//FIXME MUST BE ENFORCED THAT NAME IS UNIQUE!!!!! NOT THE SAME AS ANY SUBMODULE,PARAMETER OR GATE!!! OTHERWISE properties() WILL MESS UP!!!!!!!
//FIXME THE SAME MUST BE ENFORCED WITH SUBMODULE CREATION!!!!
    virtual void addGate(const char *gatename, cGate::Type type, bool isvector=false);

    /**
     * Sets gate vector size. If the vector size is increased, Ids of existing
     * gates in the vector may change. The specified gatename must not contain
     * a "$i" or "$o" suffix: it is not possible to set different vector size
     * for the "$i" or "$o" parts of an inout gate.
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
    virtual void setGateSize(const char *gatename, int size);

    /**
     * Helper function to implement NED's "gate++" syntax.
     * Returns the next unconnected gate from an input or output gate vector,
     * or input/output half of an inout vector. When gatename names an inout gate
     * vector, the suffix parameter should be set to 'i' or 'o' to select
     * "gatename$i" or "gatename$o"; otherwise suffix should be zero.
     * The inside parameter selects whether to use isConnectedInside() or
     * isConnectedOutside() to test if the gate is connected. The expand
     * parameter tells whether the gate vector should be expanded if all its
     * gates are used up (==connected).
     */
    virtual cGate *getOrCreateFirstUnconnectedGate(const char *gatename, char suffix,
                                                   bool inside, bool expand);

    /**
     * Helper function to implement NED's "gate++" syntax. This variant accepts
     * inout gates only, and the result is returned in the gatein and gateout
     * parameters. The meaning of the inside and expand parameters is the same as
     * with getOrCreateFirstUnconnectedGate().
     */
    virtual void getOrCreateFirstUnconnectedGatePair(const char *gatename,
                                                     bool inside, bool expand,
                                                     cGate *&gatein, cGate *&gateout);

    /**
     * Redefined from cComponent. This method must be called as part of the module
     * creation process, after moduleType->create() and before mod->buildInside().
     * It finalizes parameter values (e.g. reads the missing ones from omnetpp.ini),
     * and adds gates and gate vectors (whose size may depend on parameter values)
     * to the module.
     *
     * So the sequence of setting up a module is:
     *  1. modType->create()
     *  2. set parameter values
     *  3. mod->finalizeParameters() -- this creates gates too
     *  4. connect gates (possibly adding new gates via gate++ operations)
     *  5. mod->buildInside()
     *
     * The above sequence also explains why finalizeParameters() cannot by merged
     * into either create() or buildInside().
     */
    virtual void finalizeParameters();

    /**
     * In compound modules, this method should be called to build submodules
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
    /**
     * Returns the total size of the gate array. Since the array may contain
     * unused elements, the number of actual gates used might be less.
     * Gate IDs are an index into this array. Inout gates are represented
     * by two cGate objects.
     */
    virtual int gates() const {return gatev.size();}   //XXX rename to numGates

    /**
     * Returns a gate by its ID. Note that the gate array may contain "holes",
     * that is, this function can return NULL for some IDs in the 0..gates()-1
     * range. If the ID is out of range, an error is thrown.
     */
    virtual cGate *gate(int g);

    /**
     * Returns a gate by its ID. Note that the gate array may contain "holes",
     * that is, this function can return NULL for some IDs in the 0..gates()-1
     * range. If the ID is out of range, an error is thrown.
     */
    const cGate *gate(int g) const {return const_cast<cModule *>(this)->gate(g);}

    /**
     * Looks up a gate by its name and index. Gate names with the "$i" or "$o"
     * suffix are also accepted. Throws an error if the gate does not exist.
     * The presence of the index parameter decides whether a vector or a scalar
     * gate will be looked for.
     */
    virtual cGate *gate(const char *gatename, int index=-1);

    /**
     * Looks up a gate by its name and index. Gate names with the "$i" or "$o"
     * suffix are also accepted. Throws an error if the gate does not exist.
     * The presence of the index parameter decides whether a vector or a scalar
     * gate will be looked for.
     */
    const cGate *gate(const char *gatename, int index=-1) const {
        return const_cast<cModule *>(this)->gate(gatename, index);
    }

    /**
     * Returns the "$i" or "$o" part of an inout gate, depending on the type
     * parameter. That is, gateHalf("port", cGate::OUTPUT, 3) would return
     * gate "port[3]$o". Throws an error if the gate does not exist.
     * The presence of the index parameter decides whether a vector or a scalar
     * gate will be looked for.
     */
    virtual cGate *gateHalf(const char *gatename, cGate::Type type, int index=-1);

    /**
     * Returns the "$i" or "$o" part of an inout gate, depending on the type
     * parameter. That is, gateHalf("port", cGate::OUTPUT, 3) would return
     * gate "port[3]$o". Throws an error if the gate does not exist.
     * The presence of the index parameter decides whether a vector or a scalar
     * gate will be looked for.
     */
    const cGate *gateHalf(const char *gatename, cGate::Type type, int index=-1) const {
        return const_cast<cModule *>(this)->gateHalf(gatename, type, index);
    }

    /**
     * Checks if a gate exists. When invoked without index, it returns whether
     * gate "gatename" or "gatename[]" exists (no matter if the gate vector size
     * is currently zero). When invoked with an index, it returns whether the
     * concrete "gatename[index]" gate exists (gatename being a vector gate).
     * Gate names with the "$i" or "$o" suffix are also accepted. The presence
     * of the index parameter decides whether a vector or a scalar gate will be
     * looked for.
     */
    virtual bool hasGate(const char *gatename, int index=-1) const;

    /**
     * Returns the type of the gate (or gate vector) with the given name.
     * Gate names with the "$i" or "$o" suffix are also accepted. Throws
     * an error if there is no such gate or gate vector.
     */
    virtual cGate::Type gateType(const char *gatename) const;

    /**
     * Returns whether the given gate is a gate vector. Gate names with the "$i"
     * or "$o" suffix are also accepted.  Throws an error if there is no
     * such gate or gate vector.
     */
    virtual bool isGateVector(const char *gatename) const;

    /**
     * Returns the size of the gate vector with the given name. It returns 1 for
     * non-vector gates, and 0 if the gate doesn't exist or the vector has size 0.
     * (Zero-size vectors are represented by a single gate whose size() returns 0.)
     * Gate names with the "$i" or "$o" suffix are also accepted.  Throws an error if
     * there is no such gate or gate vector.
     *
     * Note: The gate vector size can also be obtained by calling the cGate::size()
     * method of any gate object.
     */
    virtual int gateSize(const char *gatename) const;

    /**
     * Returns the ID of the gate specified by name and index. Inout gates
     * cannot be specified (since they are actually two gate objects, not one),
     * only with a "$i" or "$o" suffix. Returns -1 if the gate doesn't exist.
     * The presence of the index parameter decides whether a vector or a scalar
     * gate will be looked for.
     */
    virtual int findGate(const char *gatename, int index=-1) const;

    /**
     * For compound modules, it checks if all gates are connected inside
     * the module (it returns <tt>true</tt> if they are OK); for simple
     * modules, it returns <tt>true</tt>. This function is called during
     * network setup.
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
     * DEPRECATED. Use displayString() and cDisplayString methods instead.
     */
    _OPPDEPRECATED void setDisplayString(const char *dispstr, bool immediate=true);

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
class SIM_API _OPPDEPRECATED cSubModIterator : public cModule::SubmoduleIterator
{
  public:
    cSubModIterator(const cModule& m) : cModule::SubmoduleIterator(&m) {}
    void init(const cModule& m) {cModule::SubmoduleIterator::init(&m);}
};

NAMESPACE_END


#endif

