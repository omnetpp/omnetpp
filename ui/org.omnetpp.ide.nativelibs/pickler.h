//=========================================================================
//  PICKLER.H - part of
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

// This file was "inspired by" torch/csrc/jit/serialization/pickler.h in PyTorch

#ifndef __OMNETPP_PICKLER_H
#define __OMNETPP_PICKLER_H

#include <string>
#include <array>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdlib>
#include <memory>
#include <cstring>

#include "sharedmemory.h"

namespace omnetpp {

// See Python's pickletools.py for a detailed description of each of these codes
enum class PickleOpCode : char {
    MARK = '(',
    STOP = '.',
    POP = '0',
    POP_MARK = '1',
    DUP = '2',
    FLOAT = 'F',
    INT = 'I',
    BININT = 'J',
    BININT1 = 'K',
    LONG = 'L',
    BININT2 = 'M',
    NONE = 'N',
    PERSID = 'P',
    BINPERSID = 'Q',
    REDUCE = 'R',
    STRING = 'S',
    BINSTRING = 'T',
    SHORT_BINSTRING = 'U',
    // NB: Avoid using UNICODE as it is a macro in the Windows API
    UNICODE_ = 'V',
    BINUNICODE = 'X',
    APPEND = 'a',
    BUILD = 'b',
    GLOBAL = 'c',
    DICT = 'd',
    EMPTY_DICT = '}',
    APPENDS = 'e',
    GET = 'g',
    BINGET = 'h',
    INST = 'i',
    LONG_BINGET = 'j',
    LIST = 'l',
    EMPTY_LIST = ']',
    OBJ = 'o',
    PUT = 'p',
    BINPUT = 'q',
    LONG_BINPUT = 'r',
    SETITEM = 's',
    TUPLE = 't',
    EMPTY_TUPLE = ')',
    SETITEMS = 'u',
    BINFLOAT = 'G',

    // Protocol 2
    PROTO = '\x80',
    NEWOBJ = '\x81',
    EXT1 = '\x82',
    EXT2 = '\x83',
    EXT4 = '\x84',
    TUPLE1 = '\x85',
    TUPLE2 = '\x86',
    TUPLE3 = '\x87',
    NEWTRUE = '\x88',
    NEWFALSE = '\x89',
    LONG1 = '\x8a',
    LONG4 = '\x8b',

    // Protocol 3 (Python 3.x)
    BINBYTES = 'B',
    SHORT_BINBYTES = 'C',

    // Protocol 4
    SHORT_BINUNICODE = '\x8c',
    BINUNICODE8 = '\x8d',
    BINBYTES8 = '\x8e',
    EMPTY_SET = '\x8f',
    ADDITEMS = '\x90',
    FROZENSET = '\x91',
    NEWOBJ_EX = '\x92',
    STACK_GLOBAL = '\x93',
    MEMOIZE = '\x94',
    FRAME = '\x95'
};

/**
 * This is a generic pickler class, supporting string memoization, which delegates the
 * storing of the pickled data to the instantiator via a writer function.
 */
class Pickler {
    Pickler() = delete;
    Pickler(const Pickler &) = delete;
    Pickler(Pickler &&) = delete;

    Pickler &operator=(const Pickler &) = delete;
    Pickler &operator=(Pickler &&) = delete;

  protected:

    // Stream to write binary data to
    // Code shouldn't call writerFunc directly without first flush()ing.
    std::function<void(const char*, size_t)> writerFunc;

    static constexpr size_t bufferSize = 64 * 1024;
    // Buffer to avoid calling a writerFunc on a per-byte basis.
    std::array<char, bufferSize> buffer;
    size_t bufferPos = 0;

    uint32_t nextMemoId = 0;
    std::unordered_map<std::string, uint32_t> memoizedStrings;

  public:
    Pickler(std::function<void(const char*, size_t)> writer) : writerFunc(writer) { }

    // Push protocol onto the stack
    void protocol();

    // Push STOP PickleOpCode onto the stack
    void stop();

    void startTuple();
    void endTuple();

    void startList();
    void endList();

    // These convert values to bytes and add them to the stack (NB: since T is to
    // the left of a '::', its type cannot be deduced by the compiler so one must
    // explicitly instantiate the template, i.e. push<int>(int) works, push(int)
    // does not)
    template <typename T>
    void push(typename std::common_type<T>::type value) {
        const char *begin = reinterpret_cast<const char*>(&value);
        if (bufferPos + sizeof(T) > buffer.size())
            flushNonEmpty();

        static_assert(sizeof(T) <= bufferSize, "Buffer size assumption");
        memcpy(buffer.data() + bufferPos, begin, sizeof(T));
        bufferPos += sizeof(T);
    }

    void pushEmptyDict();

    void pushBool(bool value);
    void pushInt(int64_t value);
    void pushDouble(double value);

    void pushDoublesAsRawBytes(const std::vector<double>& values); // little-endian (doesn't swap)

    void pushString(const std::string& string);

    // unmemoized version
    void pushStringImpl(const std::string& string);

    void pushBytes(const std::string& string);

    void pushBinGet(uint32_t memo_id);

    void pushNone();

    // Add a BINPUT op and return the memoization id used
    size_t pushNextBinPut();

    // Caller checks that bufferPos > 0
    void flushNonEmpty();
    void flush();

    virtual ~Pickler() { flush(); };
};

/**
 * A specialized pickler that stores the pickled data into a SHM object it creates for itself.
 */
class ShmPickler : public Pickler
{
    std::string shmName;
    unsigned char *shm = nullptr;
    size_t writtenBytes = 0;
    size_t sizeLimit;

    // should be (one byte under) 2 GiB. this is the most we can pass into ftruncate -
    // - macOS has no ftruncate64 and off_t is signed, and most likely 32 bit
    static const size_t reserveSize = std::numeric_limits<int32_t>::max();

    void writeIntoShm(const char *bytes, size_t count);

    void unmap();
    void cleanup();

  public:

    ShmPickler(const std::string shmNameFragment = "pickle", size_t sizeLimit = -1);

    std::string getShmNameAndSize();

    // THIS DOES NOT REMOVE THE SHM OBJECT
    virtual ~ShmPickler() { unmap(); }
};

} // namespace omnetpp

#endif
