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
 * FIXME revise this
 *
 * Base class for almost all classes in the \opp library.
 *
 * It is usually NOT a good idea to subclass your own classes
 * (esp. data storage classes) from cNamedObject,
 * because it is a relatively heavyweight class (about 24 bytes on a 32-bit
 * architecture) with many virtual functions that need to be redefined.
 * You may consider choosing cObject instead as a base class.
 *
 * The main areas covered by cNamedObject are:
 *    -# storage of a name string via name() and setName()
 *    -# providing a mechanism to recursively traverse all simulation objects
 *       (forEachChild() method and cVisitor class). This mechanism constitutes
 *       the foundation Tkenv, the simulation GUI.
 *    -# ownership management, to safeguard against common programming errors.
 *       Owner pointer also enables navigating the object tree upwards.
 *
 * When subclassing cNamedObject, some virtual member functions are expected to be
 * redefined: dup() are mandatory to be redefined, and often
 * you'll want to redefine info() and detailedInfo() as well.
 *
 * <b>Ownership management</b> helps \opp catch common programming
 * errors. As a definition, <i>ownership means the exclusive right and duty
 * to delete owned objects.</i>
 *
 * cNamedObjects hold a pointer to their owner objects; the owner() method returns
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
 *      cNamedObject destructor asks for the owner's blessing -- but cQueue will
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
 * The above ownership mechanisms are at work when any cNamedObject-subclass object
 * gets inserted into any cNamedObject-subclass container (cQueue, cArray).
 *
 * Some more details, in case you're writing a class that acts as a container:
 *
 *    - you should use the functions take(), drop() on inserting/removing objects
 *    - you should delete the owned objects in the destructor
 *    - the copy constructor of a container should dup() the owned objects
 *      and take() the copies
 *    - if you want to have a class which contains cNamedObject-subclasses as
 *      data members: your class (the enclosing object) should own them --
 *      call take() from the constructor and drop() from the destructor.
 *
 * @ingroup SimCore
 */
class SIM_API cNamedObject : public cObject
{
  private:
    const char *namep;  // object name (stringpooled if flags & FL_NAMEPOOLING)

  protected:
    unsigned short flags;  // FL_NAMEPOOLING flag; other bits used by derived classes
    unsigned short unused; // space lost to due to word aligment; FIXME make use of it in subclasses (cModule, cSimpleModule, cGate)
    enum {FL_NAMEPOOLING = 0x1};

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

