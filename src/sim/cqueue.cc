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
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include <string.h>          // memcmp, memcpy, memset
#include "macros.h"
#include "cqueue.h"

//=== Registration
Register_Class( cQueue )

//=========================================================================
//=== cQueue - member functions

cQueue::cQueue(cQueue& queue) : cObject()
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
        if (headp->obj->owner()==this) free( headp->obj );
        delete headp;
        headp=tmp;
    }
    tailp = NULL;
    n = 0;
}

cQueue& cQueue::operator=(cQueue& queue)
{
    if (this==&queue) return *this;

    clear();

    cObject::operator=(queue);
    compare = queue.compare;
    asc = queue.asc;

    bool old_tk = takeOwnership();
    for( cQueueIterator iter(queue); iter.end(); iter++)
    {
        if (iter()->owner()==&queue)
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
        opp_error("(%s)%s: setup() can only be called when queue is empty",
                  className(),fullName());

    compare=cmp; asc=a;
}

//== STRUCTURE OF THE LIST:
//==  'headp' and 'tailp' point to the ends of the list (NULL if it is empty).
//==  The list is double-linked, but 'headp->prev' and 'tailp->next' are NULL.

sQElem *cQueue::find_qelem(cObject *obj)
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
    if (retobj->owner()==this) drop( retobj );
    return retobj;
}


void cQueue::insert(cObject *obj)
{
    if (!obj) {
        opp_error("(%s)%s: cannot insert NULL pointer in queue",className(),fullName());
        return;
    }

    if (takeOwnership()) take(obj);

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
    if (!obj) {
        opp_error("(%s)%s: cannot insert NULL pointer in queue", className(),fullName());
        return;
    }
    sQElem *p = find_qelem(where);
    if (p)
        insbefore_qelem(p,obj);
    else
        opp_error("(%s)%s: insertBefore(w,o): object w=`%s' not in queue",
                  className(),fullName(),where->name());
}

void cQueue::insertAfter(cObject *where, cObject *obj)
{
    if (!obj) {
        opp_error("(%s)%s: cannot insert NULL pointer in queue", className(),fullName());
        return;
    }
    sQElem *p = find_qelem(where);
    if (p)
        insafter_qelem(p,obj);
    else
        opp_error("(%s)%s: insertAfter(w,o): object w=`%s' not in queue",
                  className(),fullName(),where->name());
}

cObject *cQueue::remove(cObject *obj)
{
    if(!obj) return NO(cObject);

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
    if(!tailp) {
        opp_error("(%s)%s: pop(): queue empty",className(),fullName());
        return NO(cObject);
    }
    return remove_qelem( tailp );
}


