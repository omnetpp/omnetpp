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
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __COBJECT_H
#define __COBJECT_H

#include <typeinfo>
#include <iostream>
#include "util.h"
#include "cexception.h"

using std::ostream;


#define FULLPATHBUF_SIZE  1024

//=== classes declared here
class  cObject;
class  cStaticFlag;

//=== classes mentioned
class  cIterator;
class  cHead;

//=== Global objects:
SIM_API extern cHead networks;            ///< List of available networks.
SIM_API extern cHead modinterfaces;       ///< List of all module interfaces.
SIM_API extern cHead modtypes;            ///< List of all module types.
SIM_API extern cHead linktypes;           ///< List of link types.
SIM_API extern cHead functions;           ///< List of function types.
SIM_API extern cHead classes;             ///< List of cClassRegister objects.
SIM_API extern cHead enums;               ///< List of cEnum objects.


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
 * cObject is a relatively heavyweight object for a base class. If you need
 * to create a new class, you may or may not choose starting from cObject
 * as base class.
 *
 * The two main areas covered by cObject are:
 *    -# basic reflection (name string via name() and setName(); className())
 *    -# ownership management
 *
 * cObject provides a <b>name</b> member (a dynamically allocated string).
 *
 * When subclassing cObject, some virtual member functions are expected to be
 * redefined: className() and dup() are mandatory to be redefined, and often
 * you'll want to redefine info() and writeContents(), too.
 *
 * OMNeT++ provides a fairly complex memory management via the object ownership
 * mechanism. It usually works well without you paying attention, but it
 * generally helps to understand how it works.
 *
 * Rule 1: <b>Ownership means exclusive right and duty to delete owned objects.</b>
 * The owner of any 'o' object is returned by o->owner(), and can be changed
 * with o->setOwner(). The destructor of cObject deletes (via discard()) all owned
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
      - destructor calls discard() for owned objects (see later).
   Objects contained as data members:
      the enclosing object should own them.
   What container objects derived from cObject should do:
      - they use the functions: take(obj), drop(obj), discard(obj)
      - when an object is inserted, if takeOwnership() is true, should
        take ownership of object by calling take(obj).
        TAKEOWNERSHIP() DEFAULTS TO true.
      - when an object is removed, they should call drop(obj) for it if
        they were the owner.
      - copy constructor copies should dup() and take ownership of objects
        that were owned by the original.
      - destructor doesn't need not call discard() for objects: this will be
        done in cObject's destructor.
*/
class SIM_API cObject
{
    friend class cHead;
    friend class cIterator;
    friend class const_cIterator;
    friend class cStaticFlag;

  protected:
    char *namestr;                    // name string
    char stor;                        // storage: Static/Auto/Dynamic ('S'/'A'/'D')

    bool tkownership;                 // for derived containers: take ownership of objects?
    cObject *ownerp, *prevp, *nextp;  // ptr to owner; linked list ptrs
    cObject *firstchildp;             // list of owned objects

    static int staticflag;            // to determine 'storage' (1 while in main())
    static int heapflag;              // to determine 'storage' (1 immediately after 'new')
    static char fullpathbuf[FULLPATHBUF_SIZE]; // buffer for fullPath()

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
     * Implementation:
     * <pre>
     *     obj->setOwner( this );
     * </pre>
     */
    void take(cObject *object)
        {object->setOwner( this );}

    /**
     * Releases ownership of `object'. Actually it gives ownership of `object'
     * back to its default owner.
     * The function called by the container object when obj
     * is removed from the container -- releases the ownership of the
     * object and hands it over to its default owner.
     *
     * Implementation:
     * <pre>
     *     obj->setOwner( obj->defaultOwner() );
     * </pre>
     */
    void drop(cObject *object)
        {object->setOwner( object->defaultOwner() );}

    /**
     * Disposes of `object'; it MUST be owned by this object.
     * The function is called when the container object has to delete
     * the contained object obj. It the object was dynamically
     * allocated (by operator new), it is deleted, otherwise (e.g., if
     * it is a global or a local variable) it is just removed from the
     * ownership hierarchy.
     *
     * Implementation:
     * <pre>
     *     if(obj->storage()=='D')
     *         delete obj;
     *     else
     *         obj->setOwner(NULL);
     * </pre>
     */
    void discard(cObject *object)
        {if(object->storage()=='D') delete object; else object->setOwner(NULL);}
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
     * Ownership: By default, the object is assumed to have been created by the
     * currently active simple module, so the owner will be the "local objects" list
     * of that module. (More precisely, the owner will be set using
     * <code>setOwner(defaultOwner())</code>, and cObject::defaultOwner() returns
     * simulation.localList(), an alias to the currently active simple module's local
     * objects list. Note: Overriding defaultOwner() won't change this because
     * virtual functions are only effective after constructor has completed.
     * If you want to change the initial owner in a derived class, you have to
     * call setOwner() directly in the derived class' constructor.)
     *
     * If there is no active simple module (we're not in a simulation),
     * the owner will be NULL.
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
     *    -# <b>deletes all owned objects</b> by calling their discard() method
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
     * Direct call to the virtual destructor. This function is used
     * internally at cleanup (e.g. at the end of the simulation)
     * for disposing of objects left on module stacks of activity() modules.
     */
    void destruct() {this->~cObject();}

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
    void setName(const char *s)  {delete namestr; namestr=opp_strdup(s);}

    /**
     * Returns pointer to the object's name. The function never returns
     * NULL; rather, it returns ptr to "".
     */
    const char *name() const     {return namestr ? namestr : "";}

    /**
     * Returns true if the object's name is identical to the
     * string passed.
     */
    bool isName(const char *s) const {return !opp_strcmp(namestr,s);}

    /**
     * Returns a name that includes the object 'index' (e.g. in a module vector),
     * like "modem[5]".
     * To be redefined in descendants. E.g., see cModule::fullName().
     */
    virtual const char *fullName() const
        {return name();}

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
     * Sets the owner of the object. NULL is legal a legal value and means
     * that no object will own this one (ownerp=NULL).
     *
     * @see defaultOwner()
     */
    void setOwner(cObject *newowner);

    /**
     * Returns pointer to the default owner of the object.
     * This function is used by the drop() member function.
     */
    virtual cObject *defaultOwner() const;
    //@}

    /** @name Ownership control flag.
     *
     * The ownership control flag is to be used by derived container classes.
     * If the flag is set, the container should take() any object that is
     * inserted into it.
     */
    //@{

    /**
     * Sets the flag which determines whether the container object
     * should automatically take ownership of the objects that are inserted
     * into it.
     */
    void takeOwnership(bool tk) {tkownership=tk;}

    /**
     * Returns the flag which determines whether the container object
     * should automatically take ownership of the objects that are inserted
     * into it.
     */
    bool takeOwnership() const   {return tkownership;}
    //@}

    /** @name Reflection, support for debugging and snapshots. */
    //@{

    /**
     * Returns a pointer to the class name string. Since OMNeT++ 2.3, this method
     * is implemented once here in cObject, using typeid (C+ RTTI), and
     * it no longer needs to be redefined in each class.
     */
    virtual const char *className() const;

    /**
     * Produces a one-line description of object into `buf'.
     * This function is used by the graphical user interface (TkEnv). See
     * also <I>Functions supporting snapshots</I>.
     */
    virtual void info(char *buf);

    /**
     * This function is called internally by cSimpleModule::snapshot().
     * It writes out info about the object into the stream. Relies on
     * writeContents(). writeTo() does not need to
     * be redefined.
     */
    virtual void writeTo(ostream& os);

    /**
     * This function is called internally by writeTo(). It is
     * expected to write textual information about the object and other
     * objects it contains to the stream. The default version (cObject::writeContents())
     * uses forEach() to call info() for contained objects. Redefine
     * as needed.
     */
    virtual void writeContents(ostream& os);
    //@}

    /** @name Support for parallel execution.
     *
     * Packs/unpacks object from/to PVM or MPI send buffer.
     * These functions are used by the simulation kernel for parallel execution.
     * These functions should be redefined in all derived classes whose instances
     * are expected to travel across segments in a parallel distribution run.
     */
    //@{

    /**
     * Serializes the object into a PVM or MPI send buffer.
     * In OMNeT++'s PVM interface, this method makes calls pvm_pkint(), etc.
     */
    virtual int netPack();

    /**
     * Deserializes the object from a PVM or MPI receive buffer.
     * In OMNeT++'s PVM interface, this method makes calls pvm_upkint(), etc.
     */
    virtual int netUnpack();
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
     * Returns the storage class of the object. The return value is one
     * of the characters S/A/D which stand for static, auto and dynamic,
     * respectively. (The storage class is determined by the constructor,
     * with some help from cObject's operator new().)
     */
    char storage() const         {return stor;}

    /**
     * cObject's operator new does more than the global new().
     * It cooperates with cObject's constructor to determine the storage
     * class of the object (static, auto or dynamic).
     *
     * @see storage()
     */
    void *operator new(size_t m);

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
};


//
// Internal helper class for cObject.
//
class cStaticFlag
{
  public:
    cStaticFlag()  {cObject::staticflag = 1;}
    ~cStaticFlag() {cObject::staticflag = 0;}
    static bool isSet() {return cObject::staticflag;}
};


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
template<class T>
T check_and_cast(cObject *p)
{
    if (!p)
        throw new cException("check_and_cast(): cannot cast NULL pointer to type '%s'",opp_typename(typeid(T)));
    T ret = dynamic_cast<T>(p);
    if (!ret)
        throw new cException("check_and_cast(): cannot cast (%s *)%s to type '%s'",p->className(),p->fullPath(),opp_typename(typeid(T)));
    return ret;
}

#endif

