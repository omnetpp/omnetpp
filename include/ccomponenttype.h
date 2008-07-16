//==========================================================================
//  CCOMPONENTTYPE.H - part of
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

#ifndef __CCOMPONENTTYPE_H
#define __CCOMPONENTTYPE_H

#include <string>
#include <map>
#include <set>
#include "cpar.h"
#include "cgate.h"
#include "cownedobject.h"

NAMESPACE_BEGIN


class cModule;
class cChannel;
class cProperties;


/**
 * Common base class for cModuleType and cChannelType.
 *
 * The getName() method returns the unqualified name (without namespace, e.g.
 * "Queue"), and getFullName() returns the qualified name (with namespace,
 * e.g. "inet.network.Queue").
 *
 * @ingroup Internals
 */
class SIM_API cComponentType : public cNoncopyableOwnedObject
{
  protected:
    std::string qualifiedName;

    typedef std::map<std::string, cParImpl *> StringToParMap;
    StringToParMap sharedParMap;

    struct Less {bool operator()(cParImpl *a, cParImpl *b) const;};
    typedef std::set<cParImpl *, Less> ParImplSet;
    ParImplSet sharedParSet;

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

    cParImpl *getSharedParImpl(const char *key) const;
    void putSharedParImpl(const char *key, cParImpl *value);

    cParImpl *getSharedParImpl(cParImpl *p) const;
    void putSharedParImpl(cParImpl *p);

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor. Takes the fully qualified component type name.
     */
    cComponentType(const char *qname=NULL);

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
     */
    virtual const char *getFullName() const  {return qualifiedName.c_str();}
    //@}

    /**
     * Finds a component type by fully qualified NED type name.
     * Returns NULL if not found.
     */
    static cComponentType *find(const char *qname);

    /**
     * Finds a component type by fully qualified NED type name.
     * Throws an error if not found.
     */
    static cComponentType *get(const char *qname);
};


/**
 * Abstract class for creating a module of a specific type.
 *
 * A cModuleType object exist for each module type (simple or compound).
 *
 * @ingroup Internals
 */
class SIM_API cModuleType : public cComponentType
{
    friend class cModule;
    friend class cCompoundModule;
  protected:
    /**
     * Creates the module object. To be defined in subclasses.
     */
    virtual cModule *createModuleObject() = 0;

    /**
     * Adds parameters and gates to a newly created module object.
     * Gate vectors will be created with zero size, and a further
     * call to setupGateVectors() will be needed once parameter values
     * have been finalized.
     */
    virtual void addParametersAndGatesTo(cModule *mod) = 0;

    /**
     * Sets gate vector sizes on the module. This must be called AFTER all
     * parameters have been set (see finalizeParameters()) because gate
     * vector sizes may depend on parameter values; and it it must be
     * invoked before connecting the gates and calling mod->buildInside().
     */
    virtual void setupGateVectors(cModule *mod) = 0;

    /**
     * Creates and connects submodules of a newly created module object.
     * To be defined in subclasses. addParametersAndGatesTo() and
     * setupGateVectors() must have been already called at this point.
     *
     * NOTE: If you have an old simulation model that attempts to call
     * this method directly, it is using an API call which has been DEPRECATED
     * since OMNeT++ 2.3b1 -- please change the code to <tt>mod->buildInside()</tt>.
     */
    virtual void buildInside(cModule *mod) = 0;

    /**
     * Utility function: instantiates the given class, and tries to cast the
     * result to cModule. Raises an error if class was not found or could
     * not be cast.
     */
    cModule *instantiateModuleClass(const char *classname);

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor.
     */
    cModuleType(const char *qname=NULL);
    //@}

    /** @name Misc */
    //@{
    /**
     * Returns true if the module was declared with the "network" keyword.
     */
    virtual bool isNetwork() const = 0;
    //@}

    /** @name Module creation */
    //@{

    /**
     * Creates a module which is not element of a module vector.
     * In addition to creating an object of the correct type,
     * this function inserts the module into the simulation's data structure,
     * and adds the parameters and gates specified in the NED declaration.
     */
    virtual cModule *create(const char *name, cModule *parentmod);

    /**
     * Creates a module to be an element of a module vector.
     * The last two arguments specify the vector size and the index
     * of the new module within the vector.
     */
    virtual cModule *create(const char *name, cModule *parentmod, int vectorsize, int index);

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
    virtual cModule *createScheduleInit(const char *name, cModule *parentmod);
    //@}

    /**
     * Finds a module type by fully qualified NED type name.
     * Returns NULL if not found.
     */
    static cModuleType *find(const char *qname);

    /**
     * Finds a module type by fully qualified NED type name.
     * Throws an error if not found.
     */
    static cModuleType *get(const char *qname);
};


/**
 * Abstract base class for creating a channel of a given type.
 *
 * @ingroup Internals
 */
class SIM_API cChannelType : public cComponentType
{
  protected:
    static cChannelType *idealChannelType;
    static cChannelType *datarateChannelType;

  protected:
    /**
     * Creates the channel object. To be defined in subclasses.
     */
    virtual cChannel *createChannelObject() = 0;

    /**
     * Adds parameters to a newly created channel object.
     * To be defined in subclasses.
     */
    virtual void addParametersTo(cChannel *channel) = 0;

    /**
     * Utility function: instantiates the given class, and tries to cast the
     * result to cChannel. Raises an error if class was not found or could not be cast.
     */
    cChannel *instantiateChannelClass(const char *classname);

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor.
     */
    cChannelType(const char *qname=NULL);

    /** @name Channel object creation */
    //@{
    /**
     * Factory method to create a channel object.
     *
     * In addition to creating an object of the correct type,
     * this methods inserts it into the simulation's data structure,
     * and adds the parameters specified in the NED declaration.
     */
    virtual cChannel *create(const char *name);
    //@}

    /**
     * Finds a channel type by fully qualified NED type name.
     * Returns NULL if not found.
     */
    static cChannelType *find(const char *qname);

    /**
     * Finds a channel type by fully qualified NED type name.
     * Throws an error if not found.
     */
    static cChannelType *get(const char *qname);

    /**
     * Equivalent to <tt>cChannelType::get("ned.IdealChannel")</tt>, but more efficient.
     */
    static cChannelType *getIdealChannel();

    /**
     * Equivalent to <tt>cChannelType::get("ned.DatarateChannel")</tt>, but more efficient.
     */
    static cChannelType *getDatarateChannel();

    //FIXME comments
    static cIdealChannel *createIdealChannel(const char *name);
    static cDatarateChannel *createDatarateChannel(const char *name);
};

NAMESPACE_END


#endif


