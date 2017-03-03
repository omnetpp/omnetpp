//==========================================================================
//  CPROPERTY.H - part of
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

#ifndef __OMNETPP_CPROPERTY_H
#define __OMNETPP_CPROPERTY_H

#include <map>
#include <vector>
#include "simkerneldefs.h"
#include "globals.h"
#include "cobject.h"
#include "cstringpool.h"
#include "cproperties.h"

namespace omnetpp {


/**
 * @brief Stores a (NED) property with its (possibly compound) value.
 *
 * The value consists of key-valuelist pairs; the key might be empty
 * (default key).
 *
 * @ingroup SimSupport
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

  private:
    void copy(const cProperty& other);

  protected:
    static void releaseValues(CharPtrVector& vals);
    int findKey(const char *key) const;
    CharPtrVector& getValuesVector(const char *key) const;

  public:
    // internal: merge a property onto this one
    virtual void updateWith(const cProperty *property);

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
    explicit cProperty(const char *name=nullptr, const char *index=nullptr);

    /**
     * Copy constructor.
     */
    cProperty(const cProperty& other) : cNamedObject(other) {setFlag(FL_ISLOCKED,false); propindex=propfullname=nullptr; copy(other);}

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
    virtual void setName(const char *name) override;

    /**
     * Redefined to return the property name plus optional index.
     */
    virtual const char *getFullName() const override;

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cProperty *dup() const override   {return new cProperty(*this);}

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

    /** @name Property getter/setter methods */
    //@{
    /**
     * Sets the index of this property; see NED syntax
     * <tt>@propname[index](keys-and-values)</tt>.
     */
    virtual void setIndex(const char *index);

    /**
     * Returns the index of this property; see NED syntax
     * <tt>@propname[index](keys-and-values)</tt>.
     * Returns nullptr if the property has no index
     * (<tt>@propname(keys-and-values)</tt>).
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
     * or DEFAULTKEY for the default key.
     */
    virtual bool containsKey(const char *key) const;

    /**
     * Adds the given key to the property. Has no effect if the key already
     * exists.
     */
    virtual void addKey(const char *key);

    /**
     * Returns the number of values for the given key in the property.
     * Specify "" or DEFAULTKEY for the default key.
     *
     * Returns 0 if the given key does not exist.
     */
    virtual int getNumValues(const char *key) const;

    /**
     * Expands or trims the list of values for the given key in the property,
     * by discarding elements or adding "" elements. Specify "" or DEFAULTKEY
     * for the default key. Note that simply setting an element above
     * getNumValues(key) will also expand the list.
     *
     * Throws an error if the given key does not exist.
     */
    virtual void setNumValues(const char *key, int size);

    /**
     * Returns the indexth value for the given key in the property.
     * Specify "" or DEFAULTKEY for the default key. If the key
     * does not exist or the index is out of bounds, nullptr is returned.
     */
    virtual const char *getValue(const char *key="", int index=0) const;

    /**
     * Replaces a value for the given key in the property. Specify "" or
     * DEFAULTKEY for the default key. cProperty will create its own copy
     * of the string passed. index may be greater than getNumValues(key);
     * that will cause the values list to expand, the new elements filled with "".
     *
     * Throws an error if the given key does not exist, or the index is
     * negative.
     */
    virtual void setValue(const char *key, int index, const char *value);

    /**
     * Erases the given key and all its values.
     */
    virtual void erase(const char *key);
    //@}
};

}  // namespace omnetpp


#endif


