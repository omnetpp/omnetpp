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

#include <stdarg.h>
#include "cobject.h"


class cModule;
class cChannel;

/**
 * Common base class for cModuleType and cChannelType
 */
class SIM_API cComponentType : public cNoncopyableObject
{
  public:
    cComponentType(const char *name=NULL);
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
  protected:
    // internal
    cModule *createModuleObject();

  protected:
    /**
     * Adds parameters and gates to a newly created module object.
     * To be defined in subclasses.
     */
    virtual void addParametersGatesTo(cModule *mod) = 0;

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor.
     */
    cModuleType(const char *name=NULL);
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
     * DEPRECATED. It invokes <tt>mod->buildInside()</tt>; occurrences
     * should be replaced by that code, too.
     */
    // note: this cannot be inline, due to header file dependencies
    void buildInside(cModule *mod);

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

    //FIXME add static find() function to search the registry
};


/**
 * Abstract base class for creating a channel of a given type.
 *
 * @ingroup Internals
 */
class SIM_API cChannelType : public cComponentType
{
  protected:
    // internal
    cChannel *createChannelObject();

  protected:
    /**
     * Adds parameters to a newly created channel object.
     * To be defined in subclasses.
     */
    virtual void addParametersTo(cChannel *channel) = 0;

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor.
     */
    cChannelType(const char *name=NULL);

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

    //FIXME add static find() function to search the registry
};

#endif


