//==========================================================================
//  COWNEDOBJECT.H - part of
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

#ifndef __OMNETPP_COWNEDOBJECT_H
#define __OMNETPP_COWNEDOBJECT_H

#include <utility>
#include <typeinfo>
#include <iostream>
#include "simkerneldefs.h"
#include "cnamedobject.h"
#include "cexception.h"

namespace omnetpp {

class cOwnedObject;
class cNoncopyableOwnedObject;
class cStaticFlag;
class cSimulation;
class cSoftOwner;
class cMessage;
class cPacket;

namespace internal { class Void; }

/**
 * @brief A cObject that keeps track of its owner. It serves as base class
 * for many classes in the \opp library.
 *
 * Instances of cOwnedObjects are kept track of by the simulation kernel,
 * and may be inserted into cQueue and cArray.
 *
 * It is not always a good idea to subclass your own classes from
 * cOwnedObject, especially if they are small data objects. The more
 * lightweight cObject is often a better choice.
 *
 * Ownership management helps \opp catch common programming errors.
 * The term ownership means the exclusive right and duty to delete
 * owned objects.
 *
 * cOwnedObjects hold a pointer to their owner objects; the getOwner() method
 * returns this pointer. An example will help to understand how it is used:
 *
 *    - when you insert a cMessage into a cQueue, the cQueue will become
 *      the owner of the message, and will set the message's owner to itself.
 *
 *    - a message object can be at one place only at any given time.
 *      When you try to insert the same cMessage again into another (or the same)
 *      cQueue, you'll get an error message that it is already in a cQueue --
 *      sparing you a sure crash later.
 *
 *    - similarly, if you try to send the same message, you'll get an error
 *      message that it cannot be sent because it is still enqueued --
 *      another crash scenario eliminated. Like the previous one, this
 *      test is done by checking the owner pointer in cMessage.
 *
 *    - even if you try to delete the message while it is in the queue,
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
 *      insert it into a queue. 
 *      *In short: the ownership mechanism is good to your health.*
 *
 *    - when the queue is deleted, it also deletes all objects it contains.
 *      (The cQueue always owns all objects inserted into it -- no exception).
 *
 * The above ownership mechanisms are at work when any cOwnedObject-subclass object
 * gets inserted into any cOwnedObject-subclass container (cQueue, cArray).
 *
 * Some more details, in case you are writing a class that acts as a container:
 *
 *    - you should use the functions take(), drop() on inserting/removing objects
 *    - you should delete the owned objects in the destructor
 *    - the copy constructor of a container should dup() the owned objects
 *      and take() the copies
 *    - if you want to have a class which contains cOwnedObject-subclasses as
 *      data members: your class (the enclosing object) should own them --
 *      call take() from the constructor and drop() from the destructor.
 *
 * @ingroup Fundamentals
 */
class SIM_API cOwnedObject : public cNamedObject
{
    friend class cObject;
    friend class cNoncopyableOwnedObject;
    friend class cSoftOwner;
    friend class cSimulation;
    friend class cMessage;  // because of refcounting business
    friend class cPacket;   // because of refcounting business

  private:
    cObject *owner;    // owner pointer
    unsigned int pos;  // used only when owner is a cSoftOwner

    // list in which objects are accumulated if there is no simple module in context
    // (see also setOwningContext() and cSimulation::setContextModule())
    static cSoftOwner *owningContext;

    // global variables for statistics
    static long totalObjectCount;
    static long liveObjectCount;

  private:
    cOwnedObject(const char *name, bool namepooling, internal::Void *dummy);
    void copy(const cOwnedObject& obj);

  public:
    // internal
    virtual void removeFromOwnershipTree();

    // internal
    static void setOwningContext(cSoftOwner *list);

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
     * Copy constructor.
     */
    cOwnedObject(const cOwnedObject& obj);

    /**
     * Destructor.
     */
    virtual ~cOwnedObject();

    /**
     * The assignment operator. Derived classes should contain similar
     * methods (`cClassName& cClassName::operator=(cClassName&)`).
     *
     * Assignment copies the contents of the object EXCEPT for the name string.
     * If you want to copy the name string, you can do it by hand:
     * `setName(o.getName()`).
     *
     * Ownership of the object is not affected by assignments.
     */
    cOwnedObject& operator=(const cOwnedObject& o);

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

    /** @name Object ownership */
    //@{
    /**
     * Returns pointer to the owner of the object.
     */
    virtual cObject *getOwner() const override {return owner;}

    /**
     * Returns true.
     */
    virtual bool isOwnedObject() const override {return true;}

    /**
     * The object that will be the owner of new or dropped (see drop()) objects.
     * The default owner is set internally; it is usually the component in context
     * (see cSimulation::getContext()).
     */
    static cSoftOwner *getOwningContext();
    //@}

    /** @name Statistics. */
    //@{
    /**
     * Returns the total number of objects created since the start of the program
     * (or since the last reset). The counter is incremented by cOwnedObject constructor.
     * Counter is `signed` to make it easier to detect if it overflows
     * during very long simulation runs.
     * May be useful for profiling or debugging memory leaks.
     */
    static long getTotalObjectCount() {return totalObjectCount;}

    /**
     * Returns the number of objects that currently exist in the program.
     * The counter is incremented by cOwnedObject constructor and decremented by
     * the destructor.
     * May be useful for profiling or debugging memory leaks.
     */
    static long getLiveObjectCount() {return liveObjectCount;}

    /**
     * Reset counters used by getTotalObjectCount() and getLiveObjectCount().
     * (Note that getLiveObjectCount() may go negative after a reset call.)
     */
    static void resetObjectCounters()  {totalObjectCount=liveObjectCount=0L;}
    //@}
};


/**
 * @brief Base class for cOwnedObject-based classes that do not wish to support
 * assignment and duplication.
 *
 * @ingroup Misc
 */
class SIM_API cNoncopyableOwnedObject : public cOwnedObject, noncopyable
{
    friend class cSoftOwner;
  private:
    cNoncopyableOwnedObject(const char *name, bool namepooling, internal::Void *dummy) : cOwnedObject(name, namepooling, dummy) {}
    virtual void parsimPack(cCommBuffer *) const override {throw cRuntimeError(this, E_CANTPACK);}
    virtual void parsimUnpack(cCommBuffer *) override {throw cRuntimeError(this, E_CANTPACK);}

  public:
    /**
     * Constructor
     */
    explicit cNoncopyableOwnedObject(const char *name=nullptr, bool namepooling=true) :
        cOwnedObject(name, namepooling) {}

    /**
     * Duplication not supported, this method is redefined to throw an error.
     */
    virtual cNoncopyableOwnedObject *dup() const override;
};


//
// Internal class: provides a flag that shows if control is in the main() function.
//
class SIM_API cStaticFlag
{
  public:
    cStaticFlag();
    ~cStaticFlag();
    static bool insideMain();
    static bool isExiting();
    static void setExiting();
};

}  // namespace omnetpp


#endif

