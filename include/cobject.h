//==========================================================================
//   COBJECT.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//  Declaration of the following classes:
//    cObject      : general base class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __COBJECT_H
#define __COBJECT_H

#include <typeinfo>
#include <iostream>
#include "cenvir.h"
#include "defs.h"
#include "util.h"
#include "cpolymorphic.h"
#include "cexception.h"


#define MAX_INTERNAL_NAME 11  /* should be 4n+3 */

//=== classes declared here
class  cObject;
class  cStaticFlag;

//=== classes mentioned
class  cCommBuffer;
class  cArray;
class  cDefaultList;


/**
 * Prototype for functions that can be used by cQueue objects configured as
 * priority queues to compare two objects.
 * Return values of CompareFunc should be
 * - greater than zero if a > b
 * - smaller than zero if a < b
 *
 * @ingroup EnumsTypes
 */
typedef int (*CompareFunc)(cObject *a, cObject *b);

/**
 * Prototype for functions that can be used with the foreach() mechanism
 * defined in cObject.
 * @ingroup EnumsTypes
 */
typedef bool (*ForeachFunc)(cObject *,bool);


/**
 * cObject is the base class for almost all classes in the OMNeT++ library.
 *
 * Containing 5 pointers and 2 flags as data and several virtual functions,
 * cObject is a relatively heavyweight class (about 24 bytes on a 32-bit
 * architecture). For small classes you may consider choosing cPolymorphic
 * as a base class.
 *
 * The two main areas covered by cObject are:
 *    -# name string via name() and setName()
 *    -# ownership management
 *
 * className() is inherited from cPolymorphic.
 *
 * cObject provides a <b>name</b> member (a dynamically allocated string).
 *
 * When subclassing cObject, some virtual member functions are expected to be
 * redefined: dup() are mandatory to be redefined, and often
 * you'll want to redefine info() and writeContents(), too.
 *
 * OMNeT++ provides a fairly complex memory management via the object ownership
 * mechanism. It usually works well without you paying attention, but it
 * generally helps to understand how it works.
 *
 *FIXME change description!!!
 * <b>Ownership means exclusive right and duty to delete owned objects.</b>
 * The owner of any 'o' object is returned by o->owner(), and can be changed
 * with o->setOwner(). The destructor of cObject deletes (via delete ) all owned
 * objects, and so do all classes derived from cObject.
 *
 * Ownership internally works via 4 pointers as cObject data members:
 * ownerp points to owner object; prevp/nextp points to previous/next object
 * in owner's owned objects list; firstchildp points to first owned object.
 * The setOwner() method works on the ownerp, prevp and nextp pointers.
 *
 * The object ownership mechanism is competely independent of the way
 * containers (cArray, cQueue) hold contained objects. For example, a cQueue may
 * actually own all, some, or none of the object in the queue. (Container
 * classes use mechanisms independent of firstchildp to store contained objects;
 * e.g., cArray uses an array, while cQueue uses a separate list).
 * Exception is cHead, which displays owned objects as contents.
 *
 * @ingroup SimCore
 */

/* comment to integrate:
   What cObject does:
      - owner of a new object can be explicitly given, if omitted,
        defaultOwner() will will be used.
      - an object created thru the copy constructor:
          - will have the same owner as original;
          - does not dup() or take objects owned by the original.
      - destructor calls delete for owned objects (see later)
   Objects contained as data members:
      the enclosing object should own them.
   What container objects derived from cObject should do:
      - they use the functions: take(obj), drop(obj), delete obj
      - when an object is removed, they should call drop(obj) for it if
        they were the owner.
      - copy constructor copies should dup() and take ownership of objects
        that were owned by the original.
*/
class SIM_API cObject : public cPolymorphic
{
  private:
    friend class cDefaultList;

    union
    {
        char *p;
        char chars[MAX_INTERNAL_NAME+1];
        // last char: !=0 if p is used; =0 if chars[] is used (then also serves as EOS for 10-long strings)
    } nameunion;

    cObject *ownerp;       // owner pointer
    int pos;               // used if owner is a cDefaultList

    // list in which objects are accumulated if there's no simple module in context
    // (see also setDefaultOwner() and cSimulation::setContextModule())
    static cDefaultList *defaultowner;

    // global variables for statistics
    static long total_objs;
    static long live_objs;

  protected:
    static char fullpathbuf[MAX_OBJECTFULLPATH]; // buffer for fullPath()

  private:
    // internal
    virtual void ownedObjectDeleted(cObject *obj);

    // internal
    virtual void yieldOwnership(cObject *obj, cObject *to);

  public:
    // internal
    virtual void removeFromOwnershipTree();

    // internal
    static void setDefaultOwner(cDefaultList *list);

    // called internally by cSimpleModule::snapshot(), and in turn it calls writeContents()
    virtual void writeTo(std::ostream& os);

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
    virtual void take(cObject *obj);

    /**
     * Releases ownership of `object'. Actually it gives ownership of `object'
     * back to its default owner.
     * The function called by the container object when obj is removed
     * from the container -- releases the ownership of the object and
     * hands it over to its default owner.
     *
     * The obj pointer should not be NULL.
     */
    virtual void drop(cObject *obj);

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
    void dropAndDelete(cObject *obj);
    //@}

    /** @name Helper functions. */
    //@{

    /**
     * throws a cException ("copying not supported") stating that assignment, copy constructor and dup()
     * won't work for this object. This is a convenience function to be
     * called from the operator=() method of cObject subclasses that do
     * not wish to implement object copying.
     */
    void copyNotSupported() const;
    //@}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor. In derived classes, it is usually implemented
     * as <tt>{operator=(obj);</tt>
     */
    cObject(const cObject& obj);

    /**
     * Create object without a name.
     *
     * The initial owner of the object will be defaultOwer(); during the simulation,
     * this the 'locals' member of the currently active simple module;
     * otherwise it is the 'defaultList' global variable.
     */
    cObject();

    /**
     * Create object with given name. See cObject() constructor about initial
     * ownership of the object.
     */
    explicit cObject(const char *name);

    /**
     * Destructor. It does the following:
     *    -# notifies the environment by calling ev.objectDeleted(this) that
     *       the object was deleted. This enables any open inspector windows
     *       to be closed.
     *    -# removes the object from the owner object's list (setOwner(NULL))
     *    -# <b>deletes all owned objects</b> by calling delete
     *    -# deallocates object name
     */
    virtual ~cObject();

    /**
     * Duplicates this object.  Duplicates the object and returns
     * a pointer to the new one. Must be redefined in derived classes!
     * In derived classes, it is usually implemented as
     * <tt>return new cObject(*this)</tt>.
     */
    virtual cObject *dup() const    {return new cObject(*this);}

    /**
     * The assignment operator. Derived classes should contain similar
     * methods (<tt>cClassName& cClassName::operator=(cClassName&)</tt>).
     *
     * Assigment copies the contents of the object EXCEPT for the name string.
     * If you want to copy the name string, you can do it by hand:
     * <tt>setName(o.name()</tt>).
     *
     * Ownership of the object is not affected by assigments.
     */
    cObject& operator=(const cObject& o);
    //@}

    /** @name Handling the name string.
     *
     * NOTE: "" and NULL are treated liberally: "" is stored as NULL and
     * NULL is returned as "".
     */
    //@{

    /**
     * Sets object's name. The object creates its own copy of the string.
     * NULL pointer may also be passed.
     */
    void setName(const char *s);

    /**
     * Returns pointer to the object's name. The function never returns
     * NULL; rather, it returns ptr to "".
     */
    const char *name() const  {return nameunion.chars[MAX_INTERNAL_NAME] ? nameunion.p : nameunion.chars;}

    /**
     * Returns true if the object's name is identical to the
     * string passed.
     */
    bool isName(const char *s) const {return !opp_strcmp(name(),s);}

    /**
     * Returns a name that includes the object 'index' (e.g. in a module vector),
     * like "modem[5]".
     * To be redefined in descendants. E.g., see cModule::fullName().
     */
    virtual const char *fullName() const  {return name();}

    /**
     * Returns the full path of the object in the object hierarchy,
     * like "comp.modem[5].baud-rate". NOTE: the returned pointer points
     * into a static buffer, which is overwritten by subsequent calls!
     */
    virtual const char *fullPath() const;

    /**
     * Returns the full path of the object in the object hierarchy,
     * like "comp.modem[5].baud-rate". The result is placed into the buffer passed.
     */
    virtual const char *fullPath(char *buffer, int buffersize) const;
    //@}

    /** @name Object ownership. See object description for more info on ownership management. */
    //@{

    /**
     * Returns pointer to the owner of the object.
     */
    cObject *owner() const {return ownerp;}

    /**
     * Returns false, which means if this A object is the owner of some other object B,
     * then B cannot be taken by any other object (see take()) -- an error will
     * be raised saying the object is owned by A. This method only returns true
     * in cDefaultList.
     */
    virtual bool isSoftOwner()  {return false;}

    /**
     * The object that will be the owner of new or dropped (see drop()) objects.
     * The default owner is set internally, it is usually the simple module processing
     * the current event.
     */
    static cDefaultList *defaultOwner();

    //@}

    /** @name Reflection, support for debugging and snapshots. */
    //@{
    /**
     * This function is indirectly invoked by snapshot(). It is
     * expected to write textual information about the object and other
     * objects it contains to the stream.
     *
     * This default version (cObject::writeContents()) prints detailedInfo()
     * and uses forEach() to call info() for contained objects. It only needs
     * to be redefined if this behaviour is should be customized.
     */
    virtual void writeContents(std::ostream& os);
    //@}

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
     * Call f for each contained object.
     *
     * Makes sense with container objects derived from cObject.
     * Calls the <I>f</I> function recursively for each object contained
     * in this object.
     *
     * The passed function is called for each
     * object twice: once on entering and once on leaving the object.
     * In addition, after the first ('entering') call to the function,
     * it signals with its return value whether it wants to go deeper
     * in the contained objects or not.
     *
     * Functions passed to forEach() will use static variables
     * to store other necessary information. (Yes, this limits their
     * recursive use :-( ).
     *
     * forEach() takes a function do_fn (of ForeachFunc
     * type) with 2 arguments: a cObject* and a bool.
     * First, forEach() should call do_fn(this,true)
     * to inform the function about entering the object. Then, if this
     * call returned true, it must call forEach(do_fn)
     * for every contained object. Finally, it must call do_fn(this,false)
     * to let do_fn know that there's no more contained object.
     *
     * Functions using forEach() work in the following way:
     * they call do_fn(NULL, false, <additional args>)
     * to initialize the static variables inside the function with the
     * additional args passed. Then they call forEach(do_fn)
     * for the given object. Finally, read the results by calling do_fn(NULL,
     * false, <additional args>), where additional args can
     * be pointers where the result should be stored. ForeachFuncs
     * mustn't call themselves recursively!
     *
     * <I>I know the foreach() mechanism described here is a bit weird.
     * Actually I wrote it a long ago, and changing it now would take quite
     * some work. And after all, it works..</I>
     */
    virtual void forEach(ForeachFunc f);

    /**
     * Finds the object with the given name. This function is useful when called
     * on cObject subclasses that are containers. This method
     * finds the object with the given name in a container object and
     * returns a pointer to it or NULL if the object hasn't
     * been found. If deep is false, only objects directly
     * contained will be searched, otherwise the function searches the
     * whole subtree for the object. It uses the forEach() mechanism.
     */
    cObject *findObject(const char *name, bool deep=true);

    /**
     * This function compares to objects by name. It can be used in a
     * priority queue (class cQueue) as a sorting criterion.
     */
    static int cmpbyname(cObject *one, cObject *other);
    //@}

    /** @name Statistics. */
    //@{
    /**
     * Returns the total number of objects created since the start of the program
     * (or since the last reset). The counter is incremented by cObject constructor.
     * Counter is <tt>signed</tt> to make it easier to detect if it overflows
     * during very long simulation runs.
     * May be useful for profiling or debugging memory leaks.
     */
    static long totalObjectCount() {return total_objs;}

    /**
     * Returns the number of objects that currently exist in the program.
     * The counter is incremented by cObject constructor and decremented by
     * the destructor.
     * May be useful for profiling or debugging memory leaks.
     */
    static long liveObjectCount() {return live_objs;}

    /**
     * Reset counters used by totalObjectCount() and liveObjectCount().
     * (Note that liveObjectCount() may go negative after a reset call.)
     */
    static void resetMessageCounters()  {total_objs=live_objs=0L;}
    //@}
};


//
// Internal helper class for cObject.
//
class cStaticFlag
{
  private:
    static bool staticflag;  // set to true while in main()
  public:
    cStaticFlag()  {staticflag = true;}
    ~cStaticFlag() {staticflag = false;}
    static bool isSet() {return staticflag;}
};

std::ostream& operator<< (std::ostream& os, const cObject *p);
std::ostream& operator<< (std::ostream& os, const cObject& o);

inline std::ostream& operator<< (std::ostream& os, cObject *p) {
    return os << (const cObject *)p;
}

inline std::ostream& operator<< (std::ostream& os, cObject& o) {
    return os << (const cObject&)o;
}


/**
 * Cast an object pointer to the given C++ type and throw exception if fails.
 * The method calls dynamic_cast<T>(p) where T is a type you supplied;
 * if the result is NULL (which indicates incompatible types), an exception
 * is thrown.
 *
 * Example:
 * <pre>
 *   cMessage *msg = receive();
 *   // MyPacket is a subclass of cMessage. The next line makes sure
 *   // it is actually a MyPacket that we received -- if not, the simulation
 *   // stops with an error message as the result of the exception
 *   MyPacket *pkt = check_and_cast<MyPacket *>(msg);
 * </pre>
 *
 * @ingroup Functions
 */
// Note: this function cannot be put into utils.h (circular dependencies)
template<class T>
T check_and_cast(cPolymorphic *p)
{
    if (!p)
        throw new cException("check_and_cast(): cannot cast NULL pointer to type '%s'",opp_typename(typeid(T)));
    T ret = dynamic_cast<T>(p);
    if (!ret)
        throw new cException("check_and_cast(): cannot cast (%s *)%s to type '%s'",p->className(),p->fullPath(),opp_typename(typeid(T)));
    return ret;
}

#endif

