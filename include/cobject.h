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

#include <iostream.h>
#include "defs.h"
#include "util.h"

//=== classes declared here
class  cObject;
class  cStaticFlag;

//=== classes mentioned
class  TInspector;
class  cIterator;
class  cHead;

//=== Global objects:
SIM_API extern cHead superhead;           // list of global lists
SIM_API extern cHead networks;            // list of available networks
SIM_API extern cHead modinterfaces;       // list of all module interfaces
SIM_API extern cHead modtypes;            // list of all module types
SIM_API extern cHead linktypes;           // list of link types
SIM_API extern cHead functions;           // list of function types
SIM_API extern cHead classes;             // list of cClassRegister objects
SIM_API extern cHead inspectorfactories;  // list of cInspectorFactory objects

typedef int (*CompareFunc)(cObject *, cObject *);
typedef bool (*ForeachFunc)(cObject *,bool);


//==========================================================================
// cObject: base class for other objects
//         name string
//         ownership control
//         virtual functions for derived classes


/**
 * FIXME: //=== classes declared here
 * //=== classes mentioned
 * //=== Global objects:
 * 
 * cObject: base class for other objects
 *         name string
 *         ownership control
 *         virtual functions for derived classes
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

  protected:
    //
    // take(), drop(), free():
    //  intended to be used by derived container classes
    //

    // become the owner of `object':

    /**
     * The function called by the container object when it takes ownership
     * of the obj object that is inserted into it. Implementation:
     * obj->setOwner( this ).
     */
    void take(cObject *object)
        {object->setOwner( this );}

    // give ownership of `object' to its default owner

    /**
     * The function called by the container object when obj
     * is removed from the container -- releases the ownership of the
     * object and hands it over to its default owner. Implementation:
     * <BR>
     * obj->setOwner( obj->defaultOwner() );
     */
    void drop(cObject *object)
        {object->setOwner( object->defaultOwner() );}

    // dispose of `object'; it MUST be owned by this object

    /**
     * The function is called when the container object has to delete
     * the contained object obj. It the object was dynamically
     * allocated (by new), it is deleted, otherwise (e.g., if
     * it is a global or a local variable) it is just removed from the
     * ownership hierarchy. Implementation:<BR>
     * {if(obj->storage()=='D') delete obj; else obj->setOwner(NULL);}
     */
    void free(cObject *object)
        {if(object->storage()=='D') delete object; else object->setOwner(NULL);}

  public:
    //
    // Creation, destruction, copying
    //

    // create a copy of `obj':

    /**
     * Copy constructor. In derived classes, it is usually implemented
     * as {operator=(obj);}.
     */
    cObject(const cObject& obj);

    // create object with no name and default owner:

    /**
     * FIXME: create object with no name and default owner:
     */
    cObject();

    // create object with given name and default owner:

    /**
     * FIXME: create object with given name and default owner:
     */
    explicit cObject(const char *name);

    // create object with given name and specified owner:

    /**
     * Creates a cObject with the given name. The owner will
     * be the h object (if the pointer is not NULL),
     * that is, the constructor contains a setOwner( h ) call.
     */
    cObject(const char *name, cObject *ownerobj);

    // virtual destructor:

    /**
     * Virtual destructor. Deletes the name and notifies the user interface
     * that the object has been destructed.
     */
    virtual ~cObject();

    // duplicates this object; must be redefined in derived classes!

    /**
     * Duplicates the object and returns a pointer to the new one. In
     * derived classes, it is usually implemented as {return new
     * cObject(*this);}.
     */
    virtual cObject *dup()    {return new cObject(*this);}

    // virtual destructor call (used for disposing of objects on module stacks)

    /**
     * Direct call to the virtual destructor.
     */
    void destruct() {this->~cObject();}

    // copies the object EXCEPT for the NAME string;
    //  derived classes are expected to define similar functions (e.g.cPar::operator=(cPar&))

    /**
     * The assignment operator. Most derived classes contain a cSomeClass&<I>
     * </I>cSomeClass:: operator=(cSomeClass&) function.
     * The assignment operators do not copy the object name. If you want
     * to do so, you can copy it be hand: setName(o.name());
     */
    cObject& operator=(const cObject& o);

    //
    // Handling the name string member
    //
    // "" and NULL are treated liberally:
    //   "" is stored as NULL and NULL is returned as ""
    //


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

    // longer names composed of name()

    /**
     * To be redefined in descendants. E.g., see cModule::fullName().
     */
    virtual const char *fullName() const  // "name[index]", e.g. "modem[5]"
        {return name();}

    /**
     * MISSINGDOC: cObject:char*fullPath()
     */
    virtual const char *fullPath() const; // "comp.modem[5].baud-rate"

    //
    // Object ownership
    //

    // returns the owner of this object

    /**
     * Returns pointer to the owner of the object.
     */
    cObject *owner() const {return ownerp;}

    // make `newowner' the owner of this object

    /**
     * Sets the owner pointer of the object. See documentation of cHead
     * for more information.
     */
    void setOwner(cObject *newowner);

    // returns pointer to a default owner:

    /**
     * This function should return a pointer to the default owner of
     * the object. The function is used by the drop()<I> </I>member
     * function, redefined in cObject-descendant container classes.
     */
    virtual cObject *defaultOwner();

    //
    // Ownership control
    //
    // The ownership control flag is to be used by derived container classes.
    // If the flag is set, the container should take() any object that is
    // inserted into it.
    //

    /**
     * Sets/returns the flag which determines whether the container object
     * should automatically take ownership of the objects that are inserted
     * into it.
     */
    void takeOwnership(bool tk) {tkownership=tk;}

    /**
     * Sets/returns the flag which determines whether the container object
     * should automatically take ownership of the objects that are inserted
     * into it.
     */
    bool takeOwnership()        {return tkownership;}

    //
    // Support for debugging, snapshots etc.
    //

    // return pointer to type string; must be redefined in derived classes!

    /**
     * Returns a pointer to the class name string, "cObject".
     * In derived classes, usual implementation is {return "classname";}.
     */
    virtual const char *className() const {return "cObject";}

    // put a one-line description of object into `buf':

    /**
     * Copies a short description of the object into buf. This
     * function is used by the graphical user interface (TkEnv). See
     * also <I>Functions supporting snapshots</I>.
     */
    virtual void info(char *buf);

    // create an inspector window. As of Jan 1999. this function should
    // not be used directly any more; see inspectorFactoryName() and
    // Register_InspectorFactory() instead.

    /**
     * FIXME: create an inspector window. As of Jan 1999. this function should
     * not be used directly any more; see inspectorFactoryName() and
     * Register_InspectorFactory() instead.
     */
    virtual TInspector *inspector(int type, void *data);

    // return name of inspector factory object

    /**
     * Returns the name of the class which can create inspector windows
     * for objects of this class (e.g. in Tkenv).
     */
    virtual const char *inspectorFactoryName() const {return "cObjectIFC";}

    // write out header + call writeContents():

    /**
     * This function is called internally by cSimpleModule::snapshot().
     * It writes out info about the object into the stream. Relies on
     * writeContents(). writeTo() does not need to
     * be redefined.
     */
    virtual void writeTo(ostream& os);

    // write contained objects as default:

    /**
     * This function is called by internally writeTo(). It is
     * expected to write textual information about the object and other
     * objects it contains to the stream. The default version (cObject::writeContents())
     * uses forEach to call info() for contained objects. Redefine
     * as needed.
     */
    virtual void writeContents(ostream& os);

    //
    // Support for parallel execution
    //   (pack/unpack object from/to PVM send buffer)
    //

    /**
     * FIXME: 
     * Support for parallel execution
     *   (pack/unpack object from/to PVM send buffer)
     * 
     */
    virtual int netPack();

    /**
     * These functions are expected to be redefined in all derived objects.
     * In OMNeT++'s PVM interface, they call pvm_pkint(), pvm_upkint()
     * etc. functions.
     */
    virtual int netUnpack();

    //
    // Miscellaneous functions
    //

    // call function `f' for each contained object:

    /**
     * Makes sense with container objects derived from cObject.
     * Calls the <I>f</I> function recursively for each object contained
     * in this object.
     * 
     * <U><B>The </B></U><U><B>forEach()</B></U><U><B>
     * mechanism</B></U>
     * 
     * The forEach() mechanism implemented in OMNeT++ is very
     * special and slightly odd. The passed function is called for each
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
     * <I>(I know this all is kind of weird, but I wrote it a long ago.
     * Changing it now would take quite some work, and after all, it
     * works..)</I>
     */
    virtual void forEach(ForeachFunc f);

    // storage class: S/D/A

    /**
     * Returns the storage class of the object. The return value is one
     * of the characters S/A/D which stand for static, auto
     * and dynamic, respectively.
     */
    char storage()              {return stor;}

    // memory management

    /**
     * cObject's operator new does more than the global
     * new()<I>.</I> It cooperates with cObject's constructor
     * to determine the storage class of the object (static, auto or
     * dynamic).
     */
    void *operator new(size_t m);   // extra: sets 'storage'

    /**
     * MISSINGDOC: cObject:void deleteChildren()
     */
    void deleteChildren();

    /**
     * MISSINGDOC: cObject:void destructChildren()
     */
    void destructChildren();

    // find an object by name through recursive forEach() calls

    /**
     * Finds the object with the given name in a container object and
     * returns a pointer to it or NULL if the object hasn't
     * been found. If deep is false, only objects directly
     * contained will be searched, otherwise the function searches the
     * whole subtree for the object.
     */
    cObject *findObject(const char *name, bool deep=true);

    // sorting function for cQueue

    /**
     * This function compares to objects by name. It can be used in a
     * priority queue (class cQueue) as a sorting criterion.
     */
    static int cmpbyname(cObject *one, cObject *other);
};


/**
 * MISSINGDOC: cStaticFlag
 */
class cStaticFlag
{
  public:

    /**
     * MISSINGDOC: cStaticFlag:cStaticFlag()
     */
    cStaticFlag()  {cObject::staticflag = 1;}
    ~cStaticFlag() {cObject::staticflag = 0;}
};

#endif
