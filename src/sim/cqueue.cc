//=========================================================================
//
//  CQUEUE.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Member functions of
//    cQueue : (priority) queue of cObject descendants
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include <string.h>          // memcmp, memcpy, memset
#include "macros.h"
#include "cqueue.h"
#include "cexception.h"

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

cQueue::cQueue(const char *name, CompareFunc cmp, bool a) :
cObject( name )
{
    headp=tailp=NULL;
    n=0;
    setup( cmp, a );
}

cQueue::~cQueue()
{
    while (headp)
    {
        // delete only the holder structs; owned objects will be
        // deleted by cObject's destructor
        //
        sQElem *tmp = headp->next;
        delete headp;
        headp=tmp;
    }
}

void cQueue::info(char *buf)
{
    cObject::info( buf );

    if( n==0 )
        sprintf( buf+strlen(buf), " (empty)" );
    else
        sprintf( buf+strlen(buf), " (n=%d)", n);
}

void cQueue::forEach( ForeachFunc do_fn )
{
    if (do_fn(this,true))
        // loop through elements in reverse order
        for( sQElem *p=tailp; p!=NULL; p=p->prev )
             p->obj->forEach( do_fn );
    do_fn(this,false);
}

void cQueue::clear()
{
    sQElem *tmp;
    while (headp)
    {
        tmp = headp->next;
        if (headp->obj->owner()==this)
            discard( headp->obj );
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

    bool old_tk = takeOwnership();
    for( cQueueIterator iter(queue, 0); !iter.end(); iter--)
    {
        if (iter()->owner()==const_cast<cQueue*>(&queue))
            {takeOwnership(true); insert( iter()->dup() );}
        else
            {takeOwnership(false); insert( iter() );}
    }
    takeOwnership( old_tk );
    return *this;
}

void cQueue::setup(CompareFunc cmp, bool a)
{
    if (!empty())
        throw new cException("(%s)%s: setup() can only be called when queue is empty",
                  className(),fullName());

    compare=cmp; asc=a;
}

//== STRUCTURE OF THE LIST:
//==  'headp' and 'tailp' point to the ends of the list (NULL if it is empty).
//==  The list is double-linked, but 'headp->prev' and 'tailp->next' are NULL.

sQElem *cQueue::find_qelem(cObject *obj) const
{
    sQElem *p = headp;
    while( p && p->obj!=obj )
        p = p->next;
    return p;
}

void cQueue::insbefore_qelem(sQElem *p, cObject *obj)
{
    sQElem *e = new sQElem;
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

void cQueue::insafter_qelem(sQElem *p, cObject *obj)
{
    sQElem *e = new sQElem;
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

cObject *cQueue::remove_qelem(sQElem *p)
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
        throw new cException("(%s)%s: cannot insert NULL pointer in queue",className(),fullName());

    if (takeOwnership()) 
        take(obj);

    sQElem *p = headp;
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
        sQElem *e = new sQElem;
        e->obj = obj;
        headp = tailp = e;
        e->prev = e->next = NULL;
        n++;
    }
}

void cQueue::insertBefore(cObject *where, cObject *obj)
{
    if (!obj)
        throw new cException("(%s)%s: cannot insert NULL pointer in queue", className(),fullName());

    sQElem *p = find_qelem(where);
    if (!p) 
        throw new cException("(%s)%s: insertBefore(w,o): object w=`%s' not in queue",
                  className(),fullName(),where->name());

    if (takeOwnership()) 
        take(obj);
    insbefore_qelem(p,obj);
}

void cQueue::insertAfter(cObject *where, cObject *obj)
{
    if (!obj)
        throw new cException("(%s)%s: cannot insert NULL pointer in queue", className(),fullName());

    sQElem *p = find_qelem(where);
    if (!p) 
        throw new cException("(%s)%s: insertAfter(w,o): object w=`%s' not in queue",
                  className(),fullName(),where->name());

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

    sQElem *p = find_qelem(obj);
    if(!p) {
        opp_warning("(%s)%s: remove(): Object `%s' not in queue",
                     className(),fullName(),obj->fullName());
        return obj;
    }
    return remove_qelem( p );
}

cObject *cQueue::pop()
{
    if(!tailp)
        throw new cException("(%s)%s: pop(): queue empty",className(),fullName());

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


