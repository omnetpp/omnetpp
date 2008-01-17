//==========================================================================
//  CPROPERTIES.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CPROPERTIES_H
#define __CPROPERTIES_H

#include "simkerneldefs.h"
#include "globals.h"
#include "cobject.h"
#include <vector>

NAMESPACE_BEGIN

class cProperty;


/**
 * Stores properties.
 *
 * @ingroup Internals
 */
class SIM_API cProperties : public cObject
{
  protected:
    bool islocked;
    int refcount;
    std::vector<cProperty *> propv;

  public:
    // internal: locks the object and all contained properties against modifications.
    // The object cannot be unlocked -- one must copy contents to an unlocked
    // object, or call dup() (new objects are created in unlocked state).
    virtual void lock();

    // internal: increment/decrement reference count
    int addRef()  {return ++refcount;}
    int removeRef()  {return --refcount;}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor.
     */
    explicit cProperties();

    /**
     * Copy constructor.
     */
    cProperties(const cProperties& other) {refcount=0; islocked=false; operator=(other);}

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
    virtual cProperties *dup() const  {return new cProperties(*this);}

    /**
     * Returns object name.
     */
    virtual const char *name() const  {return "properties";}

    /**
     * Produces a one-line description of object contents.
     */
    virtual std::string info() const;

    /**
     * Serializes the object into a buffer.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from a buffer.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Properties */
    //@{
    /**
     * Returns the number of properties.
     */
    virtual int numProperties() const  {return propv.size();}

    /**
     * Returns the names of cProperty object stored in this object.
     * The strings in the returned array do not need to be deallocated and
     * must not be modified.
     */
    virtual const std::vector<const char *> getNames() const;

    /**
     * Returns kth property, where 0 <= k < numProperties().
     */
    virtual cProperty *get(int k) const;

    /**
     * Returns the given property, or NULL if it does not exist.
     * Name and index correspond to the the NED syntax
     * <tt>@propertyname[index](keys-and-values)</tt>, where "[index]"
     * is optional.
     */
    virtual cProperty *get(const char *name, const char *index=NULL) const;

    /**
     * Returns unique indices for a property. Name and index correspond to the
     * NED syntax <tt>@propertyname[index](keys-and-values)</tt>.
     * The strings in the returned array do not need to be deallocated and
     * must not be modified.
     */
    virtual const std::vector<const char *> getIndicesFor(const char *name) const;

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

NAMESPACE_END


#endif


