//=========================================================================
//
//  CLLIST.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Member functions of
//    cLinkedList : linked list of void* pointers
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
#include "cllist.h"

//=== Registration
Register_Class( cLinkedList )

//=========================================================================
//=== cLinkedList - member functions

cLinkedList::cLinkedList(cLinkedList& llist) : cObject()
{
     head = tail = NULL; n = 0;
     setName( llist.name() );
     operator=(llist);
}

cLinkedList::cLinkedList(char *namestr) :
cObject( namestr )
{
     head=tail=NULL;
     n=0;
     delfunc = NULL;
     dupfunc = NULL;
     itemsize = 0;
}

cLinkedList::~cLinkedList()
{
     clear();
}

void cLinkedList::info(char *buf)
{
     cObject::info( buf );

     if( n==0 )
        sprintf( buf+strlen(buf), " (empty)" );
     else
        sprintf( buf+strlen(buf), " (n=%d)", n);
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
    sLLElem *tmp;
    while (head)
    {
        tmp = head->next;
        if (dupfunc || itemsize>0) // we're expected to do memory mgmt
        {
            if (delfunc) delfunc(head->item); else delete head->item;
        }
        delete head;
        head=tmp;
    }
    tail = NULL;
    n = 0;
}

cLinkedList& cLinkedList::operator=(cLinkedList& llist)
{
      clear();

      cObject::operator=(llist);

      for( cLinkedListIterator iter(llist); iter.end(); iter++)
      {
         void *item;
         if (dupfunc)
            item = dupfunc(iter());
         else if (itemsize>0)
            memcpy(item=new char[itemsize],iter(),itemsize);
         else
            item=iter();
         insertHead( item );
      }
      return *this;
}

//== STRUCTURE OF THE LIST:
//==  'head' and 'tail' point to the ends of the list (NULL if it is empty).
//==  The list is double-linked, but 'head->prev' and 'tail->next' are NULL.

sLLElem *cLinkedList::find_llelem(void *item)
{
      sLLElem *p = head;
      while( p && p->item!=item )
               p = p->next;
      return p;
}

void cLinkedList::insbefore_llelem(sLLElem *p, void *item)
{
      sLLElem *e = new sLLElem;
      e->item = item;

      e->prev = p->prev;
      e->next = p;
      p->prev = e;
      if (e->prev)
           e->prev->next = e;
      else
           head = e;
      n++;
}

void cLinkedList::insafter_llelem(sLLElem *p, void *item)
{
      sLLElem *e = new sLLElem;
      e->item = item;

      e->next = p->next;
      e->prev = p;
      p->next = e;
      if (e->next)
           e->next->prev = e;
      else
           tail = e;
      n++;
}

void *cLinkedList::get_llelem(sLLElem *p)
{
      if( p->prev )
         p->prev->next = p->next;
      else
         head = p->next;
      if( p->next )
         p->next->prev = p->prev;
      else
         tail = p->prev;

      void *retitem = p->item;
      delete p;
      n--;
      return retitem;
}


void cLinkedList::insertHead(void *item)
{
      sLLElem *p = head;

      if (p)
          insbefore_llelem(p,item);
      else if (tail)
          insafter_llelem(tail,item);
      else
      {
          // insert as the only item
          sLLElem *e = new sLLElem;
          e->item = item;
          head = tail = e;
          e->prev = e->next = NULL;
          n++;
      }
}

void cLinkedList::insertBefore(void *where, void *item)
{
      sLLElem *p = find_llelem(where);
      if (p)
         insbefore_llelem(p,item);
      else
         opp_error("(%s)%s: insertBefore(w,o): item w not in list",
                           className(),fullName());
}

void cLinkedList::insertAfter(void *where, void *item)
{
      sLLElem *p = find_llelem(where);
      if (p)
         insafter_llelem(p,item);
      else
         opp_error("(%s)%s: insertAfter(w,o): item w not in list",
                           className(),fullName());
}

void *cLinkedList::get(void *item)
{
      if(!item) return NULL;

      sLLElem *p = find_llelem(item);
      if (!p) {
           opp_warning("(%s)%s: get(): item not in list",className(),fullName());
           return item;
      }
      return get_llelem( p );
}

void *cLinkedList::getTail()
{
      if (!tail) {
           opp_error("(%s)%s: getTail(): list empty",className(),fullName());
           return NULL;
      }
      return get_llelem( tail );
}

void *cLinkedList::getHead()
{
      if (!head) {
           opp_error("(%s)%s: getHead(): list empty",className(),fullName());
           return NULL;
      }
      return get_llelem( head );
}

