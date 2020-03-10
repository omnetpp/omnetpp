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
#define OPP_SHM_NAME_MAX 32

// commit is ignored on POSIX systems (Linux, Darwin) because they can overcommit just fine
void createSharedMemory(const char *name, int64_t size, bool commit = true);
void *mapSharedMemory(const char *name, int64_t size);
void commitSharedMemory(void *start, int64_t size); // does not do anything on POSIX systems
void unmapSharedMemory(void *buffer, int64_t size);
void removeSharedMemory(const char *name);

} // namespace omnetpp

#endif
