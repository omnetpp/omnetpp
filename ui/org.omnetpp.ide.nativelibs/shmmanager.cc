//=========================================================================
//  SHMMANAGER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2019 Andras Varga
  Copyright (C) 2006-2019 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "shmmanager.h"
#include <cstring>
#include "omnetpp/platdep/platmisc.h" // getpid()
#include "common/stlutil.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

#define HEADERSIZE 8
#define RESERVESIZE ((1ull<<31)-1-HEADERSIZE)  // 2G (on macOS, size is int32_t)

ShmSendBuffer::ShmSendBuffer(ShmSendBufferManager *owner, const std::string& fullName, size_t commitSize, bool extendable) :
    owner(owner), name(fullName), committedSize(commitSize)
{
    reservedSize = extendable ? RESERVESIZE : commitSize;
    handle = createSharedMemory(name.c_str(), reservedSize + HEADERSIZE, false);
    mappedStart = mapSharedMemory(name.c_str(), reservedSize + HEADERSIZE);
    commitSharedMemory(mappedStart, committedSize + HEADERSIZE);
    memset(mappedStart, 0, HEADERSIZE);
}

ShmSendBuffer::~ShmSendBuffer()
{
    unmapSharedMemory(mappedStart, reservedSize + HEADERSIZE);
    closeSharedMemory(handle);
    removeSharedMemory(name.c_str());
    owner->deleted(this);
}

void *ShmSendBuffer::getAddress() const
{
    return (char *)mappedStart + HEADERSIZE;
}

void ShmSendBuffer::extendTo(size_t newSize)
{
    Assert(newSize <= reservedSize);
    if (committedSize >= newSize)
        return;
    size_t increment = newSize - committedSize;
    commitSharedMemory((char *)mappedStart + HEADERSIZE + committedSize, increment);
    committedSize = newSize;
}

std::string ShmSendBuffer::getNameAndSize() const
{
    return name + " " + std::to_string(committedSize);
}

bool ShmSendBuffer::isConsumed() const
{
    return *(char *)mappedStart != 0;  // target process changed it from zero
}

//----

ShmSendBufferManager::~ShmSendBufferManager()
{
    Mutex mutex(lock.writeLock());
    for (ShmSendBuffer *p : buffers) {
        if (p && !p->isConsumed())
            std::cerr << "ShmSendBufferManager: unconsumed send buffer found in destructor! call clear() explicitly if this is normal\n";
        delete p;
    }
}

ShmSendBuffer *ShmSendBufferManager::create(const char *label, size_t commitSize, bool extendable)
{
    garbageCollect(); // when a new Python->IDE call occurs, there's a high chance that buffers from earlier calls are not longer needed -> GC them

    Mutex mutex(lock.writeLock());

    static int counter = 0;
    char name[OPP_SHM_NAME_MAX];
    snprintf(name, OPP_SHM_NAME_MAX, "opp-%ld-%d-%s", getpid()%1000000L, ++counter, label);
    ShmSendBuffer *result = new ShmSendBuffer(this, name, commitSize, extendable);

    buffers.push_back(result);
    return result;
}

void ShmSendBufferManager::deleted(ShmSendBuffer *p)
{
    auto it = find(buffers, p);
    assert(it != buffers.end());
    *it = nullptr;
}

void ShmSendBufferManager::clear()
{
    Mutex mutex(lock.writeLock());
    for (ShmSendBuffer *p : buffers)
        delete p; // ptr will be set to nullptr in deleted()
}

void ShmSendBufferManager::garbageCollect()
{
    Mutex mutex(lock.writeLock());
    for (ShmSendBuffer *&p : buffers)
        if (p && p->isConsumed())
            delete p; // ptr will be set to nullptr in deleted()
}

} // namespace scave
} // namespace omnetpp
