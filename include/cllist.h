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
  Copyright (C) 1992-2001 Andras Varga
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

/**
 * FIXME: //=== classes declared here
 * 
 * sLLElem : used internally by cLinkedList and cLinkedListIterator
 */
struct sLLElem
{
    void *item;
    sLLElem *prev, *next;
};

//-------------------------------------------------------------------------
// cLinkedList: a double-linked list of items, with a cQueue-like interface


/**
 * FIXME: 
 * cLinkedList: a double-linked list of items, with a cQueue-like interface
 */
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

    /**
     * FIXME: if both dupfunc and itemsize are 0, we do
     * no memory management (we treat pointers as
     * mere pointers)
     */
    sLLElem *find_llelem(void *item);

    /**
     * MISSINGDOC: cLinkedList:void insbefore_llelem(sLLElem*,void*)
     */
    void insbefore_llelem(sLLElem *p, void *item);

    /**
     * MISSINGDOC: cLinkedList:void insafter_llelem(sLLElem*,void*)
     */
    void insafter_llelem(sLLElem *p, void *item);

    /**
     * MISSINGDOC: cLinkedList:void*remove_llelem(sLLElem*)
     */
    void *remove_llelem(sLLElem *p);

  public:

    /**
     * Copy constructor. Contained items that are owned by the list will
     * be duplicated using the function passed in configPointer()
     * so that the new list will have its own copy of them. By default,
     * there's no duplication function so only the pointers are copied.
     */
    cLinkedList(cLinkedList& llist);

    /**
     * Constructor. It accepts the object name.
     */
    explicit cLinkedList(const char *name=NULL);

    /**
     * Destructor calls clear().
     */
    virtual ~cLinkedList();

    // redefined functions

    /**
     * Returns pointer to the class name string,"cLinkedList".
     */
    virtual const char *className() const  {return "cLinkedList";}

    /**
     * Duplication and assignment work all right with cLinkedList.
     * Contained items are treated as configured with configPointer().
     * By default, only pointers are copied.
     */
    virtual cObject *dup()  {return new cLinkedList(*this);}

    /**
     * Redefined.
     */
    virtual void info(char *buf);

    /**
     * Redefined.
     */
    virtual const char *inspectorFactoryName() const {return "cLinkedListIFC";}

    /**
     * MISSINGDOC: cLinkedList:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cLinkedList:int netUnpack()
     */
    virtual int netUnpack();

    /**
     * Duplication and assignment work all right with cLinkedList.
     * Contained items are treated as configured with configPointer().
     * By default, only pointers are copied.
     */
    cLinkedList& operator=(cLinkedList& queue);

    // new functions

    /**
     * FIXME: new functions
     */
    void config( VoidDelFunc _delfunc, VoidDupFunc _dupfunc, size_t _itemsize=0);


    /**
     * Inserts the given object into the list, maintaining the sorting
     * order.
     */
    void insert(void *item);                     // insert item at head

    /**
     * Inserts exactly before and after the given item.
     */
    void insertBefore(void *where, void *item);  // insert at specific position

    /**
     * Inserts exactly before and after the given item.
     */
    void insertAfter(void *where, void *item);   // insert at specific position

    /**
     * MISSINGDOC: cLinkedList:void*head()
     */
    void *head()  {return n!=0 ? headp->item : NULL;}  // peek head item

    /**
     * Returns the last item in the list or null pointer if the list
     * is empty.
     */
    void *tail()  {return n!=0 ? tailp->item : NULL;}  // peek tail item

    /**
     * Unlinks and returns the given item.
     */
    void *remove(void *item);                    // remove item

    /**
     * Unlinks and returns the last item in the list.
     */
    void *pop();                                 // remove item at tail


    /**
     * Returns the number of items contained in the list.
     */
    int length() {return n;}        // number of items;

    /**
     * Returns true if the list is empty.
     */
    bool empty() {return n==0;}     // see if queue is empty or not

    /**
     * MISSINGDOC: cLinkedList:bool contains(void*)
     */
    bool contains(void *item)  {return find_llelem(item)!=NULL;}


    /**
     * As a result, the container will be empty. Contained items will
     * be deleted as configured by configPointer().
     */
    void clear();                   // delete whole contents
};

//==========================================================================
//  cLinkedListIterator : walks along a cLinkedList
//   NOTE: not a cObject descendant!


/**
 * FIXME: 
 *  cLinkedListIterator : walks along a cLinkedList
 *   NOTE: not a cObject descendant!
 */
class SIM_API cLinkedListIterator
{
  private:
    sLLElem *p;
  public:

    /**
     * Constructor, cIterator will walk on the list passed as
     * argument. The current item will be the first (if a==1) or the
     * last (a==0) item in the list.
     */
    cLinkedListIterator(cLinkedList& q, int athead=1)
            {p=&q ? (athead ? q.headp : q.tailp) : NO(sLLElem);}

    /**
     * Reinitializes the iterator object.
     */
    void init(cLinkedList& q, int athead=1)
            {p=&q ? (athead ? q.headp : q.tailp) : NO(sLLElem);}

    /**
     * Returns the current item.
     */
    void *operator()()        {return p->item;}

    /**
     * Returns true if we have reached the end of the list.
     */
    bool end()                {return (bool)(p==NULL);}

    /**
     * Steps to the next/previous object in the list.
     */
    void *operator++(int)  {sLLElem *t=p; if(p) p=p->next; return t->item;}

    /**
     * Steps to the next/previous object in the list.
     */
    void *operator--(int)  {sLLElem *t=p; if(p) p=p->prev; return t->item;}
};

#endif
