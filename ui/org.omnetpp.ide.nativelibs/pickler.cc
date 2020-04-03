//=========================================================================
//  PICKLER.CC - part of
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

// This file was "inspired by" torch/csrc/jit/serialization/pickler.cpp in PyTorch

#include "pickler.h"

#include <string>
#include "scave/memoryutils.h" // getAvailableMemoryBytes()
#include "omnetpp/platdep/platmisc.h" // getpid()

namespace omnetpp {

// Protocol 2 is the highest that can be decoded by Python 2
// See https://docs.python.org/3/library/pickle.html#data-stream-format
constexpr static uint8_t PROTOCOL_VERSION = 2;

void Pickler::protocol()
{
    push<PickleOpCode>(PickleOpCode::PROTO);
    push<uint8_t>(PROTOCOL_VERSION);
}

void Pickler::startTuple()
{
    // All attributes get pushed into a tuple and their indices saved in the
    // module def
    push<PickleOpCode>(PickleOpCode::MARK);
}

void Pickler::endTuple()
{
    push<PickleOpCode>(PickleOpCode::TUPLE);
}

void Pickler::startList()
{
    // All elements get appended into a list and their indices saved in the
    // module def
    push<PickleOpCode>(PickleOpCode::EMPTY_LIST);
    push<PickleOpCode>(PickleOpCode::MARK);
}

void Pickler::endList()
{
    push<PickleOpCode>(PickleOpCode::APPENDS);
}

void Pickler::stop()
{
    push<PickleOpCode>(PickleOpCode::STOP);
    flush();
}

void Pickler::pushInt(int64_t n)
{
    if (
      n >= std::numeric_limits<uint8_t>::min() &&
      n <= std::numeric_limits<uint8_t>::max()) {
        push<PickleOpCode>(PickleOpCode::BININT1);
        push<uint8_t>(n);
    }
    else if (
      n >= std::numeric_limits<uint16_t>::min() &&
      n <= std::numeric_limits<uint16_t>::max()) {
        push<PickleOpCode>(PickleOpCode::BININT2);
        push<uint16_t>(n);
    }
    else if (
      n >= std::numeric_limits<int32_t>::min() &&
      n <= std::numeric_limits<int32_t>::max()) {
        push<PickleOpCode>(PickleOpCode::BININT);
        push<int32_t>(n);
    }
    else {
        // Push 8 byte integer
        push<PickleOpCode>(PickleOpCode::LONG1);
        push<uint8_t>(8);
        push<int64_t>(n);
    }
}

void Pickler::pushBool(bool value)
{
    push<PickleOpCode>(value ? PickleOpCode::NEWTRUE : PickleOpCode::NEWFALSE);
}

void Pickler::pushBinGet(uint32_t memo_id)
{
    if (memo_id <= std::numeric_limits<uint8_t>::max()) {
        push<PickleOpCode>(PickleOpCode::BINGET);
        push<uint8_t>(memo_id);
    }
    else {
        // Memoized too many items, issue a LONG_BINGET instead
        push<PickleOpCode>(PickleOpCode::LONG_BINGET);
        push<uint32_t>(memo_id);
    }
}

// unmemoized encoding of a string
void Pickler::pushStringImpl(const std::string& string)
{
    push<PickleOpCode>(PickleOpCode::BINUNICODE);
    push<uint32_t>(string.size());
    pushBytes(string);
}

void Pickler::pushString(const std::string& string)
{
    auto it = memoizedStrings.find(string);
    if (it == memoizedStrings.end()) {
        pushStringImpl(string);
        memoizedStrings[string] = pushNextBinPut();
    }
    else
        pushBinGet(it->second);
}

void Pickler::pushNone()
{
    push<PickleOpCode>(PickleOpCode::NONE);
}

void Pickler::pushBytes(const std::string& string)
{
    static const size_t kSmallStr = 32;
    if (string.size() <= kSmallStr &&
        bufferPos + string.size() <= buffer.size()) {
        // Small string that fits: buffer the data.
        memcpy(buffer.data() + bufferPos, string.data(), string.size());
        bufferPos += string.size();
    }
    else {
        // Otherwise, first flush, then write directly.
        flush();
        writerFunc(string.data(), string.size());
    }
}

static inline double swapDouble(double value)
{
    const char* bytes = reinterpret_cast<const char*>(&value);
    double flipped;
    char* out_bytes = reinterpret_cast<char*>(&flipped);
    for (size_t i = 0; i < sizeof(double); ++i)
        out_bytes[i] = bytes[sizeof(double) - i - 1];

    return *reinterpret_cast<double*>(out_bytes);
}

void Pickler::pushDouble(double value)
{
    push<PickleOpCode>(PickleOpCode::BINFLOAT);
    // Python pickle format is big endian, swap.
    push<double>(swapDouble(value));
}

void Pickler::pushDoublesAsRawBytes(const std::vector<double>& values)
{
    // number of bytes
    int s = values.size() * 8;

    push<PickleOpCode>(PickleOpCode::BINBYTES);
    push<int32_t>(s);

    flush();
    writerFunc((char*)values.data(), s);
}

void Pickler::pushEmptyDict()
{
    push<PickleOpCode>(PickleOpCode::EMPTY_DICT);
}

size_t Pickler::pushNextBinPut()
{
    if (nextMemoId <= std::numeric_limits<uint8_t>::max()) {
        push<PickleOpCode>(PickleOpCode::BINPUT);
        push<uint8_t>(nextMemoId);
    }
    else {
        // Memoized too many items, issue a LONG_BINPUT instead
        push<PickleOpCode>(PickleOpCode::LONG_BINPUT);
        push<uint32_t>(nextMemoId);
    }
    //AT_ASSERT(memo_id_ <= std::numeric_limits<uint32_t>::max());
    return nextMemoId++;
}

void Pickler::flushNonEmpty()
{
    writerFunc(buffer.data(), bufferPos);
    bufferPos = 0;
}

void Pickler::flush()
{
    if (bufferPos != 0)
        flushNonEmpty();
}


// ---------------- ShmPickler ----------------


void ShmPickler::writeIntoShm(const char *bytes, size_t count)
{
    if (!isMapped())
        throw std::runtime_error("The SHM for pickling is not mapped");

    if (sizeLimit >= 0 && (writtenBytes + count) > sizeLimit) {
        cleanup();
        throw std::runtime_error("Pickle size limit exceeded");
    }

    if ((writtenBytes + count) > reserveSize) {
        cleanup();
        throw std::runtime_error("Reserved shared memory exceeded during pickling");
    }

    if (scave::getAvailableMemoryBytes() < count) {
        cleanup();
        throw std::runtime_error("Ran out of memory during pickling");
    }

    void *shmEnd = (void *)(shm + writtenBytes);

    commitSharedMemory(shmEnd, count);
    memcpy(shmEnd, bytes, count);
    writtenBytes += count;
}

void ShmPickler::unmap()
{
    if (!isMapped())
        throw std::runtime_error("Cannot unmap SHM: not mapped");

    flush();
    unmapSharedMemory(shm, reserveSize);
    shm = nullptr;
}

void ShmPickler::cleanup()
{
    if (isMapped())
        unmap();

    if (exists()) {
        removeSharedMemory(shmName.c_str());
        shmName.clear();
    }
}

std::string ShmPickler::getShmNameAndSize()
{
    if (!exists())
        throw std::runtime_error("Pickler SHM is not available (maybe it was already released?)");

    if (isMapped())
        flush();

    return shmName + " " + std::to_string(writtenBytes);
}

void ShmPickler::release()
{
    if (isMapped())
        unmap();

    // without removing the SHM objects
    shmName.clear();
}

ShmPickler::ShmPickler(const std::string shmNameFragment, size_t sizeLimit)
    : Pickler([this](const char* bytes, size_t count) {
        writeIntoShm(bytes, count);
    }), sizeLimit(sizeLimit)
{
    // shmNameFragment must be shorter than ~20 characters!
    static int counter = 0;
    shmName = "/" + shmNameFragment + "_" + std::to_string(getpid() % 1000) + "_" + std::to_string(counter++ % 1000000);

    if (shmName.size() >= OPP_SHM_NAME_MAX) {
        std::string error = "SHM name is too long: '" + shmName + "'";
        shmName.clear();
        throw std::runtime_error(error);
    }

    createSharedMemory(shmName.c_str(), reserveSize, false);
    shm = (unsigned char *)mapSharedMemory(shmName.c_str(), reserveSize);
}

ShmPickler::~ShmPickler()
{
    cleanup();
}

} // namespace omnetpp
