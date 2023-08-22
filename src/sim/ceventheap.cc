//=========================================================================
//  CEVENTHEAP.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cEventHeap : future event set, implemented as heap
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

//  Author: Andras Varga, based on the code from Gabor Lencse
//          (the original is taken from G. H. Gonnet's book pp. 273-274)

#include <cstdio>           // sprintf
#include <cstring>          // strlen
#include <cstdlib>          // qsort
#include <sstream>
#include "omnetpp/globals.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/ceventheap.h"

namespace omnetpp {

Register_Class(cEventHeap);

#define CBHEAPINDEX(i)    (-2-(i))
#define CBINC(i)          ((i) = ((i)+1)&(cbsize-1))
#define CBDEC(i)          ((i) = ((i)-1)&(cbsize-1))

inline bool operator>(cEvent& a, cEvent& b)
{
    return b.shouldPrecede(&a);
}

inline bool operator<=(cEvent& a, cEvent& b)
{
    return !(a > b);
}

static int qsort_cmp_msgs(const void *p1, const void *p2)
{
    cEvent *m1 = *(cEvent **)p1;
    cEvent *m2 = *(cEvent **)p2;
    return cEvent::compareBySchedulingOrder(m1, m2);
}

//----

cEventHeap::cEventHeap(const char *name, int intialCapacity) : cFutureEventSet(name),
    heapCapacity(intialCapacity)
{
    heap = new cEvent *[heapCapacity+1];  // +1 is necessary because h[0] is not used
    cb = new cEvent *[cbsize];
}

cEventHeap::cEventHeap(const cEventHeap& other) : cFutureEventSet(other)
{
    copy(other);
}

cEventHeap::~cEventHeap()
{
    clear();
    delete[] heap;
    delete[] cb;
}

std::string cEventHeap::str() const
{
    if (isEmpty())
        return std::string("empty");
    std::stringstream out;
    out << "length=" << getLength();
    return out.str();
}

void cEventHeap::forEachChild(cVisitor *v)
{
    sort();

    for (int i = cbhead; i != cbtail; CBINC(i))
        v->visit(cb[i]);

    for (int i = 1; i <= heapLength; i++)
        if (heap[i])
            if (!v->visit(heap[i]))
                return;
}

void cEventHeap::clear()
{
    for (int i = cbhead; i != cbtail; CBINC(i))
        dropAndDelete(cb[i]);
    cbhead = cbtail = 0;

    for (int i = 1; i <= heapLength; i++)
        dropAndDelete(heap[i]);
    heapLength = 0;
}

void cEventHeap::copy(const cEventHeap& other)
{
    // copy heap
    heapLength = other.heapLength;
    heapCapacity = other.heapCapacity;
    delete[] heap;
    heap = new cEvent *[heapCapacity+1];
    for (int i = 1; i <= heapLength; i++)
        take(heap[i] = other.heap[i]->dup());

    // copy circular buffer
    cbhead = other.cbhead;
    cbtail = other.cbtail;
    cbsize = other.cbsize;
    useCb = other.useCb;
    delete[] cb;
    cb = new cEvent *[cbsize];
    for (int i = cbhead; i != cbtail; CBINC(i))
        take(cb[i] = other.cb[i]->dup());
}

cEventHeap& cEventHeap::operator=(const cEventHeap& other)
{
    if (this == &other)
        return *this;
    cFutureEventSet::operator=(other);
    clear();
    copy(other);
    return *this;
}

cEvent *cEventHeap::get(int k)
{
    if (k < 0)
        return nullptr;

    // first few elements map into the circular buffer
    int cblen = cblength();
    if (k < cblen)
        return cbget(k);
    k -= cblen;

    // map the rest to h[1]..h[n] (h[] is 1-based)
    if (k >= heapLength)
        return nullptr;
    return heap[k+1];
}

void cEventHeap::sort()
{
    qsort(heap+1, heapLength, sizeof(cEvent *), qsort_cmp_msgs);
    for (int i = 1; i <= heapLength; i++)
        heap[i]->heapIndex = i;
}

void cEventHeap::insert(cEvent *event)
{
    take(event);

    event->insertOrder = insertCount++;

    if (!useCb) {
        heapInsert(event);
        return;
    }

    // is event eligible for putting it into the cb?
    bool eligible = false;
    simtime_t now = simTime();
    if (event->getArrivalTime() == now) {
        ASSERT(cbhead == cbtail || cb[cbhead]->getArrivalTime() == now); // causality violation
        if (event->getSchedulingPriority() == 0) {
            if (heapLength == 0 || heap[1]->getArrivalTime() > now)
                eligible = true;
        }
        else if (event->getSchedulingPriority() < 0)
            flushCb();  // move all events into the heap
    }

    if (eligible)
        cbInsert(event);
    else
        heapInsert(event);
}

void cEventHeap::cbInsert(cEvent *event)
{
    cb[cbtail] = event;
    event->heapIndex = CBHEAPINDEX(cbtail);
    CBINC(cbtail);
    if (cbtail == cbhead)
        cbgrow();
}

void cEventHeap::heapInsert(cEvent *event)
{
    if (++heapLength > heapCapacity) {
        heapCapacity *= 2;
        cEvent **newHeap = new cEvent *[heapCapacity+1];
        for (int i = 1; i <= heapLength-1; i++)
            newHeap[i] = heap[i];
        delete[] heap;
        heap = newHeap;
    }

    int i, j;
    for (j = heapLength; j > 1; j = i) {
        i = j>>1;
        if (*heap[i] <= *event)  // direction
            break;
        (heap[j] = heap[i])->heapIndex = j;
    }
    (heap[j] = event)->heapIndex = j;
}

void cEventHeap::cbgrow()
{
    int newsize = 2*cbsize;  // cbsize MUST be power of 2
    cEvent **newcb = new cEvent *[newsize];
    for (int i = 0; i < cbsize; i++)
        (newcb[i] = cb[(cbhead+i)&(cbsize-1)])->heapIndex = CBHEAPINDEX(i);
    delete[] cb;

    cb = newcb;
    cbhead = 0;
    cbtail = cbsize;
    cbsize = newsize;
}

void cEventHeap::flushCb()
{
    for (int i = cbhead; i != cbtail; CBINC(i))
        heapInsert(cb[i]);
    cbtail = cbhead;
}

void cEventHeap::shiftup(int from)
{
    // restores heap structure (in a sub-heap)
    int i, j;
    cEvent *temp;

    i = from;
    while ((j = i<<1) <= heapLength) {
        if (j < heapLength && (*heap[j] > *heap[j+1]))  // direction
            j++;
        if (*heap[i] > *heap[j]) {  // is change necessary?
            temp = heap[j];
            (heap[j] = heap[i])->heapIndex = j;
            (heap[i] = temp)->heapIndex = i;
            i = j;
        }
        else
            break;
    }
}

cEvent *cEventHeap::peekFirst() const
{
    return cbhead != cbtail ? cb[cbhead] : heapLength != 0 ? heap[1] : nullptr;
}

cEvent *cEventHeap::removeFirst()
{
    if (cbhead != cbtail) {
        // remove head element from circular buffer
        cEvent *event = cb[cbhead];
        CBINC(cbhead);
        drop(event);
        event->heapIndex = -1;
        return event;
    }
    else if (heapLength > 0) {
        // heap: first is taken out and replaced by the last one
        cEvent *event = heap[1];
        (heap[1] = heap[heapLength--])->heapIndex = 1;
        shiftup();
        drop(event);
        event->heapIndex = -1;
        return event;
    }
    return nullptr;
}

cEvent *cEventHeap::remove(cEvent *event)
{
    // make sure it is really on the heap
    if (event->heapIndex == -1)
        return nullptr;

    if (event->heapIndex < 0) {
        // event is in the circular buffer
        int i = -event->heapIndex-2;
        ASSERT(cb[i] == event);  // sanity check

        // remove
        int iminus1 = i;
        CBINC(i);
        for (  /**/; i != cbtail; iminus1 = i, CBINC(i))
            (cb[iminus1] = cb[i])->heapIndex = CBHEAPINDEX(iminus1);
        CBDEC(cbtail);
    }
    else {
        // event is on the heap

        // sanity check:
        // ASSERT(h[event->heapindex]==event);

        // last element will be used to fill the hole
        int father, out = event->heapIndex;
        cEvent *fill = heap[heapLength--];
        while ((father = out>>1) != 0 && *heap[father] > *fill) {
            (heap[out] = heap[father])->heapIndex = out;  // father is moved down
            out = father;
        }
        (heap[out] = fill)->heapIndex = out;
        shiftup(out);
    }

    drop(event);
    event->heapIndex = -1;
    return event;
}

void cEventHeap::putBackFirst(cEvent *event)
{
    take(event);

    CBDEC(cbhead);
    cb[cbhead] = event;
    event->heapIndex = CBHEAPINDEX(cbhead);

    if (cbtail == cbhead)
        cbgrow();
}

// like ASSERT(), but active in release mode as well
#define ENSURE(expr) \
  ((void) ((expr) ? 0 : (throw omnetpp::cRuntimeError("ENSURE(): Condition '%s' does not hold in function '%s' at %s:%d", \
                                   #expr, __FUNCTION__, __FILE__, __LINE__), 0)))

void cEventHeap::checkHeap()
{
    simtime_t now = simTime();
    ENSURE((cbsize & (cbsize-1)) == 0); // cbsize must be power of 2
    ENSURE(cbhead >= 0 && cbhead < cbsize && cbtail >= 0 && cbtail < cbsize);
    for (int i = cbhead; i < cbtail; CBINC(i)) {
        cEvent *event = cb[i];
        ENSURE(event->getOwner() == this);
        ENSURE(event->heapIndex == CBHEAPINDEX(i));
        ENSURE(event->getArrivalTime() == now);
        ENSURE(event->getSchedulingPriority() == 0);
    }

    for (int i = 1; i <= heapLength; i++) {
        cEvent *event = heap[i];
        ENSURE(event->getOwner() == this);
        ENSURE(event->heapIndex == i);
        ENSURE(event->getArrivalTime() >= now);
        if (i > 1) {
            cEvent *parent = heap[i>>1];
            ENSURE(*parent <= *event); // heap order property
        }
    }

    if (heapLength >= 1 && cbhead != cbtail)
        ENSURE(*cb[cbhead] <= *heap[1]);

}

}  // namespace omnetpp

