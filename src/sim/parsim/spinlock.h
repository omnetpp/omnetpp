//=========================================================================
//  SPINLOCK.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SPINLOCK_H
#define __OMNETPP_SPINLOCK_H

#include <thread>
#include <atomic>

namespace omnetpp {

/**
 * @brief Implements spin lock.
 */
class Spinlock
{
  private:
    std::atomic_flag lockFlag = ATOMIC_FLAG_INIT;

  public:
    void lock() {
        while (lockFlag.test_and_set(std::memory_order_acquire)) {
            // Spin without yielding or causing a context switch
        }
    }

    void unlock() {
        lockFlag.clear(std::memory_order_release);
    }
};

/**
 * @brief Condition variable for Spinlock. Note that no notify_one() /
 * notify_all() is needed, as the predicate determines readiness.
 */
class SpinConditionVariable
{
  public:
    template <typename Lock, typename Predicate>
    void wait(Lock& lock, Predicate predicate) {
        while (!predicate()) {
            // Unlock temporarily to allow other threads to acquire the lock
            lock.unlock();

            // Brief spin to avoid tight contention
            for (volatile int i = 0; i < 1000; ++i) { } // Lightweight spin delay

            // Reacquire the lock before rechecking the predicate
            lock.lock();
        }
    }
};

}  // namespace omnetpp

#endif
