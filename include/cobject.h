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
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __COBJECT_H
#define __COBJECT_H

#include "util.h"

#ifdef USE_STD_NAMESPACE
#include <iostream>
using std::ostream;
#else
#include <iostream.h>
#endif

#define FULLPATHBUF_SIZE  1024

//=== classes declared here
class  cObject;
class  cStaticFlag;

//=== classes mentioned
class  TInspector;
class  cIterator;
class  cHead;

//=== Global objects:
SIM_API extern cHead networks;            ///< List of available networks.
SIM_API extern cHead modinterfaces;       ///< List of all module interfaces.
SIM_API extern cHead modtypes;            ///< List of all module types.
SIM_API extern cHead linktypes;           ///< List of link types.
SIM_API extern cHead functions;           ///< List of function types.
SIM_API extern cHead classes;             ///< List of cClassRegister objects.
SIM_API extern cHead inspectorfactories;  ///< List of cInspectorFactory objects.
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
 * cObject provides a name member (a dynamically allocated string) and
 * a number of virtual functions. These functions either provide a default
 * behavior (mostly good for all derived classes), or they are expected
 * to be redefined in all derived classes.
 *
 * Some of the features provided by cObject:
 *  - name string
 *  - ownership control
 *  - virtual functions for derived classes
 *
 * @ingroup SimCore
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
     * Raises an error stating that assignment, copy constructor and dup()
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
     * Create object with no name and default owner.
     */
    cObject();

    /**
     * Create object with given name and default owner.
     */
    explicit cObject(const char *name);

    /**
     * Virtual destructor. Deletes the name and notifies the user interface
     * that the object has been destructed.
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
     * The assignment operator. Most derived classes contain a
     * <tt>cClassName& cClassName::operator=(cClassName&)</tt> function.
     * Copies the object EXCEPT for the NAME string; derived classes are
     * expected to define similar functions (e.g. <tt>cPar::operator=(cPar&)</tt>)
     * If you want to copy the name string, you can do it by hand:
     * <tt>setName(o.name()</tt>).
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
     * Returns true if the object's name is identical with the
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

    /** @name Object ownership. */
    //@{

    /**
     * Returns pointer to the owner of the object.
     */
    cObject *owner() const {return ownerp;}

    /**
     * Sets the owner of the object. If NULL is passed, the default owner of
     * the object will be used.
     *
     * @see defaultOwner()
     */
    void setOwner(cObject *newowner);

    /**
     * Returns pointer to the default owner of the object.
     * This function is used by the setOwner() and drop() member functions.
     * drop() is used in container classes derived from cObject.
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
     * Returns a pointer to the class name string, "cObject".
     * In derived classes, usual implementation is <tt>{return "classname";}</tt>.
     */
    virtual const char *className() const {return "cObject";}

    /**
     * Produces a one-line description of object into `buf'.
     * This function is used by the graphical user interface (TkEnv). See
     * also <I>Functions supporting snapshots</I>.
     */
    virtual void info(char *buf);

    /**
     * Create an inspector window.
     * Used internally. As of Jan 1999, this function should not be used
     * directly any more; see inspectorFactoryName() and
     * Register_InspectorFactory() instead.
     */
    virtual TInspector *inspector(int type, void *data);

    /**
     * Returns the name of the inspector factory class associated with this class.
     * Inspector factories are used by graphical user interfaces like Tkenv.
     * They are capable of creating inspector windows for objects of this class
     * (and maybe also other classes).
     */
    virtual const char *inspectorFactoryName() const {return "cObjectIFC";}

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
     * uses forEach to call info() for contained objects. Redefine
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
};

#endif

