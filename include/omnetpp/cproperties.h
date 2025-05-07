//==========================================================================
//  CPROPERTIES.H - part of
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

#ifndef __OMNETPP_CPROPERTIES_H
#define __OMNETPP_CPROPERTIES_H

#include <vector>
#include "simkerneldefs.h"
#include "globals.h"
#include "cobject.h"

namespace omnetpp {

class cProperty;


/**
 * @brief A collection of properties (cProperty).
 *
 * @ingroup ModelComponents
 */
class SIM_API cProperties : public cObject
{
  protected:
    bool isLocked = false;
    int refCount = 0;
    std::vector<cProperty *> properties;

  private:
    void copy(const cProperties& other);

  public:
    // internal: locks the object and all contained properties against modifications.
    // The object cannot be unlocked -- one must copy contents to an unlocked
    // object, or call dup() (new objects are created in unlocked state).
    virtual void lock();

    // internal: increment/decrement reference count
    int addRef()  {return ++refCount;}
    int removeRef()  {return --refCount;}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor.
     */
    explicit cProperties() {}

    /**
     * Copy constructor.
     */
    cProperties(const cProperties& other) : cObject(other) {refCount=0; isLocked=false; copy(other);}

    /**
     * Destructor.
     */
    virtual ~cProperties();

    /**
     * Assignment operator.
     */
    cProperties& operator=(const cProperties& other);
    //@}

    /** @name Redefined cObject functions */
    //@{
    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cProperties *dup() const override  {return new cProperties(*this);}

    /**
     * Returns object name.
     */
    virtual const char *getName() const override  {return "properties";}

    /**
     * Produces a one-line description of the object's contents.
     */
    virtual std::string str() const override;

    /**
     * Serializes the object into a buffer.
     */
    virtual void parsimPack(cCommBuffer *buffer) const override;

    /**
     * Deserializes the object from a buffer.
     */
    virtual void parsimUnpack(cCommBuffer *buffer) override;
    //@}

    /** @name Properties */
    //@{
    /**
     * Returns the number of properties.
     */
    virtual int getNumProperties() const  {return properties.size();}

    /**
     * Returns the names of cProperty object stored in this object.
     * The strings in the returned array do not need to be deallocated and
     * must not be modified.
     */
    virtual const std::vector<const char *> getNames() const;

    /**
     * Returns kth property, where 0 <= k < getNumProperties().
     */
    virtual cProperty *get(int k) const;

    /**
     * Returns the given property, or nullptr if it does not exist.
     * Name and index correspond to the the NED syntax
     * `@propertyname[index](keys-and-values)`, where "[index]"
     * is optional.
     */
    virtual cProperty *get(const char *name, const char *index=nullptr) const;

    /**
     * Returns the property as a boolean. If the property is missing,
     * this method returns false; otherwise, only the first value in the
     * default key ("") is examined. If it is "false", this method returns
     * false; in all other cases (missing, empty, some other value) it
     * returns true.
     *
     * Examples:
     * `@foo`: true,
     * `@foo()`: true,
     * `@foo(false)`: false,
     * `@foo(true)`: true,
     * `@foo(any)`: true,
     * `@foo(a=x,b=y,c=z)`: true;
     * `@foo(somekey=false)`: true (!)
     */
    virtual bool getAsBool(const char *name, const char *index=nullptr) const;

    /**
     * Returns unique indices for a property. Name and index correspond to the
     * NED syntax `@propertyname[index](keys-and-values)`.
     * The strings in the returned array do not need to be deallocated and
     * must not be modified.
     */
    virtual std::vector<const char *> getIndicesFor(const char *name) const;

    /**
     * Adds the given property to this object.
     */
    virtual void add(cProperty *p);

    /**
     * Removes the given property from this object, and deletes it.
     */
    virtual void remove(int k);
    //@}
};

}  // namespace omnetpp


#endif


