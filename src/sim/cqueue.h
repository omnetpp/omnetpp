//==========================================================================
//   CQUEUE.H  - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cQueue        : (optionally) sorted queue of cObjects
//    cQueueIterator: walks along a queue
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CQUEUE_H
#define __CQUEUE_H

#include "cobject.h"

//=== classes declared here
class  cQueue;
class  cQueueIterator;

//==========================================================================
// sQElem : used internally by cQueue and cQueueIterator
struct sQElem
{
    cObject *obj;
    sQElem *prev, *next;
};

//-------------------------------------------------------------------------
// cQueue: a sorted queue of cObjs

class cQueue : public cObject
{
        friend class cQueueIterator;
    private:
        sQElem *head, *tail;            // inserting at head, removal at tail
        int n;                          // number of items in queue
        CompareFunc compare;            // compare function
        bool asc;                       // order: TRUE=ascending
    protected:
        sQElem *find_qelem(cObject *obj);
        void insbefore_qelem(sQElem *p, cObject *obj);
        void insafter_qelem(sQElem *p, cObject *obj);
        cObject *get_qelem(sQElem *p);

    public:
        cQueue(cQueue& queue);
        cQueue(char *namestr=NULL, CompareFunc cmp=NULL, bool a=FALSE);
        virtual ~cQueue();

        // redefined functions
        virtual char *isA()     {return "cQueue";}
        virtual cObject *dup()     {return new cQueue(*this);}
        virtual void info(char *buf);
        virtual char *inspectorFactoryName() {return "cQueueIFC";}
        virtual void forEach(ForeachFunc f);
        virtual int netPack();
        virtual int netUnpack();
        cQueue& operator=(cQueue& queue);

        // new functions
        void setup(CompareFunc cmp=NULL, bool a=FALSE); //reconfig but not reorder!

        void insertHead(cObject *obj);     // insert at head, using cmpfunc
        void insertBefore(cObject *where, cObject *obj);
        void insertAfter(cObject *where, cObject *obj);
        cObject *peekTail()  {return n!=0 ? tail->obj : NO(cObject);}
        cObject *peekHead()  {return n!=0 ? head->obj : NO(cObject);}
        cObject *get(cObject *obj);
        cObject *getTail();
        cObject *getHead();

        int length() {return n;}        // number of items;
        bool empty() {return n==0;}     // see if queue is empty or not
        bool isEnqueued(cObject *obj)   {return find_qelem(obj)!=NULL;}

        void clear();                   // delete whole contents
};

//==========================================================================
//  cQueueIterator : walks along a cQueue
//   NOTE: not a cObject descendant!

class cQueueIterator
{
   private:
      sQElem *p;
   public:
      cQueueIterator(cQueue& q, int athead=1)
              {p=&q ? (athead ? q.head : q.tail) : NO(sQElem);}
      void init(cQueue& q, int athead=1)
              {p=&q ? (athead ? q.head : q.tail) : NO(sQElem);}
      cObject& operator[](int)  {return *p->obj;} //OBSOLETE
      cObject *operator()()     {return p->obj;}
      bool end()                {return (bool)(p==NULL);}
      cObject *operator++(int)  {sQElem *t=p; if(p) p=p->next; return t->obj;}
      cObject *operator--(int)  {sQElem *t=p; if(p) p=p->prev; return t->obj;}
};

#endif
