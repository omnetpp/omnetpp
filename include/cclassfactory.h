//==========================================================================
//  CCLASSFACTORY.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CCLASSFACTORY_H
#define __CCLASSFACTORY_H

#include "defs.h"
#include "globals.h"
#include "cobject.h"


/**
 * The class behind the createOne() function and the Register_Class() macro.
 * Each instance is a factory for a particular class: it knows how to create
 * an object of that class.
 *
 * @see Register_Class(), Define_Module() macros
 * @ingroup Internals
 */
class SIM_API cClassFactory : public cNoncopyableObject
{
  private:
    cPolymorphic *(*creatorfunc)();
    std::string descr;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor.
     */
    cClassFactory(const char *name, cPolymorphic *(*f)(), const char *description=NULL);
    //@}

    /** @name Redefined cObject methods. */
    //@{
    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
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
    cPolymorphic *createOne() const  {return creatorfunc();}

    /**
     * Returns a description string.
     */
    const char *description() const  {return descr.c_str();}
    //@}

    /** @name Static factory methods */
    //@{
    /**
     * FIXME comment
     */
    static cClassFactory *find(const char *classname);

    /**
     * Creates an instance of a particular class; the result has to be cast
     * to the appropriate type by hand. The class must have been registered
     * previously with the Register_Class() macro. This function internally
     * relies on the cClassFactory class.
     *
     * If the class is not registered, this function throws an exception.
     * If you'd prefer having a NULL pointer returned instead, use the
     * createOneIfClassIsKnown() function.
     *
     * Example:
     *
     * <tt>cObject *param = createOne( "cMessagePar" );</tt>
     *
     * createOne() is used e.g. in parallel simulation when an object is received
     * from another partition and it has to be demarshalled.
     *
     * @see createOneIfClassIsKnown()
     * @see Register_Class() macro
     * @see cClassFactory class
     */
    static cPolymorphic *createOne(const char *classname);

    /**
     * A variant of the createOne() function; this function doesn't throw an
     * exception if the class is not registered, but returns a NULL pointer
     * instead.
     *
     * @see createOne()
     */
    static cPolymorphic *createOneIfClassIsKnown(const char *classname);
    //@}
};


/**
 * @name Miscellaneous functions.
 * @ingroup Functions
 */
//@{
/**
 * Shortcut to cClassFactory::createOne().
 */
inline cPolymorphic *createOne(const char *classname) {
    return cClassFactory::createOne(classname);
}

/**
 * Shortcut to cClassFactory::createOneIfClassIsKnown().
 */
inline cPolymorphic *createOneIfClassIsKnown(const char *classname) {
    return cClassFactory::createOneIfClassIsKnown(classname);
}
//@}

#endif


