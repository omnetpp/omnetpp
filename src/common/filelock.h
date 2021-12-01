//=========================================================================
//  FILELOCK.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FILELOCK_H_
#define __FILELOCK_H_

#include "omnetpp/platdep/platmisc.h"
#include "commondefs.h"

namespace omnetpp {
namespace common {

enum FileLockType
{
    FILE_LOCK_SHARED,
    FILE_LOCK_EXCLUSIVE
};

/**
 * This class provides a recursive almost portable file lock.
 * Unfortunatly Unix systems support both advisory and mandatory locking,
 * while Windows supports only mandatory locking. Therefore all processes
 * must use this locking mechanism properly to get correct portable file
 * locking.
 */
class COMMON_API FileLock
{
    private:
        FILE *file; // cannot be nullptr
        const char *fileName; // may be nullptr
        int counter; // 0 means unlocked
#ifdef _WIN32
        HANDLE handle; // os specific handle
#endif

    public:
        FileLock(FILE *file, const char *fileName);
        void lock(FileLockType fileLockType);
        void unlock(FileLockType fileLockType);
};

/**
 * This class provides an unwind safe way of grabbing and releasing a lock.
 */
class COMMON_API FileLockAcquirer
{
    private:
        bool enableLocking;
        FileLock *fileLock;
        FileLockType fileLockType;

    public:
        FileLockAcquirer(FileLock *fileLock, FileLockType fileLockType, bool enableLocking = true);
        ~FileLockAcquirer();
};

}  // namespace common
}  // namespace omnetpp

#endif
