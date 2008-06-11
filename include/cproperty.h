//==========================================================================
//  CPROPERTY.H - part of
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

#ifndef __CPROPERTY_H
#define __CPROPERTY_H

#include <map>
#include <vector>
#include "simkerneldefs.h"
#include "globals.h"
#include "cobject.h"
#include "cstringpool.h"
#include "cproperties.h"

NAMESPACE_BEGIN


/**
 * Stores a property with its value. The value consists of key-valuelist pairs;
 * the key might be empty (default key).
 *
 * @ingroup Internals
 */
class SIM_API cProperty : public cNamedObject
{
  public:
    static const char *DEFAULTKEY;

  private:
    enum {
      FL_ISLOCKED = 4,
      FL_ISIMPLICIT = 8,
    };

  protected:
    // property names, keys and values are all stringpooled to reduce memory consumption
    static cStringPool stringPool;

    cProperties *ownerp;

    const char *propindex;
    mutable const char *propfullname;

    typedef std::vector<const char *> CharPtrVector;
    CharPtrVector keyv;
    std::vector<CharPtrVector> valuesv;

  protected:
    static void releaseValues(CharPtrVector& vals);
    int findKey(const char *key) const;
    CharPtrVector& getValuesVector(const char *key) const;

  public:
    // internal: locks the object against modifications. It cannot be unlocked
    // -- one must copy contents to an unlocked object, or call dup()
    // (new objects are created in unlocked state).
    virtual void lock() {setFlag(FL_ISLOCKED, true);}

    // internal:
    virtual bool isLocked() const {return flags&FL_ISLOCKED;}

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
    cProperty(const cProperty& other) {setFlag(FL_ISLOCKED,false); propindex=propfullname=NULL; operator=(other);}

    /**
     * Destructor.
     */
    virtual ~cProperty();

    /**
     * Assignment operator.
     */
    cProperty& operator=(const cProperty& other);
    //@}

    /** @name Redefined cObject functions */
    //@{
    /**
     * Redefined.
     * @see setIndex()
     */
    virtual void setName(const char *name);

    /**
     * Redefined to return the property name plus optional index.
     */
    virtual const char *getFullName() const;

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
     * Sets the index of this property; see NED syntax
     * <tt>\@propname[index](keys-and-values)</tt>.
     */
    virtual void setIndex(const char *index);

    /**
     * Returns the index of this property; see NED syntax
     * <tt>\@propname[index](keys-and-values)</tt>.
     * Returns NULL if the property has no index
     * (<tt>\@propname(keys-and-values)</tt>).
     */
    virtual const char *getIndex() const;

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
    virtual const std::vector<const char *>& getKeys() const;

    /**
     * Returns true if the property contains the given key. Specify ""
     * for the default key.
     */
    virtual bool hasKey(const char *key) const;

    /**
     * Adds the given key to the property. Has no effect if the key already
     * exists.
     */
    virtual void addKey(const char *key);

    /**
     * Returns the number of values for the given key in the property.
     * Specify "" for the default key.
     *
     * Throws an error of the given key does not exist.
     */
    virtual int getNumValues(const char *key) const;

    /**
     * Expands or trims the list of values for the given key in the property,
     * by discarding elements or adding "" elements. Specify "" for the default
     * key. Note that simply setting an element above getNumValues(key) will also
     * expand the list.
     *
     * Throws an error of the given key does not exist.
     */
    virtual void setNumValues(const char *key, int size);

    /**
     * Returns the kth value for the given key in the property.
     * Specify "" for the default key. For k>getNumValues(key), it returns "".  XXX why not NULL?
     */
    //XXX should return NULL/"" if key does not exist? currently it's an error
    virtual const char *getValue(const char *key, int k=0) const;

    /**
     * Replaces a value for the given key in the property. Specify "" for
     * the default key. cProperty will create its own copy of the string passed.
     * k may be greater than getNumValues(k), which will cause the values list,
     * to expand, the new elements filled with "".
     */
    //XXX create key if does not exist?
    virtual void setValue(const char *key, int k, const char *value);

    /**
     * Erases the given key and all its values.
     */
    virtual void erase(const char *key);
    //@}
};

NAMESPACE_END


#endif


