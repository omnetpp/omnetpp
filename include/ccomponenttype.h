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
class cNEDDeclaration;


/**
 * Prototype for functions that are called by cModuleType objects
 * to create modules of a specific type.
 * @ingroup EnumsTypes
 */
typedef cModule *(*ModuleCreateFunc)();


/**
 * Class for creating a module of a specific type.
 *
 * A cModuleType object exist for each module type (simple or compound).
 * A cModuleType object 'knows' how to create a module of a given type,
 * thus a module can be created without having to include the .h file
 * with the C++ declaration of the module class ("class FddiMAC...").
 * A cModuleType object is created through a Define_Module macro. Thus,
 * each module type must have a Define_Module() line, e.g:
 *
 * Define_Module( MySimpleModule );
 *
 * nedtool automatically generates Define_Module for compound modules, but the
 * user is responsible for adding one for each simple module type.
 *
 * @ingroup Internals
 */
class SIM_API cModuleType : public cNoncopyableObject
{
    friend class cModule;

  protected:
    char *interface_name;
    cNEDDeclaration *iface;
    ModuleCreateFunc create_func;

    // internal: here it invokes create_func
    virtual cModule *createModuleObject();

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor.
     */
    cModuleType(const char *classname, const char *interf_name, ModuleCreateFunc cf);

    /**
     * Destructor.
     */
    virtual ~cModuleType();
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
     * DEPRECATED. Use <tt>mod->buildInside()</tt> instead; that's what
     * this method does anyway.
     */
    virtual void buildInside(cModule *mod);

    /**
     * This is a convenience function to get a module up and running in one step.
     *
     * First, the module is created using create() and buildInside(), then
     * starter messages are created (using mod->scheduleStart(simulation.simTime())),
     * then initialize() is called (mod->callInitialize()). It is important that
     * scheduleStart() be called before initialize(), because initialize()
     * functions might contain scheduleAt() calls which could otherwise insert
     * a message BEFORE the starter messages for module.
     *
     * This method works for simple and compound modules alike. Not applicable
     * if the module:
     *  - has parameters to be set
     *  - gate vector sizes to be set
     *  - gates to be connected before initialize()
     */
    virtual cModule *createScheduleInit(char *name, cModule *parentmod);

    /**
     * Returns pointer to the module declaration object corresponding to this
     * module type.
     *
     * @see cNEDDeclaration
     */
    virtual cNEDDeclaration *moduleNEDDeclaration();
    //@}

    //FIXME add static find() function to search the registry
};


/**
 * Abstract base class for channel types. One is expected to redefine the
 * create() method to construct and return a channel object (cChannel subclass)
 * of the appropriate type and attributes set. The class has to be registered
 * via the Define_Channel() macro.
 *
 * @ingroup Internals
 */
class SIM_API cChannelType : public cNoncopyableObject
{
  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor.
     */
    cChannelType(const char *name=NULL) : cNoncopyableObject(name) {}

    /** @name Channel object creation */
    //@{
    /**
     * Factory method to create a channel object.
     */
    virtual cChannel *create(const char *name) = 0;
    //@}

    //FIXME add static find() function to search the registry
};

#endif


