//==========================================================================
//  CNAMEDOBJECT.H - part of
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

#ifndef __OMNETPP_CNAMEDOBJECT_H
#define __OMNETPP_CNAMEDOBJECT_H

#include <typeinfo>
#include <iostream>
#include "simkerneldefs.h"
#include "cobject.h"
#include "cexception.h"

namespace omnetpp {


/**
 * @brief Extends cObject with a name string. Also includes a "flags" member,
 * with bits open for use by subclasses.
 *
 * @ingroup Fundamentals
 */
class SIM_API cNamedObject : public cObject
{
  private:
    const char *name = nullptr;  // object name (stringpooled if flags&FL_NAMEPOOLING!=0)

  protected:
    uint32_t flags = FL_NAMEPOOLING;  // FL_NAMEPOOLING flag; other bits used by derived classes
    enum {FL_NAMEPOOLING = 1};

  private:
    void copy(const cNamedObject& other);

  protected:
    // internal: set a bit in flags; flag is one of the FL_xxx constants
    void setFlag(int flag, bool value) {if (value) flags|=flag; else flags&=~flag;}
    int getFlag(int flag) {return flags & flag;}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Create object without a name.
     */
    cNamedObject() {}

    /**
     * Create object with given name. Name pooling is an optimization feature.
     */
    explicit cNamedObject(const char *name, bool namepooling=true);

    /**
     * Copy constructor.
     */
    cNamedObject(const cNamedObject& obj);

    /**
     * Destructor.
     */
    virtual ~cNamedObject();

    /**
     * The assignment operator. Derived classes should contain similar
     * methods (<tt>cClassName& cClassName::operator=(cClassName&)</tt>).
     *
     * Assignment copies the contents of the object EXCEPT for the name string.
     * If you want to copy the name string, you can do it by hand:
     * <tt>setName(o.getName()</tt>).
     */
    cNamedObject& operator=(const cNamedObject& o);

    // Note: dup() is still the original cObject one, which throws an error
    // to indicate that dup() has to be redefined in each subclass.

    /**
     * Serializes the object into a buffer.
     */
    virtual void parsimPack(cCommBuffer *buffer) const override;

    /**
     * Deserializes the object from a buffer.
     */
    virtual void parsimUnpack(cCommBuffer *buffer) override;
    //@}

    /** @name Handling the name string. */
    //@{
    /**
     * Sets object's name. The object creates its own copy of the string.
     * nullptr may also be passed, which will be interpreted as an
     * empty string ("").
     */
    virtual void setName(const char *s);

    /**
     * Returns pointer to the object's name, a string stored in the object.
     * This function never returns nullptr.
     */
    virtual const char *getName() const override  {return name ? name : "";}

    /**
     * Turn name pooling on/off. Name pooling is an optimization technique that
     * saves memory if several objects have identical names.
     */
    virtual void setNamePooling(bool b);

    /**
     * Returns whether name pooling is turned on for this object.
     */
    virtual bool getNamePooling() {return flags&FL_NAMEPOOLING;}
    //@}
};

}  // namespace omnetpp


#endif

