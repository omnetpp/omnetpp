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
#include <bitset> // only for an ASSERT

namespace omnetpp {
namespace qtenv {

/**
 * A limited STL-like circular buffer implementation.
 */
template <typename T>
class QTENV_API circular_buffer
{
    private:
        T *storage;
        int capacity;  // size of storage, always a power of 2
        int head = 0;  // index of the first element in storage (inclusive)
        int tail = 0;  // index after the last element in storage (exclusive)

    private:
        inline int wrapIndex(int index) const {
            // This is a % in disguise, and it works because capacity is always a power of 2.
            return index & (capacity - 1);
        }
        void grow() { grow(capacity * 2); }
        void grow(int newCapacity) {
            int oldSize = size(); // this is valid element count
            ASSERT(newCapacity > capacity); // this is backing buffer capacity
            ASSERT(std::bitset<sizeof(int)*8>(newCapacity).count() == 1); // must be a power of 2
            T *newStorage = new T[newCapacity];

            // There is no std::move_n, but adding std::make_move_iterator achieves the same.
            if (head < tail)
                // Current contents are contiguous, one copy is enough.
                std::copy_n(std::make_move_iterator(storage + head), tail - head, newStorage);
            else if (head > tail) {
                // Copy the first part of the range (at the end of the current buffer)
                // to the beginning of the new one.
                std::copy_n(std::make_move_iterator(storage + head), capacity - head, newStorage);
                // Then copy the rest (at the beginning of the current buffer) after them.
                std::copy_n(std::make_move_iterator(storage), tail, newStorage + (capacity - head));
            } // else: empty buffer, nothing to copy

            delete[] storage;
            storage = newStorage;

            head = 0;
            tail = oldSize;
            capacity = newCapacity;
        }

        // unimplemented:
        circular_buffer(const circular_buffer&) = delete;
        void operator=(const circular_buffer&) = delete;

    public:
        class iterator : public std::iterator<std::random_access_iterator_tag, T, int> {
            private:
                circular_buffer<T> *buffer;
                // This is an ordinary index, as seen from the outside, not one into the internal
                // storage - the wraparound logic is handled entirely by the buffer.
                int position;

            public:
                iterator(circular_buffer<T> *buf, int pos) : buffer(buf), position(pos) {}
                T& operator*() const {
                    return (*buffer)[position];
                }
                iterator& operator++() {
                    ++position;
                    return *this;
                }
                iterator operator++(int) {
                    iterator tmp(*this);
                    ++position;
                    return tmp;
                }
                iterator& operator--() {
                    --position;
                    return *this;
                }
                iterator operator--(int) {
                    iterator tmp(*this);
                    --position;
                    return tmp;
                }
                iterator& operator+=(int n) {
                    position += n;
                    return *this;
                }
                iterator operator+(int n) const {
                    iterator tmp(*this);
                    tmp += n;
                    return tmp;
                }
                iterator& operator-=(int n) {
                    position -= n;
                    return *this;
                }
                iterator operator-(int n) const {
                    iterator tmp(*this);
                    tmp -= n;
                    return tmp;
                }
                int operator-(const iterator& other) const {
                    return position - other.position;
                }
                T& operator[](int n) const {
                    return (*buffer)[position + n];
                }
                bool operator==(const iterator& other) const {
                    return buffer == other.buffer && position == other.position;
                }
                bool operator!=(const iterator& other) const {
                    return buffer != other.buffer || position != other.position;
                }
        };

        circular_buffer(int capacity=32) :
            storage(new T[capacity]), capacity(capacity) { }
        ~circular_buffer() {
            delete[] storage;
        }
        void push_back(T d) {
            // can't let it get full, it would look empty
            if (wrapIndex(tail + 1) == head)
                grow();
            storage[tail] = d;
            tail = wrapIndex(tail + 1);
        }
        void pop_front() {
            ASSERT(head != tail);
            head = wrapIndex(head + 1);
        }
        void pop_front(int n) {
            ASSERT(size() >= n);
            head = wrapIndex(head + n);
        }
        T& front() const {
            ASSERT(head != tail);
            return storage[head];
        }
        T& back() const {
            ASSERT(head != tail);
            return storage[wrapIndex(tail - 1)];
        }
        T& operator[](int i) const {
            ASSERT(size() > i);
            return storage[wrapIndex(head + i)];
        }
        int size() const {
            return wrapIndex(tail - head);
        }
        void resize(int newSize) {
            if (newSize < size())
                // shrinking is trivial
                tail = wrapIndex(head + newSize);
            else if (newSize > size()) {
                // growing, check if need to reallocate buffer
                if (newSize >= capacity) { // new size must be less than (not equal) to cbsize, because full and empty buffer would be indistinguishable
                    int minCapacity = capacity;
                    // can't allocate to exactly the new size because there's no way
                    // to tell apart full and empty buffer
                    while (minCapacity <= newSize)
                        minCapacity *= 2;
                    // reallocates unconditionally, only copies valid
                    // elements, keeps the rest default-constructed
                    grow(minCapacity);
                }
                else {
                    // no need to reallocate, but need to fill with default
                    for (int i = size(); i < newSize; i++)
                        storage[wrapIndex(head + i)] = T();
                }
                tail = wrapIndex(head + newSize);
            }
            ASSERT(size() == newSize);
        }
        // Set the size of the buffer, reallocating if necessary,
        // leaving its contents undefined. Caller is responsible for
        // assigning a valid new value to all values in the returned
        // pointer all through [0] to [count-1] (inclusive).
        T *prepare_fill(int count) {
            int minCapacity = 32;
            // can't allocate to exactly the new size because there's no way
            // to tell apart full and empty buffer
            while (minCapacity <= count)
                minCapacity *= 2;
            if (capacity < minCapacity) {
                delete[] storage;
                storage = new T[minCapacity];
                capacity = minCapacity;
            }
            head = 0;
            tail = count;
            return storage;
        }
        bool empty() const {
            return head == tail;
        }
        void clear() {
            head = tail = 0;
        }
        iterator begin() {
            return iterator(this, 0);
        }
        iterator end() {
            return iterator(this, size());
        }
};

}  // namespace qtenv
}  // namespace omnetpp

#endif

