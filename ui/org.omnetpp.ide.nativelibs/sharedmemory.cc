//=========================================================================
//  SHAREDMEMORY.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2019 Andras Varga
  Copyright (C) 2006-2019 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "sharedmemory.h"

#include <stdexcept>
#include <string>

extern "C" {
#if defined(__linux__)
  #include <malloc.h>
  #include <sys/sysinfo.h>
#elif defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #define NOGDI
  #include <windows.h>
#elif defined(__APPLE__)
  #include <sys/types.h>
  #include <sys/sysctl.h>
  #include <mach/mach.h>
#endif

#if !defined(_WIN32)
  #include <unistd.h>
  #include <errno.h>
  #include <sys/mman.h>
  #include <sys/stat.h>        /* For mode constants */
  #include <fcntl.h>           /* For O_* constants */
#endif

#include "string.h"
}

namespace omnetpp {

void createSharedMemory(const char *name, int64_t size, bool commit)
{
    #if defined(_WIN32)

        HANDLE hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE,      // use paging file
            NULL,                      // default security
            PAGE_READWRITE | (commit ? SEC_COMMIT : SEC_RESERVE),
            (size >> 32) & 0xFFFFFFFF, // maximum object size (high-order DWORD)
            size & 0xFFFFFFFF,         // maximum object size (low-order DWORD)
            name);                     // name of mapping object

        if (hMapFile == NULL) {
            char err[2048];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 2048, NULL);
            throw std::runtime_error(std::string("Error creating shared memory file mapping '") + name + "': " + err);
        }

    #else // POSIX (linux, mac)

        int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        if (fd == -1)
            throw std::runtime_error(std::string("Error opening SHM file descriptor for '") + name + "': " + strerror(errno));

        if (ftruncate(fd, size) == -1)
            throw std::runtime_error(std::string("Error setting SHM file descriptor to size ") + std::to_string(size) + " for '" + name + "': " + strerror(errno));

        if (close(fd) == -1)
            throw std::runtime_error(std::string("Error closing SHM file descriptor for '") + name + "': " + strerror(errno));

    #endif
}

void *mapSharedMemory(const char *name, int64_t size)
{
    // must be set by the platform specific fragments
    void *buffer = nullptr;

#if defined(_WIN32)

    // this did NOT work: HANDLE opened = OpenFileMapping(PAGE_READWRITE, FALSE, name);

    HANDLE hMapFile = CreateFileMapping(
      INVALID_HANDLE_VALUE,    // use paging file
      NULL,                    // default security
      PAGE_READWRITE,          // read/write access
      0,                       // maximum object size (high-order DWORD)
      size,                    // maximum object size (low-order DWORD) - ignored, but must not be 0
      name);                   // name of mapping object

    if (GetLastError() != ERROR_ALREADY_EXISTS)
        throw std::runtime_error(std::string("Trying to map a nonexistent shared memory mapping '") + name + "'");

    if (hMapFile == NULL) {
        char err[2048];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 2048, NULL);
        throw std::runtime_error(std::string("Error opening shared memory file mapping '") + name + "': " + err);
    }

    buffer = MapViewOfFile(
      hMapFile,   // handle to map object
      FILE_MAP_ALL_ACCESS, // read/write permission
      0, // no offset (high)
      0, // no offset (low)
      0); // no size, map the whole object

    if (buffer == NULL) {
        char err[2048];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 2048, NULL);
        throw std::runtime_error(std::string("Error mapping view of file '") + name + "': " + err);
    }

#else // POSIX (linux, mac)

    int fd = shm_open(name, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
        throw std::runtime_error(std::string("Error opening SHM file descriptor '") + name + "': " + strerror(errno));

    buffer = mmap(0, size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (buffer == MAP_FAILED)
        throw std::runtime_error(std::string("Error mmap-ing SHM file descriptor '") + name + "': " + strerror(errno));

    if (close(fd) == -1)
        throw std::runtime_error(std::string("Error closing SHM file descriptor '") + name + "': " + strerror(errno));

#endif

    return buffer;
}


void commitSharedMemory(void *start, int64_t size)
{
    #if defined(_WIN32)
        VirtualAlloc(
            start,
            size,
            MEM_COMMIT,
            PAGE_READWRITE
        );
    #else // POSIX (linux, mac)
    // no-op, we are always overcommitting anyways
    #endif
}


void unmapSharedMemory(void *buffer, int64_t size)
{
    #if defined(_WIN32)
        if (UnmapViewOfFile(buffer) == 0) {
            char err[2048];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 2048, NULL);
            throw std::runtime_error(std::string("Error unmapping file view of size ") + std::to_string(size) + ": " + err);
        }
    #else // POSIX (linux, mac)
        if (munmap(buffer, size) == -1)
            throw std::runtime_error(std::string("Error unmapping SHM buffer of size ") + std::to_string(size) + ": " + strerror(errno));
    #endif
}


void removeSharedMemory(const char *name)
{
    #if defined(_WIN32)

        HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    // use paging file
        NULL,                    // default security
        PAGE_READWRITE,          // read/write access
        0,                       // maximum object size (high-order DWORD)
        1,                       // maximum object size (low-order DWORD) - ignored, but must not be 0
        name);                   // name of mapping object

        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            char err[2048];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 2048, NULL);
            throw std::runtime_error(std::string("Trying to remove a nonexistent mapping '") + name + "': " + err);
        }

        if (CloseHandle(hMapFile) == 0) {
            char err[2048];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 2048, NULL);
            throw std::runtime_error(std::string("Error closing file mapping handle '") + name + "': " + err);
        }

    #else // POSIX (linux, mac)
        if (shm_unlink(name) == -1)
            throw std::runtime_error(std::string("Error unlinking SHM object '") + name + "': " + strerror(errno));
    #endif
}

} // namespace omnetpp
