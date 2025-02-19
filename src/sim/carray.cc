//=========================================================================
//  CARRAY.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cArray : flexible array to store cObject objects
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>  // memcpy, memset
#include <algorithm>  // min, max
#include <sstream>
#include "omnetpp/carray.h"
#include "omnetpp/globals.h"
#include "omnetpp/cexception.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {

Register_Class(cArray);


void cArray::Iterator::init(const cArray& a, bool atHead)
{
    array = const_cast<cArray *>(&a);  // we don't want a separate Const_Iterator class

    if (atHead) {
        // fast-forward to first non-empty slot
        // (Note: we exploit that get(k) just returns nullptr when k is out of bounds)
        k = 0;
        while (!array->get(k) && k < array->size())
            k++;
    }
    else {
        // rewind to first non-empty slot
        k = array->size() - 1;
        while (!array->get(k) && k >= 0)
            k--;
    }
}

void cArray::Iterator::advance()
{
    k++;
    while (!array->get(k) && k < array->size())  // skip holes
        k++;
}

void cArray::Iterator::retreat()
{
    k--;
    while (!array->get(k) && k >= 0)  // skip holes
        k--;
}

//----

cArray::cArray(const cArray& other) : cOwnedObject(other)
{
    copy(other);
}

cArray::cArray(const char *name, int cap, int dt) : cOwnedObject(name)
{
    setFlag(FL_TKOWNERSHIP, true);
    delta = std::max(1, dt);
    capacity = std::max(cap, 0);
    vect = new cObject *[capacity];
    for (int i = 0; i < capacity; i++)
        vect[i] = nullptr;
}

cArray::~cArray()
{
    clear();
    delete[] vect;
}

void cArray::copy(const cArray& other)
{
    capacity = other.capacity;
    delta = other.delta;
    firstfree = other.firstfree;
    last = other.last;
    delete[] vect;
    vect = new cObject *[capacity];
    std::copy_n(other.vect, capacity, vect);

    for (int i = 0; i <= last; i++) {
        if (vect[i]) {
            if (!vect[i]->isOwnedObject())
                vect[i] = vect[i]->dup();
            else if (vect[i]->getOwner() == const_cast<cArray *>(&other))
                take(static_cast<cOwnedObject *>(vect[i] = vect[i]->dup()));
        }
    }
}

cArray& cArray::operator=(const cArray& other)
{
    if (this == &other)
        return *this;
    clear();
    cOwnedObject::operator=(other);
    copy(other);
    return *this;
}

std::string cArray::str() const
{
    if (last == -1)
        return std::string("empty");
    std::stringstream out;
    out << "size=" << last + 1;
    return out.str();
}

void cArray::forEachChild(cVisitor *v)
{
    for (int i = 0; i <= last; i++)
        if (vect[i])
            if (!v->visit(vect[i]))
                return;
}

void cArray::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cOwnedObject::parsimPack(buffer);

    buffer->pack(capacity);
    buffer->pack(delta);
    buffer->pack(firstfree);
    buffer->pack(last);

    for (int i = 0; i <= last; i++) {
        if (buffer->packFlag(vect[i] != nullptr)) {
            if (vect[i]->isOwnedObject() && vect[i]->getOwner() != this)
                throw cRuntimeError(this, "parsimPack(): Refusing to transmit an object not owned by the container");
            buffer->packObject(vect[i]);
        }
    }
#endif
}

void cArray::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cOwnedObject::parsimUnpack(buffer);

    delete[] vect;

    buffer->unpack(capacity);
    buffer->unpack(delta);
    buffer->unpack(firstfree);
    buffer->unpack(last);

    vect = new cObject *[capacity];
    for (int i = 0; i <= last; i++) {
        if (!buffer->checkFlag())
            vect[i] = nullptr;
        else {
            vect[i] = buffer->unpackObject();
            if (vect[i]->isOwnedObject())
                take(static_cast<cOwnedObject *>(vect[i]));
        }
    }
#endif
}

void cArray::clear()
{
    for (int i = 0; i <= last; i++) {
        cObject *obj = vect[i];
        if (obj) {
            if (!obj->isOwnedObject())
                delete obj;
            else if (obj->getOwner() == this)
                dropAndDelete(static_cast<cOwnedObject *>(obj));
            vect[i] = nullptr;  // this is not strictly necessary
        }
    }
    firstfree = 0;
    last = -1;
}

void cArray::setCapacity(int newCapacity)
{
    if (newCapacity < size())
        throw cRuntimeError(this, "setCapacity: New capacity %d cannot be less than current size %d", newCapacity, size());

    cObject **newVect = new cObject *[newCapacity];
    for (int i = 0; i <= last; i++)
        newVect[i] = vect[i];
    for (int i = last + 1; i < newCapacity; i++)
        newVect[i] = nullptr;
    delete[] vect;
    vect = newVect;
    capacity = newCapacity;
}

int cArray::add(cObject *obj)
{
    if (!obj)
        throw cRuntimeError(this, "Cannot insert nullptr");

    if (obj->isOwnedObject() && getTakeOwnership())
        take(static_cast<cOwnedObject *>(obj));

    int retval;
    if (firstfree < capacity) {  // fits in current vector
        vect[firstfree] = obj;
        retval = firstfree;
        last = std::max(last, firstfree);
        do {
            firstfree++;
        } while (firstfree <= last && vect[firstfree] != nullptr);
    }
    else {  // must allocate bigger vector
        cObject **v = new cObject *[capacity + delta];
        std::copy_n(vect, capacity, v);
        std::fill_n(v + capacity, delta, nullptr);
        delete[] vect;
        vect = v;
        vect[capacity] = obj;
        retval = last = capacity;
        firstfree = capacity + 1;
        capacity += delta;
    }
    return retval;
}

int cArray::addAt(int m, cObject *obj)
{
    if (!obj)
        throw cRuntimeError(this, "Cannot insert nullptr");

    if (m < capacity) {  // fits in current vector
        if (m < 0)
            throw cRuntimeError(this, "addAt(): Negative position %d", m);
        if (vect[m] != nullptr)
            throw cRuntimeError(this, "addAt(): Position %d already used", m);
        vect[m] = obj;
        if (obj->isOwnedObject() && getTakeOwnership())
            take(static_cast<cOwnedObject *>(obj));
        last = std::max(m, last);
        if (firstfree == m)
            do {
                firstfree++;
            } while (firstfree <= last && vect[firstfree] != nullptr);
    }
    else {  // must allocate bigger vector
        cObject **v = new cObject *[m + delta];
        std::copy_n(vect, capacity, v);
        std::fill_n(v + capacity, m + delta - capacity, nullptr);
        delete[] vect;
        vect = v;
        vect[m] = obj;
        if (obj->isOwnedObject() && getTakeOwnership())
            take(static_cast<cOwnedObject *>(obj));
        capacity = m + delta;
        last = m;
        if (firstfree == m)
            firstfree++;
    }
    return m;
}

int cArray::set(cObject *obj)
{
    if (!obj)
        throw cRuntimeError(this, "Cannot insert nullptr");

    int i = find(obj->getName());
    if (i < 0) {
        return add(obj);
    }
    else {
        if (!vect[i]->isOwnedObject())
            delete vect[i];
        else if (vect[i]->getOwner() == this)
            dropAndDelete(static_cast<cOwnedObject *>(vect[i]));
        vect[i] = obj;
        if (obj->isOwnedObject() && getTakeOwnership())
            take(static_cast<cOwnedObject *>(obj));
        return i;
    }
}

int cArray::find(cObject *obj) const
{
    int i;
    for (i = 0; i <= last; i++)
        if (vect[i] == obj)
            return i;

    return -1;
}

int cArray::find(const char *objname) const
{
    int i;
    for (i = 0; i <= last; i++)
        if (vect[i] && vect[i]->isName(objname))
            return i;

    return -1;
}

cObject *cArray::get(int m)
{
    if (m >= 0 && m <= last)
        return vect[m];
    else
        return nullptr;
}

const cObject *cArray::get(int m) const
{
    if (m >= 0 && m <= last)
        return vect[m];
    else
        return nullptr;
}

cObject *cArray::get(const char *objname)
{
    int m = find(objname);
    if (m == -1)
        return nullptr;
    return get(m);
}

const cObject *cArray::get(const char *objname) const
{
    int m = find(objname);
    if (m == -1)
        return nullptr;
    return get(m);
}

cObject *cArray::remove(const char *objname)
{
    int m = find(objname);
    if (m == -1)
        return nullptr;
    return remove(m);
}

cObject *cArray::remove(cObject *obj)
{
    if (!obj)
        return nullptr;

    int m = find(obj);
    if (m == -1)
        return nullptr;
    return remove(m);
}

cObject *cArray::remove(int m)
{
    if (m < 0 || m > last || vect[m] == nullptr)
        return nullptr;

    cObject *obj = vect[m];
    vect[m] = nullptr;
    firstfree = std::min(firstfree, m);
    if (m == last)
        do {
            last--;
        } while (last >= 0 && vect[last] == nullptr);
    if (obj->isOwnedObject() && obj->getOwner() == this)
        drop(static_cast<cOwnedObject *>(obj));
    return obj;
}

}  // namespace omnetpp

