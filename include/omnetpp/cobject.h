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
#include <iostream>
#include "simkerneldefs.h"
#include "cvisitor.h"

// Note: we include regmacros.h here in order to prevent the compiler from
// misunderstanding Register_Class() in other source files as a function declaration
#include "regmacros.h"

namespace omnetpp {

class cCommBuffer;
class cClassDescriptor;
class cOwnedObject;
class cSoftOwner;


/**
 * @brief cObject is a lightweight class which serves as the root of the
 * \opp class hierarchy. cObject, via virtual and pure virtual methods,
 * defines several properties and mechanisms that are used throughout
 * the entire simulation library.
 *
 * These mechanisms are the following:
 *
 * - Name string. There are three virtual methods: getName(), getFullName()
 *   and getFullPath(). They are expected to be overridden subclasses as cObject
 *   only contains trivial implementations, e.g. getName() always returns the
 *   empty string. getName() returns a "base name"; getFullName() augments
 *   the base name with auxiliary information such as an index; and getFullPath()
 *   includes the object's location in the object hierarchy. An example: for a
 *   module in a network simulation, getName() may return "eth", getFullName()
 *   may return "eth[2]", and getFullPath() may return "Net.host[4].eth[2]".
 *
 * - Class name. getClassName() returns the fully qualified C++ class name.
 *   Its operation is based on the C++ typeid() operator and the type_info class.
 *
 * - Info string. The str() method can be overridden in subclasses to return a
 *   one-line concise description of the object's contents, e.g. for display
 *   in graphical user interfaces.
 *
 * - Cloning. The dup() method is to be overridden in subclasses to return an
 *   exact copy of this object.
 *
 * - Iteration. The forEachChild() method is to be overridden in subclasses to
 *   iterate over (cObject-derived) objects that it contains or holds.
 *   This method is primarily used by graphical user interfaces to display
 *   an object hierarchy, and for searching.
 *
 * - Ownership mechanism. The ownership mechanism helps define an object
 *   containment hierarchy, and also defines responsibility of deletion.
 *   Several methods belong here, the most visible one being getOwner().
 *   A cObject itself does not keep track of its owner (the method returns
 *   nullptr), an actual owner pointer is added by the subclass cOwnedObject.
 *   cObject also provides the drop(), take() and dropAndDelete() methods
 *   that sublasses may utilize in implementing their ownership management.
 *
 * - Packing/unpacking. The parsimPack() and parsimUnpack() methods can be
 *   overridden to allow instances to be transferred via MPI for parallel
 *   simulation.
 *
 * - Reflection. The getDescriptor() method returns a cClassDescriptor object
 *   that allows for enumerating the object's fields, getting/setting their
 *   values, etc.
 *
 * In model code, it usually makes sense to use cObject as a base class for
 * any class that is expected to be polymorphic. In this case, cObject does
 * not add any runtime overhead (as it contains no data members). The
 * appropriate member functions (dup(), str(), etc.) are expected to be
 * redefined.
 *
 * @ingroup Fundamentals
 */
class SIM_API cObject
{
    friend class cOwnedObject;
    friend class cSoftOwner;

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
    // internal
    void takeAllObjectsFrom(cSoftOwner *list);

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
    cObject(const cObject& other) = default;

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
    bool isName(const char *s) const;

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
     * Returns a string that contains the class name and the full name
     * of the object in a human-friendly format. Example: "(cMessage)msg"
     *
     * @see getClassName(), getFullName()
     */
    virtual std::string getClassAndFullName() const;

    /**
     * Returns a string that contains the class name and the full path of the
     * object in a human-friendly format. Example: "(cMessage)foo.bar.msg"
     *
     * @see getClassName(), getFullPath()
     */
    virtual std::string getClassAndFullPath() const;

    /**
     * Returns the this pointer. Utility function for the logging macros.
     */
    const cObject *getThisPtr() const  {return this;}  //Note: nonvirtual

    /**
     * Returns a brief, one-line description of the object. The returned string
     * does (should) NOT include the object's name and class. This method is
     * used to display object information at several places in the Qtenv GUI,
     * among others.
     */
    virtual std::string str() const;

    /**
     * Prints the object on the given stream. The default implementation prints
     * the class name, the object full name, and the brief description.
     * The default stream output operator (operator<<) for cObject descendants
     * delegates to this method.
     *
     * @see getClassName(), getFullName, str()
     */
    virtual std::ostream& printOn(std::ostream& os) const;

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
     * Returns the owner (or parent) object. nullptr as return value is legal,
     * and may mean "unknown" (=not tracked) or "no owner".
     * This default implementation just returns nullptr (meaning "unknown").
     * (With the cOwnedObject subclass which does maintain an owner pointer,
     * a nullptr return value would mean "no owner".)
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
     * Returns true if this object is a "soft owner" and false otherwise.
     * Currently the only soft owner class is cSoftOwner (and its subclasses,
     * most notably cModule and cSimpleModule), all others are "hard owners".
     * An object is soft owner if it allows another object to take ownership
     * (via take()) of an object it currently owns. Hard owners will raise
     * an error if some other object tries to take() an object that they own.
     */
    virtual bool isSoftOwner() const {return false;}

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

    /**
     * Returns the descriptor object for (the class of) this object.
     */
    virtual cClassDescriptor *getDescriptor() const;
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

// operator<<

template<typename T>
typename std::enable_if<std::is_base_of<cObject, T>::value, std::ostream&>::type
operator<<(std::ostream& os, const T *p) {
    if (p)
        return p->printOn(os);
    else
        return os << "<nullptr>";
}

template<typename T>
typename std::enable_if<std::is_base_of<cObject, T>::value, std::ostream&>::type
operator<<(std::ostream& os, const T& o) {
    return o.printOn(os);
}

// as_cObject

template <typename T>
typename std::enable_if<std::is_polymorphic<T>::value, cObject*>::type
as_cObject(T *p) { return dynamic_cast<cObject*>(p); }

template <typename T>
typename std::enable_if<!std::is_polymorphic<T>::value, cObject*>::type
as_cObject(T *p) { return nullptr; }

template <typename T>
typename std::enable_if<std::is_polymorphic<T>::value, const cObject*>::type
as_cObject(const T *p) { return dynamic_cast<const cObject*>(p); }

template <typename T>
typename std::enable_if<!std::is_polymorphic<T>::value, const cObject*>::type
as_cObject(const T *p) { return nullptr; }

/**
 * @brief Utility class, to make it impossible to call the operator= and copy
 * constructor of any class derived from it.
 *
 * NOTE: <tt>dup()</tt> must be redefined as <tt>{copyNotSupported(); return nullptr;}</tt>
 * in classes directly subclassing from noncopyable, but this is not needed
 * in classes derived from them. For example, cSoftOwner is noncopyable,
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
    // private, and in addition deleted
    noncopyable(const noncopyable& x) = delete;
    const noncopyable& operator=(const noncopyable&) = delete;
};

}  // namespace omnetpp


#endif

