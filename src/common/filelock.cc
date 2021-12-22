//=========================================================================
//  FILELOCK.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <cinttypes>
#include "filelock.h"
#include "exception.h"
#include "commonutil.h"

namespace omnetpp {
namespace common {

FileLock::FileLock(FILE *file, const char *fileName)
{
    Assert(file);
    this->file = file;
    this->fileName = fileName;
    this->counter = 0;
#ifdef _WIN32
    this->handle = (HANDLE)_get_osfhandle(fileno(file));
#endif
}

void FileLock::lock(FileLockType fileLockType)
{
    if (counter == 0) {
#ifdef _WIN32
        OVERLAPPED overlapped;
        overlapped.hEvent = 0;
        overlapped.Offset = overlapped.OffsetHigh = 0;
        overlapped.Internal = overlapped.InternalHigh = 0;
        if (LockFileEx(handle, fileLockType == FILE_LOCK_EXCLUSIVE ? LOCKFILE_EXCLUSIVE_LOCK : 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, &overlapped) == 0)
            throw opp_runtime_error("Cannot lock file '%s', error code %lu", fileName ? fileName : "", GetLastError());
#else
        struct flock flock;
        flock.l_type = fileLockType == FILE_LOCK_EXCLUSIVE ? F_WRLCK : F_RDLCK;
        flock.l_whence = SEEK_SET;
        flock.l_start = 0;
        flock.l_len = 0;
        flock.l_pid = getpid();
        if (fcntl(fileno(file), F_SETLKW, &flock) == -1)
            throw opp_runtime_error("Cannot lock file '%s', error code %d", fileName ? fileName : "", errno);
#endif
    }
    counter++;
}

void FileLock::unlock(FileLockType fileLockType)
{
    counter--;
    if (counter == 0) {
#ifdef _WIN32
        OVERLAPPED overlapped;
        overlapped.hEvent = 0;
        overlapped.Offset = overlapped.OffsetHigh = 0;
        overlapped.Internal = overlapped.InternalHigh = 0;
        if (UnlockFileEx(handle, 0, 0xFFFFFFFF, 0xFFFFFFFF, &overlapped) == 0)
            throw opp_runtime_error("Cannot unlock file '%s', error code %lu", fileName ? fileName : "", GetLastError());
#else
        struct flock flock;
        flock.l_type = F_UNLCK;
        flock.l_whence = SEEK_SET;
        flock.l_start = 0;
        flock.l_len = 0;
        flock.l_pid = getpid();
        if (fcntl(fileno(file), F_SETLKW, &flock) == -1)
            throw opp_runtime_error("Cannot unlock file '%s', error code %d", fileName ? fileName : "", errno);
#endif
    }
}

FileLockAcquirer::FileLockAcquirer(FileLock *fileLock, FileLockType fileLockType, bool enableLocking)
{
    Assert(fileLock);
    this->fileLock = fileLock;
    this->fileLockType = fileLockType;
    this->enableLocking = enableLocking;
    if (enableLocking)
        fileLock->lock(fileLockType);
}

FileLockAcquirer::~FileLockAcquirer()
{
    if (enableLocking)
        fileLock->unlock(fileLockType);
}

}  // namespace common
}  // namespace omnetpp
