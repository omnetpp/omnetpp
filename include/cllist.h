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

//
// Used internally by cLinkedList and cLinkedListIterator.
//
struct sLLElem
{
    void *item;
    sLLElem *prev, *next;
};

//-------------------------------------------------------------------------

/**
 * A double-linked list of non-cObject items. cLinkedList has a cQueue-like
 * interface.
 *
 * Memory management of contained items is controlled by the configPointer()
 * function. As default, pointers are treated as mere pointers, so
 * items are never duplicated or deleted.
 *
 * @ingroup Containers
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
    // internal use.
    // if both dupfunc and itemsize are 0, we do no memory management
    // (we treat pointers as mere pointers)
    sLLElem *find_llelem(void *item) _CONST;

    // internal use
    void insbefore_llelem(sLLElem *p, void *item);

    // internal use
    void insafter_llelem(sLLElem *p, void *item);

    // internal use
    void *remove_llelem(sLLElem *p);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor. Contained items that are owned by the list will
     * be duplicated using the function passed in configPointer()
     * so that the new list will have its own copy of them. By default,
     * there's no duplication function so only the pointers are copied.
     */
    cLinkedList(_CONST cLinkedList& llist);

    /**
     * Constructor. It accepts the object name.
     */
    explicit cLinkedList(const char *name=NULL);

    /**
     * Destructor calls clear().
     */
    virtual ~cLinkedList();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's
     * operator=() for more details.
     * Duplication and assignment work all right with cLinkedList.
     * Contained items are treated as configured with configPointer().
     * By default, only pointers are copied.
     */
    cLinkedList& operator=(_CONST cLinkedList& queue);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Returns pointer to a string containing the class name, "cLinkedList".
     */
    virtual const char *className() const  {return "cLinkedList";}

    /**
     * Duplication and assignment work all right with cLinkedList.
     * Contained items are treated as configured with configPointer().
     * By default, only pointers are copied.
     */
    virtual cObject *dup() _CONST  {return new cLinkedList(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Returns the name of the inspector factory class associated with this class.
     * See cObject for more details.
     */
    virtual const char *inspectorFactoryName() const {return "cLinkedListIFC";}

    /**
     * Serializes the object into a PVM or MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netPack();

    /**
     * Deserializes the object from a PVM or MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netUnpack();
    //@}

    /** @name Container functions. */
    //@{

    /**
     * Configures memory management for contained items.
     *
     * <TABLE BORDER=1>
     * <TR><TD WIDTH=96><B>delete func.</B></TD><TD WIDTH=89><B>dupl.func.</B>
     * </TD><TD WIDTH=92><B>itemsize</B></TD><TD WIDTH=317><B>behaviour</B>
     * </TD></TR>
     * <TR><TD WIDTH=96>NULL</TD><TD WIDTH=89>NULL
     * </TD><TD WIDTH=92>0</TD><TD WIDTH=317>Pointer is treated as mere pointer - no memory management. Duplication copies the pointer, and deletion does nothing.
     * </TD></TR>
     * <TR><TD WIDTH=96>NULL</TD><TD WIDTH=89>NULL
     * </TD><TD WIDTH=92>>0 size</TD><TD WIDTH=317>Plain memory management. Duplication is done with operator new char[size]+memcpy(), and deletion is done via operator delete.
     * </TD></TR>
     * <TR><TD WIDTH=96>NULL or user's delete func.</TD><TD WIDTH=89>user's dupfunc.
     * </TD><TD WIDTH=92>indifferent</TD><TD WIDTH=317>Sophisticated memory management. Duplication is done by calling the user-supplied duplication function, which should do the allocation and the appropriate copying. Deletion is done by calling the user-supplied delete function, or the delete operator if it is not supplied.
     * </TD></TR>
     * </TABLE>
     */
    void config( VoidDelFunc _delfunc, VoidDupFunc _dupfunc, size_t _itemsize=0);

    /**
     * Inserts the given object into the list, maintaining the sorting order.
     */
    void insert(void *item);

    /**
     * Inserts exactly before the given item. If the item to be inserted before
     * is not in the list, an error occurs.
     */
    void insertBefore(void *where, void *item);

    /**
     * Inserts exactly after the given item. If the item to be inserted after
     * is not in the list, an error occurs.
     */
    void insertAfter(void *where, void *item);

    /**
     * Returns the first item in the list or NULL pointer if the list
     * is empty.
     */
    void *head() _CONST  {return n!=0 ? headp->item : NULL;}

    /**
     * Returns the last item in the list or NULL pointer if the list
     * is empty.
     */
    void *tail() _CONST  {return n!=0 ? tailp->item : NULL;}

    /**
     * Unlinks and returns the given item. If the item is not in the list,
     * an error occurs.
     */
    void *remove(void *item);

    /**
     * Unlinks and returns the last item in the list. If the list is empty,
     * an error occurs.
     */
    void *pop();

    /**
     * Returns the number of items contained in the list.
     */
    int length() _CONST {return n;}

    /**
     * Returns true if the list is empty.
     */
    bool empty() _CONST {return n==0;}

    /**
     * Returns true if the list contains the given pointer.
     */
    bool contains(void *item) _CONST  {return find_llelem(item)!=NULL;}

    /**
     * As a result, the container will be empty. Contained items will
     * be deleted as configured by configPointer().
     */
    void clear();
    //@}
};

//==========================================================================

/**
 * Walks along a cLinkedList object.
 * To examine each element in the list, the cLinkedListIterator
 * iterator class can be used. Once a cLinkedListIterator
 * object is created for the list (the cLinkedList object),
 * the ++ and -- operators can be used to step from one element of
 * the list to the next/previous one.
 *
 * NOTE: not a cObject descendant!
 */
class SIM_API cLinkedListIterator
{
  private:
    sLLElem *p;

  public:
    /**
     * Constructor. Takes the cLinkedList object as argument.
     * The current item will be the first (if athead==1, default) or the last
     * (if athead==0) item in the list.
     */
    cLinkedListIterator(_CONST cLinkedList& q, int athead=1)  //FIXME: make bool!
            {p=&q ? (athead ? q.headp : q.tailp) : NO(sLLElem);}

    /**
     * Reinitializes the iterator object.
     */
    void init(_CONST cLinkedList& q, int athead=1)
            {p=&q ? (athead ? q.headp : q.tailp) : NO(sLLElem);}

    /**
     * Returns the current item.
     */
    void *operator()() _CONST        {return p->item;}

    /**
     * Returns true if we have reached the end (with operator++) or the beginning
     * (with operator--) of the list.
     */
    bool end() _CONST                {return (bool)(p==NULL);}

    /**
     * Returns the current item and steps to the next one.
     */
    void *operator++(int)  {sLLElem *t=p; if(p) p=p->next; return t->item;}

    /**
     * Returns the current item and steps to the previous one.
     */
    void *operator--(int)  {sLLElem *t=p; if(p) p=p->prev; return t->item;}
};

#endif

