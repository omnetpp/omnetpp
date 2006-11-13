//==========================================================================
//  CPOLYMORPHIC.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Declaration of the following classes:
//    cPolymorphic : general base class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CPOLYMORPHIC_H
#define __CPOLYMORPHIC_H

#include <string>
#include "defs.h"
#include "util.h"

class cClassDescriptor;

/**
 * Ultimate base class for cObject, and thus for nearly all
 * \opp classes. cPolymorphic is a <b>lightweight common base class</b>:
 * it only contains virtual member functions but NO DATA MEMBERS at all.
 *
 * It is recommended to use cPolymorphic as a base class for any class
 * that has at least one virtual member function. This makes the class more
 * interoperable with \opp, and causes no extra overhead at all.
 * sizeof(cPolymorphic) should yield 4 on a 32-bit architecture (4-byte
 * <i>vptr</i>), and using cPolymorphic as a base class doesn't add anything
 * to the size because a class with a virtual function already has a vptr.
 *
 * cPolymorphic allows the object to be displayed in graphical user
 * interface (Tkenv) via the className(), info() and detailedInfo() methods
 * which you may choose to redefine in your own subclasses.
 *
 * Using cPolymorphic also strengthens type safety. <tt>cPolymorphic *</tt>
 * pointers should replace <tt>void *</tt> in most places where you need
 * pointers to "any data structure". Using cPolymorphic will allow safe
 * downcasts using <tt>dynamic_cast</tt> and also \opp's
 * <tt>check_and_cast</tt>.
 *
 * @ingroup SimCore
 */
class SIM_API cPolymorphic
{
  public:
    // internal: returns a descriptor object for this object
    virtual cClassDescriptor *getDescriptor();

  public:
    /**
     * Constructor. It has an empty body. (The class doesn't have data members
     * and there's nothing special to do at construction time.)
     */
    cPolymorphic() {}

    /**
     * Destructor. It has an empty body (the class doesn't have data members.)
     * It is declared here only to make the class polymorphic and make its
     * destructor virtual.
     */
    virtual ~cPolymorphic() {}

    /**
     * Returns a pointer to the class name string. This method is implemented
     * using typeid (C++ RTTI), and it does not need to be overridden
     * in subclasses.
     */
    virtual const char *className() const;

    /** @name Empty virtual functions which can be redefined in subclasses */
    //@{

    /**
     * Can be redefined (as done in cObject) to return an object name.
     * This version just returns "".
     */
    virtual const char *fullName() const;

    /**
     * Can be redefined (as done in cObject) to return an object full path,
     * which contains the object name (more precisely, fullName()) together
     * with the object's location in the object hierarchy.
     * This version just returns fullName() (which is by default "").
     */
    virtual std::string fullPath() const;

    /**
     * Can be redefined to produce a one-line description of object into `buf'.
     * The string appears in the graphical user interface (Tkenv) e.g. when
     * the object is displayed in a listbox. The returned string should
     * possibly be at most 80-100 characters long, and must not contain
     * newline.
     *
     * @see detailedInfo()
     */
    virtual std::string info() const;

    /**
     * DEPRECATED. Copies first MAX_OBJECTINFO characters of
     * <tt>info().c_str()</tt> into `buf'. Use info() instead.
     */
    virtual void info(char *buf) _OPPDEPRECATED;

    /**
     * Can be redefined to produce a detailed, multi-line, arbitrarily long
     * description of the object. The string appears in the graphical
     * user interface (Tkenv) together with other object data (e.g. class name)
     * wherever it is feasible to display a multi-line string.
     */
    virtual std::string detailedInfo() const;

    /**
     * Should be redefined in subclasses to create an exact copy of this object.
     * The default implementation just throws an error, to indicate that
     * the method was not redefined.
     */
    virtual cPolymorphic *dup() const;
    //@}
};

#endif

