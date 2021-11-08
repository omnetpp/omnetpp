//=========================================================================
//  SHMMANAGER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Attila Torok
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2020 Andras Varga
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SHMMANAGER_H
#define __OMNETPP_SHMMANAGER_H

#include <string>
#include <vector>
#include <memory>
#include "common/rwlock.h"
#include "sharedmemory.h"

// Uncomment this to turn on ShmSendBuffer instance count tracking and additional debug output.
//#define SHMSENDBUFFER_DEBUG 1

namespace omnetpp {
namespace scave {

class ShmSendBufferManager;

/**
 * Sharedmemory-based buffer for sending data from the IDE to a Python process.
 *
 * ShmSendBuffer objects are created and kept track of by a ShmSendBufferManager instance.
 * We keep one ShmSendBufferManager per Python process.
 *
 * Shmems are disposed of in one of the following ways:
 *
 *  - When the Python process has read one and no longer needs its contents,
 *    it indicates this by writing a 1-byte at the start of the shmem.
 *    ** ShmSendBuffer reserves an 8-byte header for this purpose. **
 *
 *  - ShmSendBufferManager::garbageCollect() needs to be called periodically;
 *    it deallocates ShmSendBuffers that have been consumed by the Python
 *    process.
 *
 *  - When a new ShmSendBuffer is allocated, we also run garbageCollect().
 *
 *  - When the receiver Python process dies, we clean up all ShmSendBuffers
 *    by calling clear() on ShmSendBufferManager.
 */
class ShmSendBuffer
{
    friend class ShmSendBufferManager;
private:
#ifdef SHMSENDBUFFER_DEBUG
    static int numInstances;
#endif
    std::string name;
    void *mappedStart; // points to the header
    size_t reservedSize; // excluding header
    size_t committedSize; // excluding header
    shmhandle_t handle;
protected:
    ShmSendBuffer(const std::string& fullName, size_t commitSize, bool extendable);
    ShmSendBuffer(const ShmSendBuffer&) = delete;
    ShmSendBuffer(ShmSendBuffer&&) = delete;
    void operator=(const ShmSendBuffer&) = delete;
    void operator=(ShmSendBuffer&&) = delete;
public:
    ~ShmSendBuffer();
    void *getAddress() const; // points after the header
    const char *getName() const {return name.c_str();}
    size_t getDataSize() const { return committedSize; } // excluding header
    std::string getNameAndTotalSize() const; // including header
    void extendTo(size_t newSize); // excluding header
    bool isConsumed() const;
    std::vector<int8_t> getContentCopy() const;
};

/**
 * This class acts as a factory for, and keeps track of, ShmSendBuffer objects.
 */
class ShmSendBufferManager
{
    friend class ShmSendBuffer;

protected:
    int targetPid;  // TODO unused for now
    std::vector<std::shared_ptr<ShmSendBuffer>> buffers;  // disposed elements are nullptr
    common::ReentrantReadWriteLock lock;

public:
    ShmSendBufferManager(int targetPid) : targetPid(targetPid) {}
    ~ShmSendBufferManager();

    // create and map send buffer
    std::shared_ptr<ShmSendBuffer> create(const char *label, size_t commitSize, bool extendable);

    // create with preexisting content
    std::shared_ptr<ShmSendBuffer> create(const char *label, const std::vector<int8_t>& content);

    // unmaps and releases all send buffers
    void clear();

    // release send buffers consumed by the target process
    bool garbageCollect();
};

}  // namespace scave
}  // namespace omnetpp

#endif // __OMNETPP_SHMMANAGER_H
