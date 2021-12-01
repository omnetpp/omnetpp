//=========================================================================
//  CQUEUE.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cQueue : queue of cObject descendants
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

class FunctionBasedComparator : public cQueue::Comparator
{
   cQueue::CompareFunc f;
  public:
   FunctionBasedComparator(cQueue::CompareFunc f) : f(f) {}
   virtual FunctionBasedComparator *dup() const override {return new FunctionBasedComparator(f);}
   virtual bool less(cObject *a, cObject *b) override {return f(a,b) < 0;}
};


cQueue::cQueue(const cQueue& queue) : cOwnedObject(queue)
{
    copy(queue);
}

cQueue::cQueue(const char *name, Comparator *cmp) : cOwnedObject(name), comparator(cmp)
{
}

cQueue::cQueue(const char *name, CompareFunc cmp) : cOwnedObject(name),
        comparator(cmp ? new FunctionBasedComparator(cmp) : nullptr)
{
}

cQueue::~cQueue()
{
    clear();
    delete comparator;
}

std::string cQueue::str() const
{
    if (len == 0)
        return std::string("empty");
    std::stringstream out;
    out << "length=" << len;
    return out.str();
}

void cQueue::forEachChild(cVisitor *v)
{
    for (QElem *p = frontp; p != nullptr; p = p->next)
        if (!v->visit(p->obj))
            return;
}

void cQueue::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cOwnedObject::parsimPack(buffer);

    if (comparator)
        throw cRuntimeError(this, "parsimPack(): Cannot transmit comparison function");

    buffer->pack(len);

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

    buffer->unpack(len);

    Comparator *oldCmp = comparator;
    comparator = nullptr;  // temporarily, so that insert() keeps the original order
    for (int i = 0; i < len; i++) {
        cObject *obj = buffer->unpackObject();
        insert(obj);
    }
    comparator = oldCmp;
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
    len = 0;
}

void cQueue::copy(const cQueue& queue)
{
    delete comparator;
    comparator = nullptr;  // temporarily, so that insert() keeps the original order
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
    if (queue.comparator)
        comparator = queue.comparator->dup();
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

void cQueue::setup(Comparator *cmp)
{
    delete comparator;
    comparator = cmp;
}

void cQueue::setup(CompareFunc cmp)
{
    setup(cmp ? new FunctionBasedComparator(cmp) : nullptr);
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
    len++;
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
    len++;
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
    len--;
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
        len = 1;
    }
    else if (comparator == nullptr) {
        insafter_qelem(backp, obj);
    }
    else {
        // priority queue: seek insertion place
        QElem *p = backp;
        while (p && comparator->less(obj, p->obj))
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
    return len;
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

