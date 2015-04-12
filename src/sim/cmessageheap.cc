//=========================================================================
//  CMESSAGEHEAP.CC - part of
//
//                  OMNeT++/OMNEST
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
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include <string.h>          // strlen
#include <stdlib.h>          // qsort
#include <sstream>
#include "omnetpp/globals.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cmessageheap.h"

NAMESPACE_BEGIN

Register_Class(cMessageHeap);


#define CBHEAPINDEX(i) (-2-(i))
#define CBINC(i)       ((i)=((i)+1)&(cbsize-1))
#define CBDEC(i)       ((i)=((i)-1)&(cbsize-1))


inline bool operator > (cEvent& a, cEvent& b)
{
    return a.getArrivalTime() > b.getArrivalTime() ? true :
           a.getArrivalTime() < b.getArrivalTime() ? false :
           (a.getSchedulingPriority() > b.getSchedulingPriority()) ? true :
           (a.getSchedulingPriority() < b.getSchedulingPriority()) ? false :
            a.getInsertOrder() > b.getInsertOrder();
}

inline bool operator <= (cEvent& a, cEvent& b)
{
    return !(a>b);
}


static int qsort_cmp_msgs(const void *p1, const void *p2)
{
    cEvent *m1 = *(cEvent **)p1;
    cEvent *m2 = *(cEvent **)p2;

    if (m1->getArrivalTime() < m2->getArrivalTime())
        return -1;
    if (m1->getArrivalTime() > m2->getArrivalTime())
        return 1;

    int dpri = m1->getSchedulingPriority() - m2->getSchedulingPriority();
    if (dpri)
        return dpri;

    return (m1->getInsertOrder() < m2->getInsertOrder()) ? -1 : 1;
}

//----

cMessageHeap::cMessageHeap(const char *name, int siz) : cOwnedObject(name, false)
{
    insertcntr = 0L;
    n = 0;
    size = siz;
    h = new cEvent *[size+1];    // +1 is necessary because h[0] is not used

    cbsize = 4; // must be power of 2!
    cb = new cEvent *[cbsize];
    cbhead = cbtail = 0;
}

cMessageHeap::cMessageHeap(const cMessageHeap& heap) : cOwnedObject(heap)
{
    cb = NULL; h=NULL; n=0;
    copy(heap);
}

cMessageHeap::~cMessageHeap()
{
    clear();
    delete [] h;
    delete [] cb;
}

std::string cMessageHeap::info() const
{
    if (isEmpty())
        return std::string("empty");
    std::stringstream out;
    out << "length=" << getLength();
    return out.str();
}

void cMessageHeap::forEachChild(cVisitor *v)
{
    sort();

    for (int i=cbhead; i!=cbtail; CBINC(i))
        v->visit(cb[i]);

    for (int i=1; i<=n; i++)
        if (h[i])
            v->visit(h[i]);
}

void cMessageHeap::clear()
{
    for (int i=cbhead; i!=cbtail; CBINC(i))
        dropAndDelete(cb[i]);
    cbhead = cbtail = 0;

    for (int i=1; i<=n; i++)
        dropAndDelete(h[i]);
    n = 0;
}

void cMessageHeap::copy(const cMessageHeap& heap)
{
    // copy heap
    n = heap.n;
    size = heap.size;
    delete [] h;
    h = new cEvent *[size+1];
    for (int i=1; i<=n; i++)
        take( h[i]=heap.h[i]->dup() );

    // copy circular buffer
    cbhead = heap.cbhead;
    cbtail = heap.cbtail;
    cbsize = heap.cbsize;
    delete [] cb;
    cb = new cEvent *[cbsize];
    for (int i=cbhead; i!=cbtail; CBINC(i))
        take( cb[i]=heap.cb[i]->dup() );
}

cMessageHeap& cMessageHeap::operator=(const cMessageHeap& heap)
{
    if (this==&heap) return *this;
    clear();
    cOwnedObject::operator=(heap);
    copy(heap);
    return *this;
}

cEvent *cMessageHeap::peek(int m)
{
    if (m < 0)
        return NULL;

    // first few elements map into the circular buffer
    int cblen = cblength();
    if (m < cblen)
        return cbget(m);
    m -= cblen;

    // map the rest to h[1]..h[n] (h[] is 1-based)
    if (m >= n)
        return NULL;
    return h[m+1];
}

void cMessageHeap::sort()
{
    qsort(h+1,n,sizeof(cEvent *),qsort_cmp_msgs);
    for (int i=1; i<=n; i++)
        h[i]->heapindex=i;
}

void cMessageHeap::insert(cEvent *event)
{
    take(event);

    if (event->getArrivalTime()==simTime() && event->getSchedulingPriority()==0 && (n==0 || h[1]->getArrivalTime()!=simTime()))
    {
        // scheduled for *now* -- use circular buffer
        cb[cbtail] = event;
        event->heapindex = CBHEAPINDEX(cbtail);
        CBINC(cbtail);
        if (cbtail==cbhead)
            cbgrow();
    }
    else
    {
        // use heap
        int i,j;

        event->insertordr = insertcntr++;

        if (++n > size)
        {
            size *= 2;
            cEvent **hnew = new cEvent *[size+1];
            for (i=1; i<=n-1; i++)
                 hnew[i]=h[i];
            delete [] h;
            h = hnew;
        }

        for (j=n; j>1; j=i)
        {
            i = j>>1;
            if (*h[i] <= *event)   // direction
                break;

            (h[j]=h[i])->heapindex=j;
        }
        (h[j]=event)->heapindex=j;
    }
}

void cMessageHeap::cbgrow()
{
    int newsize = 2*cbsize; // cbsize MUST be power of 2
    cEvent **newcb = new cEvent*[newsize];
    for (int i=0; i<cbsize; i++)
        (newcb[i] = cb[(cbhead+i)&(cbsize-1)])->heapindex = CBHEAPINDEX(i);
    delete [] cb;

    cb = newcb;
    cbhead = 0;
    cbtail = cbsize;
    cbsize = newsize;
}

void cMessageHeap::shiftup(int from)
{
    // restores heap structure (in a sub-heap)
    int i,j;
    cEvent *temp;

    i = from;
    while ((j=i<<1) <= n)
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

cEvent *cMessageHeap::peekFirst() const
{
    return cbhead!=cbtail ? cb[cbhead] : n!=0 ? h[1] : NULL;
}

cEvent *cMessageHeap::removeFirst()
{
    if (cbhead != cbtail)
    {
        // remove head element from circular buffer
        cEvent *event = cb[cbhead];
        CBINC(cbhead);
        drop(event);
        event->heapindex=-1;
        return event;
    }
    else if (n>0)
    {
        // heap: first is taken out and replaced by the last one
        cEvent *event = h[1];
        (h[1]=h[n--])->heapindex=1;
        shiftup();
        drop(event);
        event->heapindex=-1;
        return event;
    }
    return NULL;
}

cEvent *cMessageHeap::remove(cEvent *event)
{
    // make sure it is really on the heap
    if (event->heapindex==-1)
        return NULL;

    if (event->heapindex<0)
    {
        // event is in the circular buffer
        int i = -event->heapindex-2;
        ASSERT(cb[i]==event); // sanity check

        // remove
        int iminus1 = i; CBINC(i);
        for (/**/; i!=cbtail; iminus1=i, CBINC(i))
            (cb[iminus1] = cb[i])->heapindex = CBHEAPINDEX(iminus1);
        CBDEC(cbtail);
    }
    else
    {
        // event is on the heap

        // sanity check:
        // ASSERT(h[event->heapindex]==event);

        // last element will be used to fill the hole
        int father, out = event->heapindex;
        cEvent *fill = h[n--];
        while ((father=out>>1)!=0 && *h[father] > *fill)
        {
            (h[out]=h[father])->heapindex=out;  // father is moved down
            out=father;
        }
        (h[out]=fill)->heapindex=out;
        shiftup(out);
    }

    drop(event);
    event->heapindex=-1;
    return event;
}

void cMessageHeap::putBackFirst(cEvent *event)
{
    take(event);

    CBDEC(cbhead);
    cb[cbhead] = event;
    event->heapindex = CBHEAPINDEX(cbhead);

    if (cbtail==cbhead)
        cbgrow();
}

NAMESPACE_END

