//==========================================================================
//  CCLASSFACTORY.H - part of
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

#ifndef __CCLASSFACTORY_H
#define __CCLASSFACTORY_H

#include "simkerneldefs.h"
#include "globals.h"
#include "cownedobject.h"

NAMESPACE_BEGIN


/**
 * The class behind the createOne() function and the Register_Class() macro.
 * Each instance is a factory for a particular class: it knows how to create
 * an object of that class.
 *
 * @see Register_Class(), Define_Module() macros
 * @ingroup Internals
 */
class SIM_API cClassFactory : public cNoncopyableOwnedObject
{
  private:
    cObject *(*creatorfunc)();
    std::string descr;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor.
     */
    cClassFactory(const char *name, cObject *(*f)(), const char *description=NULL);
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Produces a one-line description of object contents.
     * See cObject for more details.
     */
    virtual std::string info() const;
    //@}

    /** @name New methods */
    //@{
    /**
     * Creates an instance of a particular class by calling the creator
     * function. The result has to be cast to the appropriate type
     * (preferably by dynamic_cast or check_and_cast).
     */
    cObject *createOne() const  {return creatorfunc();}

    /**
     * Returns a description string.
     */
    const char *getDescription() const  {return descr.c_str();}
    //@}

    /** @name Static factory methods */
    //@{
    /**
     * Finds the factory object for the class given in the classname parameter,
     * or NULL if not found. The class name string should be given with any
     * potential namespace, enclosing class etc. The class must have been
     * registered previously with the Register_Class() macro.
     */
    static cClassFactory *find(const char *classname);

    /**
     * Like find(), but throws an error if the object was not found.
     */
    static cClassFactory *get(const char *classname);

    /**
     * Creates an instance of a particular class; the result has to be cast
     * to the appropriate type by hand. The class must have been registered
     * previously with the Register_Class() macro. The class name string
     * should be given with any potential namespace, enclosing class etc.
     *
     * If the class is not registered, this function throws an exception.
     * If you'd prefer having a NULL pointer returned instead, use the
     * createOneIfClassIsKnown() function.
     *
     * Example:
     *
     * <tt>cMessage *msg = cClassFactory::createOne("INET::EthernetFrame");</tt>
     *
     * createOne() is used e.g. in parallel simulation, when an object is received
     * from another partition in serialized form and has to be demarshalled.
     *
     * @see createOneIfClassIsKnown()
     * @see Register_Class() macro
     * @see cClassFactory class
     */
    static cObject *createOne(const char *classname);

    /**
     * A variant of the createOne() function; this function doesn't throw an
     * exception if the class is not registered, but returns a NULL pointer
     * instead.
     *
     * @see createOne()
     */
    static cObject *createOneIfClassIsKnown(const char *classname);
    //@}
};


/**
 *
 * @addtogroup Functions
 */
//@{
/**
 * Shortcut to cClassFactory::createOne().
 */
inline cObject *createOne(const char *classname) {
    return cClassFactory::createOne(classname);
}

/**
 * Shortcut to cClassFactory::createOneIfClassIsKnown().
 */
inline cObject *createOneIfClassIsKnown(const char *classname) {
    return cClassFactory::createOneIfClassIsKnown(classname);
}
//@}

NAMESPACE_END


#endif


