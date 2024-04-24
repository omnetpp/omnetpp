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

class cMessage;
class cGate;
class cSimulation;
class cModuleType;
class cCanvas;
class cOsgCanvas;
struct SendOptions;

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
 * @ingroup ModelComponents
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
     * ```
     * for (cModule::GateIterator it(module); !it.end(); ++it) {
     *     cGate *gate = *it;
     *     ...
     * }
     * ```
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
        GateIterator(const cModule *module) : module(module) {reset();}

        /**
         * Reinitializes the iterator.
         */
        void reset();

        /**
         * Returns the current gate, or nullptr if the iterator is not
         * at a valid position.
         */
        cGate *operator*() const {cGate *result=current(); ASSERT(result||end()); return result;}

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
     * Iteration order corresponds to declaration order in NED files.
     *
     * It is not allowed to delete or insert modules during iteration.
     * If such thing occurs, restart the iteration by calling reset(),
     * or query the submodule list in advance.
     *
     * Usage:
     * ```
     * for (cModule::SubmoduleIterator it(module); !it.end(); ++it) {
     *     cModule *submodule = *it;
     *     ...
     * }
     * ```
     *
     * When submodules may be created in the loop body:
     *
     * ```
     * for (cModule::SubmoduleIterator it(module); !it.end();) {
     *     cModule *submodule = *it;
     *     ...
     *     ... (code that may create/delete/move submodules)
     *     ...
     *     if (!it.changesDetected())
     *         it++;
     *     else
     *         it.reset(); // start over
     * }
     * ```
     */
    class SIM_API SubmoduleIterator
    {
      private:
        const cModule *parent;
        int scalarsSlot;
        int vectorsSlot;
        int vectorIndex;
        cModule *currentScalar;
        cModule *currentVector;
        cModule *current; // element the iterator is at
        int initialModuleChangeCount;

      private:
        void advance();
        void bumpScalars();
        void bumpVectors();

      public:
        /**
         * Constructor. It takes the parent module on which to iterate.
         */
        SubmoduleIterator(const cModule *module) : parent(module) {reset();}

        /**
         * Reinitializes the iterator.
         */
        void reset();

        /**
         * Returns a pointer to the current module. Returns nullptr if the iterator
         * has reached the end of the list.
         */
        cModule *operator*() const {return current;}

        /**
         * Returns true if the iterator reached the end of the list.
         */
        bool end() const {return current == nullptr;}

        /**
         * Prefix increment operator (++it). Moves the iterator to the next
         * submodule. It has no effect if the iterator has reached either
         * end of the list.
         */
        SubmoduleIterator& operator++() {advance(); return *this;}

        /**
         * Postfix increment operator (it++). Moves the iterator to the next
         * submodule, and returns the iterator's previous state. It has no effect
         * if the iterator has reached either end of the list.
         */
        SubmoduleIterator operator++(int) {SubmoduleIterator tmp(*this); advance(); return tmp;}

        /**
         * Returns true if it was detected that the list of submodules has
         * changed since the the start of the iteration due to insertion,
         * removal, or possibly as a byproduct of name/index change. When this
         * method returns true, incrementing the iterator will result in an
         * exception being thrown. One possible solution for when changesDetected()
         * returns true is starting over the iteration (see reset()).
         */
        bool changesDetected() const;
    };

    /**
     * @brief Walks along the channels inside a module, that is, the channels
     * among the module and its submodules.
     *
     * This is the same set of channels whose getParentModule() would return
     * the iterated module.
     *
     * Usage:
     * ```
     * for (cModule::ChannelIterator it(module); !it.end(); ++it) {
     *     cChannel *channel = *it;
     *     ...
     * }
     * ```
     */
    class SIM_API ChannelIterator
    {
      private:
        const cModule *parent;
        int slot; // index into the channels[] array
        cChannel *current;

      private:
        void advance();

      public:
        /**
         * Constructor. It takes the parent module on which to iterate.
         */
        ChannelIterator(const cModule *module) : parent(module) {reset();}

        /**
         * Reinitializes the iterator.
         */
        void reset();

        /**
         * Returns a pointer to the current channel. Returns nullptr if the iterator
         * has reached the end of the list.
         */
        cChannel *operator*() const {return current;}

        /**
         * Returns true if the iterator reached the end of the list.
         */
        bool end() const  {return current == nullptr;}

        /**
         * Prefix increment operator (++it). Moves the iterator to the next
         * channel. It has no effect if the iterator has reached either
         * end of the list.
         */
        ChannelIterator& operator++() { advance(); return *this;}

        /**
         * Postfix increment operator (it++). Moves the iterator to the next
         * channel, and returns the iterator's previous state. It has no effect
         * if the iterator has reached either end of the list.
         */
        ChannelIterator operator++(int) {ChannelIterator tmp(*this); advance(); return tmp;}
    };

  private:
    enum {
        FL_BUILDINSIDE_CALLED = 1 << 10, // whether buildInside() has been called
        FL_RECORD_EVENTS      = 1 << 11, // enables recording events in this module
        FL_BUILTIN_ANIMATIONS = 1 << 12, // whether built-in animations are requested on this module's graphical inspector
    };

  private:
    mutable const char *fullPath = nullptr; // module full path string (computed lazily)
    mutable opp_pooledstring fullName = nullptr; // buffer to store full name of object

    cModule *parentModule = nullptr;
    struct SubmoduleVector {
        std::string name;
        std::vector<cModule*> array;
    };
    struct SubcomponentData {
        std::vector<cModule*> scalarSubmodules; // scalar submodules in creation order
        std::vector<SubmoduleVector> submoduleVectors; // submodule vectors in creation order
        std::vector<cChannel*> channels;  // channels among submodules
        int submoduleChangeCount = 0;
    };
    SubcomponentData *subcomponentData = nullptr;

    typedef std::set<cGate::Name> GateNamePool;
    static GateNamePool gateNamePool;
    cGate::Desc *gateDescArray = nullptr;  // array with one element per gate or gate vector
    int gateDescArraySize = 0;  // size of the descv array

    int vectorIndex = -1;       // index if module vector, -1 otherwise

    mutable cCanvas *canvas = nullptr;        // nullptr when unused
    mutable cOsgCanvas *osgCanvas = nullptr;  // nullptr when unused

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
    virtual void setInitialNameAndIndex(const char *name, int index);

    // internal: called from setName() and setIndex()
    void updateFullName();

    // internal: called from setName(), setIndex(), and changeParentTo()
    void invalidateFullPathRec();

    // internal: used by changeParentTo()
    void reassignModuleIdRec();

    // internal: inserts a submodule. Called as part of the module creation process.
    void insertSubmodule(cModule *mod);

    // internal: removes a submodule
    void removeSubmodule(cModule *mod);

    // internal: inserts a channel. Called from cGate::connectTo()
    void insertChannel(cChannel *channel);

    // internal: removes a channel
    void removeChannel(cChannel *channel);

    // internal: returns the ptr array for a submodule vector, exception if not found
    std::vector<cModule*>& getSubmoduleArray(const char *name) const;

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

    // internal: helper for deleteModule()
    virtual void doDeleteModule();

    // helper for findModuleByPath()
    virtual cModule *doFindModuleByPath(const char *s) const override;

  public:
    // internal: may only be called between simulations, when no modules exist
    static void clearNamePools();

    // internal utility function. Takes O(n) time as it iterates on the gates
    int gateCount() const;

    // internal utility function. Takes O(n) time as it iterates on the gates
    cGate *gateByOrdinal(int k) const;

    // internal: calls refreshDisplay() recursively
    virtual void callRefreshDisplay() override;

    // internal: calls preDelete() recursively
    virtual void callPreDelete(cComponent *root) override;

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
     * an ID to the module, inserting it into the global `simulation`
     * object (see cSimulation), etc.).
     */
    cModule();

    /**
     * Destructor. 
     * 
     * Note: It is not allowed delete modules directly, only via
     * the deleteModule() method.
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
     * Sets module's name. If the module is part of a submodule vector,
     * the name change implies relocating the module into another
     * submodule vector (which must already exist, and the same index
     * position in it must be available and empty).
     *
     * @see setIndex(), setNameAndIndex()
     */
    virtual void setName(const char *s) override;

    /**
     * Returns the full name of the module, which is getName() plus the
     * index in square brackets (e.g. "module[4]"). Redefined to add the
     * index.
     */
    virtual const char *getFullName() const override;

    /**
     * Returns the full path name of the module. Example: `"net.node[12].gen"`.
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
     * Sets the module's index. The module must be part of a submodule
     * vector. The index change implies relocating the module into another
     * slot in the same submodule vector; the destination index position
     * must be available and empty.
     */
    virtual void setIndex(int index);

    /**
     * Sets the module's name and index. If index is not -1, the change implies
     * relocating the module into another submodule vector and index position.
     * The destination submodule vector must already exist, and its indexth
     * slot must be available and empty.
     */
    virtual void setNameAndIndex(const char *name, int index=-1);

    /**
     * Adds a scalar gate of the given type to the module. If type is INOUT,
     * two gate objects will be created, "gatename$i" and "gatename$o" (the
     * specified gatename must not contain a "$i" or "$o" suffix itself).
     *
     * Returns a pointer to the new gate if type is INPUT or OUTPUT, and nullptr
     * if type is INOUT.
     */
    virtual cGate *addGate(const char *gatename, cGate::Type type);

    /**
     * Adds a gate vector of the given type and size to the module. If type is
     * INOUT, two gate vectors will be created, "gatename$i[]" and "gatename$o[]"
     * (the specified gatename must not contain a "$i" or "$o" suffix itself).
     */
    virtual void addGateVector(const char *gatename, cGate::Type type, int size);

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
     * Returns true if the C++ class of this module is a subclass of cSimpleModule,
     * and false otherwise, i.e. it is equivalent to a `dynamic_cast`.
     *
     * In contrast, to check whether this module is a NED compound or simple module
     * (i.e. whether it was declared with the "module" or with the "simple" keyword in NED),
     * use `cModuleType::isSimple()`: `getModuleType()->isSimple()`.
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
    virtual cModule *getParentModule() const override  { return parentModule; }

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
    bool isVector() const  {return vectorIndex != -1;}

    /**
     * Returns the index of the module in a module vector. If the module is not
     * member of a module vector, an exception is thrown.
     */
    int getIndex() const;

    /**
     * Returns the size of the module vector the module is in. If the module
     * is not member of a module vector, an exception is thrown.
     */
    int getVectorSize() const;
    //@}

    /** @name Submodule access. */
    //@{
    /**
     * Returns true if the module has submodules, and false otherwise.
     * To enumerate the submodules use SubmoduleIterator.
     */
    virtual bool hasSubmodules() const;

    /**
     * Returns true if the module has a submodule vector (not necessarily of
     * nonzero size) of the given name, and false otherwise.
     */
    virtual bool hasSubmoduleVector(const char *name) const;

    /**
     * Returns the names of the module's submodule vectors, including zero-size
     * submodule vectors.
     */
    virtual std::vector<std::string> getSubmoduleVectorNames() const;

    /**
     * Returns the size of the submodule vector of the given name. Throws
     * an error if there is no such submodule vector.
     */
    virtual int getSubmoduleVectorSize(const char *name) const;

    /**
     * Adds a blank submodule vector with the given name and size. Throws an error
     * if a submodule vector with the given name already exists.
     *
     * Important: This operation does NOT actually create any submodules.
     * The slots in the vector will be nullptr, i.e. getSubmodule(name,index)
     * returns nullptr for all indices.
     *
     * Modules can be added one by one by using cModuleType::create(name,
     * parentModule, index). Deleting a module (via cModule::deleteModule())
     * which is part of a submodule vector will also remove it from the
     * submodule vector and set its slot to nullptr.
     */
    virtual void addSubmoduleVector(const char *name, int size);

    /**
     * Deletes the submodule vector with the given name. All submodules
     * in the given vector will be deleted. Throws an error if a submodule
     * vector with the given name does not exist.
     */
    virtual void deleteSubmoduleVector(const char *name);

    /**
     * Resizes the given submodule vector, filling new elements with nullptr.
     * A submodule vector can be extended any time; it can be shrunk only if it
     * contains no submodules in the index range which is going to be removed.
     * Throws an error if there is no such submodule vector, or the removed
     * range contains submodules. Modules can be removed from the submodule
     * vector by calling their deleteModule() method.
     */
    virtual void setSubmoduleVectorSize(const char *name, int size);

    /**
     * Creates and initializes a submodule. If index is present, then the
     * submodule becomes an element in a submodule vector; the submodule
     * vector of the given name must already exist, have a sufficient size
     * (size>index), and the indexth position must be empty.
     *
     * Note: This method just delegates to cModuleType::createScheduleInit().
     * If parameters need to be set or gates need to be connected before
     * building the internal structure (in case type is a compound module)
     * and initializing the module, use cModuleType::create() instead of
     * this method.
     *
     * @see cModuleType::createScheduleInit(), cModuleType::create()
     */
    virtual cModule *addSubmodule(cModuleType *type, const char *name, int index=-1);

    /**
     * Returns true if a submodule with the given name (and index) exists,
     * and false otherwise. Index must be specified exactly if the module
     * is member of a module vector.
     */
    virtual bool hasSubmodule(const char *name, int index=-1) const;

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
     * Returns the names of the module's submodules and submodule vectors,
     * including zero-size submodule vectors.
     */
    virtual std::vector<std::string> getSubmoduleNames() const;

    /**
     * Returns true of the given module has this module as an ancestor, and
     * false otherwise.
     */
    virtual bool containsModule(cModule *module) const;
    //@}

    /** @name Gates. */
    //@{

    /**
     * Returns true if the module has any gates, and false otherwise.
     * If it does, getGateNames() or GateIterator be used to enumerate them.
     */
    virtual bool hasGates() const;

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
     * Note: As of \opp 4.0, gate IDs are no longer small integers and are
     * not suitable for enumerating all gates of a module. Use GateIterator
     * for that purpose.
     */
    virtual cGate *gate(int id);

    /**
     * Returns a gate by its ID. It throws an error for invalid (or stale) IDs.
     *
     * Note: As of \opp 4.0, gate IDs are no longer small integers and are
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
     * @see gateType(), isGateVector(), gateSize()
     */
    virtual std::vector<std::string> getGateNames() const;

    /**
     * Returns the type of the gate (or gate vector) with the given name.
     * Gate names with the "$i" or "$o" suffix are also accepted. Throws
     * an error if there is no such gate or gate vector.
     */
    virtual cGate::Type gateType(const char *gatename) const;

    /**
     * Returns true if there is a gate vector with the given name, and false
     * otherwise. Names with the "$i" and "$o" suffix are also accepted.
     * Zero-size gate vectors will also be included.
     */
    virtual bool hasGateVector(const char *gatename) const;

    /**
     * Returns whether the given gate is a gate vector. Gate names with the "$i"
     * or "$o" suffix are also accepted.  Throws an error if there is no
     * such gate or gate vector.
     */
    virtual bool isGateVector(const char *gatename) const;

    /**
     * Returns the size of the gate vector with the given name. Gate names with
     * the "$i" or "$o" suffix are also accepted.  Throws an error if there is
     * no such gate, or it is not a gate vector.
     *
     * @see cGate::getVectorSize()
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
     * the module (it returns `true` if they are OK); for simple
     * modules, it returns `true`. This function is called during
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
    virtual void arrived(cMessage *msg, cGate *ongate, const SendOptions& options, simtime_t t);
    //@}

    /** @name Utilities. */
    //@{
    /**
     * Searches for the parameter in the module and parent modules, up to the
     * system module. If the parameter is not found, throws cRuntimeError.
     *
     * This method is deprecated because it represents wrong practice.
     */
    [[deprecated]] virtual cPar& getAncestorPar(const char *parname);

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
     * of initialization, and returns `true` if more stages are required.
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
     * Deletes the module and recursively all its submodules.
     *
     * A running module can also delete itself. When an activity()-based
     * simple module deletes itself from within its activity(), the
     * deleteModule() call will not return (it throws an exception which
     * gets caught by the simulation kernel, and the simulation kernel
     * will delete the module).
     *
     * When a handleMessage()-based module deletes itself, the deleteModule()
     * returns normally -- then, of course, the code should not try to
     * access data members or functions of the deleted module, and should
     * return as soon as possible.
     *
     * It is not allowed to delete the system module. Also, a module cannot
     * be deleted while its initialization is in progress.
     */
    virtual void deleteModule();

    /**
     * Moves the module under a new parent module. This functionality
     * may be useful for some (rare) mobility scenarios.
     *
     * Note: THIS METHOD CHANGES THE MODULE ID. To maintain a time-independent
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
     *  -# if the module is part of a module vector, the new parent must already
     *     have a submodule vector of the same name, with size>index, and
     *     the corresponding (indexth) array element being vacant (nullptr).
     *
     *  @see getId()
     */
    virtual void changeParentTo(cModule *mod);
    //@}
};

}  // namespace omnetpp

#endif
