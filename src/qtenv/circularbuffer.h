//==========================================================================
//  CIRCULARBUFFER.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_CIRCULARBUFFER_H
#define __OMNETPP_QTENV_CIRCULARBUFFER_H

#include "omnetpp/simkerneldefs.h" // for ASSERT
#include "omnetpp/cexception.h"
#include "qtenvdefs.h"

namespace omnetpp {
namespace qtenv {

/**
 * A limited STL-like circular buffer implementation.
 */
template <typename T>
class QTENV_API circular_buffer
{
    private:
        T *cb;           // size of the circular buffer
        int cbsize;      // always power of 2
        int cbhead = 0;  // inclusive
        int cbtail = 0;  // exclusive

    private:
        void grow() {
            ASSERT(cbhead == cbtail); // here it means full, not empty

            int newsize = 2*cbsize;
            T *newcb = new T[newsize];

            std::copy_n(cb + cbhead, cbsize - cbhead, newcb);
            std::copy_n(cb, cbtail, newcb + (cbsize - cbhead));

            delete[] cb;
            cb = newcb;

            cbhead = 0;
            cbtail = cbsize;
            cbsize = newsize;
        }

        // unimplemented:
        circular_buffer(const circular_buffer&);
        void operator=(const circular_buffer&);

    public:
        circular_buffer(int capacity=32) :
            cb(new T[capacity]), cbsize(capacity) { }
        ~circular_buffer() {
            delete[] cb;
        }
        void push_back(T d) {
            cb[cbtail] = d;
            cbtail = (cbtail + 1) & (cbsize-1);
            if (cbtail == cbhead)
                grow();
        }
        void pop_front() {
            ASSERT(cbhead != cbtail);
            cbhead = (cbhead + 1) & (cbsize-1);
        }
        T front() const {
            ASSERT(cbhead != cbtail);
            return cb[cbhead];
        }
        T back() const {
            ASSERT(cbhead != cbtail);
            return cb[(cbtail - 1) & (cbsize-1)];
        }
        T operator[](int i) const {
            ASSERT(cbhead != cbtail);
            return cb[(cbhead + i) & (cbsize-1)];
        }
        int size() const {
            return (cbtail - cbhead) & (cbsize-1);
        }
        bool empty() const {
            return cbhead == cbtail;
        }
        void clear() {
            cbhead = cbtail = 0;
        }
};

}  // namespace qtenv
}  // namespace omnetpp

#endif

