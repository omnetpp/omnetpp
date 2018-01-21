//==========================================================================
//  COBJECT.H - part of
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

#ifndef __OMNETPP_COBJECT_H
#define __OMNETPP_COBJECT_H

#include <string>
#include "simkerneldefs.h"
#include "simutil.h"
#include "cvisitor.h"

// Note: we include regmacros.h here in order to prevent the compiler from
// misunderstanding Register_Class() in other source files as a function declaration
#include "regmacros.h"

namespace omnetpp {

class cCommBuffer;
class cClassDescriptor;
class cOwnedObject;


/**
 * @brief Root of the \opp class hierarchy. cObject is a lightweight class
 * without any data members.
 *
 * cObject and its subclass cOwnedObject define an ownership mechanism.
 * Any cObject may become owner of other objects, but owned objects must be
 * subclassed from cOwnedObject. cObject also contains methods that allow
 * the object to be displayed in graphical user interfaces (Tkenv, Qtenv).
 *
 * It is recommended to use cObject as a base class for any class
 * that has at least one virtual member function. This makes the class more
 * interoperable with \opp, and causes no extra overhead at all.
 * sizeof(cObject) should yield 4 on a 32-bit architecture (4-byte vptr),
 * and using cObject as a base class does not add anything to the size
 * of an object, because a class with a virtual function already has a vptr.
 * Subclasses are expected to redefine member functions such as dup(),
 * str(), forEachChild(), etc.
 *
 * Note: The (cPolymorphic, cObject) classes in \opp versions 2.x and 3.x
 * were renamed to (cObject, cOwnedObject) in version 4.0.
 *
 * @ingroup SimSupport
 */
class SIM_API cObject
{
    friend class cOwnedObject;
    friend class cDefaultList;

  public:
    // internal: returns a descriptor object for this object
    virtual cClassDescriptor *getDescriptor() const;

#ifdef SIMFRONTEND_SUPPORT
    // internal: used by the UI to optimize refreshes
    virtual bool hasChangedSince(int64_t refreshSerial);
    static int64_t getChangeCounter() {return changeCounter;}
#endif

  protected:
#ifdef SIMFRONTEND_SUPPORT
    // internal
    static int64_t changeCounter;
#endif

    // internal
    virtual void ownedObjectDeleted(cOwnedObject *obj);

    // internal
    virtual void yieldOwnership(cOwnedObject *obj, cObject *to);

  public:
    /**
     * Constructor. It has an empty body. (The class does not have data members
     * and there is nothing special to do at construction time.)
     */
    cObject() {}

    /**
     * Copy constructor. It has an empty body. (The class does not have data members
     * and there is nothing special to do at construction time.)
     * Subclasses should call the copy constructor of their base classes from their own
     * copy constructors.
     */
    cObject(const cObject& other) {}

    /**
     * Destructor. It has an empty body (the class does not have data members.)
     * It is declared here only to make the class polymorphic and make its
     * destructor virtual.
     */
    virtual ~cObject();

    /**
     * Returns the (fully qualified) class name. This method is implemented
     * using typeid (C++ RTTI), and it does not need to be overridden in
     * subclasses.
     */
    virtual const char *getClassName() const;

    /** @name Empty virtual functions which can be redefined in subclasses */
    //@{
    /**
     * Returns pointer to the object's name. It should never return nullptr.
     * This default implementation just returns an empty string ("").
     */
    virtual const char *getName() const  {return "";}

     /**
     * Returns true if the object's name is identical to the string passed.
     */
    bool isName(const char *s) const {return !opp_strcmp(getName(),s);}

    /**
     * When this object is part of a vector (like a submodule can be part of
     * a module vector, or a gate can be part of a gate vector), this method
     * returns the object's name with the index in brackets; for example:
     * "out[5]".
     *
     * This default implementation just returns getName().
     */
    virtual const char *getFullName() const  {return getName();}

    /**
     * Returns the full path of the object in the object hierarchy,
     * like "net.host[2].tcp.winsize". This method relies on getOwner():
     * if there is an owner object, this method returns the owner's fullPath
     * plus this object's fullName, separated by a dot; otherwise it simply
     * returns fullName.
     */
    virtual std::string getFullPath() const;

    /**
     * Returns the this pointer. Utility function for the logging macros.
     */
    const cObject *getThisPtr() const  {return this;}  //Note: nonvirtual

    /**
     * Produce a one-line description of the object. The string is displayed
     * at various places e.g. in graphical user interfaces.
     */
    virtual std::string str() const;

    /**
     * Deprecated, use str() instead. This method also delegates to str().
     */
    _OPPDEPRECATED virtual std::string info() const {return str();}

    /**
     * Deprecated. This method returns an empty string.
     */
    _OPPDEPRECATED virtual std::string detailedInfo() const {return "";}

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
     * Makes this object the owner of obj. This method should be called by
     * container classes when they take ownership of an object inserted into
     * them.
     *
     * The obj pointer should not be nullptr.
     */
    virtual void take(cOwnedObject *obj);

    /**
     * Releases ownership of obj, giving it back to its default owner.
     * This method should be called by container classes when an object is
     * removed from the container.
     *
     * The obj pointer should not be nullptr.
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
     * Passing nullptr is allowed.
     *
     * @see drop()
     */
    void dropAndDelete(cOwnedObject *obj);
    //@}

  public:
    /** @name Support for parallel execution.
     *
     * These functions pack/unpack the object from/to a communication buffer,
     * and should be redefined in classes whose instances are expected to
     * travel across partitions.
     */
    //@{
    /**
     * Serializes the object into a buffer. This default implementation throws
     * an exception ("packing not implemented").
     */
    virtual void parsimPack(cCommBuffer *buffer) const;

    /**
     * Deserializes the object from a buffer. This default implementation just
     * throws an exception ("packing not implemented").
     */
    virtual void parsimUnpack(cCommBuffer *buffer);
    //@}

    /** @name Miscellaneous functions. */
    //@{
    /**
     * Returns owner (or parent) object. This default implementation just
     * returns nullptr.
     */
    virtual cObject *getOwner() const {return nullptr;}

    /**
     * Returns true if this class is a subclass of cOwnedObject.
     * This is a performance optimization, to avoid frequent calls to
     * dynamic_cast\<\>. In cObject this method returns false, in
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
     * returns a pointer to it or nullptr if the object has not
     * been found. If deep is false, only objects directly
     * contained will be searched, otherwise the function searches the
     * whole subtree for the object. It uses the forEachChild() mechanism.
     *
     * Do not use it for finding submodules! Use cModule::getModuleByRelativePath()
     * instead.
     */
    cObject *findObject(const char *name, bool deep=true);
    //@}

    /** @name Helper functions. */
    //@{
    /**
     * Convenience function: throws a cRuntimeError ("copying not supported")
     * stating that assignment, copy constructor and dup() will not work
     * for this object. You can call this from operator=() if you do not want to
     * implement object copying.
     */
    void copyNotSupported() const;
    //@}
};


/**
 * @brief Utility class, to make it impossible to call the operator= and copy
 * constructor of any class derived from it.
 *
 * NOTE: <tt>dup()</tt> must be redefined as <tt>{copyNotSupported(); return nullptr;}</tt>
 * in classes directly subclassing from noncopyable, but this is not needed
 * in classes derived from them. For example, cDefaultList is noncopyable,
 * so cModule and cSimpleModule no longer need to redefine <tt>dup()</tt>.
 *
 * This class is from boost, which bears the following copyright:
 *
 * (C) Copyright Boost.org 1999-2015. Permission to copy, use, modify, sell
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

}  // namespace omnetpp


#endif

