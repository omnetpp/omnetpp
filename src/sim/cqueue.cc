//=========================================================================
//  CQUEUE.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cQueue : (priority) queue of cObject descendants
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include <string.h>          // memcmp, memcpy, memset
#include "macros.h"
#include "cqueue.h"
#include "cexception.h"
#include "errmsg.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

using std::ostream;


//=== Registration
Register_Class(cQueue);

//=========================================================================
//=== cQueue - member functions

cQueue::cQueue(const cQueue& queue) : cObject()
{
    headp = tailp = NULL; n = 0;
    setName( queue.name() );
    operator=(queue);
}

cQueue::cQueue(const char *name) : cObject(name)
{
    tkownership = true;
    headp=tailp=NULL;
    n=0;
    compare=NULL; asc=false;
}

cQueue::cQueue(const char *name, CompareFunc cmp, bool a) : cObject(name)
{
    tkownership = true;
    headp=tailp=NULL;
    n=0;
    compare=cmp; asc=a;
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
    // loop through elements in reverse order
    for (QElem *p=tailp; p!=NULL; p=p->prev)
         v->visit(p->obj);
}

void cQueue::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    cObject::netPack(buffer);

    buffer->pack(n);
    buffer->pack(asc);

    if (compare)
        throw new cRuntimeError(this,"netPack(): cannot transmit compare function");

    for (cQueue::Iterator iter(*this, 0); !iter.end(); iter--)
    {
        if (iter()->owner() != this)
            throw new cRuntimeError(this,"netPack(): cannot transmit pointer to \"external\" object");
        buffer->packObject(iter());
    }
#endif
}

void cQueue::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    cObject::netUnpack(buffer);

    buffer->unpack(n);
    buffer->unpack(asc);

    for (int i=0; i<n; i++)
    {
        cObject *obj = buffer->unpackObject();
        insert(obj);
    }
#endif
}

void cQueue::clear()
{
    while (headp)
    {
        QElem *tmp = headp->next;
        if (headp->obj->owner()==this)
            dropAndDelete(headp->obj);
        delete headp;
        headp=tmp;
    }
    tailp = NULL;
    n = 0;
}

cQueue& cQueue::operator=(const cQueue& queue)
{
    if (this==&queue) return *this;

    clear();

    cObject::operator=(queue);
    compare = queue.compare;
    asc = queue.asc;
    tkownership = queue.tkownership;

    bool old_tk = takeOwnership();
    for (cQueue::Iterator iter(queue, 0); !iter.end(); iter--)
    {
        if (iter()->owner()==const_cast<cQueue*>(&queue))
            {takeOwnership(true); insert( (cObject *)iter()->dup() );}
        else
            {takeOwnership(false); insert( iter() );}
    }
    takeOwnership( old_tk );
    return *this;
}

void cQueue::setup(CompareFunc cmp, bool a)
{
    if (!empty())
        throw new cRuntimeError(this, "setup() can only be called when queue is empty");

    compare=cmp; asc=a;
}

//== STRUCTURE OF THE LIST:
//==  'headp' and 'tailp' point to the ends of the list (NULL if it is empty).
//==  The list is double-linked, but 'headp->prev' and 'tailp->next' are NULL.

cQueue::QElem *cQueue::find_qelem(cObject *obj) const
{
    QElem *p = headp;
    while( p && p->obj!=obj )
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
        headp = e;
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
        tailp = e;
    n++;
}

cObject *cQueue::remove_qelem(QElem *p)
{
    if( p->prev )
        p->prev->next = p->next;
    else
        headp = p->next;
    if( p->next )
        p->next->prev = p->prev;
    else
        tailp = p->prev;

    cObject *retobj = p->obj;
    delete p;
    n--;
    if (retobj->owner()==this)
        drop( retobj );
    return retobj;
}


void cQueue::insert(cObject *obj)
{
    if (!obj)
        throw new cRuntimeError(this,"cannot insert NULL pointer in queue");

    if (takeOwnership())
        take(obj);

    QElem *p = headp;
    if (compare)           // seek insertion place if necessary
    {
        if (asc)
            while ( p && (*compare)(obj,p->obj)>0 )
                p = p->next;
        else /* desc */
            while ( p && (*compare)(obj,p->obj)<0 )
                p = p->next;
    }

    if (p)
        insbefore_qelem(p,obj);
    else if (tailp)
        insafter_qelem(tailp,obj);
    else
    {
        // insert as the only item
        QElem *e = new QElem;
        e->obj = obj;
        headp = tailp = e;
        e->prev = e->next = NULL;
        n++;
    }
}

void cQueue::insertBefore(cObject *where, cObject *obj)
{
    if (!obj)
        throw new cRuntimeError(this,"cannot insert NULL pointer in queue");

    QElem *p = find_qelem(where);
    if (!p)
        throw new cRuntimeError(this, "insertBefore(w,o): object w=`%s' not in queue", where->name());

    if (takeOwnership())
        take(obj);
    insbefore_qelem(p,obj);
}

void cQueue::insertAfter(cObject *where, cObject *obj)
{
    if (!obj)
        throw new cRuntimeError(this,"cannot insert NULL pointer in queue");

    QElem *p = find_qelem(where);
    if (!p)
        throw new cRuntimeError(this, "insertAfter(w,o): object w=`%s' not in queue",where->name());

    if (takeOwnership())
        take(obj);
    insafter_qelem(p,obj);
}

cObject *cQueue::head() const
{
    return n!=0 ? headp->obj : NULL;
}

cObject *cQueue::tail() const
{
    return n!=0 ? tailp->obj : NULL;
}

cObject *cQueue::remove(cObject *obj)
{
    if(!obj) return NULL;

    QElem *p = find_qelem(obj);
    if (!p)
        return NULL;
    return remove_qelem( p );
}

cObject *cQueue::pop()
{
    if(!tailp)
        throw new cRuntimeError(this,"pop(): queue empty");

    return remove_qelem( tailp );
}

int cQueue::length() const
{
    return n;
}

bool cQueue::contains(cObject *obj) const
{
    return find_qelem(obj)!=NULL;
}

cObject *cQueue::get(int i) const
{
    //XXX
    // QElem *p = headp;
    // while (p!=NULL && i>0)
    //     p = p->next, i--;
    QElem *p = tailp;
    while (p!=NULL && i>0)
        p = p->prev, i--;
    return p ? p->obj : NULL;
}


