//==========================================================================
//  CNAMEDOBJECT.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CNAMEDOBJECT_H
#define __CNAMEDOBJECT_H

#include <typeinfo>
#include <iostream>
#include "simkerneldefs.h"
#include "util.h"
#include "cobject.h"
#include "cexception.h"
#include "cstringpool.h"

NAMESPACE_BEGIN


/**
 * Extends cObject with a name string. Also includes a "flags" member
 * which subclasses can utilize as economic storage of boolean variables.
 * (This class only uses one bit from "flags").
 *
 * @ingroup SimCore
 */
class SIM_API cNamedObject : public cObject
{
  private:
    const char *namep;  // object name (stringpooled if flags&FL_NAMEPOOLING!=0)

  protected:
    unsigned short flags;  // FL_NAMEPOOLING flag; other bits used by derived classes
    unsigned short unused; // space lost to due to word aligment; one may make use of it in subclasses (cModule, cSimpleModule, cGate)
    enum {FL_NAMEPOOLING = 1};

  protected:
    // internal: set a bit in flags; flag is one of the FL_xxx constants
    void setFlag(int flag, bool value) {if (value) flags|=flag; else flags&=~flag;}

  private:
    // pool for shared storage of object names
    static cStringPool stringPool;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Create object without a name.
     */
    cNamedObject();

    /**
     * Create object with given name. Name pooling is an optimization feature.
     */
    explicit cNamedObject(const char *name, bool namepooling=true);

    /**
     * Copy constructor. In derived classes, it is usually implemented
     * as <tt>{operator=(obj);</tt>
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
     * Assigment copies the contents of the object EXCEPT for the name string.
     * If you want to copy the name string, you can do it by hand:
     * <tt>setName(o.name()</tt>).
     */
    cNamedObject& operator=(const cNamedObject& o);

    // Note: dup() is still the original cObject one, which throws an error
    // to indicate that dup() has to be redefined in each subclass.

    /**
     * Serializes the object into a buffer.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from a buffer.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Handling the name string. */
    //@{
    /**
     * Sets object's name. The object creates its own copy of the string.
     * NULL pointer may also be passed, which will be interpreted as an
     * empty string ("").
     */
    virtual void setName(const char *s);

    /**
     * Returns pointer to the object's name, a string stored in the object.
     * This function never returns NULL.
     */
    virtual const char *name() const  {return namep ? namep : "";}

    /**
     * Turn name pooling on/off. Name pooling is an optimization technique that saves
     * memory if several objects have identical names.
     */
    virtual void setNamePooling(bool b);
    //@}
};

NAMESPACE_END


#endif

