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
     head = tail = NULL; n = 0;
     setName( queue.name() );
     operator=(queue);
}

cQueue::cQueue(char *namestr, CompareFunc cmp, bool a) :
cObject( namestr )
{
     head=tail=NULL;
     n=0;
     setup( cmp, a );
}

cQueue::~cQueue()
{
      while (head)
      {
          // delete only the holder structs; owned objects will be
          // deleted by cObject's destructor
          //
          sQElem *tmp = head->next;
          delete head;
          head=tmp;
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
     if (do_fn(this,TRUE))
        // loop through elements in reverse order
        for( sQElem *p=tail; p!=NULL; p=p->prev )
            p->obj->forEach( do_fn );
     do_fn(this,FALSE);
}

void cQueue::clear()
{
      sQElem *tmp;
      while (head)
      {
          tmp = head->next;
          if (head->obj->owner()==this) free( head->obj );
          delete head;
          head=tmp;
      }
      tail = NULL;
      n = 0;
}

cQueue& cQueue::operator=(cQueue& queue)
{
      clear();

      cObject::operator=(queue);
      compare = queue.compare;
      asc = queue.asc;

      bool old_tk = takeOwnership();
      for( cQueueIterator iter(queue); iter.end(); iter++)
      {
         if (iter()->owner()==&queue)
             {takeOwnership(TRUE); insertHead( iter()->dup() );}
         else
             {takeOwnership(FALSE); insertHead( iter() );}
      }
      takeOwnership( old_tk );
      return *this;
}

void cQueue::setup(CompareFunc cmp, bool a)
{
      compare=cmp; asc=a;
}

//== STRUCTURE OF THE LIST:
//==  'head' and 'tail' point to the ends of the list (NULL if it is empty).
//==  The list is double-linked, but 'head->prev' and 'tail->next' are NULL.

sQElem *cQueue::find_qelem(cObject *obj)
{
      sQElem *p = head;
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
           head = e;
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
           tail = e;
      n++;
}

cObject *cQueue::get_qelem(sQElem *p)
{
      if( p->prev )
         p->prev->next = p->next;
      else
         head = p->next;
      if( p->next )
         p->next->prev = p->prev;
      else
         tail = p->prev;

      cObject *retobj = p->obj;
      delete p;
      n--;
      if (retobj->owner()==this) drop( retobj );
      return retobj;
}


void cQueue::insertHead(cObject *obj)
{
      if (!obj) {
         opp_error("(%s)%s: cannot insert NULL pointer in queue",className(),fullName());
         return;
      }

      if (takeOwnership()) take(obj);

      sQElem *p = head;
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
      else if (tail)
          insafter_qelem(tail,obj);
      else
      {
          // insert as the only item
          sQElem *e = new sQElem;
          e->obj = obj;
          head = tail = e;
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

cObject *cQueue::get(cObject *obj)
{
      if(!obj) return NO(cObject);

      sQElem *p = find_qelem(obj);
      if(!p) {
           opp_warning("(%s)%s: get(): Object `%s' not in queue",
                              className(),fullName(),obj->fullName());
           return obj;
      }
      return get_qelem( p );
}

cObject *cQueue::getTail()
{
      if(!tail) {
           opp_error("(%s)%s: getTail(): queue empty",className(),fullName());
           return NO(cObject);
      }
      return get_qelem( tail );
}

cObject *cQueue::getHead()
{
      if(!head) {
           opp_error("(%s)%s: getHead(): queue empty",className(),fullName());
           return NO(cObject);
      }
      return get_qelem( head );
}

