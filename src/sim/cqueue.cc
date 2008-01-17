//=========================================================================
//  CQUEUE.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cQueue : (priority) queue of cOwnedObject descendants
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "cqueue.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

USING_NAMESPACE

using std::ostream;


Register_Class(cQueue);


cQueue::cQueue(const cQueue& queue) : cOwnedObject()
{
    frontp = backp = NULL;
    n = 0;
    setName( queue.name() );
    operator=(queue);
}

cQueue::cQueue(const char *name) : cOwnedObject(name)
{
    tkownership = true;
    frontp = backp = NULL;
    n = 0;
}

cQueue::~cQueue()
{
    clear();
}

std::string cQueue::info() const
{
    if (n==0)
        return std::string("empty");
    std::stringstream out;
    out << "length=" << n;
    return out.str();
}

void cQueue::forEachChild(cVisitor *v)
{
    // loop through elements
    for (QElem *p=frontp; p!=NULL; p=p->next)
         v->visit(p->obj);
}

void cQueue::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cOwnedObject::netPack(buffer);

    buffer->pack(n);

    for (cQueue::Iterator iter(*this, 0); !iter.end(); iter--)
    {
        if (iter()->owner() != this)
            throw cRuntimeError(this,"netPack(): cannot transmit pointer to \"external\" object");
        buffer->packObject(iter());
    }
#endif
}

void cQueue::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cOwnedObject::netUnpack(buffer);

    buffer->unpack(n);

    for (int i=0; i<n; i++)
    {
        cOwnedObject *obj = buffer->unpackObject();
        insert(obj);
    }
#endif
}

void cQueue::clear()
{
    while (frontp)
    {
        QElem *tmp = frontp->next;
        if (frontp->obj->owner()==this)
            dropAndDelete(frontp->obj);
        delete frontp;
        frontp = tmp;
    }
    backp = NULL;
    n = 0;
}

cQueue& cQueue::operator=(const cQueue& queue)
{
    if (this==&queue) return *this;

    clear();

    cOwnedObject::operator=(queue);
    tkownership = queue.tkownership;

    bool old_tk = takeOwnership();
    for (cQueue::Iterator iter(queue, false); !iter.end(); iter++)
    {
        if (iter()->owner()==const_cast<cQueue*>(&queue))
            {takeOwnership(true); insert((cOwnedObject *)iter()->dup());}
        else
            {takeOwnership(false); insert(iter());}
    }
    takeOwnership(old_tk);
    return *this;
}

cQueue::QElem *cQueue::find_qelem(cOwnedObject *obj) const
{
    QElem *p = frontp;
    while (p && p->obj!=obj)
        p = p->next;
    return p;
}

void cQueue::insbefore_qelem(QElem *p, cOwnedObject *obj)
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

void cQueue::insafter_qelem(QElem *p, cOwnedObject *obj)
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

cOwnedObject *cQueue::remove_qelem(QElem *p)
{
    if (p->next)
        p->next->prev = p->prev;
    else
        backp = p->prev;
    if (p->prev)
        p->prev->next = p->next;
    else
        frontp = p->next;

    cOwnedObject *retobj = p->obj;
    delete p;
    n--;
    if (retobj->owner()==this)
        drop(retobj);
    return retobj;
}


void cQueue::insert(cOwnedObject *obj)
{
    if (!obj)
        throw cRuntimeError(this, "cannot insert NULL pointer in queue");

    if (takeOwnership())
        take(obj);

    if (backp)
    {
        insafter_qelem(backp, obj);
    }
    else
    {
        // insert as the only item
        QElem *e = new QElem;
        e->obj = obj;
        e->next = e->prev = NULL;
        frontp = backp = e;
        n = 1;
    }
}

void cQueue::insertBefore(cOwnedObject *where, cOwnedObject *obj)
{
    if (!obj)
        throw cRuntimeError(this, "cannot insert NULL pointer in queue");

    QElem *p = find_qelem(where);
    if (!p)
        throw cRuntimeError(this, "insertBefore(w,o): object w=`%s' not in queue", where->name());

    if (takeOwnership())
        take(obj);
    insbefore_qelem(p,obj);
}

void cQueue::insertAfter(cOwnedObject *where, cOwnedObject *obj)
{
    if (!obj)
        throw cRuntimeError(this,"cannot insert NULL pointer in queue");

    QElem *p = find_qelem(where);
    if (!p)
        throw cRuntimeError(this, "insertAfter(w,o): object w=`%s' not in queue",where->name());

    if (takeOwnership())
        take(obj);
    insafter_qelem(p,obj);
}

cOwnedObject *cQueue::front() const
{
    return frontp ? frontp->obj : NULL;
}

cOwnedObject *cQueue::back() const
{
    return backp ? backp->obj : NULL;
}

cOwnedObject *cQueue::remove(cOwnedObject *obj)
{
    if (!obj)
        return NULL;
    //FIXME: handle special cases faster: if obj==front() or ==back(), don't invoke find_qelem()
    QElem *p = find_qelem(obj);
    if (!p)
        return NULL;
    return remove_qelem(p);
}

cOwnedObject *cQueue::pop()
{
    if (!frontp)
        throw cRuntimeError(this,"pop(): queue empty");

    return remove_qelem(frontp);
}

int cQueue::length() const
{
    return n;
}

bool cQueue::contains(cOwnedObject *obj) const
{
    return find_qelem(obj)!=NULL;
}

cOwnedObject *cQueue::get(int i) const
{
    QElem *p = frontp;
    while (p!=NULL && i>0)
        p = p->next, i--;
    return p ? p->obj : NULL;
}


