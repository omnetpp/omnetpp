//==========================================================================
//   CCOMPONENT.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

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

NAMESPACE_BEGIN

class cComponentType;
class cProperties;
class cDisplayString;
class cRNG;


/**
 * Common base for module and channel classes: cModule and cChannel.
 * cComponent provides parameters, properties and RNG mapping.
 *
 * @ingroup SimCore
 */
class SIM_API cComponent : public cDefaultList //implies noncopyable
{
    friend class cPar; // needs to call handleParameterChange()
  private:
    enum {
      FL_PARAMSFINALIZED = 4,   // whether parameters have been set up
      FL_EVLOGENABLED = 8,      // whether logging via ev<< is enabled
      FL_DISPSTR_CHECKED = 16,  // for hasDisplayString(): whether the FL_DISPSTR_NOTEMPTY flag is valid
      FL_DISPSTR_NOTEMPTY = 32, // for hasDisplayString(): whether the display string is not empty
    };

  protected:
    cComponentType *componenttype;  // component type object

    short rngmapsize;  // size of rngmap array (RNGs>=rngmapsize are mapped one-to-one to physical RNGs)
    int *rngmap;       // maps local RNG numbers (may be NULL if rngmapsize==0)

    short paramvsize;
    short numparams;
    cPar *paramv;  // array of cPar objects

    cDisplayString *dispstr; // display string (created on demand)

  public:
    // internal: currently used by Cmdenv
    void setEvEnabled(bool e)  {setFlag(FL_EVLOGENABLED,e);}
    bool isEvEnabled()  {return flags&FL_EVLOGENABLED;}

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
    bool areParamsFinalized() const {return flags&FL_PARAMSFINALIZED;}

    // internal: used from Tkenv: find out if this module has a display string.
    // displayString() would create the object immediately which we want to avoid.
    bool hasDisplayString();

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
     * To make it easier to write predictable simple modules, the function does
     * NOT get called during initialize() or finish(). If you need
     * notifications within those two functions as well, add the following
     * code into your initialize() and/or finish() methods:
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
     * Lie about the class name: we return the NED type name instead of the
     * real one, that is, "MobileHost" instead of "cCompoundModule" for example.
     */
    virtual const char *className() const;

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
    virtual cProperties *properties() const = 0;

    /**
     * Returns the associated component type.
     */
    cComponentType *componentType() const  {return componenttype;}

    /**
     * Returns the fully qualified NED type name of the component.
     * This is a shortcut to <tt>componentType()->fullName()</tt>.
     */
    virtual const char *nedTypeName() const;

    /**
     * Redefined to return true in cModule and subclasses, otherwise returns false.
     */
    virtual bool isModule() const  {return false;}

    /**
     * Returns the module containing this module/channel. This is not necessarily
     * the same object as owner(), especially for channel objects. For the system
     * module, it returns NULL.
     */
    virtual cModule *parentModule() const = 0;

    /**
     * Returns the global RNG mapped to local RNG number k. For large indices
     * (k >= map size) the global RNG k is returned, provided it exists.
     */
    cRNG *rng(int k) const  {return ev.rng(k<rngmapsize ? rngmap[k] : k);}
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
    virtual int params() const  {return numparams;}

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

    /** @name Display strings, animation. */
    //@{
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

    /** @name Statistics collection */
    //@{

    /**
     * Records a double into the scalar result file.
     */
    void recordScalar(const char *name, double value, const char *unit=NULL);

#ifndef USE_DOUBLE_SIMTIME
    /**
     * Convenience method, delegates to recordScalar(const char *, double).
     */
    void recordScalar(const char *name, simtime_t value, const char *unit=NULL) {recordScalar(name, value.dbl(), unit);}
#endif

    /**
     * Records the given statistics into the scalar result file.
     * Delegates to cStatistic::recordAs().
     */
    void recordStatistic(cStatistic *stats, const char *unit=NULL);

    /**
     * Records the given statistics into the scalar result file with the given name.
     * Delegates to cStatistic::recordAs().
     */
    void recordStatistic(const char *name, cStatistic *stats, const char *unit=NULL);
    //@}
};

NAMESPACE_END


#endif

