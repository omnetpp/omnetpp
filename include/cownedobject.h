//==========================================================================
//  COWNEDOBJECT.H - part of
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

#ifndef __COWNEDOBJECT_H
#define __COWNEDOBJECT_H

#include <typeinfo>
#include <iostream>
#include "cenvir.h"
#include "simkerneldefs.h"
#include "util.h"
#include "cnamedobject.h"
#include "cexception.h"

NAMESPACE_BEGIN


class cOwnedObject;
class cStaticFlag;

class cArray;
class cDefaultList;


/**
 * Prototype for functions that can be used by cQueue objects configured as
 * priority queues to compare two objects.
 * Return values of CompareFunc should be
 * - greater than zero if a > b
 * - smaller than zero if a < b
 *
 * @ingroup EnumsTypes
 */
typedef int (*CompareFunc)(cOwnedObject *a, cOwnedObject *b);


/**
 * FIXME revise this
 *
 * Base class for almost all classes in the \opp library.
 *
 * It is usually NOT a good idea to subclass your own classes
 * (esp. data storage classes) from cOwnedObject,
 * because it is a relatively heavyweight class (about 24 bytes on a 32-bit
 * architecture) with many virtual functions that need to be redefined.
 * You may consider choosing cObject instead as a base class.
 *
 * The main areas covered by cOwnedObject are:
 *    -# storage of a name string via name() and setName()
 *    -# providing a mechanism to recursively traverse all simulation objects
 *       (forEachChild() method and cVisitor class). This mechanism constitutes
 *       the foundation Tkenv, the simulation GUI.
 *    -# ownership management, to safeguard against common programming errors.
 *       Owner pointer also enables navigating the object tree upwards.
 *
 * When subclassing cOwnedObject, some virtual member functions are expected to be
 * redefined: dup() are mandatory to be redefined, and often
 * you'll want to redefine info() and detailedInfo() as well.
 *
 * <b>Ownership management</b> helps \opp catch common programming
 * errors. As a definition, <i>ownership means the exclusive right and duty
 * to delete owned objects.</i>
 *
 * cOwnedObjects hold a pointer to their owner objects; the owner() method returns
 * this pointer. An example will help to understand how it is used:
 *
 *    - when you insert a cMessage into a cQueue, the cQueue will become
 *      the owner of the message, and will set the message's owner to itself.
 *
 *    - a message object can be at one place only at any given time.
 *      When you try to insert the same cMessage again into another (or the same)
 *      cQueue, you'll get an error message that it's already in a cQueue --
 *      sparing you a sure crash later.
 *
 *    - similarly, if you try to send the same message, you'll get an error
 *      message that it cannot be sent because it's still enqueued --
 *      another crash scenario eliminated. Like the previous one, this
 *      test is done by checking the owner pointer in cMessage.
 *
 *    - even if you try to delete the message while it's in the queue,
 *      you'll get an error message instead of just a crash. This is because
 *      cOwnedObject destructor asks for the owner's blessing -- but cQueue will
 *      protest by throwing an exception.
 *
 *    - when you remove the message from the cQueue, the cQueue will "release"
 *      the object; the current module will become the message's "soft" owner,
 *      changing the owner pointer in the message object.
 *
 *    - "soft" owner means that now you can send the message object or insert
 *      it into another cQueue -- the module as a soft owner will let it go.
 *
 *    - the same mechanism can ensure that when a self-message is currently
 *      scheduled (owner is the scheduled-events list) or sent to another module
 *      (owner is the other module) you cannot send or schedule it, or
 *      insert it into a queue. <i>In short: the ownership mechanism is good to your
 *      health.</i>
 *
 *    - when the queue is deleted, it also deletes all objects it contains.
 *      (The cQueue always owns all objects inserted into it -- no exception).
 *
 * The above ownership mechanisms are at work when any cOwnedObject-subclass object
 * gets inserted into any cOwnedObject-subclass container (cQueue, cArray).
 *
 * Some more details, in case you're writing a class that acts as a container:
 *
 *    - you should use the functions take(), drop() on inserting/removing objects
 *    - you should delete the owned objects in the destructor
 *    - the copy constructor of a container should dup() the owned objects
 *      and take() the copies
 *    - if you want to have a class which contains cOwnedObject-subclasses as
 *      data members: your class (the enclosing object) should own them --
 *      call take() from the constructor and drop() from the destructor.
 *
 * @ingroup SimCore
 */
class SIM_API cOwnedObject : public cNamedObject
{
    friend class cDefaultList;
    friend class cSimulation;
    friend class cMessage;  // because of refcounting business

  private:
    cOwnedObject *ownerp;  // owner pointer
    unsigned int pos;      // used only if owner is a cDefaultList

  private:
    // list in which objects are accumulated if there's no simple module in context
    // (see also setDefaultOwner() and cSimulation::setContextModule())
    static cDefaultList *defaultowner;

    // global variables for statistics
    static long total_objs;
    static long live_objs;

  private:
    // internal
    virtual void ownedObjectDeleted(cOwnedObject *obj);

    // internal
    virtual void yieldOwnership(cOwnedObject *obj, cOwnedObject *to);

  public:
    // internal
    virtual void removeFromOwnershipTree();

    // internal
    static void setDefaultOwner(cDefaultList *list);

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
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Create object without a name. The object will be initially owned by
     * defaultOwer().
     */
    cOwnedObject();

    /**
     * Create object with given name. The object will be initially owned by
     * defaultOwer(). Name pooling is an optimization feature.
     */
    explicit cOwnedObject(const char *name, bool namepooling=true);

    /**
     * Copy constructor. In derived classes, it is usually implemented
     * as <tt>{operator=(obj);</tt>
     */
    cOwnedObject(const cOwnedObject& obj);

    /**
     * Destructor.
     */
    virtual ~cOwnedObject();

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
    cOwnedObject& operator=(const cOwnedObject& o);

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

    /** @name Object ownership */
    //@{
    /**
     * Returns pointer to the owner of the object.
     */
    virtual cOwnedObject *owner() const {return ownerp;}

    /**
     * Returns true.
     */
    virtual bool isOwnedObject() const {return true;}

    /**
     * Returns false in cOwnedObject and in all derived classes except cDefaultList.
     * An object A is a "soft owner" if it allows a B object take() an object A owns.
     * "Hard owners" will raise an error if some other object tries to take()
     * an object they own. The only soft owner class is cDefaultList.
     */
    virtual bool isSoftOwner()  {return false;}

    /**
     * The object that will be the owner of new or dropped (see drop()) objects.
     * The default owner is set internally, it is usually the simple module processing
     * the current event.
     */
    static cDefaultList *defaultOwner();
    //@}

    /** @name Statistics. */
    //@{
    /**
     * Returns the total number of objects created since the start of the program
     * (or since the last reset). The counter is incremented by cOwnedObject constructor.
     * Counter is <tt>signed</tt> to make it easier to detect if it overflows
     * during very long simulation runs.
     * May be useful for profiling or debugging memory leaks.
     */
    static long totalObjectCount() {return total_objs;}

    /**
     * Returns the number of objects that currently exist in the program.
     * The counter is incremented by cOwnedObject constructor and decremented by
     * the destructor.
     * May be useful for profiling or debugging memory leaks.
     */
    static long liveObjectCount() {return live_objs;}

    /**
     * Reset counters used by totalObjectCount() and liveObjectCount().
     * (Note that liveObjectCount() may go negative after a reset call.)
     */
    static void resetObjectCounters()  {total_objs=live_objs=0L;}
    //@}
};


/**
 * Base class for cOwnedObject-based classes that do not wish to support
 * assignment and duplication.
 *
 * @ingroup SimCore
 */
class SIM_API cNoncopyableOwnedObject : public cOwnedObject, noncopyable
{
  public:
    /**
     * Constructor
     */
    explicit cNoncopyableOwnedObject(const char *name=NULL, bool namepooling=true) :
        cOwnedObject(name, namepooling) {}

//FIXME disable copy ctor as well

    /**
     * Duplication not supported, this method is redefined to throw an error.
     */
    virtual cNoncopyableOwnedObject *dup() const;

    /**
     * Redefined to throw an error.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Redefined to throw an error.
     */
    virtual void netUnpack(cCommBuffer *buffer);
};


//
// Internal helper class for cOwnedObject.
//
class SIM_API cStaticFlag
{
  private:
    static bool staticflag;  // set to true while in main()
  public:
    cStaticFlag()  {staticflag = true;}
    ~cStaticFlag() {staticflag = false;}
    static bool isSet() {return staticflag;}
};

std::ostream& operator<< (std::ostream& os, const cOwnedObject *p);
std::ostream& operator<< (std::ostream& os, const cOwnedObject& o);

inline std::ostream& operator<< (std::ostream& os, cOwnedObject *p) {
    return os << (const cOwnedObject *)p;
}

inline std::ostream& operator<< (std::ostream& os, cOwnedObject& o) {
    return os << (const cOwnedObject&)o;
}


/**
 * Cast an object pointer to the given C++ type and throw exception if fails.
 * The method calls dynamic_cast<T>(p) where T is a type you supplied;
 * if the result is NULL (which indicates incompatible types), an exception
 * is thrown.
 *
 * In the following example, MyPacket is a subclass of cMessage, and we want
 * to assert that the message received is actually a MyPacket. If not,
 * the simulation stops with an error message as the result of the exception.
 * <pre>
 *   cMessage *msg = receive();
 *   MyPacket *pkt = check_and_cast<MyPacket *>(msg);
 * </pre>
 *
 * @ingroup Functions
 */
// Note: this function cannot be put into utils.h (circular dependencies)
template<class T>
T check_and_cast(cObject *p)
{
    if (!p)
        throw cRuntimeError("check_and_cast(): cannot cast NULL pointer to type '%s'",opp_typename(typeid(T)));
    T ret = dynamic_cast<T>(p);
    if (!ret)
        throw cRuntimeError("check_and_cast(): cannot cast (%s *)%s to type '%s'",p->className(),p->fullPath().c_str(),opp_typename(typeid(T)));
    return ret;
}

/**
 * The 'const' version of check_and_cast<>().
 */
template<class T>
const T check_and_cast(const cObject *p)
{
    return check_and_cast<T>(const_cast<cObject *>(p));
}

NAMESPACE_END


#endif

