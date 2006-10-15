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

// XXX what about this:
// - remove name from cObject (move it into a "cNamedObject : public cObject" instead);
// - then cPar and cProperty could be cObject! (penalty would be only 4 bytes from "int index" in cObject)
// - problem: setName() method would have to be removed from cObject, which affects existing code...

/**
 * Stores a property with its value. The value consists of key-valuelist pairs;
 * the key might be empty (default key).
 *
 * @ingroup Internals
 */
class SIM_API cProperty : cPolymorphic
{
  protected:
    // property names and property key names are stringpooled to reduce memory consumption
    static cStringPool stringPool;

    bool islocked;
    bool isimplicit;
    short index_;
    cProperties *ownerp;
    const char *propertyname;  // stringpooled

    typedef std::vector<const char *> CharPtrVector;
    CharPtrVector keyv;
    std::vector<CharPtrVector> valuesv;

  protected:
    static void releaseValues(CharPtrVector& vals);
    int findKey(const char *key) const;

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
     * Constructor. The name must begin with '@'.
     */
    explicit cProperty(const char *name=NULL);

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
     * Redefined to return the property name. Property names start with '@'.
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
     * Sets the index of this property.
     */
    virtual void setIndex(short index);

    /**
     * Returns the index of this property. This is nonzero only with
     * properties that have multiple instances on the same owner.
     */
    virtual short index() const;

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
     * Returns the values for the given key in the property.
     */
    virtual const std::vector<const char *>& values(const char *key) const;

    /**
     * Replaces the value list for the given key in the property;
     * specify "" for the default key. cProperty will create its
     * own copy of the strings in the value list.
     */
    virtual void setValues(const char *key, const std::vector<const char *>& valueList);

    /**
     * Erases the given key and all its values.
     */
    virtual void erase(const char *key);
    //@}
};

#endif


