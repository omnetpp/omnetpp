//=========================================================================
//  CLLIST.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cLinkedList : linked list of void* pointers
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
#include "cllist.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

//=== Registration
Register_Class(cLinkedList);

//=========================================================================
//=== cLinkedList - member functions

cLinkedList::cLinkedList(const cLinkedList& llist) : cObject()
{
    headp = tailp = NULL; n = 0;
    setName( llist.name() );
    operator=(llist);
}

cLinkedList::cLinkedList(const char *name) : cObject(name)
{
    headp=tailp=NULL;
    n=0;
    delfunc = NULL;
    dupfunc = NULL;
    itemsize = 0;
}

cLinkedList::~cLinkedList()
{
    clear();
}

std::string cLinkedList::info() const
{
    if (n==0)
        return std::string("empty");
    std::stringstream out;
    out << "length=" << n;
    return out.str();
}

void cLinkedList::netPack(cCommBuffer *buffer)
{
    throw new cRuntimeError(this,"netPack() not supported -- don't know how to pack an item");
}

void cLinkedList::netUnpack(cCommBuffer *buffer)
{
    throw new cRuntimeError(this,"netUnpack() not supported");
}

void cLinkedList::config( VoidDelFunc _delfunc, VoidDupFunc _dupfunc,
                          size_t _itemsize)
{
    delfunc = _delfunc;
    dupfunc = _dupfunc;
    itemsize = _itemsize;
}

void cLinkedList::clear()
{
    Elem *tmp;
    while (headp)
    {
        tmp = headp->next;
        if (dupfunc || itemsize>0) // we're expected to do memory mgmt
        {
            if (delfunc)
                delfunc(headp->item);
            else
                delete (char *)headp->item; // delete void* is no longer legal :-(
        }
        delete headp;
        headp=tmp;
    }
    tailp = NULL;
    n = 0;
}

cLinkedList& cLinkedList::operator=(const cLinkedList& llist)
{
    if (this==&llist) return *this;

    clear();

    cObject::operator=(llist);

    for (cLinkedList::Iterator iter(llist, 0); !iter.end(); iter--)
    {
       void *item;
       if (dupfunc)
          item = dupfunc(iter());
       else if (itemsize>0)
          memcpy(item=new char[itemsize],iter(),itemsize);
       else
          item=iter();
       insert( item );
    }
    return *this;
}

//== STRUCTURE OF THE LIST:
//==  'headp' and 'tailp' point to the ends of the list (NULL if it is empty).
//==  The list is double-linked, but 'headp->prev' and 'tailp->next' are NULL.

cLinkedList::Elem *cLinkedList::find_llelem(void *item) const
{
    Elem *p = headp;
    while( p && p->item!=item )
             p = p->next;
    return p;
}

void cLinkedList::insbefore_llelem(Elem *p, void *item)
{
    Elem *e = new Elem;
    e->item = item;

    e->prev = p->prev;
    e->next = p;
    p->prev = e;
    if (e->prev)
         e->prev->next = e;
    else
         headp = e;
    n++;
}

void cLinkedList::insafter_llelem(Elem *p, void *item)
{
    Elem *e = new Elem;
    e->item = item;

    e->next = p->next;
    e->prev = p;
    p->next = e;
    if (e->next)
         e->next->prev = e;
    else
         tailp = e;
    n++;
}

void *cLinkedList::remove_llelem(Elem *p)
{
    if( p->prev )
       p->prev->next = p->next;
    else
       headp = p->next;
    if( p->next )
       p->next->prev = p->prev;
    else
       tailp = p->prev;

    void *retitem = p->item;
    delete p;
    n--;
    return retitem;
}


void cLinkedList::insert(void *item)
{
    Elem *p = headp;

    if (p)
        insbefore_llelem(p,item);
    else if (tailp)
        insafter_llelem(tailp,item);
    else
    {
        // insert as the only item
        Elem *e = new Elem;
        e->item = item;
        headp = tailp = e;
        e->prev = e->next = NULL;
        n++;
    }
}

void cLinkedList::insertBefore(void *where, void *item)
{
    Elem *p = find_llelem(where);
    if (!p)
        throw new cRuntimeError(this,"insertBefore(w,o): item w not in list");
    insbefore_llelem(p,item);
}

void cLinkedList::insertAfter(void *where, void *item)
{
    Elem *p = find_llelem(where);
    if (!p)
        throw new cRuntimeError(this,"insertAfter(w,o): item w not in list");
    insafter_llelem(p,item);
}

void *cLinkedList::remove(void *item)
{
    if(!item) return NULL;

    Elem *p = find_llelem(item);
    if (!p)
        return NULL;
    return remove_llelem( p );
}

void *cLinkedList::pop()
{
    if (!tailp)
        throw new cRuntimeError(this,"pop(): list empty");
    return remove_llelem( tailp );
}


