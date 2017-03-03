//=========================================================================
//  CQUEUE.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cQueue : queue of cObject descendants
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstring>
#include <sstream>
#include "omnetpp/globals.h"
#include "omnetpp/cqueue.h"
#include "omnetpp/cexception.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {

using std::ostream;

Register_Class(cQueue);

cQueue::cQueue(const cQueue& queue) : cOwnedObject(queue)
{
    frontp = backp = nullptr;
    n = 0;
    copy(queue);
}

cQueue::cQueue(const char *name, CompareFunc cmp) : cOwnedObject(name)
{
    takeOwnership = true;
    frontp = backp = nullptr;
    n = 0;
    compare = cmp;
}

cQueue::~cQueue()
{
    clear();
}

std::string cQueue::str() const
{
    if (n == 0)
        return std::string("empty");
    std::stringstream out;
    out << "length=" << n;
    return out.str();
}

void cQueue::forEachChild(cVisitor *v)
{
    // loop through elements
    for (QElem *p = frontp; p != nullptr; p = p->next)
        v->visit(p->obj);
}

void cQueue::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cOwnedObject::parsimPack(buffer);

    if (compare)
        throw cRuntimeError(this, "parsimPack(): Cannot transmit comparison function");

    buffer->pack(n);

    for (cQueue::Iterator it(*this); !it.end(); ++it) {
        cObject *obj = *it;
        if (obj->isOwnedObject() && obj->getOwner() != this)
            throw cRuntimeError(this, "parsimPack(): Refusing to transmit an object not owned by the queue");
        buffer->packObject(obj);
    }
#endif
}

void cQueue::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cOwnedObject::parsimUnpack(buffer);

    buffer->unpack(n);

    CompareFunc old_cmp = compare;
    compare = nullptr;  // temporarily, so that insert() keeps the original order
    for (int i = 0; i < n; i++) {
        cObject *obj = buffer->unpackObject();
        insert(obj);
    }
    compare = old_cmp;
#endif
}

void cQueue::clear()
{
    while (frontp) {
        QElem *tmp = frontp->next;
        cObject *obj = frontp->obj;
        if (!obj->isOwnedObject())
            delete obj;
        else if (obj->getOwner() == this)
            dropAndDelete(static_cast<cOwnedObject *>(obj));
        delete frontp;
        frontp = tmp;
    }
    backp = nullptr;
    n = 0;
}

void cQueue::copy(const cQueue& queue)
{
    compare = nullptr;  // temporarily, so that insert() keeps the original order
    for (cQueue::Iterator it(queue); !it.end(); ++it) {
        cObject *obj = *it;
        if (!obj->isOwnedObject()) {
            insert(obj->dup());
        }
        else if (obj->getOwner() == const_cast<cQueue *>(&queue)) {
            setTakeOwnership(true);
            insert(obj->dup());
        }
        else {
            setTakeOwnership(false);
            insert(obj);
        }
    }

    takeOwnership = queue.takeOwnership;
    compare = queue.compare;
}

cQueue& cQueue::operator=(const cQueue& queue)
{
    if (this == &queue)
        return *this;
    clear();
    cOwnedObject::operator=(queue);
    copy(queue);
    return *this;
}

void cQueue::setup(CompareFunc cmp)
{
    compare = cmp;
}

cQueue::QElem *cQueue::find_qelem(cObject *obj) const
{
    QElem *p = frontp;
    while (p && p->obj != obj)
        p = p->next;
    return p;
}

void cQueue::insbefore_qelem(QElem *p, cObject *obj)
{
    QElem *e = new QElem;
    e->obj = obj;

    e->prev = p->prev;
    e->next = p;
    p->prev = e;
    if (e->prev)
        e->prev->next = e;
    else
        frontp = e;
    n++;
}

void cQueue::insafter_qelem(QElem *p, cObject *obj)
{
    QElem *e = new QElem;
    e->obj = obj;

    e->next = p->next;
    e->prev = p;
    p->next = e;
    if (e->next)
        e->next->prev = e;
    else
        backp = e;
    n++;
}

cObject *cQueue::remove_qelem(QElem *p)
{
    if (p->next)
        p->next->prev = p->prev;
    else
        backp = p->prev;
    if (p->prev)
        p->prev->next = p->next;
    else
        frontp = p->next;

    cObject *retobj = p->obj;
    delete p;
    n--;
    if (retobj->isOwnedObject() && retobj->getOwner() == this)
        drop(static_cast<cOwnedObject *>(retobj));
    return retobj;
}

void cQueue::insert(cObject *obj)
{
    if (!obj)
        throw cRuntimeError(this, "Cannot insert nullptr");

    if (obj->isOwnedObject() && getTakeOwnership())
        take(static_cast<cOwnedObject *>(obj));

    if (!backp) {
        // insert as the only item
        QElem *e = new QElem;
        e->obj = obj;
        e->next = e->prev = nullptr;
        frontp = backp = e;
        n = 1;
    }
    else if (compare == nullptr) {
        insafter_qelem(backp, obj);
    }
    else {
        // priority queue: seek insertion place
        QElem *p = backp;
        while (p && compare(obj, p->obj) < 0)
            p = p->prev;
        if (p)
            insafter_qelem(p, obj);
        else
            insbefore_qelem(frontp, obj);
    }
}

void cQueue::insertBefore(cObject *where, cObject *obj)
{
    if (!obj)
        throw cRuntimeError(this, "Cannot insert nullptr");

    QElem *p = find_qelem(where);
    if (!p)
        throw cRuntimeError(this, "insertBefore(w,o): Object w='%s' not in the queue", where->getName());

    if (obj->isOwnedObject() && getTakeOwnership())
        take(static_cast<cOwnedObject *>(obj));
    insbefore_qelem(p, obj);
}

void cQueue::insertAfter(cObject *where, cObject *obj)
{
    if (!obj)
        throw cRuntimeError(this, "Cannot insert nullptr");

    QElem *p = find_qelem(where);
    if (!p)
        throw cRuntimeError(this, "insertAfter(w,o): Object w='%s' not in the queue", where->getName());

    if (obj->isOwnedObject() && getTakeOwnership())
        take(static_cast<cOwnedObject *>(obj));
    insafter_qelem(p, obj);
}

cObject *cQueue::front() const
{
    return frontp ? frontp->obj : nullptr;
}

cObject *cQueue::back() const
{
    return backp ? backp->obj : nullptr;
}

cObject *cQueue::remove(cObject *obj)
{
    if (!obj)
        return nullptr;
    //FIXME: handle special cases faster: if obj==front() or ==back(), don't invoke find_qelem()
    QElem *p = find_qelem(obj);
    if (!p)
        return nullptr;
    return remove_qelem(p);
}

cObject *cQueue::pop()
{
    if (!frontp)
        throw cRuntimeError(this, "pop(): Queue empty");

    return remove_qelem(frontp);
}

int cQueue::getLength() const
{
    return n;
}

bool cQueue::contains(cObject *obj) const
{
    return find_qelem(obj) != nullptr;
}

cObject *cQueue::get(int i) const
{
    QElem *p = frontp;
    while (p != nullptr && i > 0)
        p = p->next, i--;
    return p ? p->obj : nullptr;
}

}  // namespace omnetpp

