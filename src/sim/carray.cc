//=========================================================================
//  CARRAY.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cArray : flexible array to store cObject objects
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>  // memcmp, memcpy, memset
#include "carray.h"
#include "globals.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

USING_NAMESPACE

Register_Class(cArray);


//XXX this needs to be tested (test suite!) with:
// 1) non-cOwnedObject objects, where getOwner() returns NULL
// 2) non-cOwnedObject objects, where getOwner()!=NULL (and possible the array)
// 3) cOwnedObject objects with takeownership==false
// 4) cOwnedObject objects with takeownership==true

void cArray::Iterator::init(const cArray& a, bool athead)
{
    array = const_cast<cArray *>(&a); // we don't want a separate Const_Iterator class

    if (athead)
    {
        // fast-forward to first non-empty slot
        // (Note: we exploit that get(k) just returns NULL when k is out of bounds)
        k = 0;
        while (!array->get(k) && k<array->size())
            k++;

    }
    else
    {
        // rewind to first non-empty slot
        k = array->size()-1;
        while (!array->get(k) && k>=0)
            k--;
    }
}

cObject *cArray::Iterator::operator++(int)
{
    if (k<0 || k>=array->size())
        return NULL;
    cObject *obj = array->get(k);

    k++;
    while (!array->get(k) && k<array->size())
        k++;
    return obj;
}

cObject *cArray::Iterator::operator--(int)
{
    if (k<0 || k>=array->size())
        return NULL;
    cObject *obj = array->get(k);
    k--;
    while (!array->get(k) && k>=0)
        k--;
    return obj;
}

//----

cArray::cArray(const cArray& list) : cOwnedObject()
{
    vect=NULL;
    last=-1;
    setName( list.getName() );
    operator=(list);
}

cArray::cArray(const char *name, int cap, int dt) :
cOwnedObject( name )
{
    setFlag(FL_TKOWNERSHIP,true);
    delta = Max(1,dt);
    capacity = Max(cap,0);
    firstfree = 0;
    last = -1;
    vect = new cObject *[capacity];
    for (int i=0; i<capacity; i++)
        vect[i] = NULL;
}

cArray::~cArray()
{
    clear();
    delete [] vect;
}

cArray& cArray::operator=(const cArray& list)
{
    if (this == &list)
        return *this;

    clear();

    cOwnedObject::operator=(list);

    capacity = list.capacity;
    delta = list.delta;
    firstfree = list.firstfree;
    last = list.last;
    delete [] vect;
    vect = new cObject *[capacity];
    memcpy(vect, list.vect, capacity * sizeof(cObject *));

    for (int i=0; i<=last; i++)
        if (vect[i] && vect[i]->isOwnedObject() && vect[i]->getOwner()==const_cast<cArray*>(&list))
            take((cOwnedObject *)(vect[i] = vect[i]->dup()));
    return *this;
}

std::string cArray::info() const
{
    if (last==-1)
        return std::string("empty");
    std::stringstream out;
    out << "size=" << last+1;
    return out.str();
}

void cArray::forEachChild(cVisitor *v)
{
    for (int i=0; i<=last; i++)
        if (vect[i])
            v->visit(vect[i]);
}

void cArray::parsimPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cOwnedObject::parsimPack(buffer);

    buffer->pack(capacity);
    buffer->pack(delta);
    buffer->pack(firstfree);
    buffer->pack(last);

    for (int i = 0; i <= last; i++)
    {
        if (buffer->packFlag(vect[i]!=NULL))
        {
            if (vect[i]->getOwner() != this)
                throw cRuntimeError(this,"parsimPack(): cannot transmit pointer to \"external\" object");
            buffer->packObject(vect[i]);
        }
    }
#endif
}

void cArray::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cOwnedObject::parsimUnpack(buffer);

    delete [] vect;

    buffer->unpack(capacity);
    buffer->unpack(delta);
    buffer->unpack(firstfree);
    buffer->unpack(last);

    vect = new cObject *[capacity];
    for (int i = 0; i <= last; i++)
    {
        if (!buffer->checkFlag())
            vect[i] = NULL;
        else {
            vect[i] = buffer->unpackObject();
            if (vect[i]->isOwnedObject())
                take((cOwnedObject *)vect[i]);
        }
    }
#endif
}

void cArray::clear()
{
    for (int i=0; i<=last; i++)
    {
        if (vect[i] && vect[i]->isOwnedObject() && vect[i]->getOwner()==this)
            dropAndDelete((cOwnedObject *)vect[i]);
        vect[i] = NULL;  // this is not strictly necessary
    }
    firstfree = 0;
    last = -1;
}

int cArray::add(cObject *obj)
{
    if (!obj)
        throw cRuntimeError(this,"cannot insert NULL pointer");

    int retval;
    if (obj->isOwnedObject() && getTakeOwnership())
        take((cOwnedObject *)obj);
    if (firstfree < capacity)  // fits in current vector
    {
        vect[firstfree] = obj;
        retval = firstfree;
        last = Max(last,firstfree);
        do {
            firstfree++;
        } while (firstfree<=last && vect[firstfree]!=NULL);
    }
    else // must allocate bigger vector
    {
        cObject **v = new cObject *[capacity+delta];
        memcpy(v, vect, sizeof(cObject*)*capacity );
        memset(v+capacity, 0, sizeof(cObject*)*delta);
        delete [] vect;
        vect = v;
        vect[capacity] = obj;
        retval = last = capacity;
        firstfree = capacity+1;
        capacity += delta;
    }
    return retval;
}

int cArray::addAt(int m, cObject *obj)
{
    if (!obj)
        throw cRuntimeError(this,"cannot insert NULL pointer");

    if (m<capacity)  // fits in current vector
    {
        if (m<0)
            throw cRuntimeError(this,"addAt(): negative position %d",m);
        if (vect[m]!=NULL)
            throw cRuntimeError(this,"addAt(): position %d already used",m);
        vect[m] = obj;
        if (obj->isOwnedObject() && getTakeOwnership())
            take((cOwnedObject *)obj);
        last = Max(m,last);
        if (firstfree==m)
            do {
                firstfree++;
            } while (firstfree<=last && vect[firstfree]!=NULL);
    }
    else // must allocate bigger vector
    {
        cObject **v = new cObject *[m+delta];
        memcpy(v, vect, sizeof(cObject*)*capacity);
        memset(v+capacity, 0, sizeof(cObject*)*(m+delta-capacity));
        delete [] vect;
        vect = v;
        vect[m] = obj;
        if (obj->isOwnedObject() && getTakeOwnership())
            take((cOwnedObject *)obj);
        capacity = m+delta;
        last = m;
        if (firstfree==m)
            firstfree++;
    }
    return m;
}

int cArray::set(cObject *obj)
{
    if (!obj)
        throw cRuntimeError(this,"cannot insert NULL pointer");

    int i = find(obj->getName());
    if (i<0)
    {
        return add(obj);
    }
    else
    {
        if (vect[i]->isOwnedObject() && vect[i]->getOwner()==this)
            dropAndDelete((cOwnedObject *)vect[i]);
        vect[i] = obj;
        if (obj->isOwnedObject() && getTakeOwnership())
            take((cOwnedObject *)obj);
        return i;
    }
}

int cArray::find(cObject *obj) const
{
    int i;
    for (i=0; i<=last; i++)
        if (vect[i]==obj)
            break;
    if (i<=last)
        return i;
    else
        return -1;
}

int cArray::find(const char *objname) const
{
    int i;
    for (i=0; i<=last; i++)
        if (vect[i] && vect[i]->isName(objname))
            break;
    if (i<=last)
        return i;
    else
        return -1;
}

cObject *cArray::get(int m)
{
    if (m>=0 && m<=last && vect[m])
        return vect[m];
    else
        return NULL;
}

const cObject *cArray::get(int m) const
{
    if (m>=0 && m<=last && vect[m])
        return vect[m];
    else
        return NULL;
}

cObject *cArray::get(const char *objname)
{
    int m = find( objname );
    if (m==-1)
        return NULL;
    return get(m);
}

const cObject *cArray::get(const char *objname) const
{
    int m = find( objname );
    if (m==-1)
        return NULL;
    return get(m);
}

cObject *cArray::remove(const char *objname)
{
    int m = find( objname );
    if (m==-1)
        return NULL;
    return remove(m);
}

cObject *cArray::remove(cObject *obj)
{
    if (!obj) return NULL;

    int m = find( obj );
    if (m==-1)
        return NULL;
    return remove(m);
}

cObject *cArray::remove(int m)
{
    if (m<0 || m>last || vect[m]==NULL)
        return NULL;

    cObject *obj = vect[m]; vect[m] = NULL;
    firstfree = Min(firstfree, m);
    if (m==last)
        do {
            last--;
        } while (last>=0 && vect[last]==NULL);
    if (obj->isOwnedObject() && obj->getOwner()==this)
        drop((cOwnedObject *)obj);
    return obj;
}


