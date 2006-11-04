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
#include "cobject.h"
#include "cpar.h"
#include "cgate.h"


class cModule;
class cChannel;
class cProperties;


/**
 * Common base class for cModuleType and cChannelType
 */
class SIM_API cComponentType : public cNoncopyableObject
{
  protected:
    std::string desc;

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor.
     */
    cComponentType(const char *name=NULL, const char *description=NULL);
    //@}

    /** @name Redefined cObject methods. */
    //@{
    /**
     * Produces a one-line description of object contents.
     * See cObject for more details.
     */
    virtual std::string info() const;
    //@}

    /** @name Extra information */
    //@{
    /**
     * Sets a description string.
     */
    void setDescription(const char *s)  {desc = s?s:"";}

    /**
     * Returns a description string.
     */
    const char *description() const  {return desc.c_str();}

    /**
     * Returns the NED declaration as text, if available.
     */
    virtual std::string declaration() const = 0;
    //@}

    /** @name Inheritance */
    //@{
    /**
     * Returns the name of the component this one extends
     */
    virtual const char *extendsName() const = 0;

    /**
     * Returns the number of interfaces.
     */
    virtual int numInterfaceNames() const = 0;

    /**
     * Returns the name of the kth interface (k=0..numInterfaceNames()-1).
     */
    virtual const char *interfaceName(int k) const = 0;

    /**
     * For simple modules and channels, it returns the name of the C++ class that
     * has to be instantiated; otherwise it returns NULL.
     */
    virtual const char *implementationClassName() const = 0;
    //@}

    /** @name Properties */
    //@{
    /**
     * Return the properties for this component. The returned object should not
     * be modified.
     */
    virtual cProperties *properties() const = 0;
    //@}

    /** @name Parameters */
    //@{
    /**
     * Returns total number of the parameters.
     */
    virtual int numPars() const = 0;

    /**
     * Returns the name of the kth parameter.
     */
    virtual const char *parName(int k) const = 0;

    /**
     * Returns the type of the kth parameter.
     */
    virtual cPar::Type parType(int k) const = 0;

    /**
     * Return the properties for parameter k. The returned object should not
     * be modified.
     */
    virtual cProperties *parProperties(int k) const = 0;

    /**
     * Returns index of the parameter specified with its name.
     * Returns -1 if the object doesn't exist.
     */
    virtual int findPar(const char *parname) const = 0;

    /**
     * Check if a parameter exists.
     */
    bool hasPar(const char *s) const {return findPar(s)>=0;}
    //@}

    /**
     * Find a component type object by name.
     */
    static cComponentType *find(const char *name);
};


/**
 * Abstract class for creating a module of a specific type.
 *
 * A cModuleType object exist for each module type (simple or compound).
 * FIXME document
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
     * To be defined in subclasses.
     */
    virtual void addParametersGatesTo(cModule *mod) = 0;

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
    cModuleType(const char *name=NULL, const char *description=NULL);
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
     * this function inserts it into cSimulation's module vector and adds the
     * parameters and gates specified in the interface description.
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

    /** @name Gates */
    //@{
    /**
     * Returns the number of gates
     */
    virtual int numGates() const = 0;

    /**
     * Returns the name of the kth gate.
     */
    virtual const char *gateName(int k) const = 0;

    /**
     * Returns the type of the kth gate.
     */
    virtual cGate::Type gateType(int k) const = 0;

    /**
     * Return the properties for gate k. The returned object should not
     * be modified.
     */
    virtual cProperties *gateProperties(int k) const = 0;

    /**
     * Returns index of the given gate (0..numGates()), or -1 if not found.
     */
    virtual int findGate(const char *name) const = 0;

    /**
     * Check if a gate exists.
     */
    bool hasGate(const char *s) const  {return findGate(s)>=0;}
    //@}

    /**
     * Find a component type object by name.
     */
    static cModuleType *find(const char *name);
};


/**
 * Abstract base class for creating a channel of a given type.
 *
 * @ingroup Internals
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
    cChannelType(const char *name=NULL, const char *description=NULL);

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
     * Find a component type object by name.
     */
    static cChannelType *find(const char *name);
};

#endif


