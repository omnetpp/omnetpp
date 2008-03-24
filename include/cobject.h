//==========================================================================
//  COBJECT.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Declaration of the following classes:
//    cObject : general base class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __COBJECT_H
#define __COBJECT_H

#include <string>
#include "simkerneldefs.h"
#include "util.h"
#include "cvisitor.h"

// Note: we include regmacros.h here in order to prevent the compiler from
// misunderstanding Register_Class() in other source files as a function declaration
#include "regmacros.h"

NAMESPACE_BEGIN

class cCommBuffer;
class cClassDescriptor;
class cOwnedObject;

//FIXME deprecate using cOwnedObject as base class for user classes!!! nobody can use ownership properly...


/**
 * Ultimate base class for cOwnedObject, and thus for nearly all
 * \opp classes. cObject is a <b>lightweight common base class</b>:
 * it only contains virtual member functions but NO DATA MEMBERS at all.
 *
 * Note: In OMNeT++ 2.x and 3.x, cObject was called cPolymorphic, and the
 * then-cObject class, a heavier base class has been renamed to cOwnedObject.
 *
 * It is recommended to use cObject as a base class for any class
 * that has at least one virtual member function. This makes the class more
 * interoperable with \opp, and causes no extra overhead at all.
 * sizeof(cObject) should yield 4 on a 32-bit architecture (4-byte vptr),
 * and using cObject as a base class doesn't add anything to the size because
 * a class with a virtual function already has a vptr.
 *
 * cObject allows the object to be displayed in graphical user
 * interface (Tkenv) via the className(), info() and detailedInfo() methods
 * which you may choose to redefine in your own subclasses.
 *
 * Using cObject also strengthens type safety. <tt>cObject *</tt>
 * pointers should replace <tt>void *</tt> in most places where you need
 * pointers to "any data structure". Using cObject will allow safe
 * downcasts using <tt>dynamic_cast</tt> and also \opp's
 * <tt>check_and_cast</tt>.
 *
 * @ingroup SimCore
 */
class SIM_API cObject
{
    friend class cOwnedObject;
    friend class cDefaultList;

  public:
    // internal: returns a descriptor object for this object
    virtual cClassDescriptor *getDescriptor();

  protected:
    // internal
    virtual void ownedObjectDeleted(cOwnedObject *obj);

    // internal
    virtual void yieldOwnership(cOwnedObject *obj, cObject *to);

  public:
    /**
     * Constructor. It has an empty body. (The class doesn't have data members
     * and there's nothing special to do at construction time.)
     */
    cObject() {}

    /**
     * Destructor. It has an empty body (the class doesn't have data members.)
     * It is declared here only to make the class polymorphic and make its
     * destructor virtual.
     */
    virtual ~cObject();

    /**
     * Returns a pointer to the class name string. This method is implemented
     * using typeid (C++ RTTI), and it does not need to be overridden
     * in subclasses.
     */
    virtual const char *className() const;

    /** @name Empty virtual functions which can be redefined in subclasses */
    //@{
    /**
     * Returns pointer to the object's name. It should never return NULL.
     * This default implementation just returns an empty string ("").
     */
    virtual const char *name() const  {return "";}

     /**
     * Returns true if the object's name is identical to the string passed.
     */
    bool isName(const char *s) const {return !opp_strcmp(name(),s);}

    /**
     * When this object is part of a vector (like a submodule can be part of
     * a module vector, or a gate can be part of a gate vector), this method
     * returns the object's name with the index in brackets; for example:
     * "out[5]".
     *
     * This default implementation just returns name().
     */
    virtual const char *fullName() const  {return name();}

    /**
     * Returns the full path of the object in the object hierarchy,
     * like "net.host[2].tcp.winsize". This method relies on owner():
     * if there is an owner object, this method returns the owner's fullPath
     * plus this object's fullName, separated by a dot; otherwise it simply
     * returns fullName.
     */
    virtual std::string fullPath() const;

    /**
     * Can be redefined to produce a one-line description of object.
     * The string appears in the graphical user interface (Tkenv) e.g. when
     * the object is displayed in a listbox. The returned string should
     * possibly be at most 80-100 characters long, and must not contain
     * newline.
     *
     * @see detailedInfo()
     */
    virtual std::string info() const;

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
    virtual cObject *dup() const;
    //@}

  protected:
    /** @name Ownership control.
     *
     * The following functions are intended to be used by derived container
     * classes to manage ownership of their contained objects.
     * See object description for more info on ownership management.
     */
    //@{

    /**
     * Makes this object the owner of 'object'.
     * The function called by the container object when it takes ownership
     * of the obj object that is inserted into it.
     *
     * The obj pointer should not be NULL.
     */
    virtual void take(cOwnedObject *obj);

    /**
     * Releases ownership of `object', giving it back to its default owner.
     * The function called by the container object when obj is removed
     * from the container -- releases the ownership of the object and
     * hands it over to its default owner.
     *
     * The obj pointer should not be NULL.
     */
    virtual void drop(cOwnedObject *obj);

    /**
     * This is a shortcut for the sequence
     * <pre>
     *   drop(obj);
     *   delete obj;
     * </pre>
     *
     * It is especially useful when writing destructors and assignment operators.
     *
     * Passing NULL is allowed.
     *
     * @see drop()
     */
    void dropAndDelete(cOwnedObject *obj);
    //@}

  public:
    /** @name Support for parallel execution.
     *
     * Packs/unpacks object from/to a buffer. These functions are used by parallel execution.
     * These functions should be redefined in all derived classes whose instances
     * are expected to travel across partitions.
     */
    //@{
    /**
     * Serializes the object into a buffer.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from a buffer.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Miscellaneous functions. */
    //@{
    /**
     * May be redefined to return an owner or parent object. This default
     * implementation just returns NULL.
     */
    virtual cObject *owner() const {return NULL;}

    /**
     * Returns true if this class is a subclass of cOwnedObject.
     * This is a performance optimization, to avoid frequent calls to
     * dynamic_cast<>. In cObject this method returns false, in
     * cOwnedObject it returns true, and it MUST NOT be defined on
     * any other class.
     */
    virtual bool isOwnedObject() const {return false;}

    /**
     * Enables traversing the object tree, performing some operation on
     * each object. The operation is encapsulated in the particular subclass
     * of cVisitor.
     *
     * This method should be redefined in every subclass to call v->visit(obj)
     * for every obj object contained.
     */
    virtual void forEachChild(cVisitor *v);

    /**
     * Finds the object with the given name. This function is useful when called
     * on subclasses that are containers. This method
     * finds the object with the given name in a container object and
     * returns a pointer to it or NULL if the object hasn't
     * been found. If deep is false, only objects directly
     * contained will be searched, otherwise the function searches the
     * whole subtree for the object. It uses the forEachChild() mechanism.
     *
     * Do not use it for finding submodules! Use cModule::moduleByRelativePath()
     * instead.
     */
    cObject *findObject(const char *name, bool deep=true);
    //@}

    /** @name Helper functions. */
    //@{
    /**
     * Convenience function: throws a cRuntimeError ("copying not supported")
     * stating that assignment, copy constructor and dup() won't work
     * for this object. You can call this from operator=() if you don't want to
     * implement object copying.
     */
    void copyNotSupported() const;
    //@}
};


/**
 * Utility class, to make it impossible to call the operator= and copy
 * constructor of any class derived from it.
 *
 * NOTE: <tt>dup()</tt> must be redefined as <tt>{copyNotSupported(); return NULL;}</tt>
 * in classes directly subclassing from noncopyable, but this is not needed
 * in classes derived from them. For example, cDefaultList is noncopyable,
 * so cModule, cCompoundModule and cSimpleModule no longer need to redefine
 * <tt>dup()</tt>.
 *
 * This class is from boost, which bears the following copyright:
 *
 * (C) Copyright Boost.org 1999-2003. Permission to copy, use, modify, sell
 * and distribute this software is granted provided this copyright
 * notice appears in all copies. This software is provided "as is" without
 * express or implied warranty, and with no claim as to its suitability for
 * any purpose.
 */
class noncopyable
{
  protected:
    noncopyable() {}
    ~noncopyable() {}
  private:
    // private, and in addition unimplemented (usage causes missing linker symbol)
    noncopyable(const noncopyable& x);
    const noncopyable& operator=(const noncopyable&);
};

NAMESPACE_END


#endif

