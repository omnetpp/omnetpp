//==========================================================================
//  CCOMPONENTTYPE.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CCOMPONENTTYPE_H
#define __CCOMPONENTTYPE_H

#include <string>
#include "cownedobject.h"
#include "cpar.h"
#include "cgate.h"

NAMESPACE_BEGIN


class cModule;
class cChannel;
class cProperties;

//FIXME is it necessary at all to have separate cModuleType and cChannelType classes???

/**
 * Common base class for cModuleType and cChannelType.
 *
 * The name() method returns the unqualified name (without namespace, e.g.
 * "Queue"), and fullName() returns the qualified name (with namespace,
 * e.g. "inet.network.Queue").
 *
 * @ingroup FIXME
 */
class SIM_API cComponentType : public cNoncopyableOwnedObject
{
  protected:
    std::string qualifiedName;
    cParImplCache parimplcache;
    cParImplCache2 parimplcache2;
  public:
    // internal use
    cParImplCache *parImplCache() {return &parimplcache;}
    // internal use
    cParImplCache2 *parImplCache2() {return &parimplcache2;}

  protected:
    friend class cComponent;
    friend class cPar;
    friend class cGate;

    // internal: returns the properties for this component.
    virtual cProperties *properties() const = 0;

    // internal: returns the properties of parameter
    virtual cProperties *paramProperties(const char *paramName) const = 0;

    // internal: returns the properties of gate
    virtual cProperties *gateProperties(const char *gateName) const = 0;

    // internal: returns the properties of a submodule or a contained channel.
    // (Subcomponent type is needed because with the NED "like" syntax,
    // we need the runtime type not the NED type of the submodule.)
    virtual cProperties *subcomponentProperties(const char *subcomponentName, const char *subcomponentType) const = 0;

    // internal: returns the properties of a parameter of a submodule or a contained channel.
    virtual cProperties *subcomponentParamProperties(const char *subcomponentName, const char *subcomponentType, const char *paramName) const = 0;

    // internal: returns the properties of a submodule gate.
    virtual cProperties *subcomponentGateProperties(const char *subcomponentName, const char *subcomponentType, const char *gateName) const = 0;

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor. Takes the fully qualified component type name.
     */
    cComponentType(const char *qname=NULL);
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Returns the fully qualified name (i.e. the simple name prefixed
     * with the package name and any existing enclosing NED type names).
     */
    virtual const char *fullName() const  {return qualifiedName.c_str();}
    //@}

    /**
     * Find a component type by fully qualified name. Returns NULL if not found.
     */
    static cComponentType *find(const char *qname);

    /**
     * Performs lookup of a name that occurs in a context. First looks into the
     * context namespace, then searches up by discarding the last elements of
     * the context one by one. Returns NULL if not found.
     */
    static cComponentType *find(const char *name, const char *contextNamespace);
};


/**
 * Abstract class for creating a module of a specific type.
 *
 * A cModuleType object exist for each module type (simple or compound).
 * FIXME document
 *
 * @ingroup FIXME
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
     * Adds parameters to a newly created module object.
     * To be defined in subclasses.
     */
    virtual void addParametersTo(cModule *mod) = 0;

    /**
     * Must be invoked before connecting the gates and calling mod->buildInside().
     * The parameter values must have been finalized (read from omnetpp.ini etc)
     * at this point. This method adds gates and gate vectors (whose size may
     * depend on parameter values).
     */
    virtual void addGatesTo(cModule *mod) = 0;

    /**
     * Creates and connects submodules of a newly created module object.
     * To be defined in subclasses.
     *
     * NOTE: If you have an old simulation model that attempts to call
     * this method directly, it is using an API call which has been DEPRECATED
     * since OMNeT++ 2.3b1 -- please change the code to <tt>mod->buildInside()</tt>.
     */
    virtual void buildInside(cModule *mod) = 0;

    /**
     * Utility function: instantiates the given class, and tries to cast the
     * result to cModule. Raises an error if class was not found or could not be cast.
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
    virtual cModule *createScheduleInit(char *name, cModule *parentmod);
    //@}

    /**
     * Find a component type by fully qualified name.
     */
    static cModuleType *find(const char *qname);

    /**
     * Performs lookup of a name that occurs in a context. First looks into the
     * context namespace, then searches up by discarding the last elements of
     * the context one by one.
     */
    static cModuleType *find(const char *name, const char *contextNamespace);
};


/**
 * Abstract base class for creating a channel of a given type.
 *
 * @ingroup FIXME
 */
class SIM_API cChannelType : public cComponentType
{
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
    virtual cChannel *create(const char *name, cModule *parentmod);
    //@}

    /**
     * Find a component type by fully qualified name.
     */
    static cChannelType *find(const char *qname);

    /**
     * Performs lookup of a name that occurs in a context. First looks into the
     * context namespace, then searches up by discarding the last elements of
     * the context one by one.
     */
    static cChannelType *find(const char *name, const char *contextNamespace);
};

NAMESPACE_END


#endif


