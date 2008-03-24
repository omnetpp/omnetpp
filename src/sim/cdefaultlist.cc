//=========================================================================
//  CDEFAULTLIST.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cDefaultList : stores a set of cOwnedObjects
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>  // memcpy
#include "globals.h"
#include "carray.h"
#include "cexception.h"
#include "cdefaultlist.h"
#include "cwatch.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

NAMESPACE_BEGIN


Register_Class(cDefaultList);


// doGC will be configured from Envir
bool cDefaultList::doGC;


cDefaultList::cDefaultList(const char *name) : cNoncopyableOwnedObject(name)
{
    // careful: if we are a global variable (ctor called before main()),
    // then insert() may get called before constructor and it invoked
    // construct() already.
    if (cStaticFlag::isSet() || capacity==0)
        construct();

    // if we're invoked before main, then we are a global variable (dynamic
    // instances of cDefaultList are not supposed to be created
    // before main()) --> remove ourselves from ownership tree because
    // we shouldn't be destroyed via operator delete
    if (!cStaticFlag::isSet())
        removeFromOwnershipTree();
}

void cDefaultList::construct()
{
    capacity = 2;
    size = 0;
    vect = new cOwnedObject *[capacity];
    for (int i=0; i<capacity; i++)
        vect[i] = NULL;
}

cDefaultList::~cDefaultList()
{
    if (doGC)
    {
        // delete all owned objects. One place we make use of this is behavior is
        // when a simple module gets deleted -- there we have to delete all dynamically
        // allocated objects held by the module. But: deletion has dangers,
        // i.e. if we try to delete objects embedded in other objects/structs or
        // arrays, it will crash mysteriously to the user -- so consider not deleting.
        while (size>0)
            delete vect[0];
        delete [] vect;
    }
    else
    {
        // experimental: do not delete objects (except cWatches), just print their names
        for (int i=0; i<size; i++)
        {
            if (dynamic_cast<cWatchBase *>(vect[i]))
                delete vect[i--]; // "i--" used because delete will move last item to position i
            else
                ev.undisposedObject(vect[i]);
        }

        // we can free up the pointer array itself though
        delete [] vect;
    }
}

void cDefaultList::doInsert(cOwnedObject *obj)
{
    ASSERT(obj!=this || this==&defaultList);

    if (size>=capacity)
    {
        if (capacity==0)
        {
            // this is if we're invoked before main, before our ctor run
            construct();
        }
        else
        {
            // must allocate bigger vector (grow 25% but at least 2)
            capacity += (capacity<8) ? 2 : (capacity>>2);
            cOwnedObject **v = new cOwnedObject *[capacity];
            memcpy(v, vect, sizeof(cOwnedObject*)*size);
            delete [] vect;
            vect = v;
        }
    }

    obj->ownerp = this;
    vect[obj->pos = size++] = obj;
}

void cDefaultList::ownedObjectDeleted(cOwnedObject *obj)
{
    ASSERT(obj && obj->ownerp==this);

    // move last object to obj's old position
    int pos = obj->pos;
    (vect[pos] = vect[--size])->pos = pos;
}

void cDefaultList::yieldOwnership(cOwnedObject *obj, cObject *newowner)
{
    ASSERT(obj && obj->ownerp==this && size>0);

    // give object to its new owner
    obj->ownerp = newowner;

    // move last object to obj's old position
    int pos = obj->pos;
    (vect[pos] = vect[--size])->pos = pos;
}

void cDefaultList::takeAllObjectsFrom(cDefaultList& other)
{
    while (other.defaultListSize()>0)
        take(other.defaultListGet(0));
}

std::string cDefaultList::info() const
{
    std::stringstream out;
    out << "n=" << size;
    return out.str();
}

void cDefaultList::forEachChild(cVisitor *v)
{
    for (int i=0; i<size; i++)
        v->visit(vect[i]);
}

void cDefaultList::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cOwnedObject::netPack(buffer);

    if (size>0)
        throw cRuntimeError(this, "netPack() not supported (makes no sense)");
#endif
}

void cDefaultList::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cOwnedObject::netUnpack(buffer);
    if (size>0)
        throw cRuntimeError(this, "netUnpack(): can only unpack into empty object");
#endif
}

void cDefaultList::take(cOwnedObject *obj)
{
    // ask current owner to release it -- if it's a cDefaultList, it will.
    obj->ownerp->yieldOwnership(obj, this);
    doInsert(obj);
}

void cDefaultList::drop(cOwnedObject *obj)
{
    if (obj->ownerp!=this)
        throw cRuntimeError(this,"drop(): not owner of object (%s)%s",
                                obj->className(), obj->fullPath().c_str());
    // the following 2 lines are actually the same as defaultOwner->take(obj);
    yieldOwnership(obj, defaultowner);
    defaultowner->doInsert(obj);
}

cOwnedObject *cDefaultList::defaultListGet(int k)
{
    if (k<0 || k>=size)
        return NULL;
    return vect[k];
}

bool cDefaultList::defaultListContains(cOwnedObject *obj) const
{
    return obj && obj->owner()==const_cast<cDefaultList *>(this);
}

NAMESPACE_END
