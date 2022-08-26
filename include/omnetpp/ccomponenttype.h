//==========================================================================
//  CCOMPONENTTYPE.H - part of
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

#ifndef __OMNETPP_CCOMPONENTTYPE_H
#define __OMNETPP_CCOMPONENTTYPE_H

#include <string>
#include <map>
#include <set>
#include "cpar.h"
#include "cgate.h"
#include "cownedobject.h"
#include "clistener.h"


namespace omnetpp {

class cModule;
class cChannel;
class cProperty;
class cProperties;
class cIdealChannel;
class cDelayChannel;
class cDatarateChannel;
class cObjectFactory;


/**
 * @brief Common base class for cModuleType and cChannelType.
 *
 * The getName() method returns the unqualified name (without namespace, e.g.
 * "Queue"), and getFullName() returns the qualified name (with namespace,
 * e.g. "inet.network.Queue").
 *
 * @ingroup Internals
 */
class SIM_API cComponentType : public cNoncopyableOwnedObject
{
    friend class cSimulation; // clearSharedParImpls()
  protected:
    std::string qualifiedName;
    bool availabilityTested = false;
    bool available = false;

    typedef internal::cParImpl cParImpl;
    typedef std::map<std::string, cParImpl *> StringToParMap;
    StringToParMap sharedParMap;

    struct Less {bool operator()(cParImpl *a, cParImpl *b) const;};
    typedef std::set<cParImpl *, Less> ParImplSet;
    ParImplSet sharedParSet;

    struct SignalDesc { SimsignalType type; cObjectFactory *objectType; bool isNullable; };
    std::map<simsignal_t,SignalDesc> signalsSeen;

    mutable bool sourceFileDirectoryCached = false;
    mutable std::string sourceFileDirectory;

  protected:
    friend class cComponent;
    friend class cModule;
    friend class cChannel;
    friend class cPar;
    friend class cGate;

    // internal: returns the properties for this component.
    virtual cProperties *getProperties() const = 0;

    // internal: returns the properties of parameter
    virtual cProperties *getParamProperties(const char *paramName) const = 0;

    // internal: returns the properties of gate
    virtual cProperties *getGateProperties(const char *gateName) const = 0;

    // internal: returns the properties of a submodule.
    // (Subcomponent type is needed because with the NED "like" syntax,
    // we need the runtime type not the NED type of the submodule.)
    virtual cProperties *getSubmoduleProperties(const char *submoduleName, const char *submoduleType) const = 0;

    // internal: returns the properties of a contained connection.
    // (Subcomponent type is needed because with the NED "like" syntax,
    // we need the runtime type not the NED type of the submodule.)
    virtual cProperties *getConnectionProperties(int connectionId, const char *channelType) const = 0;

    // internal: returns the name of the C++ class that implements this NED type
    virtual const char *getImplementationClassName() const = 0;

    // internal: returns the default C++ namespace for the given NED type (for NED it's the @namespace package property)
    virtual std::string getCxxNamespaceForType(const char *type) const = 0;

    // internal: apply pattern-based ("deep") parameter settings in NED
    virtual void applyPatternAssignments(cComponent *component) = 0;

    // internal: sharedParMap access
    cParImpl *getSharedParImpl(const char *key) const;
    void putSharedParImpl(const char *key, cParImpl *value);

    // internal: sharedParSet access
    cParImpl *getSharedParImpl(cParImpl *p) const;
    void putSharedParImpl(cParImpl *p);

    // internal:
    virtual void clearSharedParImpls();

    // internal: helper for checkSignal()
    cObjectFactory *lookupClass(const char *className, const char *sourceType) const;

  public:
    // internal: delegates to the similar NedTypeInfo method
    virtual std::string getPackageProperty(const char *name) const {return "";}

    // internal: checks whether this type is available; currently tests the existence of the implementation class
    virtual bool isAvailable();

    // internal: delegates to the similar NedTypeInfo method
    virtual bool isInnerType() const {return false;}

    // internal: return the default C++ namespace at this NED type (for NED it's the @namespace package property)
    virtual std::string getCxxNamespace() const {return "";}

    // internal: return the path of the source file this declaration has been loaded from, or nullptr if not available
    virtual const char *getSourceFileName() const {return nullptr;}

    // internal: return the directory of the source file this declaration has been loaded from, or nullptr if not available
    virtual const char *getSourceFileDirectory() const;

    // internal: used by cComponent::emit() methods to validate signals
    virtual void checkSignal(simsignal_t signalID, SimsignalType type, cObject *obj = nullptr);

    // internal: returns the @signal property for the given signal, or nullptr if not found
    virtual cProperty *getSignalDeclaration(const char *signalName);

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor. Takes the fully qualified component type name.
     */
    cComponentType(const char *qname=nullptr);

    /**
     * Destructor.
     */
    virtual ~cComponentType();
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Returns the fully qualified name (i.e. the simple name prefixed
     * with the package name and any existing enclosing NED type names).
     *
     * Examples:
     *
     * The fully qualified name for a module named <tt>Fifo</tt> which is
     * in the default package is <tt>"Fifo"</tt>.
     *
     * For a module named <tt>Host</tt> which is in the package <tt>some.package</tt>,
     * the fully qualified name is <tt>"some.package.Host"</tt>.
     *
     * For a channel type <tt>Ch</tt> which is defined as an inner type inside
     * the network <tt>Network</tt> in a package named <tt>some.package</tt>, the
     * fully qualified name is <tt>"some.package.Network.Ch"</tt>.
     */
    virtual const char *getFullName() const override  {return qualifiedName.c_str();}
    //@}

    /**
     * Returns the NED source code of the component, if available.
     */
    virtual std::string getNedSource() const = 0;

    /**
     * Finds a component type by fully qualified NED type name.
     * Returns nullptr if not found.
     */
    static cComponentType *find(const char *qname);

    /**
     * Finds a component type by fully qualified NED type name.
     * Throws an error if not found.
     */
    static cComponentType *get(const char *qname);
};


/**
 * @brief Abstract class for creating a module of a specific type.
 *
 * A cModuleType object exist for each module type (simple or compound).
 *
 * @ingroup Internals
 */
class SIM_API cModuleType : public cComponentType
{
    friend class cModule;
  protected:
    // internal: create the module object
    virtual cModule *createModuleObject() = 0;

    // internal: add parameters and gates to a newly created module object.
    // Gate vectors will be created with zero size, and a further call to
    // setupGateVectors() will be needed once parameter values have been finalized/
    virtual void addParametersAndGatesTo(cModule *mod) = 0;

    // internal: set gate vector sizes on the module. This must be called AFTER all
    // parameters have been set (see finalizeParameters()) because gate vector sizes
    // may depend on parameter values; and it it must be invoked before connecting
    // the gates and calling mod->buildInside().
    virtual void setupGateVectors(cModule *mod) = 0;

    // internal: create and connect submodules of a newly created module object.
    // addParametersAndGatesTo() and setupGateVectors() must have been already called at this point.
    virtual void buildInside(cModule *mod) = 0;

    // internal: utility function: instantiates the given class, and tries to cast
    // the result to cModule. Raises an error if class was not found or could not be cast.
    cModule *instantiateModuleClass(const char *classname);

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor.
     */
    cModuleType(const char *qname=nullptr);
    //@}

    /** @name Misc */
    //@{
    /**
     * Returns true if the module type was declared with the "network" keyword.
     */
    virtual bool isNetwork() const = 0;

    /**
     * Returns true if this module type was declared as a simple module in the
     * NED file, i.e. using the "simple" keyword.
     */
    virtual bool isSimple() const = 0;
    //@}

    /** @name Module creation */
    //@{

    /**
     * Creates a submodule under the given parent module. If index is given,
     * the module creates an element in a module vector; the module vector of
     * the given name must already exist and must be sufficiently large (size>index).
     *
     * In addition to creating an object of the correct type, this function
     * inserts the module into the simulation's data structure, and adds the
     * parameters and gates specified in the NED declaration.
     *
     * After creation, the module still needs to go through a setup of several
     * steps before it is ready to use. The full process is the following:
     *
     * <pre>
     * cModule *module = create("foo", parentModule);
     * [set parameters]
     * module->finalizeParameters();
     * [connect gates]
     * module->buildInside();
     * module->scheduleStart(simTime());
     * module->callInitialize();
     * </pre>
     *
     * As callInitialize() and some of the other functions perform the preceding
     * steps if they are not yet done, the above code can be abbreviated to:
     *
     * <pre>
     * cModule *module = create("foo", parentModule);
     * [set parameters]
     * module->callInitialize();
     * </pre>
     */
    virtual cModule *create(const char *name, cModule *parentmod, int index=-1);

    /**
     * This is a convenience function to get a module up and running in one step.
     *
     * First, the module is created using create() and buildInside(), then
     * a starter message is created (for activity() modules only),
     * then initialize() is called (mod->callInitialize()).
     *
     * This method works for simple and compound modules alike. Not applicable
     * if the module:
     *  - has parameters to be set
     *  - gate vector sizes to be set
     *  - gates to be connected before initialize()
     */
    virtual cModule *createScheduleInit(const char *name, cModule *parentmod, int index=-1);
    //@}

    /**
     * Finds a module type by fully qualified NED type name.
     * Returns nullptr if not found.
     */
    static cModuleType *find(const char *qname);

    /**
     * Finds a module type by fully qualified NED type name.
     * Throws an error if not found.
     */
    static cModuleType *get(const char *qname);
};


/**
 * @brief Abstract base class for creating a channel of a given type.
 *
 * @ingroup Internals
 */
class SIM_API cChannelType : public cComponentType
{
  protected:
    static cChannelType *idealChannelType;
    static cChannelType *delayChannelType;
    static cChannelType *datarateChannelType;

  protected:
    // internal: create the channel object
    virtual cChannel *createChannelObject() = 0;

    // internal: add parameters to a newly created channel object.
    virtual void addParametersTo(cChannel *channel) = 0;

    // internal: utility function: instantiates the given class, and tries to cast the
    // result to cChannel. Raises an error if class was not found or could not be cast.
    cChannel *instantiateChannelClass(const char *classname);

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor.
     */
    cChannelType(const char *qname=nullptr);

    /** @name Channel object creation */
    //@{
    /**
     * Factory method to create a channel object with the given name.
     *
     * If the name is nullptr, a default name will be taken from the
     * @defaultname NED property of the channel type; or if there is no
     * such property, "channel" will be used.
     *
     * In addition to creating an object of the correct type,
     * this methods inserts it into the simulation's data structure,
     * and adds the parameters specified in the NED declaration.
     */
    virtual cChannel *create(const char *name);
    //@}

    /**
     * Finds a channel type by fully qualified NED type name.
     * Returns nullptr if not found.
     */
    static cChannelType *find(const char *qname);

    /**
     * Finds a channel type by fully qualified NED type name.
     * Throws an error if not found.
     */
    static cChannelType *get(const char *qname);

    /**
     * Equivalent to <tt>cChannelType::get("ned.IdealChannel")</tt>,
     * but more efficient.
     */
    static cChannelType *getIdealChannelType();

    /**
     * Equivalent to <tt>cChannelType::get("ned.DelayChannel")</tt>,
     * but more efficient.
     */
    static cChannelType *getDelayChannelType();

    /**
     * Equivalent to <tt>cChannelType::get("ned.DatarateChannel")</tt>,
     * but more efficient.
     */
    static cChannelType *getDatarateChannelType();

    /**
     * Creates an IdealChannel via <tt>getIdealChannelType()->create(name)</tt>.
     */
    static cIdealChannel *createIdealChannel(const char *name);

    /**
     * Creates a DelayChannel via <tt>getDelayChannelType()->create(name)</tt>.
     */
    static cDelayChannel *createDelayChannel(const char *name);

    /**
     * Creates a DatarateChannel via <tt>getDatarateChannelType()->create(name)</tt>.
     */
    static cDatarateChannel *createDatarateChannel(const char *name);
};

}  // namespace omnetpp


#endif


