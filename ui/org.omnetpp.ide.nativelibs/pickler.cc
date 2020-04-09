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
#include "shmmanager.h"

#include <string>
#include "scave/memoryutils.h" // getAvailableMemoryBytes()

namespace omnetpp {
namespace scave {

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

void Pickler::tuple2()
{
    push<PickleOpCode>(PickleOpCode::TUPLE2);
}

void Pickler::tuple3()
{
    push<PickleOpCode>(PickleOpCode::TUPLE3);
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

void Pickler::pushRawBytes(void *p, size_t size)
{
    if (bufferPos + size > bufferSize) {
        makeRoom(size);
        Assert(bufferPos + size <= bufferSize);
    }

    memcpy((char*)buffer + bufferPos, p, size);
    bufferPos += size;
}

void Pickler::pushBytes(const std::string& string)
{
    pushRawBytes((void *)string.data(), string.size());
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
    size_t size = values.size() * 8;

    push<PickleOpCode>(PickleOpCode::BINBYTES);
    push<int32_t>(size);

    pushRawBytes((void *)values.data(), size);
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
    return nextMemoId++; // should check overflow ;)
}

void Pickler::makeRoom(size_t bytesNeeded)
{
    throw std::runtime_error("Pickle buffer full"); // override to support flushing or extending the buffer
}

// ---------------- ShmPickler ----------------

ShmPickler::ShmPickler(ShmSendBuffer *sendBuffer, size_t sizeLimit) :
        Pickler(sendBuffer->getAddress(), sendBuffer->getSize()),
        sendBuffer(sendBuffer), sizeLimit(sizeLimit)
{
}

ShmPickler::~ShmPickler()
{
    delete sendBuffer;
}

void ShmPickler::makeRoom(size_t bytesNeeded)
{
    // extend buffer (increase bufferSize)
    size_t increment = std::max(bytesNeeded, std::max((size_t)4096, (bufferSize*125)/100)); // 25% but min 4K, and min bytesNeeded

    if (sizeLimit >= 0 && (bufferSize + increment) > sizeLimit)
        throw std::runtime_error("Pickle size limit exceeded");

    if (getAvailableMemoryBytes() < increment)
        throw std::runtime_error("Ran out of memory during pickling");

    bufferSize += increment;
    sendBuffer->extendTo(bufferSize);
}

} // namespace scave
} // namespace omnetpp
