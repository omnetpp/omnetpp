//==========================================================================
//  CLLIST.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cLinkedList        : linked list of pointers (cQueue-like interface)
//    cLinkedListIterator: walks along a linked list
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CLLIST_H
#define __CLLIST_H

#include "cobject.h"


/**
 * Use of this class is DISCOURAGED, it is provided for backward compatibility
 * only. Use std::deque or std::list instead.
 *
 * A double-linked list of non-cObject items. cLinkedList has a cQueue-like
 * interface.
 *
 * Memory management of contained items is controlled by the configPointer()
 * function. As default, pointers are treated as mere pointers, so
 * items are never duplicated or deleted.
 *
 * @see Iterator
 * @ingroup Containers
 */
class SIM_API cLinkedList : public cObject
{
    // a list elem
    struct Elem
    {
        void *item;
        Elem *prev, *next;
    };

  public:
    /**
     * Walks along a cLinkedList object. To examine each element in the list,
     * the Iterator class can be used. Once an Iterator object is created for the list,
     * the ++ and -- operators can be used to step from one element of
     * the list to the next/previous one.
     */
    class Iterator
    {
      private:
        Elem *p;

      public:
        /**
         * Constructor. Takes the cLinkedList object as argument.
         * The current item will be the first (if athead==true, default) or the last
         * (if athead==false) item in the list.
         */
        Iterator(const cLinkedList& q, bool athead=true)
                {p=&q ? (athead ? q.headp : q.tailp) : NULL;}

        /**
         * Reinitializes the iterator object.
         */
        void init(const cLinkedList& q, bool athead=true)
                {p=&q ? (athead ? q.headp : q.tailp) : NULL;}

        /**
         * Returns the current item.
         */
        void *operator()() const  {return p->item;}

        /**
         * Returns true if we have reached the end (with operator++) or the beginning
         * (with operator--) of the list.
         */
        bool end() const   {return (bool)(p==NULL);}

        /**
         * Returns the current item and steps to the next one.
         */
        void *operator++(int)  {if (!p) return NULL; Elem *t=p; p=p->next; return t->item;}

        /**
         * Returns the current item and steps to the previous one.
         */
        void *operator--(int)  {if (!p) return NULL; Elem *t=p; p=p->prev; return t->item;}
    };

    friend class Iterator;

  private:
    Elem *headp, *tailp;      // inserting at head, removal at tail
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
    Elem *find_llelem(void *item) const;

    // internal use
    void insbefore_llelem(Elem *p, void *item);

    // internal use
    void insafter_llelem(Elem *p, void *item);

    // internal use
    void *remove_llelem(Elem *p);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor. Contained items that are owned by the list will
     * be duplicated using the function passed in configPointer()
     * so that the new list will have its own copy of them. By default,
     * there's no duplication function so only the pointers are copied.
     */
    cLinkedList(const cLinkedList& llist);

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
    cLinkedList& operator=(const cLinkedList& queue);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Duplication and assignment work all right with cLinkedList.
     * Contained items are treated as configured with configPointer().
     * By default, only pointers are copied.
     */
    virtual cPolymorphic *dup() const  {return new cLinkedList(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Serializes the object into a PVM or MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from a PVM or MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Container functions. */
    //@{

    /**
     * Configures memory management for contained items.
     *
     * <TABLE BORDER=1>
     * <TR><TD WIDTH=96><B>delete func.</B></TD><TD WIDTH=89><B>dupl.func.</B>
     * </TD><TD WIDTH=92><B>itemsize</B></TD><TD WIDTH=317><B>behavior</B>
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
     * Inserts the given object at the head of the list.
     */
    void insert(void *item);

    /**
     * Inserts exactly before the given item. If the item to be inserted before
     * is not in the list, cRuntimeError is thrown.
     */
    void insertBefore(void *where, void *item);

    /**
     * Inserts exactly after the given item. If the item to be inserted after
     * is not in the list, cRuntimeError is thrown.
     */
    void insertAfter(void *where, void *item);

    /**
     * Returns the first item in the list or NULL pointer if the list
     * is empty.
     */
    void *head() const  {return n!=0 ? headp->item : NULL;}

    /**
     * Returns the last item in the list or NULL pointer if the list
     * is empty.
     */
    void *tail() const  {return n!=0 ? tailp->item : NULL;}

    /**
     * Unlinks and returns the given item. If the item is not in the list,
     * cRuntimeError is thrown.
     */
    void *remove(void *item);

    /**
     * Unlinks and returns the last item in the list. If the list is empty,
     * cRuntimeError is thrown.
     */
    void *pop();

    /**
     * Returns the number of items contained in the list.
     */
    int length() const {return n;}

    /**
     * Returns true if the list is empty.
     */
    bool empty() const {return n==0;}

    /**
     * Returns true if the list contains the given pointer.
     */
    bool contains(void *item) const  {return find_llelem(item)!=NULL;}

    /**
     * As a result, the container will be empty. Contained items will
     * be deleted as configured by configPointer().
     */
    void clear();
    //@}
};


#endif

