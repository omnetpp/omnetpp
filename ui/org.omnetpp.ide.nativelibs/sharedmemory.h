//=========================================================================
//  SHAREDMEMORY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, Tamas Borbely, Zoltan Bojthe
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2020 Andras Varga
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_SHAREDMEMORY_H
#define __OMNETPP_SCAVE_SHAREDMEMORY_H

#include <cstdint>

namespace omnetpp {

// macOS limitation
#define OPP_SHM_NAME_MAX 31

// fd's are int on POSIX, HANDLE's are void* on WIN32
// but intptr_t is not fixed size, so it might not be long in Java
typedef int64_t shmhandle_t;

// commit is ignored on POSIX systems (Linux, Darwin) because they can overcommit just fine
shmhandle_t createSharedMemory(const char *name, int64_t size, bool commit = true);
void *mapSharedMemory(const char *name, int64_t size);
void commitSharedMemory(void *start, int64_t size); // does not do anything on POSIX systems --TODO needCommit()?
void unmapSharedMemory(void *buffer, int64_t size);
void closeSharedMemory(shmhandle_t handle);
void removeSharedMemory(const char *name);

} // namespace omnetpp

#endif
