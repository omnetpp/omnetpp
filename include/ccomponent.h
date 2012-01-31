//==========================================================================
//   CCOMPONENT.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CCOMPONENT_H
#define __CCOMPONENT_H

#include <vector>
#include "simkerneldefs.h"
#include "cownedobject.h"
#include "cpar.h"
#include "cdefaultlist.h"
#include "simtime.h"
#include "cenvir.h"
#include "clistener.h"

NAMESPACE_BEGIN

class cComponentType;
class cProperties;
class cDisplayString;
class cRNG;
class cStatistic;

#define SIGNALMASK_UNFILLED (~(uint64)0)

/**
 * Common base for module and channel classes: cModule and cChannel.
 * cComponent provides parameters, properties and RNG mapping.
 *
 * @ingroup SimCore
 */
class SIM_API cComponent : public cDefaultList //implies noncopyable
{
    friend class cPar; // needs to call handleParameterChange()
    friend class cChannel; // allow it to access FL_INITIALIZED and releaseLocalListeners()
    friend class cModule; // allow it to access FL_INITIALIZED, releaseLocalListeners() and repairSignalFlags()
    friend class cGate;   // because of repairSignalFlags()

  private:
    enum {
      FL_PARAMSFINALIZED = 4,   // whether finalizeParameters() has been called
      FL_INITIALIZED = 8,       // whether initialize() has been called
      FL_EVLOGENABLED = 16,     // whether logging via ev<< is enabled
      FL_DISPSTR_CHECKED = 32,  // for hasDisplayString(): whether the FL_DISPSTR_NOTEMPTY flag is valid
      FL_DISPSTR_NOTEMPTY = 64, // for hasDisplayString(): whether the display string is not empty
    };

  private:
    cComponentType *componenttype;  // component type object

    short rngmapsize;  // size of rngmap array (RNGs>=rngmapsize are mapped one-to-one to physical RNGs)
    int *rngmap;       // maps local RNG numbers (may be NULL if rngmapsize==0)

    short paramvsize;
    short numparams;
    cPar *paramv;  // array of cPar objects

    cDisplayString *dispstr; // display string (created on demand)

    struct SignalData
    {
        simsignal_t signalID;
        cIListener **listeners; // NULL-terminated array

        SignalData() {signalID=SIMSIGNAL_NULL; listeners=NULL;}
        bool addListener(cIListener *l);
        bool removeListener(cIListener *l);
        int findListener(cIListener *l);
        int countListeners();
        bool hasListener() {return listeners && listeners[0];}
        static bool gt(const SignalData& e1, const SignalData& e2) {return e1.signalID > e2.signalID;}
    };

    typedef std::vector<SignalData> SignalTable;
    SignalTable *signalTable; // ordered by signalID so we can do binary search

    // flags to speed up emit() for signals 0..63 when there are no or few listeners
    uint64 signalHasLocalListeners;    // bit[k]==1: signalID k has local listeners
    uint64 signalHasAncestorListeners; // bit[k]==1: signalID k has listener in parent or in any ancestor component

    // string-to-simsignal_t mapping
    static struct SignalNameMapping {
        std::map<std::string,simsignal_t> signalNameToID;
        std::map<simsignal_t,std::string> signalIDToName;
    } *signalNameMapping;  // must be dynamically allocated on first access so that registerSignal() can be invoked from static initialization code
    static int lastSignalID;

    // dynamic assignment of signalHasLocalListeners/signalHasAncestorListeners bits (64 of them) to signalIDs
    static std::vector<uint64> signalMasks;  // index: signalID, value: mask (1<<bitIndex), or 0xffff... for "unfilled"
    static int firstFreeSignalMaskBitIndex; // 0..63; 64 means "all sold out"

    // stack of listener lists being notified, to detect concurrent modification
    static cIListener **notificationStack[];
    static int notificationSP;

  private:
    SignalData *findSignalData(simsignal_t signalID) const;
    SignalData *findOrCreateSignalData(simsignal_t signalID);
    void removeSignalData(simsignal_t signalID);
    void checkNotFiring(simsignal_t, cIListener **listenerList);
    template<typename T> void fire(cComponent *src, simsignal_t signalID, const uint64& mask, T x);
    void fireFinish();
    void signalListenerAdded(simsignal_t signalID, uint64 mask);
    void signalListenerRemoved(simsignal_t signalID, uint64 mask);
    void repairSignalFlags();
    bool computeHasListeners(simsignal_t signalID) const;
    void releaseLocalListeners();

  public:
    // internal: currently used by Cmdenv
    void setEvEnabled(bool e)  {setFlag(FL_EVLOGENABLED,e);}
    bool isEvEnabled() const  {return flags&FL_EVLOGENABLED;}

    // internal: invoked from within cEnvir::getRNGMappingFor(component)
    void setRNGMap(short size, int *map) {rngmapsize=size; rngmap=map;}

    // internal: sets associated cComponentType for the component;
    // called as part of the creation process.
    virtual void setComponentType(cComponentType *componenttype);

    // internal: adds a new parameter to the component; called as part of the creation process
    virtual void addPar(cParImpl *value);

    // internal: reallocates paramv (size must be >= numparams)
    void reallocParamv(int size);

    // internal: save parameters marked with "save-as-scalars=true"
    virtual void recordParametersAsScalars();

    // internal: has finalizeParameters() been called?
    bool parametersFinalized() const {return flags&FL_PARAMSFINALIZED;}

    // internal: has initialize() been called?
    bool initialized() const {return flags&FL_INITIALIZED;}

    // internal: used from Tkenv: find out if this module has a display string.
    // getDisplayString() would create the object immediately which we want to avoid.
    bool hasDisplayString();

    // internal: checks consistency of signal listener flags
    void checkLocalSignalConsistency() const;
    void checkSignalConsistency() const;

    // internal: clears global signals-related data structures; to be invoked before each simulation run
    static void clearSignalState();

    // internal: allocates a signalHasLocalListeners/signalHasAncestorListeners bit index to the
    // given signal and returns the corresponding mask (1<<index); returns 0 if there are no more
    // free bit indices. Result is remembered and returned in subsequent calls (until clearSignalState())
    static uint64 getSignalMask(simsignal_t signalID);

  protected:
    /** @name Initialization, finish and parameter change hooks.
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
     * is called with <tt>0,1,...numstages-1</tt> as argument. The default
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
     * Finish hook. finish() is called after end of simulation, if it
     * terminated without error. This default implementation does nothing.
     */
    virtual void finish();

    /**
     * This method is called by the simulation kernel to notify the module or
     * channel that the value of an existing parameter got changed.
     * Redefining this method allows simple modules and channels to be react on
     * parameter changes, for example by re-reading the value.
     * This default implementation does nothing.
     *
     * The parameter name can be NULL if more than one parameter has changed.
     *
     * To make it easier to write predictable components, the function does
     * NOT get called on uninitialized components (i.e. when initialized() returns
     * false). For each component the function is called (with NULL as a parname)
     * after the last stage of the initialization so the module gets a chance to
     * update its cached parameters.
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
     * Constructor. Note that module and channel objects should not be created
     * directly, via their cComponentType objects. cComponentType::create()
     * will do all housekeeping associated with creating the module (assigning
     * an ID to the module, inserting it into the <tt>simulation</tt> object,
     * etc.).
     */
    cComponent(const char *name = NULL);

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
    virtual void forEachChild(cVisitor *v);
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
     * Returns the associated component type. Guaranteed to be non-NULL.
     */
    cComponentType *getComponentType() const;

    /**
     * Returns the fully qualified NED type name of the component (i.e. the
     * simple name prefixed with the package name and any existing enclosing
     * NED type names).
     *
     * This method is a shortcut to <tt>getComponentType()->getFullName()</tt>.
     */
    virtual const char *getNedTypeName() const;

    /**
     * Redefined to return true in cModule and subclasses, otherwise returns false.
     */
    virtual bool isModule() const  {return false;}

    /**
     * Returns true for channels, and false for modules.
     */
    bool isChannel() const  {return !isModule();}

    /**
     * Returns the module containing this module/channel. This is not necessarily
     * the same object as getOwner(), especially for channel objects. For the system
     * module, it returns NULL.
     */
    virtual cModule *getParentModule() const = 0;

    /**
     * Returns the global RNG mapped to local RNG number k. For large indices
     * (k >= map size) the global RNG k is returned, provided it exists.
     */
    cRNG *getRNG(int k) const  {return ev.getRNG(k<rngmapsize ? rngmap[k] : k);}
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
     * of initialization, and returns <tt>true</tt> if more stages are required.
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
    virtual int getNumParams() const  {return numparams;}

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
     * <pre>
     * simsignal_t somethingHappenedSignal = cComponent::registerSignal("somethingHappened");
     * </pre>
     */
    static simsignal_t registerSignal(const char *name);

    /**
     * The inverse of registerSignal(): returns the name of the given signal,
     * or NULL for invalid signal handles.
     */
    static const char *getSignalName(simsignal_t signalID);

    /**
     * Emits the long value as a signal. If the given signal has listeners in this
     * component or in ancestor components, their appropriate receiveSignal() methods
     * are called. If there are no listeners, the runtime cost is usually minimal.
     */
    void emit(simsignal_t signalID, long l);

    /**
     * Emits the unsigned long value as a signal. If the given signal has listeners in
     * this component or in ancestor components, their appropriate receiveSignal() methods
     * are called. If there are no listeners, the runtime cost is usually minimal.
     */
    void emit(simsignal_t signalID, unsigned long l);

    /**
     * Emits the double value as a signal. If the given signal has listeners in this
     * component or in ancestor components, their appropriate receiveSignal() methods
     * are called. If there are no listeners, the runtime cost is usually minimal.
     */
    void emit(simsignal_t signalID, double d);

    /**
     * Emits the simtime_t value as a signal. If the given signal has listeners in this
     * component or in ancestor components, their appropriate receiveSignal() methods
     * are called. If there are no listeners, the runtime cost is usually minimal.
     *
     * Note: for technical reasons, the argument type is SimTime instead of simtime_t;
     * otherwise when compiled with USE_DOUBLE_SIMTIME we would have two "double"
     * overloads for emit().
     */
    void emit(simsignal_t signalID, const SimTime& t);

    /**
     * Emits the given string as a signal. If the given signal has listeners in this
     * component or in ancestor components, their appropriate receiveSignal() methods
     * are called. If there are no listeners, the runtime cost is usually minimal.
     */
    void emit(simsignal_t signalID, const char *s);

    /**
     * Emits the given object as a signal. If the given signal has listeners in this
     * component or in ancestor components, their appropriate receiveSignal() methods
     * are called. If there are no listeners, the runtime cost is usually minimal.
     */
    void emit(simsignal_t signalID, cObject *obj);

    /** Delegates to emit(simsignal_t, long) */
    void emit(simsignal_t signalID, bool b) {emit(signalID,(long)b);}

    /** Delegates to emit(simsignal_t, long) */
    void emit(simsignal_t signalID, char c) {emit(signalID,(long)c);}

    /** Delegates to emit(simsignal_t, unsigned long) */
    void emit(simsignal_t signalID, unsigned char c) {emit(signalID,(unsigned long)c);}

    /** Delegates to emit(simsignal_t, long) */
    void emit(simsignal_t signalID, short i) {emit(signalID,(long)i);}

    /** Delegates to emit(simsignal_t, unsigned long) */
    void emit(simsignal_t signalID, unsigned short i) {emit(signalID,(unsigned long)i);}

    /** Delegates to emit(simsignal_t, long) */
    void emit(simsignal_t signalID, int i) {emit(signalID,(long)i);}

    /** Delegates to emit(simsignal_t, unsigned long) */
    void emit(simsignal_t signalID, unsigned int i) {emit(signalID,(unsigned long)i);}

    /** Delegates to emit(simsignal_t, double) */
    void emit(simsignal_t signalID, float f) {emit(signalID,(double)f);}

    /** Delegates to emit(simsignal_t, double) */
    void emit(simsignal_t signalID, long double d) {emit(signalID,(double)d);}

    /**
     * If producing a value for a signal has a significant runtime cost, this
     * method can be used to check beforehand whether the given signal possibly
     * has any listeners at all -- if not, emitting the signal can be skipped.
     * This functions is significantly more efficient than hasListeners()
     * (amortizes in constant time), but may return "false positive".
     */
    bool mayHaveListeners(simsignal_t signalID) const {
        uint64 mask = getSignalMask(signalID);
        return (~signalHasLocalListeners & ~signalHasAncestorListeners & mask)==0; // always true for mask==0
    }

    /**
     * Returns true if the given signal has any listeners. For some signals
     * this method has a significant overhead (linear to the number of hierarchy
     * levels in the network).
     *
     * @see mayHaveListeners()
     */
    bool hasListeners(simsignal_t signalID) const {
        uint64 mask = getSignalMask(signalID);
        return mask ? ((signalHasLocalListeners|signalHasAncestorListeners) & mask) : computeHasListeners(signalID);
    }
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
    void subscribe(simsignal_t signalID, cIListener *listener);

    /**
     * Convenience method; it is equivalent to
     * <tt>subscribe(registerSignal(signalName), listener)</tt>.
     */
    void subscribe(const char *signalName, cIListener *listener);

    /**
     * Removes the given listener. It has no effect if the given listener
     * is not subscribed.
     */
    void unsubscribe(simsignal_t signalID, cIListener *listener);

    /**
     * Convenience method; it is equivalent to
     * <tt>unsubscribe(registerSignal(signalName), listener)</tt>.
     */
    void unsubscribe(const char *signalName, cIListener *listener);

    /**
     * Returns true if the given listener is subscribed to the given signal
     * at this component (i.e. it does not look at listeners subscribed
     * at ancestor components).
     */
    bool isSubscribed(simsignal_t signalID, cIListener *listener) const;

    /**
     * Convenience method; it is equivalent to
     * <tt>isSubscribed(registerSignal(signalName), listener)</tt>.
     */
    bool isSubscribed(const char *signalName, cIListener *listener) const;

    /**
     * Returns the signals which have listeners subscribed at this component.
     */
    std::vector<simsignal_t> getLocalListenedSignals() const;

    /**
     * Returns the listeners subscribed to the given signal at this component.
     */
    std::vector<cIListener*> getLocalSignalListeners(simsignal_t signalID) const;
    //@}

    /** @name Display strings, animation. */
    //@{
    /**
     * Returns the display string which defines presentation when the module
     * is displayed as a submodule in a compound module graphics.
     */
    cDisplayString& getDisplayString();

    /**
     * Shortcut to <tt>getDisplayString().set(dispstr)</tt>.
     */
    void setDisplayString(const char *dispstr);

    /**
     * When the models is running under Tkenv, it displays the given text
     * in the network graphics, as a bubble above the module's icon.
     */
    void bubble(const char *text);
    //@}

    /** @name Statistics collection */
    //@{

    /**
     * Records a double into the scalar result file.
     */
    void recordScalar(const char *name, double value, const char *unit=NULL);

    /**
     * Convenience method, delegates to recordScalar(const char *, double).
     */
    void recordScalar(const char *name, SimTime value, const char *unit=NULL) {recordScalar(name, value.dbl(), unit);}

    /**
     * Records the given statistics into the scalar result file. Delegates to
     * cStatistic::recordAs(). Note that if the statistics object is a histogram,
     * this operation may invoke its transform() method.
     */
    void recordStatistic(cStatistic *stats, const char *unit=NULL);

    /**
     * Records the given statistics into the scalar result file with the given name.
     * Delegates to cStatistic::recordAs().  Note that if the statistics object is
     * a histogram, this operation may invoke its transform() method.
     */
    void recordStatistic(const char *name, cStatistic *stats, const char *unit=NULL);
    //@}
};

NAMESPACE_END


#endif

