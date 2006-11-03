//==========================================================================
//  CPROPERTY.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CPROPERTY_H
#define __CPROPERTY_H

#include "defs.h"
#include "globals.h"
#include "cpolymorphic.h"
#include "cstringpool.h"
#include "stlwrap.h" //XXX out!!!!

#include <map>
#include <vector>

class cProperties;


/**
 * Stores a property with its value. The value consists of key-valuelist pairs;
 * the key might be empty (default key).
 *
 * @ingroup Internals
 */
class SIM_API cProperty : public cPolymorphic
{
  protected:
    // property names, keys and values are all stringpooled to reduce memory consumption
    static cStringPool stringPool;

    bool islocked;
    bool isimplicit;
    cProperties *ownerp;

    const char *propname;
    const char *propindex;
    const char *propfullname;

    typedef std::vector<const char *> CharPtrVector;
    CharPtrVector keyv;
    std::vector<CharPtrVector> valuesv;

  protected:
    static void releaseValues(CharPtrVector& vals);
    void updateFullName();
    int findKey(const char *key) const;
    CharPtrVector& valuesVector(const char *key) const;

  public:
    // internal: locks the object and all contained properties against modifications.
    // The object cannot be unlocked -- one must copy contents to an unlocked
    // object, or call dup() (new objects are created in unlocked state).
    virtual void lock() {islocked=true;}

    // internal: set the containing cProperties
    virtual void setOwner(cProperties *p) {ownerp = p;}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor. The property name should be specified without the "@" mark.
     */
    explicit cProperty(const char *name=NULL, const char *index=NULL);

    /**
     * Copy constructor.
     */
    cProperty(const cProperty& other) {islocked=false; operator=(other);}

    /**
     * Destructor.
     */
    virtual ~cProperty();

    /**
     * Assignment operator.
     */
    cProperty& operator=(const cProperty& other);
    //@}

    /** @name Redefined cPolymorphic functions */
    //@{
    /**
     * Returns the property name.
     */
    virtual const char *name() const {return propname;}

    /**
     * Redefined to return the property name plus optional index.
     */
    virtual const char *fullName() const;

    /**
     * Redefined to return the property's full path, that is, the
     * owner object's fullPath() plus the property name.
     */
    virtual std::string fullPath() const;

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cProperty *dup() const   {return new cProperty(*this);}

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

    /** @name Property getter/setter methods */
    //@{
    /**
     * Sets the property name. The string must begin with '@'.
     */
    virtual void setName(const char *name);

    /**
     * Sets the index of this property; see NED syntax
     * <tt>@propname[index](keys-and-values)</tt>.
     */
    virtual void setIndex(const char *index);

    /**
     * Returns the index of this property; see NED syntax
     * <tt>@propname[index](keys-and-values)</tt>.
     * Returns NULL if the property has no index
     * (<tt>@propname(keys-and-values)</tt>).
     */
    virtual const char *index() const;

    /**
     * Returns the "implicit" flag of this property.
     */
    virtual void setIsImplicit(bool b);

    /**
     * Returns whether this property is implicit. Implicit properties
     * are those not spelled out in the NED file, but rather, added
     * automatically during the parsing process or later.
     */
    virtual bool isImplicit() const;

    /**
     * Returns the list of keys if this property. The default key is
     * listed as "".
     */
    virtual const std::vector<const char *>& keys() const;

    /**
     * Returns true if the property contains the given key. Specify ""
     * for the default key.
     */
    virtual bool hasKey(const char *key) const;

    /**
     * Adds the given key to the property. Has no effect if the key already
     * existed.
     */
    virtual void addKey(const char *key);

    /**
     * Returns the number of values for the given key in the property.
     * Specify "" for the default key.
     */
    virtual int numValues(const char *key) const;

    /**
     * Expands or thrims the list of values for the given key in the property,
     * by discarding elements or adding "" elements. Specify "" for the default
     * key. Note that simply setting an element above numValues(key) will also
     * expand the list.
     */
    virtual void setNumValues(const char *key, int size);

    /**
     * Returns the kth value for the given key in the property.
     * Specify "" for the default key. For k>numValues(key), it returns "".
     */
    virtual const char *value(const char *key, int k) const;

    /**
     * Replaces a value for the given key in the property. Specify "" for
     * the default key. cProperty will create its own copy of the string passed.
     * k may be greater than numValues(k), which will cause the values list,
     * to expand, the new elements filled with "".
     */
    virtual void setValue(const char *key, int k, const char *value);

    /**
     * Erases the given key and all its values.
     */
    virtual void erase(const char *key);
    //@}
};

#endif


