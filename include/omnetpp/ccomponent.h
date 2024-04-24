//==========================================================================
//   CCOMPONENT.H  -  header for
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

#ifndef __OMNETPP_CCOMPONENT_H
#define __OMNETPP_CCOMPONENT_H

#include <vector>
#include <unordered_set>
#include "simkerneldefs.h"
#include "cownedobject.h"
#include "cpar.h"
#include "csoftowner.h"
#include "simtime.h"
#include "cenvir.h"
#include "clistener.h"
#include "clog.h"
#include "distrib.h"

namespace omnetpp {

class cComponentType;
class cProperties;
class cDisplayString;
class cRNG;
class cStatistic;
class cResultRecorder;

/**
 * @brief Common base for module and channel classes.
 *
 * cComponent provides parameters, properties, display string, RNG mapping,
 * initialization and finalization support, simulation signals support,
 * and several other services to its subclasses.
 *
 * @ingroup ModelComponents
 */
class SIM_API cComponent : public cSoftOwner //implies noncopyable
{
    friend class cComponentDescriptor; // listener lists, etc
    friend class cComponent__SignalListenerListDescriptor;  // sim_std.msg
    friend class cPar; // needs to call handleParameterChange()
    friend class cChannel; // allow access to FL_INITIALIZED, FL_DELETING and releaseLocalListeners()
    friend class cModule; // allow access to FL_INITIALIZED, FL_DELETING, releaseLocalListeners() and repairSignalFlags()
    friend class cSimpleModule; // allow access to FL_INITIALIZED, FL_DELETING, releaseLocalListeners() and repairSignalFlags()
    friend class cGate;   // because of repairSignalFlags()
    friend class cSimulation; // sets componentId
    friend class cResultListener; // invalidateCachedResultRecorderLists()
  public:
    enum ComponentKind { KIND_MODULE, KIND_CHANNEL, KIND_OTHER };

  private:
    enum {
      FL_PARAMSFINALIZED  = 1 << 2, // whether finalizeParameters() has been called
      FL_INITIALIZED      = 1 << 3, // whether initialize() has completed for this module
      FL_DELETING         = 1 << 4, // module or channel is being deleted (via deleteModule(), disconnect(), etc.)
      FL_DISPSTR_CHECKED  = 1 << 5, // for hasDisplayString(): whether the FL_DISPSTR_NOTEMPTY flag is valid
      FL_DISPSTR_NOTEMPTY = 1 << 6, // for hasDisplayString(): whether the display string is not empty
      FL_LOGLEVEL_SHIFT   = 7,      // 3 bits wide
    };

  private:
    typedef internal::cParImpl cParImpl;

    cComponentType *componentType;  // component type object
    cSimulation *simulation; // the simulation it belongs to
    int componentId;   // id in cSimulation

    short lastCompletedInitStage;

    short rngMapSize;  // size of the rngMap array (RNGs with index >= rngMapSize are mapped one-to-one to global RNGs)
    int *rngMap;       // maps local RNG numbers (may be nullptr if rngMapSize==0)

    short numPars;
    short parArraySize;
    cPar *parArray;  // array of cPar objects

    mutable cDisplayString *displayString; // created on demand
    opp_pooledstring displayName = nullptr;  // optional display name

  public:
    struct SignalListenerList {  // public for inspectors
        simsignal_t signalID;
        cIListener **listeners; // nullptr-terminated array

        SignalListenerList() {signalID=SIMSIGNAL_NULL; listeners=nullptr;}
        const char *getSignalName() const;
        std::string str() const;
        void dispose() {delete [] listeners;}
        bool addListener(cIListener *l);
        bool removeListener(cIListener *l);
        int findListener(cIListener *l) const;
        bool hasListener() const {return listeners && listeners[0];}
        int countListeners() const;
        cIListener *getListener(int k) const {return listeners[k];} // unsafe
        static bool gt(const SignalListenerList& e1, const SignalListenerList& e2) {return e1.signalID > e2.signalID;}
    };

  private:
    typedef std::vector<SignalListenerList> SignalTable;
    SignalTable *signalTable; // ordered by signalID so we can do binary search

    std::unordered_set<void**> *selfPointers = nullptr;

    // string-to-simsignal_t mapping
    static struct SignalNameMapping {
        std::map<std::string,simsignal_t> signalNameToID;
        std::map<simsignal_t,std::string> signalIDToName;
    } *signalNameMapping;  // must be dynamically allocated on first access so that registerSignal() can be invoked from static initialization code
    static int lastSignalID;

    // for hasListeners()/mayHaveListeners()
    static std::vector<int> signalListenerCounts;  // index: signalID, value: number of listeners anywhere

    // stack of listener lists being notified, to detect concurrent modification
    static cIListener **notificationStack[];
    static int notificationSP;

    // whether only signals declared in NED via @signal are allowed to be emitted
    static bool checkSignals;

    // for caching the result of getResultRecorders()
    struct ResultRecorderList {
        const cComponent *component;
        std::vector<cResultRecorder*> recorders;
    };

    // for getResultRecorders(); static because we don't want to increase cComponent's size
    static std::vector<ResultRecorderList*> cachedResultRecorderLists;

  private:
    SignalListenerList *findListenerList(simsignal_t signalID) const;
    SignalListenerList *findOrCreateListenerList(simsignal_t signalID);
    void throwInvalidSignalID(simsignal_t signalID) const;
    void removeListenerList(simsignal_t signalID);
    void checkNotFiring(simsignal_t, cIListener **listenerList);
    template<typename T> void fire(cComponent *src, simsignal_t signalID, T x, cObject *details);
    void fireFinish();
    void releaseLocalListeners();
    const SignalListenerList& getListenerList(int k) const {return (*signalTable)[k];} // for inspectors
    int getSignalTableSize() const {return signalTable ? signalTable->size() : 0;} // for inspectors
    void collectResultRecorders(std::vector<cResultRecorder*>& result) const;
    virtual void doEmit(simsignal_t signalID, intval_t i, cObject *details);
    virtual void doEmit(simsignal_t signalID, uintval_t i, cObject *details);

  protected:
    virtual cModule *doFindModuleByPath(const char *s) const = 0;

  public:
    // internal: used by log mechanism
    LogLevel getLogLevel() const { return (LogLevel)((flags >> FL_LOGLEVEL_SHIFT) & 0x7); }
    virtual void setLogLevel(LogLevel logLevel);

    // internal: invoked from within cEnvir::preconfigure(component)
    virtual void setRNGMap(short size, int *map) {rngMapSize=size; rngMap=map;}

    // internal: sets associated cComponentType for the component;
    // called as part of the creation process.
    virtual void setComponentType(cComponentType *componenttype);

    // internal: adds a new parameter to the component; called as part of the creation process
    virtual void addPar(cParImpl *value);

    // internal: reallocates paramv (size must be >= numparams)
    void reallocParamv(int size);

    // internal: save parameters
    virtual void recordParameters();
    virtual void recordParameterAsScalar(cPar *par);

    // internal: has finalizeParameters() been called?
    bool parametersFinalized() const {return flags&FL_PARAMSFINALIZED;}

    // internal: sets up @statistic-based result recording
    virtual void addResultRecorders();
    virtual void emitStatisticInitialValues();

    // internal: has initialize() been called?
    bool initialized() const {return flags&FL_INITIALIZED;}

    // internal: calls refreshDisplay() recursively
    virtual void callRefreshDisplay() = 0;

    // internal: calls preDelete() recursively
    virtual void callPreDelete(cComponent *root) = 0;

    // internal: used from Qtenv: find out if this module has a display string.
    // getDisplayString() would create the object immediately which we want to avoid.
    bool hasDisplayString();

    // internal: clears per-run signals-related data structures; to be invoked before each simulation run
    static void clearSignalState();

    // internal: clears signal registrations; to be invoked on exit
    static void clearSignalRegistrations();

    // internal: allocates a signalHasLocalListeners/signalHasAncestorListeners bit index to the
    // given signal and returns the corresponding mask (1<<index); returns 0 if there are no more
    // free bit indices. Result is remembered and returned in subsequent calls (until clearSignalState())
    static uint64_t getSignalMask(simsignal_t signalID);

    // internal: controls whether signals should be validated against @signal declarations in NED files
    static void setCheckSignals(bool b) {checkSignals = b;}
    static bool getCheckSignals() {return checkSignals;}

    // internal: for inspectors
    const std::vector<cResultRecorder*>& getResultRecorders() const;
    static void invalidateCachedResultRecorderLists();

    // experimental
    template<class T>
    void registerSelfPointer(T *&ptr) {
        ASSERT(dynamic_cast<cComponent*>(ptr) == this);
        if (!selfPointers)
            selfPointers = new std::unordered_set<void**>;
        selfPointers->insert((void**)&ptr);
    }

    template<class T>
    void deregisterSelfPointer(T *&ptr) {
        ASSERT(dynamic_cast<cComponent*>(ptr) == this);
        ASSERT(selfPointers != nullptr && selfPointers->find((void**)&ptr) != selfPointers->end());
        selfPointers->erase((void**)&ptr);
    }

  protected:
    /** @name Initialization, finalization, and various other hooks.
     *
     * Initialize and finish functions may be provided by the user,
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
     * is called with `0,1,...numstages-1` as argument. The default
     * implementation of numInitStages() and initialize(stage) provided here
     * defaults to single-stage initialization, that is, numInitStages()
     * returns 1 and initialize(stage) simply calls initialize() if stage is 0.
     */
    //@{

    /**
     * Multi-stage initialization hook. This default implementation does
     * single-stage init, that is, calls initialize() if stage is 0.
     */
    virtual void initialize(int stage) {if (stage==0) initialize();}

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
     * Finish hook. finish() is called after end of simulation if it
     * terminated without error. This default implementation does nothing.
     */
    virtual void finish();

    /**
     * This method is called by the simulation kernel to notify the module or
     * channel that the value of an existing parameter has changed. Redefining
     * this method allows simple modules and channels to be react on parameter
     * changes, for example by re-reading the value. This default implementation
     * does nothing.
     *
     * Notification is initially disabled until all parameters have been set up,
     * that is, until finalizeParameters() is invoked on this component. However,
     * notifications are enabled while the component goes through (single or
     * multi-stage) initialization. When implementing handleParameterChange(),
     * one must keep in mind that the component may not have been fully
     * initialized at the time of the call (e.g. may not have gone through all
     * init stages yet).
     *
     * One must be extremely careful when changing parameters from inside
     * handleParameterChange(), to avoid creating infinite notification loops.
     *
     * Note: Before \opp version 6.0, handleParameterChange() was disabled during
     * the whole initialization process, and handleParameterChange(nullptr) was
     * called after the last stage of the initialization. This is no longer so
     * in version 6.0 or later.
     */
    virtual void handleParameterChange(const char *parname);

    /**
     * This method is called on all components of the simulation by graphical
     * user interfaces like Qtenv, whenever GUI contents need to be refreshed
     * after processing some simulation events. Components that contain
     * visualization-related code are expected to override refreshDisplay(),
     * and move visualization code display string manipulation, canvas figures
     * maintenance, OSG scene graph update, etc.) into it.
     *
     * As it is unpredictable when and whether this method is invoked, the
     * simulation logic should not depend on it. It is advisable that code in
     * refreshDisplay() does not alter the state of the model at all. This
     * behavior is gently encouraged by having this method declared as const.
     * (Data members that do need to be updated inside refreshDisplay(), i.e.
     * those related to visualization, may be declared mutable to allow that).
     *
     * Qtenv invokes refreshDisplay() in Step and Run mode, after each event;
     * in Fast Run and Express Run mode, every time the screen is refreshed,
     * which is typically on the order of once per second.
     * Cmdenv does not invoke refreshDisplay() at all.
     *
     * Note that overriding refreshDisplay() is generally preferable to doing
     * display updates as part of event handling: it results in essentially
     * zero per-event runtime overhead, and potentially more consistent
     * information being displayed (as all components refresh their visualization,
     * not only the one which has just processed an event.)
     */
    virtual void refreshDisplay() const;

    /**
     * This method is called when a module tree or a channel object is deleted
     * (see cModule::deleteModule() and cGate::disconnect()) before any change
     * is done. The default implementation recurses on all modules and channels
     * that are going to be deleted. This method can be viewed as a dual of
     * component initialization, i.e. the initialize() function.
     *
     * The argument contains the module on which deleteModule() was called,
     * or the channel which being deleted e.g. due to gate disconnection.
     *
     * Overriding this method allows one to unsubscribe from model change
     * notifications or perform other deinitialization tasks to simplify
     * the job of the destructors.
     */
    virtual void preDelete(cComponent *root);
    //@}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor. Note that module and channel objects should not be created
     * directly, via their cComponentType objects. cComponentType::create()
     * will do all housekeeping associated with creating the module (assigning
     * an ID to the module, inserting it into the `simulation` object,
     * etc.).
     */
    cComponent(const char *name = nullptr);

    /**
     * Destructor.
     */
    virtual ~cComponent();
    //@}

    /** @name Redefined cObject functions */
    //@{
    /**
     * Redefined to include component parameters in the traversal as well.
     */
    virtual void forEachChild(cVisitor *v) override;

    /**
     * Returns the this pointer. Utility function for the logging macros.
     * Redefined to change the return type.
     */
    const cComponent *getThisPtr() const  {return this;}  //Note: nonvirtual
    //@}

    /**
     * Must be called after the component was created, and (with modules)
     * before buildInside(). It reads input parameters from omnetpp.ini,
     * and cModule extends this method to add gates to the module too
     * (as this is the earliest time parameter values are available,
     * and gate vector sizes may depend on parameters).
     */
    virtual void finalizeParameters();

    /** @name Misc. */
    //@{
    /**
     * Return the properties for this component. Properties cannot be changed
     * at runtime.
     */
    virtual cProperties *getProperties() const = 0;

    /**
     * Sets the display name of the component, an alternative name that may be
     * displayed on graphical user interfaces like Qtenv or the Sequence Chart.
     * Pass nullptr to clear the display name.
     */
    virtual void setDisplayName(const char *name);

    /**
     * Returns the display name of the module, an alternative name that may be
     * displayed on graphical user interfaces like Qtenv or the Sequence Chart.
     * Returns nullptr if a display name is not set.
     */
    virtual const char *getDisplayName() const {return displayName.c_str();}

    /**
     * Returns the associated component type. Guaranteed to be non-nullptr.
     */
    virtual cComponentType *getComponentType() const;

    /**
     * Returns the simulation the component is part of.
     */
    cSimulation *getSimulation() const {return simulation;}

    /**
     * Returns the component's ID in the simulation object (cSimulation).
     * Component IDs are guaranteed to be unique during a simulation run
     * (that is, IDs of deleted components are not reused for new components.)
     *
     * @see getSimulation(), cSimulation::getComponent(), cSimulation::getModule(), cSimulation::getChannel()
     */
    int getId() const  {return componentId;}

    /**
     * Returns the fully qualified NED type name of the component (i.e. the
     * simple name prefixed with the package name and any existing enclosing
     * NED type names).
     *
     * This method is a shortcut to `getComponentType()->getFullName()`.
     */
    virtual const char *getNedTypeName() const;

    /**
     * Returns a string that contains the NED type name and the full name
     * of the component in a human-friendly format. Example: "(StandardHost)host[2]"
     *
     * @see getClassAndFullName()
     */
    virtual std::string getNedTypeAndFullName() const;

    /**
     * Returns a string that contains the NED type name and the full path of the
     * component in a human-friendly format. Example: "(StandardHost)Network.subnet1.host[2]"
     *
     * @see getClassAndFullPath()
     */
    virtual std::string getNedTypeAndFullPath() const;

    /**
     * Returns the kind of the component.
     *
     * @see isModule(), isChannel()
     */
    virtual ComponentKind getComponentKind() const = 0;

    /**
     * Returns true for cModule and subclasses, otherwise false.
     */
    bool isModule() const  {return getComponentKind() == KIND_MODULE;}

    /**
     * Returns true for channels, and false otherwise.
     */
    bool isChannel() const  {return getComponentKind() == KIND_CHANNEL;}

    /**
     * Returns the module containing this module/channel. This is not necessarily
     * the same object as getOwner(), especially for channel objects. For the system
     * module, it returns nullptr.
     */
    virtual cModule *getParentModule() const = 0;

    /**
     * Returns the toplevel module in the current simulation.
     * This is a shortcut to getSimulation()->getSystemModule().
     */
    virtual cModule *getSystemModule() const;

    /**
     * Finds a module in the module tree, given by its absolute or relative path.
     * The path is a string of module names separated by dots; the special
     * module name ^ (caret) stands for the parent module. If the path starts
     * with a dot or caret, it is understood as relative to this module/channel,
     * otherwise it is taken to mean an absolute path. For absolute paths,
     * inclusion of the toplevel module's name in the path is optional.
     * The toplevel module may also be referred to as "<root>".
     *
     * This method never returns nullptr. If the module was not found,
     * an exception is thrown.
     *
     * Examples:
     *   "." means this module;
     *   "<root>" means the toplevel module;
     *   ".sink" means the sink submodule of this module;
     *   ".queue[2].srv" means the srv submodule of the queue[2] submodule;
     *   "^.host2" or ".^.host2" means the host2 sibling module;
     *   "src" or "<root>.src" means the src submodule of the toplevel module;
     *   "Net.src" also means the src submodule of the toplevel module, provided
     *   it is called Net.
     *
     * @see findModuleByPath(), cSimulation::getModuleByPath()
     */
    virtual cModule *getModuleByPath(const char *path) const;

    /**
     * Finds a module in the module tree, given by its absolute or relative path.
     * This method is similar to getModuleByPath(); the only difference is that
     * while getModuleByPath() throws an exception if the module was not found,
     * this method returns nullptr in that case.
     *
     * @see getModuleByPath(), cSimulation::findModuleByPath()
     */
    virtual cModule *findModuleByPath(const char *path) const;
    //@}

    /** @name Interface for calling initialize()/finish().
     * Those functions may not be called directly, only via
     * callInitialize() and callFinish() provided here.
     */
    //@{

    /**
     * Interface for calling initialize() from outside.
     */
    virtual void callInitialize() = 0;

    /**
     * Interface for calling initialize() from outside. It does a single stage
     * of initialization, and returns `true` if more stages are required.
     */
    virtual bool callInitialize(int stage) = 0;

    /**
     * Interface for calling finish() from outside. This method includes
     * calling finish() of contained components (submodules, channels) as well.
     */
    virtual void callFinish() = 0;
    //@}

    /** @name Parameters. */
    //@{

    /**
     * Returns total number of the component's parameters.
     */
    virtual int getNumParams() const  {return numPars;}

    /**
     * Returns reference to the parameter identified with its
     * index k. Throws an error if the parameter does not exist.
     */
    virtual cPar& par(int k);

    /**
     * Returns reference to the parameter identified with its
     * index k. Throws an error if the parameter does not exist.
     */
    const cPar& par(int k) const  {return const_cast<cComponent *>(this)->par(k);}

    /**
     * Returns reference to the parameter specified with its name.
     * Throws an error if the parameter does not exist.
     */
    virtual cPar& par(const char *parname);

    /**
     * Returns reference to the parameter specified with its name.
     * Throws an error if the parameter does not exist.
     */
    const cPar& par(const char *parname) const  {return const_cast<cComponent *>(this)->par(parname);}

    /**
     * Returns index of the parameter specified with its name.
     * Returns -1 if the object doesn't exist.
     */
    virtual int findPar(const char *parname) const;

    /**
     * Check if a parameter exists.
     */
    bool hasPar(const char *s) const {return findPar(s)>=0;}
    //@}

    /** @name Basic random number generation. */
    //@{

    /**
     * Returns the global RNG mapped to local RNG number k. For large indices
     * (k >= map size) the global RNG k is returned, provided it exists.
     */
    virtual cRNG *getRNG(int k) const;

    /**
     * Produces a random integer in the range [0,r) using the RNG given with its index.
     */
    virtual uint32_t intrand(uint32_t r, int rng=0) const  {return getRNG(rng)->intRand(r);}

    /**
     * Produces a random double in the range [0,1) using the RNG given with its index.
     */
    virtual double dblrand(int rng=0) const  {return getRNG(rng)->doubleRand();}
    //@}

    /** @name Random variate generation -- continuous distributions. */
    //@{

    /**
     * Returns a random variate with uniform distribution in the range [a,b).
     *
     * @param a, b the interval, a<b
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual double uniform(double a, double b, int rng=0) const  {return omnetpp::uniform(getRNG(rng), a, b);}

    /**
     * SimTime version of uniform(double,double,int), for convenience.
     */
    virtual SimTime uniform(SimTime a, SimTime b, int rng=0) const  {return uniform(a.dbl(), b.dbl(), rng);}

    /**
     * Returns a random variate from the exponential distribution with the
     * given mean (that is, with parameter lambda=1/mean).
     *
     * @param mean mean value
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual double exponential(double mean, int rng=0) const  {return omnetpp::exponential(getRNG(rng), mean);};

    /**
     * SimTime version of exponential(double,int), for convenience.
     */
    virtual SimTime exponential(SimTime mean, int rng=0) const  {return exponential(mean.dbl(), rng);}

    /**
     * Returns a random variate from the normal distribution with the given mean
     * and standard deviation.
     *
     * @param mean mean of the normal distribution
     * @param stddev standard deviation of the normal distribution
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual double normal(double mean, double stddev, int rng=0) const  {return omnetpp::normal(getRNG(rng), mean, stddev);};

    /**
     * SimTime version of normal(double,double,int), for convenience.
     */
    virtual SimTime normal(SimTime mean, SimTime stddev, int rng=0) const  {return normal(mean.dbl(), stddev.dbl(), rng);}

    /**
     * Normal distribution truncated to nonnegative values.
     * It is implemented with a loop that discards negative values until
     * a nonnegative one comes. This means that the execution time is not bounded:
     * a large negative mean with much smaller stddev is likely to result
     * in a large number of iterations.
     *
     * The mean and stddev parameters serve as parameters to the normal
     * distribution *before* truncation. The actual random variate returned
     * will have a different mean and standard deviation.
     *
     * @param mean mean of the normal distribution
     * @param stddev standard deviation of the normal distribution
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual double truncnormal(double mean, double stddev, int rng=0) const  {return omnetpp::truncnormal(getRNG(rng), mean, stddev);};

    /**
     * SimTime version of truncnormal(double,double,int), for convenience.
     */
    virtual SimTime truncnormal(SimTime mean, SimTime stddev, int rng=0) const  {return truncnormal(mean.dbl(), stddev.dbl(), rng);}

    /**
     * Returns a random variate from the gamma distribution with parameters
     * alpha>0, theta>0. Alpha is known as the "shape" parameter, and theta
     * as the "scale" parameter.
     *
     * Some sources in the literature use the inverse scale parameter
     * beta = 1 / theta, called the "rate" parameter. Various other notations
     * can be found in the literature; our usage of (alpha,theta) is consistent
     * with Wikipedia and Mathematica (Wolfram Research).
     *
     * Gamma is the generalization of the Erlang distribution for non-integer
     * k values, which becomes the alpha parameter. The chi-square distribution
     * is a special case of the gamma distribution.
     *
     * For alpha=1, Gamma becomes the exponential distribution with mean=theta.
     *
     * The mean of this distribution is alpha*theta, and variance is alpha*theta<sup>2</sup>.
     *
     * Generation: if alpha=1, it is generated as exponential(theta).
     *
     * For alpha>1, we make use of the acceptance-rejection method in
     * "A Simple Method for Generating Gamma Variables", George Marsaglia and
     * Wai Wan Tsang, ACM Transactions on Mathematical Software, Vol. 26, No. 3,
     * September 2000.
     *
     * The alpha\<1 case makes use of the alpha\>1 algorithm, as suggested by the
     * above paper.
     *
     * @remark the name gamma_d is chosen to avoid ambiguity with
     * a function of the same name
     *
     * @param alpha >0  the "shape" parameter
     * @param theta >0  the "scale" parameter
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual double gamma_d(double alpha, double theta, int rng=0) const  {return omnetpp::gamma_d(getRNG(rng), alpha, theta);};

    /**
     * Returns a random variate from the beta distribution with parameters
     * alpha1, alpha2.
     *
     * Generation is using relationship to Gamma distribution: if Y1 has gamma
     * distribution with alpha=alpha1 and beta=1 and Y2 has gamma distribution
     * with alpha=alpha2 and beta=2, then Y = Y1/(Y1+Y2) has beta distribution
     * with parameters alpha1 and alpha2.
     *
     * @param alpha1, alpha2 >0
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual double beta(double alpha1, double alpha2, int rng=0) const  {return omnetpp::beta(getRNG(rng), alpha1, alpha2);};

    /**
     * Returns a random variate from the Erlang distribution with k phases
     * and mean mean.
     *
     * This is the sum of k mutually independent random variables, each with
     * exponential distribution. Thus, the kth arrival time
     * in the Poisson process follows the Erlang distribution.
     *
     * Erlang with parameters m and k is gamma-distributed with alpha=k
     * and beta=m/k.
     *
     * Generation makes use of the fact that exponential distributions
     * sum up to Erlang.
     *
     * @param k number of phases, k>0
     * @param mean >0
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual double erlang_k(unsigned int k, double mean, int rng=0) const  {return omnetpp::erlang_k(getRNG(rng), k, mean);};

    /**
     * Returns a random variate from the chi-square distribution
     * with k degrees of freedom.  The chi-square distribution arises
     * in statistics. If Yi are k independent random variates from the normal
     * distribution with unit variance, then the sum-of-squares (sum(Yi^2))
     * has a chi-square distribution with k degrees of freedom.
     *
     * The expected value of this distribution is k. Chi_square with parameter
     * k is gamma-distributed with alpha=k/2, beta=2.
     *
     * Generation is using relationship to gamma distribution.
     *
     * @param k degrees of freedom, k>0
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual double chi_square(unsigned int k, int rng=0) const  {return omnetpp::chi_square(getRNG(rng), k);};

    /**
     * Returns a random variate from the student-t distribution with
     * i degrees of freedom. If Y1 has a normal distribution and Y2 has a chi-square
     * distribution with k degrees of freedom then X = Y1 / sqrt(Y2/k)
     * has a student-t distribution with k degrees of freedom.
     *
     * Generation is using relationship to gamma and chi-square.
     *
     * @param i degrees of freedom, i>0
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual double student_t(unsigned int i, int rng=0) const  {return omnetpp::student_t(getRNG(rng), i);};

    /**
     * Returns a random variate from the Cauchy distribution (also called
     * Lorentzian distribution) with parameters a,b where b>0.
     *
     * This is a continuous distribution describing resonance behavior.
     * It also describes the distribution of horizontal distances at which
     * a line segment tilted at a random angle cuts the x-axis.
     *
     * Generation uses inverse transform.
     *
     * @param a
     * @param b  b>0
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual double cauchy(double a, double b, int rng=0) const  {return omnetpp::cauchy(getRNG(rng), a, b);};

    /**
     * Returns a random variate from the triangular distribution with parameters
     * a <= b <= c.
     *
     * Generation uses inverse transform.
     *
     * @param a, b, c   a <= b <= c
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual double triang(double a, double b, double c, int rng=0) const  {return omnetpp::triang(getRNG(rng), a, b, c);};

    /**
     * Returns a random variate from the lognormal distribution with "scale"
     * parameter m and "shape" parameter w. m and w correspond to the parameters
     * of the underlying normal distribution (m: mean, w: standard deviation.)
     *
     * Generation is using relationship to normal distribution.
     *
     * @param m  "scale" parameter, m>0
     * @param w  "shape" parameter, w>0
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual double lognormal(double m, double w, int rng=0) const  {return omnetpp::lognormal(getRNG(rng), m, w);}

    /**
     * Returns a random variate from the Weibull distribution with parameters
     * a, b > 0, where a is the "scale" parameter and b is the "shape" parameter.
     * Sometimes Weibull is given with alpha and beta parameters, then alpha=b
     * and beta=a.
     *
     * The Weibull distribution gives the distribution of lifetimes of objects.
     * It was originally proposed to quantify fatigue data, but it is also used
     * in reliability analysis of systems involving a "weakest link," e.g.
     * in calculating a device's mean time to failure.
     *
     * When b=1, Weibull(a,b) is exponential with mean a.
     *
     * Generation uses inverse transform.
     *
     * @param a  the "scale" parameter, a>0
     * @param b  the "shape" parameter, b>0
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual double weibull(double a, double b, int rng=0) const  {return omnetpp::weibull(getRNG(rng), a, b);};

    /**
     * Returns a random variate from the shifted generalized Pareto distribution.
     *
     * Generation uses inverse transform.
     *
     * @param a,b  the usual parameters for generalized Pareto
     * @param c    shift parameter for left-shift
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual double pareto_shifted(double a, double b, double c, int rng=0) const  {return omnetpp::pareto_shifted(getRNG(rng), a, b, c);};
    //@}

    /** @name Random variate generation -- discrete distributions. */
    //@{

    /**
     * Returns a random integer with uniform distribution in the range [a,b],
     * inclusive. (Note that the function can also return b.)
     *
     * @param a, b  the interval, a<=b
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual int intuniform(int a, int b, int rng=0) const  {return omnetpp::intuniform(getRNG(rng), a, b);};

    /**
     * Returns a random integer with uniform distribution over [a,b), that is,
     * from [a,b-1].
     *
     * @param a, b  the interval, a<b
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual int intuniformexcl(int a, int b, int rng=0) const  {return omnetpp::intuniformexcl(getRNG(rng), a, b);};

    /**
     * Returns the result of a Bernoulli trial with probability p,
     * that is, 1 with probability p and 0 with probability (1-p).
     *
     * Generation is using elementary look-up.
     *
     * @param p  0=<p<=1
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual int bernoulli(double p, int rng=0) const  {return omnetpp::bernoulli(getRNG(rng), p);};

    /**
     * Returns a random integer from the binomial distribution with
     * parameters n and p, that is, the number of successes in n independent
     * trials with probability p.
     *
     * Generation is using the relationship to Bernoulli distribution (runtime
     * is proportional to n).
     *
     * @param n n>=0
     * @param p 0<=p<=1
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual int binomial(int n, double p, int rng=0) const  {return omnetpp::binomial(getRNG(rng), n, p);};

    /**
     * Returns a random integer from the geometric distribution with parameter p,
     * that is, the number of independent trials with probability p until the
     * first success.
     *
     * This is the n=1 special case of the negative binomial distribution.
     *
     * Generation uses inverse transform.
     *
     * @param p  0<p<=1
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual int geometric(double p, int rng=0) const  {return omnetpp::geometric(getRNG(rng), p);};

    /**
     * Returns a random integer from the negative binomial distribution with
     * parameters n and p, that is, the number of failures occurring before
     * n successes in independent trials with probability p of success.
     *
     * Generation is using the relationship to geometric distribution (runtime is
     * proportional to n).
     *
     * @param n  n>=0
     * @param p  0<p<1
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual int negbinomial(int n, double p, int rng=0) const  {return omnetpp::negbinomial(getRNG(rng), n, p);};

    /**
     * Returns a random integer from the Poisson distribution with parameter lambda,
     * that is, the number of arrivals over unit time where the time between
     * successive arrivals follow exponential distribution with parameter lambda.
     *
     * Lambda is also the mean (and variance) of the distribution.
     *
     * Generation method depends on value of lambda:
     *
     *   - 0<lambda<=30: count number of events
     *   - lambda>30: Acceptance-Rejection due to Atkinson (see Banks, page 166)
     *
     * @param lambda  > 0
     * @param rng index of the component RNG to use, see getRNG(int)
     */
    virtual int poisson(double lambda, int rng=0) const  {return omnetpp::poisson(getRNG(rng), lambda);};
    //@}

    /** @name Emitting simulation signals. */
    //@{
    /**
     * Returns the signal ID (handle) for the given signal name. Signal names
     * and IDs are global. The signal ID for a particular name gets assigned
     * at the first registerSignal() call; further registerSignal() calls for
     * the same name will return the same ID.
     *
     * Note: Since OMNeT++ 4.3, the signal registration table is not cleared
     * between runs, so it is possible to assign global simsignal_t variables
     * using static initialization:
     *
     * ```
     * simsignal_t somethingHappenedSignal = cComponent::registerSignal("somethingHappened");
     * ```
     */
    static simsignal_t registerSignal(const char *name);

    /**
     * The inverse of registerSignal(): returns the name of the given signal,
     * or nullptr for invalid signal handles.
     */
    static const char *getSignalName(simsignal_t signalID);

    /**
     * Emits the boolean value as a signal. If the given signal has listeners in this
     * component or in ancestor components, their appropriate receiveSignal() methods
     * are called. If there are no listeners, the runtime cost is usually minimal.
     */
    virtual void emit(simsignal_t signalID, bool b, cObject *details = nullptr);

    /**
     * Emits the double value as a signal. If the given signal has listeners in this
     * component or in ancestor components, their appropriate receiveSignal() methods
     * are called. If there are no listeners, the runtime cost is usually minimal.
     */
    virtual void emit(simsignal_t signalID, double d, cObject *details = nullptr);

    /**
     * Emits the simtime_t value as a signal. If the given signal has listeners in this
     * component or in ancestor components, their appropriate receiveSignal() methods
     * are called. If there are no listeners, the runtime cost is usually minimal.
     */
    virtual void emit(simsignal_t signalID, const SimTime& t, cObject *details = nullptr);

    /**
     * Emits the given string as a signal. Emitting nullptr is not allowed.
     * If the given signal has listeners in this component or in ancestor components,
     * their appropriate receiveSignal() methods are called. If there are no listeners,
     * the runtime cost is usually minimal.
     */
    virtual void emit(simsignal_t signalID, const char *s, cObject *details = nullptr);

    /**
     * Emits the given object as a signal. If the given signal has listeners in this
     * component or in ancestor components, their appropriate receiveSignal() methods
     * are called. If there are no listeners, the runtime cost is usually minimal.
     */
    virtual void emit(simsignal_t signalID, cObject *obj, cObject *details = nullptr);

    /** Delegates to emit(simsignal_t, cObject*) after a const_cast. */
    virtual void emit(simsignal_t signalID, const cObject *obj, cObject *details = nullptr) { emit(signalID, const_cast<cObject *>(obj), details); }

    /** Emits the given integer as intval_t. See other emit methods, e.g. emit(simsignal_t,double,cObject*), for more information. */
    virtual void emit(simsignal_t signalID, char c, cObject *details = nullptr) {doEmit(signalID,(intval_t)c, details);}

    /** Emits the given integer as uintval_t. See other emit methods, e.g. emit(simsignal_t,double,cObject*), for more information. */
    virtual void emit(simsignal_t signalID, unsigned char c, cObject *details = nullptr) {doEmit(signalID,(uintval_t)c, details);}

    /** Emits the given integer as intval_t. See other emit methods, e.g. emit(simsignal_t,double,cObject*), for more information. */
    virtual void emit(simsignal_t signalID, short i, cObject *details = nullptr) {doEmit(signalID,(intval_t)i, details);}

    /** Emits the given integer as uintval_t. See other emit methods, e.g. emit(simsignal_t,double,cObject*), for more information. */
    virtual void emit(simsignal_t signalID, unsigned short i, cObject *details = nullptr) {doEmit(signalID,(uintval_t)i, details);}

    /** Emits the given integer as intval_t. See other emit methods, e.g. emit(simsignal_t,double,cObject*), for more information. */
    virtual void emit(simsignal_t signalID, int i, cObject *details = nullptr) {doEmit(signalID,(intval_t)i, details);}

    /** Emits the given integer as uintval_t. See other emit methods, e.g. emit(simsignal_t,double,cObject*), for more information. */
    virtual void emit(simsignal_t signalID, unsigned int i, cObject *details = nullptr) {doEmit(signalID,(uintval_t)i, details);}

    /** Emits the given integer as intval_t. See other emit methods, e.g. emit(simsignal_t,double,cObject*), for more information. */
    virtual void emit(simsignal_t signalID, long i, cObject *details = nullptr) {doEmit(signalID,(intval_t)i, details);}

    /** Emits the given integer as uintval_t. See other emit methods, e.g. emit(simsignal_t,double,cObject*), for more information.  */
    virtual void emit(simsignal_t signalID, unsigned long i, cObject *details = nullptr) {doEmit(signalID,(uintval_t)i, details);}

    /** Emits the given integer as intval_t. See other emit methods, e.g. emit(simsignal_t,double,cObject*), for more information. */
    virtual void emit(simsignal_t signalID, long long i, cObject *details = nullptr) {doEmit(signalID,(intval_t)i, details);}

    /** Emits the given integer as uintval_t. See other emit methods, e.g. emit(simsignal_t,double,cObject*), for more information. */
    virtual void emit(simsignal_t signalID, unsigned long long i, cObject *details = nullptr) {doEmit(signalID,(uintval_t)i, details);}

    /** Delegates to emit(simsignal_t, double) */
    virtual void emit(simsignal_t signalID, float f, cObject *details = nullptr) {emit(signalID,(double)f, details);}

    /** Delegates to doEmit(simsignal_t, double) */
    virtual void emit(simsignal_t signalID, long double d, cObject *details = nullptr) {emit(signalID,(double)d, details);}

    /**
     * If producing a value for a signal has a significant runtime cost, this
     * method can be used to check beforehand whether the given signal possibly
     * has any listeners at all. if not, emitting the signal can be skipped.
     * This method has a constant cost but may return false positive.
     */
    bool mayHaveListeners(simsignal_t signalID) const {
        if (signalID < 0 || signalID > lastSignalID)
            throwInvalidSignalID(signalID);
        return signalListenerCounts[signalID] > 0;
    }

    /**
     * Returns true if the given signal has any listeners. In the current
     * implementation, this involves checking listener lists in ancestor
     * modules until the first listener is found, or up to the root.
     * This method may be useful if producing the data for an emit()
     * call would be expensive compared to a hasListeners() call.
     *
     * @see mayHaveListeners()
     */
    bool hasListeners(simsignal_t signalID) const;
    //@}

    /** @name Subscribing to simulation signals. */
    //@{
    /**
     * Adds a listener (callback object) that will be notified when a given
     * signal is emitted (see emit() methods). It is an error to subscribe
     * the same listener twice to the same signal. The order in which listeners
     * will be notified is undefined, so it is not necessarily the same order
     * in which listeners were subscribed.
     */
    virtual void subscribe(simsignal_t signalID, cIListener *listener);

    /**
     * Convenience method; it is equivalent to
     * `subscribe(registerSignal(signalName), listener)`.
     */
    virtual void subscribe(const char *signalName, cIListener *listener);

    /**
     * Removes the given listener. It has no effect if the given listener
     * is not subscribed.
     */
    virtual void unsubscribe(simsignal_t signalID, cIListener *listener);

    /**
     * Convenience method; it is equivalent to
     * `unsubscribe(registerSignal(signalName), listener)`.
     */
    virtual void unsubscribe(const char *signalName, cIListener *listener);

    /**
     * Returns true if the given listener is subscribed to the given signal
     * at this component (i.e. it does not look at listeners subscribed
     * at ancestor components).
     */
    virtual bool isSubscribed(simsignal_t signalID, cIListener *listener) const;

    /**
     * Convenience method; it is equivalent to
     * `isSubscribed(registerSignal(signalName), listener)`.
     */
    virtual bool isSubscribed(const char *signalName, cIListener *listener) const;

    /**
     * Returns the signals which have listeners subscribed at this component.
     */
    virtual std::vector<simsignal_t> getLocalListenedSignals() const;

    /**
     * Returns the listeners subscribed to the given signal at this component.
     */
    virtual std::vector<cIListener*> getLocalSignalListeners(simsignal_t signalID) const;
    //@}

    /** @name Display strings, animation. */
    //@{
    /**
     * Returns true if the current environment is a graphical user interface.
     * (For example, it returns true if the simulation is running in Qtenv,
     * and false if it's running over Cmdenv.) Modules can examine this flag
     * to decide whether or not they need to bother with visualization, e.g.
     * dynamically update display strings or draw on canvases.
     *
     * This method delegates to cEnvir::isGUI().
     */
    virtual bool hasGUI() const;

    /**
     * Returns the display string which defines presentation when the module
     * is displayed as a submodule in a compound module graphics.
     */
    virtual cDisplayString& getDisplayString() const;

    /**
     * Shortcut to `getDisplayString().set(dispstr)`.
     */
    virtual void setDisplayString(const char *dispstr);

    /**
     * When the models is running in Qtenv, it displays the given
     * text in the network graphics, as a bubble above the module's icon.
     */
    virtual void bubble(const char *text) const;

    /**
     * Searches a number of folders for a resource given with its file name or
     * relative path, and returns the path for the first match. If the resource
     * is not found, the empty string is returned.
     *
     * The list of the search folders includes the current working directory,
     * the folder of the main ini file, the folder this component's NED file
     * was loaded from, folders in the NED path, and folders in the image path
     * (OMNETPP_IMAGE_PATH).
     *
     * @see cEnvir::resolveResourcePath()
     */
    virtual std::string resolveResourcePath(const char *fileName) const;
    //@}

    /** @name Statistics collection */
    //@{

    /**
     * Records a double into the scalar result file.
     */
    virtual void recordScalar(const char *name, double value, const char *unit=nullptr);

    /**
     * Convenience method, delegates to recordScalar(const char *, double).
     */
    virtual void recordScalar(const char *name, SimTime value, const char *unit=nullptr) {recordScalar(name, value.dbl(), unit);}

    /**
     * Records the given statistics into the scalar result file. Delegates to
     * cStatistic::recordAs(). Note that if the statistics object is a histogram,
     * this operation may invoke its transform() method.
     */
    virtual void recordStatistic(cStatistic *stats, const char *unit=nullptr);

    /**
     * Records the given statistics into the scalar result file with the given name.
     * Delegates to cStatistic::recordAs().  Note that if the statistics object is
     * a histogram, this operation may invoke its transform() method.
     */
    virtual void recordStatistic(const char *name, cStatistic *stats, const char *unit=nullptr);
    //@}
};

}  // namespace omnetpp


#endif

