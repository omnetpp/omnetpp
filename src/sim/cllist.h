//==========================================================================
//   CLLIST.H  - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cLinkedList        : linked list of pointers (cQueue-like interface)
//    cLinkedListIterator: walks along a linked list
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CLLIST_H
#define __CLLIST_H

#include "cobject.h"

//=== classes declared here
class  cLinkedList;
class  cLinkedListIterator;

//==========================================================================
// sLLElem : used internally by cLinkedList and cLinkedListIterator
struct sLLElem
{
    void *item;
    sLLElem *prev, *next;
};

//-------------------------------------------------------------------------
// cLinkedList: a double-linked list of items, with a cQueue-like interface

class SIM_API cLinkedList : public cObject
{
    friend class cLinkedListIterator;
  private:
    sLLElem *headp, *tailp;   // inserting at head, removal at tail
    int n;                    // number of items in list

    VoidDelFunc delfunc;      // user func to free up item (NULL-->delete)
    VoidDupFunc dupfunc;      // user func to dupl. item (NULL-->new+memcpy)
    size_t itemsize;          // used in making shallow copy if dupfunc==0
                              // if both dupfunc and itemsize are 0, we do
                              // no memory management (we treat pointers as
                              // mere pointers)

  protected:
    sLLElem *find_llelem(void *item);
    void insbefore_llelem(sLLElem *p, void *item);
    void insafter_llelem(sLLElem *p, void *item);
    void *remove_llelem(sLLElem *p);

  public:
    cLinkedList(cLinkedList& llist);
    explicit cLinkedList(const char *name=NULL);
    virtual ~cLinkedList();

    // redefined functions
    virtual const char *className()  {return "cLinkedList";}
    virtual cObject *dup()  {return new cLinkedList(*this);}
    virtual void info(char *buf);
    virtual const char *inspectorFactoryName() {return "cLinkedListIFC";}
    virtual int netPack();
    virtual int netUnpack();
    cLinkedList& operator=(cLinkedList& queue);

    // new functions
    void config( VoidDelFunc _delfunc, VoidDupFunc _dupfunc, size_t _itemsize=0);

    void insert(void *item);                     // insert item at head
    void insertBefore(void *where, void *item);  // insert at specific position
    void insertAfter(void *where, void *item);   // insert at specific position
    void *head()  {return n!=0 ? headp->item : NULL;}  // peek head item
    void *tail()  {return n!=0 ? tailp->item : NULL;}  // peek tail item
    void *remove(void *item);                    // remove item
    void *pop();                                 // remove item at tail

    int length() {return n;}        // number of items;
    bool empty() {return n==0;}     // see if queue is empty or not
    bool contains(void *item)  {return find_llelem(item)!=NULL;}

    void clear();                   // delete whole contents
};

//==========================================================================
//  cLinkedListIterator : walks along a cLinkedList
//   NOTE: not a cObject descendant!

class SIM_API cLinkedListIterator
{
  private:
    sLLElem *p;
  public:
    cLinkedListIterator(cLinkedList& q, int athead=1)
            {p=&q ? (athead ? q.headp : q.tailp) : NO(sLLElem);}
    void init(cLinkedList& q, int athead=1)
            {p=&q ? (athead ? q.headp : q.tailp) : NO(sLLElem);}
    void *operator()()        {return p->item;}
    bool end()                {return (bool)(p==NULL);}
    void *operator++(int)  {sLLElem *t=p; if(p) p=p->next; return t->item;}
    void *operator--(int)  {sLLElem *t=p; if(p) p=p->prev; return t->item;}
};

#endif
