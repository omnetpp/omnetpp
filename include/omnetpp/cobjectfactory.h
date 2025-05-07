//==========================================================================
//  COBJECTFACTORY.H - part of
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

#ifndef __OMNETPP_COBJECTFACTORY_H
#define __OMNETPP_COBJECTFACTORY_H

#include "simkerneldefs.h"
#include "globals.h"
#include "cownedobject.h"

namespace omnetpp {


/**
 * @brief The class behind the createOne() function and the Register_Class() macro.
 *
 * Each instance is a factory for a particular class: it knows how to create
 * an object of that class.
 *
 * @see Register_Class(), Define_Module() macros
 * @ingroup Internals
 */
class SIM_API cObjectFactory : public cNoncopyableOwnedObject
{
  private:
    cObject *(*creatorFunc)();
    void *(*castFunc)(cObject *);
    std::string description;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor.
     */
    cObjectFactory(const char *name, cObject *(*creatorfunc)(), void *(*castfunc)(cObject *), const char *description=nullptr);
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string str() const override;
    //@}

    /** @name New methods */
    //@{
    /**
     * Returns true if the class this object stands for is abstract.
     * createOne() cannot be called for abstract classes.
     */
    virtual bool isAbstract() const  {return creatorFunc==nullptr;}

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
    virtual bool isInstance(cObject *obj) const  {return castFunc(obj)!=nullptr;}

    /**
     * Returns a description string.
     */
    const char *getDescription() const  {return description.c_str();}
    //@}

    /** @name Static factory methods */
    //@{
    /**
     * Finds the factory object for the class given in the className parameter,
     * or nullptr if not found. The class must have been registered previously
     * with the Register_Class() macro.
     */
    static cObjectFactory *find(const char *className, const char *contextNamespace=nullptr, bool fallbackToOmnetpp=true);

    /**
     * Like find(), but throws an error if the object was not found.
     */
    static cObjectFactory *get(const char *className, const char *contextNamespace=nullptr, bool fallbackToOmnetpp=true);

    /**
     * Creates an instance of a particular class; the result has to be cast
     * to the appropriate type by hand. The class must have been registered
     * previously with the Register_Class() macro. The class name string
     * should be given with any potential namespace, enclosing class etc.
     *
     * If the class is not registered, this function throws an exception.
     * If you'd prefer having nullptr returned instead, use the
     * createOneIfClassIsKnown() function.
     *
     * Example:
     *
     * `cMessage *msg = cObjectFactory::createOne("INET::EthernetFrame");`
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
     * exception if the class is not registered, but returns nullptr
     * instead.
     *
     * @see createOne()
     */
    static cObject *createOneIfClassIsKnown(const char *classname);
    //@}
};


/**
 * @brief Shortcut to cObjectFactory::createOne().
 * @ingroup Misc
 */
inline cObject *createOne(const char *classname) {
    return cObjectFactory::createOne(classname);
}

/**
 * @brief Shortcut to cObjectFactory::createOneIfClassIsKnown().
 * @ingroup Misc
 */
inline cObject *createOneIfClassIsKnown(const char *classname) {
    return cObjectFactory::createOneIfClassIsKnown(classname);
}

}  // namespace omnetpp


#endif


