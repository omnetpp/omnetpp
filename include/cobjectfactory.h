//==========================================================================
//  COBJECTFACTORY.H - part of
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

#ifndef __COBJECTFACTORY_H
#define __COBJECTFACTORY_H

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
class SIM_API cObjectFactory : public cNoncopyableOwnedObject
{
  private:
    cObject *(*creatorfunc)();
    void *(*castfunc)(cObject *);
    std::string descr;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor.
     */
    cObjectFactory(const char *name, cObject *(*creatorfunc)(), void *(*castfunc)(cObject *), const char *description=NULL);
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string info() const;
    //@}

    /** @name New methods */
    //@{
    /**
     * Returns true if the class this object stands for is abstract.
     * createOne() cannot be called for abstract classes.
     */
    virtual bool isAbstract() const  {return creatorfunc==NULL;}

    /**
     * Creates an instance of a particular class by calling the creator
     * function. The result has to be cast to the appropriate type
     * (preferably by dynamic_cast or check_and_cast). The method will throw
     * an error if the class is abstract (see isAbstract()).
     */
    virtual cObject *createOne() const;

    /**
     * Returns true if the given object can be cast (via dynamic_cast) to the
     * class represented by this factory object, and false otherwise.
     */
    virtual bool isInstance(cObject *obj) const  {return castfunc(obj)!=NULL;}

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
    static cObjectFactory *find(const char *classname);

    /**
     * Like find(), but throws an error if the object was not found.
     */
    static cObjectFactory *get(const char *classname);

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
     * <tt>cMessage *msg = cObjectFactory::createOne("INET::EthernetFrame");</tt>
     *
     * createOne() is used e.g. in parallel simulation, when an object is received
     * from another partition in serialized form and has to be demarshalled.
     *
     * @see createOneIfClassIsKnown()
     * @see Register_Class() macro
     * @see cObjectFactory class
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
 * Shortcut to cObjectFactory::createOne().
 */
inline cObject *createOne(const char *classname) {
    return cObjectFactory::createOne(classname);
}

/**
 * Shortcut to cObjectFactory::createOneIfClassIsKnown().
 */
inline cObject *createOneIfClassIsKnown(const char *classname) {
    return cObjectFactory::createOneIfClassIsKnown(classname);
}
//@}

NAMESPACE_END


#endif


