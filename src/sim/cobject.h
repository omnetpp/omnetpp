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
  Copyright (C) 1992,99 Andras Varga
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
extern cHead superhead;           // list of global lists
extern cHead networks;            // list of available networks
extern cHead modinterfaces;       // list of all module interfaces
extern cHead modtypes;            // list of all module types
extern cHead linktypes;           // list of link types
extern cHead functions;           // list of function types
extern cHead classes;             // list of cClassRegister objects
extern cHead inspectorfactories;  // list of cInspectorFactory objects

typedef int (*CompareFunc)(cObject *, cObject *);
typedef bool (*ForeachFunc)(cObject *,bool);


//==========================================================================
// cObject: base class for other objects
//         name string
//         ownership control
//         virtual functions for derived classes

class cObject
{
        friend class cHead;
        friend class cIterator;
        friend class cStaticFlag;

      protected:
        char *namestr;                    // name string
        char stor;                        // storage: Static/Auto/Dynamic ('S'/'A'/'D')

        bool tkownership;                 // for derived containers: take ownership of objects?
        cObject *ownerp, *prevp, *nextp;  // ptr to owner; linked list ptrs
        cObject *firstchildp;             // list of owned objects

        static int staticflag, heapflag;  // to determine 'storage'

      protected:
        //
        // take(), drop(), free():
        //  intended to be used by derived container classes
        //

        // become the owner of `object':
        void take(cObject *object)
            {object->setOwner( this );}

        // give ownership of `object' to its default owner
        void drop(cObject *object)
            {object->setOwner( object->defaultOwner() );}

        // dispose of `object'; it MUST be owned by this object
        void free(cObject *object)
            {if(object->storage()=='D') delete object; else object->setOwner(NULL);}

      public:
        //
        // Creation, destruction, copying
        //

        // create a copy of `obj':
        cObject(cObject& obj);

        // create object with no name and default owner:
        cObject();

        // create object with given namestr and default owner:
        cObject(char *name_str);

        // create object with given namestr and specified owner:
        cObject(char *name_str, cObject *ownerobj );

        // virtual destructor:
        virtual ~cObject();

        // duplicates this object; must be redefined in derived classes!
        virtual cObject *dup()    {return new cObject(*this);}

        // virtual destructor call (used for disposing of objects on module stacks)
        void destruct() {this->~cObject();}

        // copies the object EXCEPT for the NAME string;
        //  derived classes are expected to define similar functions (e.g.cPar::operator=(cPar&))
        cObject& operator=(cObject& o);

        //
        // Handling the name string member
        //
        // "" and NULL are treated liberally:
        //   "" is stored as NULL and NULL is returned as ""
        //

        void setName(char *s)  {delete namestr; namestr=opp_strdup(s);}
        char *name()           {return namestr ? namestr : (CONST_HACK)"";}
        bool isName(char *s)   {return !opp_strcmp(namestr,s);}

        // longer names composed of name()
        virtual char *fullName()     // "name[index]", e.g. "modem[5]"
            {return name();}
        virtual char *fullPath();    // "comp.modem[5].baud-rate"

        //
        // Object ownership
        //

        // returns the owner of this object
        cObject *owner()      {return ownerp;}

        // make `newowner' the owner of this object
        void setOwner(cObject *newowner);

        // returns pointer to a default owner:
        virtual cObject *defaultOwner();

        //
        // Ownership control
        //
        // The ownership control flag is to be used by derived container classes.
        // If the flag is set, the container should take() any object that is
        // inserted into it.
        //
        void takeOwnership(bool tk) {tkownership=tk;}
        bool takeOwnership()        {return tkownership;}

        //
        // Support for debugging, snapshots etc.
        //

        // return pointer to type string; must be redefined in derived classes!
        virtual char *className()  {return "cObject";}

        // put a one-line description of object into `buf':
        virtual void info(char *buf);

        // create an inspector window. As of Jan 1999. this function should
        // not be used directly any more; see inspectorFactoryName() and
        // Register_InspectorFactory() instead.
        virtual TInspector *inspector(int type, void *data);

        // return name of inspector factory object
        virtual char *inspectorFactoryName() {return "cObjectIFC";}

        // write out header + call writeContents():
        virtual void writeTo(ostream& os);

        // write contained objects as default:
        virtual void writeContents(ostream& os);

        //
        // Support for parallel execution
        //   (pack/unpack object from/to PVM send buffer)
        //
        virtual int netPack();
        virtual int netUnpack();

        //
        // Miscellaneous functions
        //

        // call function `f' for each contained object:
        virtual void forEach(ForeachFunc f);

        // storage class: S/D/A
        char storage()              {return stor;}

        // memory management
        void *operator new(size_t m);   // extra: sets 'storage'
        void deleteChildren();
        void destructChildren();

        // find an object by name through recursive forEach() calls
        cObject *findObject(char *namestr, bool deep=TRUE);

        // sorting function for cQueue
        static int cmpbyname(cObject *one,cObject *other);
};

class cStaticFlag
{
     public:
        cStaticFlag()  {cObject::staticflag = 1;}
        ~cStaticFlag() {cObject::staticflag = 0;}
};

#endif

