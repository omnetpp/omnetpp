//==========================================================================
//   CMODULE.H  -  header for
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

#ifndef __OMNETPP_CMODULE_H
#define __OMNETPP_CMODULE_H

#include <vector>
#include "ccomponent.h"
#include "globals.h"
#include "cgate.h"
#include "csimulation.h"

namespace omnetpp {

class  cMessage;
class  cGate;
class  cSimulation;
class  cModuleType;
class  cCanvas;
class  cOsgCanvas;

/**
 * @brief This class represents modules in the simulation.
 *
 * cModule can be used directly for compound modules. Simple module
 * classes need to be subclassed from cSimpleModule, a class that
 * adds more functionality to cModule.
 *
 * For navigating the module tree, see:
 * getParentModule(), getSubmodule(), cModule::SubmoduleIterator,
 * cModule::getModuleByPath(), cSimulation::getModuleByPath().
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
     * @brief Iterates through the gates of a module.
     *
     * Usage:
     * \code
     * for (cModule::GateIterator it(module); !it.end(); ++it) {
     *     cGate *gate = *it;
     *     ...
     * }
     * \endcode
     */
    class SIM_API GateIterator
    {
      private:
        const cModule *module;
        int descIndex;
        bool isOutput;
        int index;

      private:
        void bump();
        void advance();
        cGate *current() const;

      public:
        /**
         * Constructor. It takes the module on which to iterate.
         */
        GateIterator(const cModule *m)  {init(m);}

        /**
         * Reinitializes the iterator.
         */
        void init(const cModule *m);

        /**
         * Returns the current gate, or nullptr if the iterator is not
         * at a valid position.
         */
        cGate *operator*() const {cGate *result=current(); ASSERT(result||end()); return result;}

        /**
         * DEPRECATED. Use the * operator to access the object the iterator is at.
         */
        _OPPDEPRECATED cGate *operator()() const {return operator*();}

        /**
         * Returns true if the iterator reached the end of the list.
         */
        bool end() const;

        /**
         * Prefix increment operator (++it). Moves the iterator to the next
         * gate. It has no effect if the iterator has reached either
         * end of the list.
         */
        GateIterator& operator++() {if (!end()) advance(); return *this;}

        /**
         * Postfix increment operator (it++). Moves the iterator to the next
         * gate, and returns the iterator's previous state. It has no effect
         * if the iterator has reached either end of the list.
         */
        GateIterator operator++(int) {GateIterator tmp(*this); if (!end()) advance(); return tmp;}
    };

    /**
     * @brief Iterates through the submodules of a compound module.
     *
     * Usage:
     * \code
     * for (cModule::SubmoduleIterator it(module); !it.end(); ++it) {
     *     cModule *submodule = *it;
     *     ...
     * }
     * \endcode
     */
    class SIM_API SubmoduleIterator
    {
      private:
        cModule *p;

      private:
        void advance() {p = p->nextSibling;}
        void retreat() {p = p->prevSibling;}

      public:
        /**
         * Constructor. It takes the parent module on which to iterate.
         */
        SubmoduleIterator(const cModule *m)  {init(m);}

        /**
         * Reinitializes the iterator.
         */
        void init(const cModule *m)  {p = m ? const_cast<cModule *>(m->firstSubmodule) : nullptr;}

        /**
         * Returns a pointer to the current module. Returns nullptr if the iterator
         * has reached the end of the list.
         */
        cModule *operator*() const {return p;}

        /**
         * DEPRECATED. Use the * operator to access the object the iterator is at.
         */
        _OPPDEPRECATED cModule *operator()() const {return operator*();}

        /**
         * Returns true if the iterator reached the end of the list.
         */
        bool end() const  {return p==nullptr;}

        /**
         * Prefix increment operator (++it). Moves the iterator to the next
         * submodule. It has no effect if the iterator has reached either
         * end of the list.
         */
        SubmoduleIterator& operator++() {if (!end()) advance(); return *this;}

        /**
         * Postfix increment operator (it++). Moves the iterator to the next
         * submodule, and returns the iterator's previous state. It has no effect
         * if the iterator has reached either end of the list.
         */
        SubmoduleIterator operator++(int) {SubmoduleIterator tmp(*this); if (!end()) advance(); return tmp;}

        /**
         * Prefix decrement operator (--it). Moves the iterator to the previous
         * submodule. It has no effect if the iterator has reached either end
         * of the list.
         */
        SubmoduleIterator& operator--() {if (!end()) retreat(); return *this;}

        /**
         * Postfix decrement operator (it--). Moves the iterator to the previous
         * submodule, and returns the iterator's previous state. It has no effect
         * if the iterator has reached either end of the list.
         */
        SubmoduleIterator operator--(int) {SubmoduleIterator tmp(*this); if (!end()) retreat(); return tmp;}
    };

    /**
     * @brief Walks along the channels inside a module, that is, the channels
     * among the module and its submodules.
     *
     * This is the same set of channels whose getParentModule() would return
     * the iterated module.
     *
     * Implementation note:
     *
     * This iterator precollects the channels into an internal std::vector, then
     * iterates over that. This means that (1) changes made during the iterator's
     * lifetime will not be reflected in the iterator, and (2) copying the iterator
     * is expensive. As a result of (2), no postfix increment/decrement iterators
     * are provided to prevent their accidental use instead of the prefix operators.
     * (Their lack can be trivially circumvented where needed.)
     *
     * Usage:
     * \code
     * for (cModule::ChannelIterator it(module); !it.end(); ++it) {
     *     cChannel *channel = *it;
     *     ...
     * }
     * \endcode
     */
    class SIM_API ChannelIterator
    {
      private:
        std::vector<cChannel *> channels;
        int k;

      public:
        /**
         * Constructor. The iterator will walk on the module passed as argument.
         */
        ChannelIterator(const cModule *parentModule);

        /**
         * Reinitializes the iterator object.
         */
        void init(const cModule *parentModule);

        /**
         * Returns a pointer to the current channel. Returns nullptr if the iterator
         * has reached the end of the list.
         */
        cChannel *operator*() const {return end() ? nullptr : channels[k];}

        /**
         * DEPRECATED. Use the * operator to access the object the iterator is at.
         */
        _OPPDEPRECATED cChannel *operator()() const {return operator*();}

        /**
         * Returns true if the iterator has reached the end.
         */
        bool end() const {return k < 0 || k >= (int)channels.size();}

        /**
         * Prefix increment operator (++it). Moves the iterator to the next
         * channel in the list. It has no effect if the iterator has
         * reached either end of the list.
         *
         * Note: postfix increment/decrement operators are not provided, to avoid
         * large unnecessary copying overhead resulting from accidental use.
         */
        ChannelIterator& operator++() {if (!end()) k++; return *this;}

        /**
         * Prefix decrement operator (--it). Moves the iterator to the previous
         * channel in the list. It has no effect if the iterator has
         * reached either end of the list.
         *
         * Note: postfix increment/decrement operators are not provided, to avoid
         * large unnecessary copying overhead resulting from accidental use.
         */
        ChannelIterator& operator--() {if (!end()) k--; return *this;}
    };

  private:
    static std::string lastModuleFullPath; // cached result of last getFullPath() call
    static const cModule *lastModuleFullPathModule; // module of lastModuleFullPath

  private:
    enum {
        FL_BUILDINSIDE_CALLED = 1 << 9, // whether buildInside() has been called
        FL_RECORD_EVENTS      = 1 << 10, // enables recording events in this module
        FL_BUILTIN_ANIMATIONS = 1 << 11, // whether built-in animations are requested on this module's graphical inspector
    };

  private:
    mutable char *fullPath; // cached fullPath string (caching is optional, so it may be nullptr)
    mutable char *fullName; // buffer to store full name of object
    static bool cacheFullPath; // whether to cache the fullPath string or not

    // Note: parent module is stored in ownerp -- a module is always owned by its parent
    // module. If ownerp cannot be cast to a cModule, the module has no parent module
    // (e.g. the system module which is owned by the global object 'simulation').
    cModule *prevSibling, *nextSibling; // pointers to sibling submodules
    cModule *firstSubmodule;  // pointer to first submodule
    cModule *lastSubmodule;   // pointer to last submodule (needed for efficient append operation)

    typedef std::set<cGate::Name> NamePool;
    static NamePool namePool;
    int gateDescArraySize;    // size of the descv array
    cGate::Desc *gateDescArray; // array with one element per gate or gate vector

    int vectorIndex;      // index if module vector, 0 otherwise
    int vectorSize;       // vector size, -1 if not a vector
#ifdef USE_OMNETPP4x_FINGERPRINTS
    int version4ModuleId;   // OMNeT++ V4.x compatible module ID
#endif

    mutable cCanvas *canvas;  // nullptr when unused
    mutable cOsgCanvas *osgCanvas;  // nullptr when unused

  public:
    // internal: currently used by init
    void setRecordEvents(bool e)  {setFlag(FL_RECORD_EVENTS,e);}
    bool isRecordEvents() const  {return flags&FL_RECORD_EVENTS;}

  protected:
    // internal: called from destructor, recursively unsubscribes all listeners
    void releaseListeners();

    // internal: has initialize() been called?
    bool buildInsideCalled() const {return flags&FL_BUILDINSIDE_CALLED;}

    // internal: called from callInitialize(). Does one stage for this submodule
    // tree, and returns true if there are more stages to do
    virtual bool initializeModules(int stage);

    // internal: called from callInitialize(). Does one stage for channels in this
    // submodule tree, and returns true if there are more stages to do
    virtual bool initializeChannels(int stage);

    // internal: sets module name and its index within vector (if module is
    // part of a module vector). Called as part of the module creation process.
    virtual void setNameAndIndex(const char *s, int i, int n);

    // internal: called from setName() and setIndex()
    void updateFullName();

    // internal: called from setName(), setIndex(), and changeParentTo()
    void updateFullPathRec();

    // internal: used by changeParentTo()
    void reassignModuleIdRec();

    // internal: inserts a submodule. Called as part of the module creation process.
    void insertSubmodule(cModule *mod);

    // internal: removes a submodule
    void removeSubmodule(cModule *mod);

    // internal: "virtual ctor" for cGate, because in cPlaceholderModule we need
    // different gate objects; type should be INPUT or OUTPUT, but not INOUT
    virtual cGate *createGateObject(cGate::Type type);

    // internal: called from deleteGate()
    void disposeGateDesc(cGate::Desc *desc, bool checkConnected);

    // internal: called from deleteGate()
    void disposeGateObject(cGate *gate, bool checkConnected);

    // internal: add a new gatedesc by expanding gatedescv[]
    cGate::Desc *addGateDesc(const char *name, cGate::Type type, bool isVector);

    // internal: finds a gate descriptor with the given name in gatedescv[];
    // ignores (but returns) potential "$i"/"$o" suffix in gatename
    int findGateDesc(const char *gatename, char& suffix) const;

    // internal: like findGateDesc(), but throws an error if the gate does not exist
    cGate::Desc *gateDesc(const char *gatename, char& suffix) const;

    // internal: helper for setGateSize()
    void adjustGateDesc(cGate *g, cGate::Desc *newvec);

    // internal: called as part of the destructor
    void clearGates();

    // internal: builds submodules and internal connections for this module
    virtual void doBuildInside();

  public:
#ifdef USE_OMNETPP4x_FINGERPRINTS
    // internal: returns OMNeT++ V4.x compatible module ID
    int getVersion4ModuleId() const { return version4ModuleId; }
#endif

    // internal: may only be called between simulations, when no modules exist
    static void clearNamePools();

    // internal utility function. Takes O(n) time as it iterates on the gates
    int gateCount() const;

    // internal utility function. Takes O(n) time as it iterates on the gates
    cGate *gateByOrdinal(int k) const;

    // internal: calls refreshDisplay() recursively
    virtual void callRefreshDisplay() override;

    // internal: return the canvas if exists, or nullptr if not (i.e. no create-on-demand)
    cCanvas *getCanvasIfExists() {return canvas;}

    // internal: return the 3D canvas if exists, or nullptr if not (i.e. no create-on-demand)
    cOsgCanvas *getOsgCanvasIfExists() {return osgCanvas;}

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
    virtual void forEachChild(cVisitor *v) override;

    /**
     * Sets object's name. Redefined to update the stored fullName string.
     */
    virtual void setName(const char *s) override;

    /**
     * Returns the full name of the module, which is getName() plus the
     * index in square brackets (e.g. "module[4]"). Redefined to add the
     * index.
     */
    virtual const char *getFullName() const override;

    /**
     * Returns the full path name of the module. Example: <tt>"net.node[12].gen"</tt>.
     * The original getFullPath() was redefined in order to hide the global cSimulation
     * instance from the path name.
     */
    virtual std::string getFullPath() const override;

    /**
     * Overridden to add the module ID.
     */
    virtual std::string str() const override;
    //@}

    /** @name Setting up the module. */
    //@{

    /**
     * Adds a gate or gate vector to the module. Gate vectors are created with
     * zero size. When the creation of a (non-vector) gate of type cGate::INOUT
     * is requested, actually two gate objects will be created, "gatename$i"
     * and "gatename$o". The specified gatename must not contain a "$i" or "$o"
     * suffix itself.
     *
     * CAUTION: The return value is only valid when a non-vector INPUT or OUTPUT
     * gate was requested. nullptr gets returned for INOUT gates and gate vectors.
     */
    virtual cGate *addGate(const char *gatename, cGate::Type type, bool isvector=false);

    /**
     * Sets gate vector size. The specified gatename must not contain
     * a "$i" or "$o" suffix: it is not possible to set different vector size
     * for the "$i" or "$o" parts of an inout gate. Changing gate vector size
     * is guaranteed NOT to change any gate IDs.
     */
    virtual void setGateSize(const char *gatename, int size);

    /**
     * Helper function for implementing NED's "gate++" syntax.
     * Returns the next unconnected gate from an input or output gate vector,
     * or input/output half of an inout vector. When gatename names an inout gate
     * vector, the suffix parameter should be set to 'i' or 'o' to select
     * "gatename$i" or "gatename$o"; otherwise suffix should be zero.
     * The inside parameter selects whether to use isConnectedInside() or
     * isConnectedOutside() to test if the gate is connected. The expand
     * parameter tells whether the gate vector should be expanded if all its
     * gates are used up.
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
    virtual void finalizeParameters() override;

    /**
     * In compound modules, this method should be called to create submodules
     * and internal connections after module creation.
     *
     * This method delegates to doBuildInside(), switching the context to this
     * module for the duration of the call (see simulation.setContextModule()).
     *
     * @see doBuildInside()
     */
    void buildInside();
    //@}

    /** @name Information about the module itself. */
    //@{

    /**
     * Convenience function. Returns true this is a simple module
     * (i.e. subclassed from cSimpleModule), false otherwise.
     */
    virtual bool isSimple() const;

    /**
     * Redefined from cComponent to return KIND_MODULE.
     */
    virtual ComponentKind getComponentKind() const override  {return KIND_MODULE;}

    /**
     * Returns true if this module is a placeholder module, i.e.
     * represents a remote module in a parallel simulation run.
     */
    virtual bool isPlaceholder() const  {return false;}

    /**
     * Returns the module containing this module. For the system module,
     * it returns nullptr.
     */
    virtual cModule *getParentModule() const override;

    /**
     * Convenience method: casts the return value of getComponentType() to cModuleType.
     */
    cModuleType *getModuleType() const  {return (cModuleType *)getComponentType();}

    /**
     * Return the properties for this module. Properties cannot be changed
     * at runtime. Redefined from cComponent.
     */
    virtual cProperties *getProperties() const override;

    /**
     * Returns true if this module is in a module vector.
     */
    bool isVector() const  {return vectorSize>=0;}

    /**
     * Returns the index of the module if it is in a module vector, otherwise 0.
     */
    int getIndex() const  {return vectorIndex;}

    /**
     * Returns the size of the module vector the module is in. For non-vector
     * modules it returns 1.
     */
    int getVectorSize() const  {return vectorSize<0 ? 1 : vectorSize;}

    /**
     * Alias for getVectorSize().
     */
    _OPPDEPRECATED int size() const  {return getVectorSize();}
    //@}

    /** @name Submodule access. */
    //@{
    /**
     * Returns true if the module has submodules, and false otherwise.
     * To enumerate the submodules use SubmoduleIterator.
     */
    virtual bool hasSubmodules() const {return firstSubmodule!=nullptr;}

    /**
     * Finds a direct submodule with the given name and index, and returns
     * its module ID. If the submodule was not found, returns -1. Index
     * must be specified exactly if the module is member of a module vector.
     */
    virtual int findSubmodule(const char *name, int index=-1) const;

    /**
     * Finds a direct submodule with the given name and index, and returns
     * its pointer. If the submodule was not found, returns nullptr.
     * Index must be specified exactly if the module is member of a module vector.
     */
    virtual cModule *getSubmodule(const char *name, int index=-1) const;

    /**
     * Finds a module in the module tree, given by its absolute or relative path.
     * The path is a string of module names separated by dots; the special
     * module name ^ (caret) stands for the parent module. If the path starts
     * with a dot or caret, it is understood as relative to this module,
     * otherwise it is taken to mean an absolute path. For absolute paths,
     * inclusion of the toplevel module's name in the path is optional.
     * The toplevel module may also be referred to as "<root>". Returns
     * nullptr if the module was not found or the empty string was given as
     * input.
     *
     * Examples:
     *   "" means nullptr.
     *   "." means this module;
     *   "<root>" means the toplevel module;
     *   ".sink" means the sink submodule of this module;
     *   ".queue[2].srv" means the srv submodule of the queue[2] submodule;
     *   "^.host2" or ".^.host2" means the host2 sibling module;
     *   "src" or "<root>.src" means the src submodule of the toplevel module;
     *   "Net.src" also means the src submodule of the toplevel module, provided
     *   it is called Net.
     *
     *  @see cSimulation::getModuleByPath()
     */
    virtual cModule *getModuleByPath(const char *path) const;
    //@}

    /** @name Gates. */
    //@{

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
     * gate "port$o[3]". Throws an error if the gate does not exist.
     * The presence of the index parameter decides whether a vector or a scalar
     * gate will be looked for.
     */
    virtual cGate *gateHalf(const char *gatename, cGate::Type type, int index=-1);

    /**
     * Returns the "$i" or "$o" part of an inout gate, depending on the type
     * parameter. That is, gateHalf("port", cGate::OUTPUT, 3) would return
     * gate "port$o[3]". Throws an error if the gate does not exist.
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
     * Gate names with the "$i" or "$o" suffix are also accepted.
     */
    virtual bool hasGate(const char *gatename, int index=-1) const;

    /**
     * Returns the ID of the gate specified by name and index. Inout gates
     * cannot be specified (since they are actually two gate objects, not one),
     * only with a "$i" or "$o" suffix. Returns -1 if the gate does not exist.
     * The presence of the index parameter decides whether a vector or a scalar
     * gate will be looked for.
     */
    virtual int findGate(const char *gatename, int index=-1) const;

    /**
     * Returns a gate by its ID. It throws an error for invalid (or stale) IDs.
     *
     * Note: as of \opp 4.0, gate IDs are no longer small integers and are
     * not suitable for enumerating all gates of a module. Use GateIterator
     * for that purpose.
     */
    virtual cGate *gate(int id);

    /**
     * Returns a gate by its ID. It throws an error for invalid (or stale) IDs.
     *
     * Note: as of \opp 4.0, gate IDs are no longer small integers and are
     * not suitable for enumerating all gates of a module. Use GateIterator
     * for that purpose.
     */
    const cGate *gate(int id) const {return const_cast<cModule *>(this)->gate(id);}

    /**
     * Deletes a gate, gate pair, or gate vector. Note: individual gates
     * in a gate vector and one side of an inout gate (i.e. "foo$i")
     * cannot be deleted. IDs of deleted gates will not be reused later.
     */
    virtual void deleteGate(const char *gatename);

    /**
     * Returns the names of the module's gates. For gate vectors and inout gates,
     * only the base name is returned (without gate index, "[]" or the "$i"/"$o"
     * suffix). Zero-size gate vectors will also be included.
     *
     * The strings in the returned array do not need to be deallocated and
     * must not be modified.
     *
     * @see gateType(), isGateVector(), gateSize()
     */
    virtual std::vector<const char *> getGateNames() const;

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
     * non-vector gates, and 0 if the gate does not exist or the vector has size 0.
     * (Zero-size vectors are represented by a single gate whose size() returns 0.)
     * Gate names with the "$i" or "$o" suffix are also accepted.  Throws an error if
     * there is no such gate or gate vector.
     *
     * Note: The gate vector size can also be obtained by calling the cGate::size()
     * method of any gate object.
     */
    virtual int gateSize(const char *gatename) const;

    /**
     * For vector gates, it returns the ID of gate 0 in the vector, even if the
     * gate size is currently zero. All gates in the vector can be accessed
     * by ID = gateBaseId + index. For scalar gates, it returns the ID of the
     * gate. If there is no such gate or gate vector, an error gets thrown.
     *
     * Note: Gate IDs are guaranteed to be stable, i.e. they do not change if
     * the gate vector gets resized, or other gates get added/removed.
     */
    virtual int gateBaseId(const char *gatename) const;

    /**
     * For compound modules, it checks if all gates are connected inside
     * the module (it returns <tt>true</tt> if they are OK); for simple
     * modules, it returns <tt>true</tt>. This function is called during
     * network setup.
     */
    virtual bool checkInternalConnections() const;

    /**
     * This method is invoked as part of a send() call in another module.
     * It is called when the message arrives at a gates in this module which
     * is not further connected, that is, the gate's getNextGate() method
     * returns nullptr. The default, cModule implementation reports an error
     * ("message arrived at a compound module"), and the cSimpleModule
     * implementation inserts the message into the FES after some processing.
     */
    virtual void arrived(cMessage *msg, cGate *ongate, simtime_t t);
    //@}

    /** @name Utilities. */
    //@{
    /**
     * Searches for the parameter in the parent modules, up to the system
     * module. If the parameter is not found, throws cRuntimeError.
     */
    virtual cPar& getAncestorPar(const char *parname);

    /**
     * Returns the default canvas for this module, creating it if it hasn't
     * existed before.
     */
    virtual cCanvas *getCanvas() const;

    /**
     * Returns the default 3D (OpenSceneGraph) canvas for this module, creating
     * it if it hasn't existed before.
     */
    virtual cOsgCanvas *getOsgCanvas() const;

    /**
     * Sets whether built-in animations are requested on this module's
     * graphical inspector.
     */
    virtual void setBuiltinAnimationsAllowed(bool enabled) {setFlag(FL_BUILTIN_ANIMATIONS, enabled);}

    /**
     * Returns true if built-in animations are requested on this module's
     * graphical inspector, and false otherwise.
     */
    virtual bool getBuiltinAnimationsAllowed() const {return flags & FL_BUILTIN_ANIMATIONS;}
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
    virtual void callInitialize() override;

    /**
     * Interface for calling initialize() from outside. It does a single stage
     * of initialization, and returns <tt>true</tt> if more stages are required.
     */
    virtual bool callInitialize(int stage) override;

    /**
     * Interface for calling finish() from outside.
     */
    virtual void callFinish() override;
    //@}

    /** @name Dynamic module creation. */
    //@{

    /**
     * Creates a starting message for modules that need it (and recursively
     * for its submodules).
     */
    virtual void scheduleStart(simtime_t t);

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
     * NOTE: THIS METHOD CHANGES THE MODULE ID. To maintain a time-independent
     * moduleId-to-fullPath mapping, this method will cause this module, and all
     * modules under it in the module hierarchy, to be assigned a new ID.
     * This usually causes no problem in the simulation's operation, but
     * if your model stores module IDs somewhere, you'll need to invalidate or
     * update them manually.
     *
     * This function could bypass several rules which are enforced when you
     * build the model using NED, so you must observe the following:
     *
     *  -# you cannot insert the module under one of its own submodules.
     *     This is checked by this function.
     *  -# gates of the module cannot be connected when you move it.
     *     If you moved a module which is connected to its parent module
     *     or to other submodules, you'd create connections that do not obey
     *     the module hierarchy, and this is not permitted. This rule is
     *     also enforced by the implementation of this function.
     *  -# it is recommended that the module name be made unique among the
     *     submodules of its new parent.
     *  -# be aware that if the module is part of a module vector, its
     *     isVector(), getIndex() and size() functions will continue to deliver
     *     the same info -- although other elements of the vector will not
     *     necessarily be present under the same parent module.
     *
     *  @see getId()
     */
    virtual void changeParentTo(cModule *mod);
    //@}
};

}  // namespace omnetpp

#endif
