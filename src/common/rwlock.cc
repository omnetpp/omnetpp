//==========================================================================
//  RWLOCK.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "errno.h"
#include "rwlock.h"
#include "exception.h"

namespace omnetpp {
namespace common {

#ifdef _WIN32
#ifdef PTW32_STATIC_LIB
// required only if the pthread library is used as a static library
class PThreadInit
{
  public:
    PThreadInit() { pthread_win32_process_attach_np(); }
    ~PThreadInit() { pthread_win32_process_detach_np(); }
};

static PThreadInit dummy;
#endif
#endif

void ReentrantReadWriteLock::deleteThreadLocalState(void *tls)
{
    if (tls) {
        delete (ThreadLocalState *)tls;
    }
}

inline void handleErrors(int rc, const char *msg)
{
    switch (rc) {
        case 0: return;
        case EAGAIN: throw opp_runtime_error("%s (resource limit exceeded).", msg);
        case EINVAL: throw opp_runtime_error("%s (invalid argument error).", msg);
        case ENOMEM: throw opp_runtime_error("%s (out of memory).", msg);
        default: throw opp_runtime_error("%s (unknown error, errno = %d).", msg, rc);
    }
}

ReentrantReadWriteLock::ReentrantReadWriteLock()
    : _readLock(*this), _writeLock(*this)
{
    pthread_rwlock_t tmp = PTHREAD_RWLOCK_INITIALIZER;
    rwlock = tmp;
    int rc = pthread_key_create(&key, deleteThreadLocalState);
    handleErrors(rc, "Can not create key for thread local storage");
}

ReentrantReadWriteLock::~ReentrantReadWriteLock()
{
    pthread_rwlock_destroy(&rwlock);
    pthread_key_delete(key);
}

ReentrantReadWriteLock::ThreadLocalState& ReentrantReadWriteLock::getThreadLocalState()
{
    ThreadLocalState *tls = (ThreadLocalState *)pthread_getspecific(key);
    if (!tls) {
        tls = new ThreadLocalState();
        int rc = pthread_setspecific(key, tls);
        if (rc) {
            delete tls;
            handleErrors(rc, "Can not set thread local data");
        }
    }

    return *tls;
}

void ReentrantReadWriteLock::lockRead()
{
    ThreadLocalState& tls = getThreadLocalState();

    if (tls.readLockCount > 0) {
        ++tls.readLockCount;
        return;
    }

    if (writeLockCount > 0 && pthread_equal(pthread_self(), writerThread)) {
        ++tls.readLockCount;
        return;
    }

    int rc = pthread_rwlock_rdlock(&rwlock);
    handleErrors(rc, "Can not acquire read lock.");

    ++tls.readLockCount;
}

bool ReentrantReadWriteLock::tryLockRead()
{
    ThreadLocalState& tls = getThreadLocalState();

    if (tls.readLockCount > 0) {
        ++tls.readLockCount;
        return true;
    }

    if (writeLockCount > 0 && pthread_equal(pthread_self(), writerThread)) {
        ++tls.readLockCount;
        return true;
    }

    int rc = pthread_rwlock_tryrdlock(&rwlock);
    switch (rc) {
        case 0: ++tls.readLockCount; return true;
        case EBUSY: return false;
        default: handleErrors(rc, "Error in tryLockRead()"); return false;
    }
}

bool ReentrantReadWriteLock::hasReadLock()
{
    ThreadLocalState& tls = getThreadLocalState();
    return tls.readLockCount > 0;
}

void ReentrantReadWriteLock::lockWrite()
{
    int rc;

    if (writeLockCount > 0 && pthread_equal(pthread_self(), writerThread)) {
        ++writeLockCount;
        return;
    }

    ThreadLocalState& tls = getThreadLocalState();
    if (tls.readLockCount > 0) {
        // unlock the reader, and lock it again when the write unlocked and readerLockCount > 0
        rc = pthread_rwlock_unlock(&rwlock);
        handleErrors(rc, "Can not release read lock before acquiring write lock");
    }

    rc = pthread_rwlock_wrlock(&rwlock);
    handleErrors(rc, "Can not acquire write lock");

    writerThread = pthread_self();
    writeLockCount = 1;
}

bool ReentrantReadWriteLock::tryLockWrite()
{
    int rc;

    if (writeLockCount > 0 && pthread_equal(pthread_self(), writerThread)) {
        ++writeLockCount;
        return true;
    }

    ThreadLocalState& tls = getThreadLocalState();
    if (tls.readLockCount > 0) {
        // unlock the reader, and lock it again when the write unlocked and readerLockCount > 0
        rc = pthread_rwlock_unlock(&rwlock);
        handleErrors(rc, "Can not release read lock before acquiring write lock");
    }

    rc = pthread_rwlock_trywrlock(&rwlock);
    switch (rc) {
        case 0:
            writerThread = pthread_self();
            writeLockCount = 1;
            return true;

        case EBUSY:
            if (tls.readLockCount > 0) {
                // FIXME we might loose the read lock as a result of tryLockWrite()
                rc = pthread_rwlock_rdlock(&rwlock);
                handleErrors(rc, "Can not reacquire read lock in tryLockWrite()");
            }
            return false;

        default:
            handleErrors(rc, "Error in tryLockWrite()");
            return false;  // unreachable
    }
}

bool ReentrantReadWriteLock::hasWriteLock()
{
    return writeLockCount > 0 && pthread_equal(pthread_self(), writerThread);
}

void ReentrantReadWriteLock::unlockRead()
{
    ThreadLocalState& tls = getThreadLocalState();

    if (tls.readLockCount == 0)
        throw opp_runtime_error("Missing lockRead() call");

    --tls.readLockCount;

    if (writeLockCount > 0 && pthread_equal(pthread_self(), writerThread))
        return;

    if (tls.readLockCount == 0) {
        int rc = pthread_rwlock_unlock(&rwlock);
        handleErrors(rc, "Can not release read lock");
    }
}

void ReentrantReadWriteLock::unlockWrite()
{
    if (writeLockCount == 0 || !pthread_equal(pthread_self(), writerThread))
        throw opp_runtime_error("Missing lockWrite() call");

    --writeLockCount;

    if (writeLockCount == 0) {
        // release write lock
        int rc = pthread_rwlock_unlock(&rwlock);
        handleErrors(rc, "Can not release write lock");

        // reacquire read lock when needed
        ThreadLocalState& tls = getThreadLocalState();
        if (tls.readLockCount > 0) {
            rc = pthread_rwlock_rdlock(&rwlock);
            handleErrors(rc, "Can not reacquire read lock after releasing write lock");
        }
    }
}

}  // namespace common
}  // namespace omnetpp

