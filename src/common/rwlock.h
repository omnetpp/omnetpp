//=========================================================================
//  RWLOCK.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_RWLOCK_H
#define __OMNETPP_COMMON_RWLOCK_H

#include <pthread.h>

#include "commondefs.h"

namespace omnetpp {
namespace common {

class COMMON_API ILock
{
    public:
        virtual ~ILock() {};
        virtual void lock() = 0;
        virtual bool tryLock() = 0;
        virtual void unlock() = 0;
        virtual bool hasLock() = 0;
};

class COMMON_API Mutex
{
    private:
        ILock &lock;

    public:
        Mutex(ILock &lock) : lock(lock) { lock.lock(); }
        Mutex(const ILock &lock) : lock(const_cast<ILock&>(lock)) { this->lock.lock(); }
        ~Mutex() { lock.unlock(); }
};

class COMMON_API IReadWriteLock
{
    public:
        virtual ~IReadWriteLock() {};
        virtual ILock& readLock() = 0;
        virtual ILock& writeLock() = 0;
};

class COMMON_API ReentrantReadWriteLock : public IReadWriteLock
{
    private:
        struct ThreadLocalState
        {
            int readLockCount = 0;
            ThreadLocalState() {}
        };

        pthread_rwlock_t rwlock;

        // FIXME a new created for each lock, so the limit (128) can be reached easily
        pthread_key_t key;
        pthread_t writerThread;
        int writeLockCount;

        ThreadLocalState& getThreadLocalState();
        static void deleteThreadLocalState(void *tls);

        void lockRead();
        bool tryLockRead();
        void unlockRead();
        bool hasReadLock();
        void lockWrite();
        bool tryLockWrite();
        void unlockWrite();
        bool hasWriteLock();

        class ReadLock : public ILock
        {
            private:
                ReentrantReadWriteLock &rwl;
            public:
                ReadLock(ReentrantReadWriteLock &rwl) : rwl(rwl) {}
                virtual void lock() { rwl.lockRead(); }
                virtual bool tryLock() { return rwl.tryLockRead(); }
                virtual void unlock() { rwl.unlockRead(); }
                virtual bool hasLock() { return rwl.hasReadLock(); }
        };

        class WriteLock : public ILock
        {
            private:
                ReentrantReadWriteLock &rwl;
            public:
                WriteLock(ReentrantReadWriteLock &rwl) : rwl(rwl) {}
                virtual void lock() { rwl.lockWrite(); }
                virtual bool tryLock() { return rwl.tryLockWrite(); }
                virtual void unlock() { rwl.unlockWrite(); }
                virtual bool hasLock() { return rwl.hasWriteLock(); }
        };

        ReadLock _readLock;
        WriteLock _writeLock;
    public:
        ReentrantReadWriteLock();
        ~ReentrantReadWriteLock();
        ILock& readLock() { return _readLock; }
        ILock& writeLock() { return _writeLock; }
};

}  // namespace common
}  // namespace omnetpp


#endif


