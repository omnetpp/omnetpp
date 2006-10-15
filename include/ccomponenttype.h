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


class cModule;
class cChannel;

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

    /** @name Description string */
    //@{
    /**
     * Sets a description string.
     */
    void setDescription(const char *s)  {desc = s?s:"";}

    /**
     * Returns a description string.
     */
    const char *description() const  {return desc.c_str();}
    //@}

    /**
     *XXX
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

    /**
     *XXX
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
     *XXX
     */
    static cChannelType *find(const char *name);
};

#endif


