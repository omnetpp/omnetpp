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
#include <limits>

#include "scave/scavedefs.h"

namespace omnetpp {
namespace scave {

class ShmSendBuffer;

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
 * Provides serialization in Python's Pickle format, see https://docs.python.org/3/library/pickle.html.
 * String memoization is supported. Buffer allocation and management is left to subclasses.
 */
class Pickler
{
    Pickler(const Pickler &) = delete;
    Pickler(Pickler &&) = delete;

    Pickler &operator=(const Pickler &) = delete;
    Pickler &operator=(Pickler &&) = delete;

  protected:
    void *buffer;
    size_t bufferSize;
    size_t bufferPos = 0;

    uint32_t nextMemoId = 0;
    std::unordered_map<std::string, uint32_t> memoizedStrings;

    // convert values to bytes and add them to the stack
    template <typename T>
    void push(T value) {
        const char *begin = reinterpret_cast<const char*>(&value);
        if (bufferPos + sizeof(T) > bufferSize) {
            makeRoom(sizeof(T));
            Assert(bufferPos + sizeof(T) <= bufferSize);
        }

        memcpy((char *)buffer + bufferPos, begin, sizeof(T));
        bufferPos += sizeof(T);
    }

    virtual void flush() { }  // called on stop()

    virtual void makeRoom(size_t bytesNeeded);  // throws exception; override to flush or extend buffer

  public:
    Pickler(void *buffer, size_t bufferSize) : buffer(buffer), bufferSize(bufferSize) {}
    virtual ~Pickler() { }

    // Push protocol onto the stack
    void protocol();

    // Push STOP PickleOpCode onto the stack
    void stop();

    void startTuple();
    void endTuple();

    void tuple2();
    void tuple3();

    void startList();
    void endList();

    void pushEmptyDict();

    void pushBool(bool value);
    void pushInt(int64_t value);
    void pushDouble(double value);

    void pushRawBytes(void *p, size_t size);

    void pushDoublesAsRawBytes(const std::vector<double>& values); // little-endian (doesn't swap)

    void pushString(const std::string& string);

    // unmemoized version
    void pushStringImpl(const std::string& string);

    void pushBytes(const std::string& string);

    void pushBinGet(uint32_t memo_id);

    void pushNone();

    // Add a BINPUT op and return the memoization id used
    size_t pushNextBinPut();
};

/**
 * A specialized pickler that stores the pickled data into an ShmSendBuffer.
 */
class ShmPickler : public Pickler
{
  private:
    ShmSendBuffer *sendBuffer;
    size_t sizeLimit;

  protected:
    virtual void makeRoom(size_t bytesNeeded);

  public:
    ShmPickler(ShmSendBuffer *sendBuffer, size_t sizeLimit);
    ~ShmPickler();
    ShmSendBuffer *get() {ShmSendBuffer *ret = sendBuffer; sendBuffer = nullptr; return ret;}
};

} // namespace scave
} // namespace omnetpp

#endif
