//=========================================================================
//
//  CMSGHEAP.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Member functions of
//    cMessageHeap : future event set, implemented as heap
//
//  Author: Andras Varga, based on the code from Gabor Lencse
//          (the original is taken from G. H. Gonnet's book pp. 273-274)
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include <string.h>          // strlen
#include <stdlib.h>          // qsort
#include "macros.h"
#include "cmessage.h"
#include "cmsgheap.h"

//=== Registration
Register_Class(cMessageHeap);

//==========================================================================

inline int operator <= (cMessage& a, cMessage& b)
{
    return (a.arrivalTime() < b.arrivalTime()) ? 1 :
           (a.arrivalTime() > b.arrivalTime()) ? 0 :
           (a.priority() < b.priority()) ? 1 :
           (a.priority() > b.priority()) ? 0 :
            a.insertOrder() <= b.insertOrder();
}

inline int operator > (cMessage& a, cMessage& b)
{
    return !(a<=b);
}

static int qsort_cmp_msgs(const void *p1, const void *p2)
{
    cMessage *m1 = *(cMessage **)p1;
    cMessage *m2 = *(cMessage **)p2;

    simtime_t dt = m1->arrivalTime() - m2->arrivalTime();
    if (dt<0)       return -1;
    else if (dt>0)  return 1;

    int dpri = m1->priority() - m2->priority();
    if (dpri) return dpri;

    return (m1->insertOrder() < m2->insertOrder()) ? -1 : 1;
}

//==========================================================================
//=== cMessageHeap - member functions

cMessageHeap::cMessageHeap(const char *name, int siz) : cObject(name)
{
    insertcntr = 0L;
    n = 0;
    size = siz;
    h = new cMessage *[size+1];    // +1 is necessary because h[0] is not used
}

cMessageHeap::cMessageHeap(const cMessageHeap& heap) : cObject()
{
    h=NULL; n=0;
    setName( heap.name() );
    operator=(heap);
}

cMessageHeap::~cMessageHeap()
{
    // delete only the h[] array, NOT the objects (owned objects are
    //   deleted by cObject's destructor)
    delete [] h;
    h=NULL;
    n=0;
}

void cMessageHeap::info(char *buf)
{
    if (n==0)
        sprintf(buf, "(empty)" );
    else
        sprintf(buf, "(length=%d first=%s)",
                 n, simtimeToStr(h[1]->arrivalTime()));
}

void cMessageHeap::forEach( ForeachFunc do_fn )
{
    sort();

    if (do_fn(this,true))
        for (int i=1; i<=n; i++)
            if (h[i]) h[i]->forEach( do_fn );
    do_fn(this,false);
}

void cMessageHeap::clear()
{
    for (int i=1; i<=n; i++)
        if (h[i] && h[i]->owner()==this)
            {h[i]->heapindex=-1; discard(h[i]);}
    n=0;
}

cMessageHeap& cMessageHeap::operator=(const cMessageHeap& heap)
{
    if (this==&heap) return *this;

    clear();

    cObject::operator=(heap);

    n = heap.n;
    size = heap.size;
    delete [] h;
    h = new cMessage *[size+1];

    for (int i=1; i<=n; i++)
    {
        if (heap.h[i]->owner()==const_cast<cMessageHeap*>(&heap))
            take( h[i]=(cMessage *)heap.h[i]->dup() );
        else
            h[i]=heap.h[i];
    }
    return *this;
}

void cMessageHeap::sort()
{
    qsort(h+1,n,sizeof(cMessage *),qsort_cmp_msgs);
    for (int i=1; i<=n; i++) h[i]->heapindex=i;
}

void cMessageHeap::insert(cMessage *event)
{
    int i,j;

    event->insertordr = insertcntr++;

    if (++n > size)
    {
        size *= 2;
        cMessage **hnew = new cMessage *[size+1];
        for (i=1; i<=n-1; i++)
             hnew[i]=h[i];
        delete [] h;
        h = hnew;
    }

    if (takeOwnership())  take(event);

    for (j=n; j>1; j=i)
    {
        i=j/2;
        if (*h[i] <= *event)   //direction
            break;

        (h[j]=h[i])->heapindex=j;
    }
    (h[j]=event)->heapindex=j;
}

void cMessageHeap::shiftup(int from)
{
    // restores heap structure (in a sub-heap)
    int i,j;
    cMessage *temp;

    i=from;
    while ((j=2*i) <= n)
    {
        if (j<n && (*h[j] > *h[j+1]))   //direction
            j++;
        if (*h[i] > *h[j])  //is change necessary?
        {
            temp=h[j];
            (h[j]=h[i])->heapindex=j;
            (h[i]=temp)->heapindex=i;
            i=j;
        }
        else
            break;
    }
}

cMessage *cMessageHeap::peekFirst() const
{
    return n==0 ? NULL : h[1];
}

cMessage *cMessageHeap::getFirst()
{
    if (n>0)
    {
        // first is taken out and replaced by the last one
        cMessage *event = h[1];
        (h[1]=h[n--])->heapindex=1;
        shiftup();
        if (event->owner()==this) drop(event);
        event->heapindex=-1;
        return event;
    }
    return NULL;
}

cMessage *cMessageHeap::get(cMessage *event)
{
    // make sure it is really on the heap
    if (event->heapindex==-1)
        return NULL;

    // sanity check:
    // if (h[event->heapindex]!=event)  error!!!!

    // last element will be used to fill the hole
    int father, out = event->heapindex;
    cMessage *fill = h[n--];
    while ((father=out/2)!=0 && *h[father] > *fill)
    {
        (h[out]=h[father])->heapindex=out;  // father is moved down
        out=father;
    }
    (h[out]=fill)->heapindex=out;
    shiftup(out);
    if (event->owner()==this) drop(event);
    event->heapindex=-1;
    return event;
}

