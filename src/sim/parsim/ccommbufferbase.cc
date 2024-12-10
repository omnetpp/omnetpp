//=========================================================================
//  CCOMMBUFFERBASE.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cexception.h"
#include "ccommbufferbase.h"

namespace omnetpp {

cCommBufferBase::cCommBufferBase()
{
    initOwnerThread();
}

cCommBufferBase::cCommBufferBase(const cCommBufferBase& other) : cCommBuffer(other)
{
    initOwnerThread();
    other.checkThreadAccess();

    mBuffer = nullptr;
    mBufferSize = other.mBufferSize;
    mMsgSize = other.mMsgSize;
    mPosition = other.mPosition;
    if (other.mBuffer) {
        mBuffer = new char[mBufferSize];
        std::copy(other.mBuffer, other.mBuffer + mBufferSize, mBuffer);
    }
}

cCommBufferBase::cCommBufferBase(cCommBufferBase&& other)
{
    initOwnerThread();
    other.checkThreadAccess();

    mBuffer = other.mBuffer;
    mBufferSize = other.mBufferSize;
    mMsgSize = other.mMsgSize;
    mPosition = other.mPosition;

    other.mBuffer = nullptr;
    other.mBufferSize = other.mMsgSize = other.mPosition = 0;
}

void cCommBufferBase::initOwnerThread()
{
#ifndef NDEBUG
    ownerThread = std::this_thread::get_id();
#endif
}

void cCommBufferBase::setOwnerThread(std::thread::id tid)
{
#ifndef NDEBUG
    ownerThread = tid;
#endif
}

std::thread::id cCommBufferBase::getOwnerThread() const
{
#ifndef NDEBUG
    return ownerThread;
#else
    throw cRuntimeError("cCommBufferBase::getOwnerThread() should not be called in release mode builds");
#endif
}

cCommBufferBase& cCommBufferBase::operator=(cCommBufferBase&& other) noexcept
{
    if (this != &other) {
        checkThreadAccess();
        other.checkThreadAccess();
        if (mBuffer) {
            delete[] mBuffer;
            mBuffer = nullptr;
        }
        std::swap(mBuffer, other.mBuffer);
        mBufferSize = other.mBufferSize;
        mMsgSize = other.mMsgSize;
        mPosition = other.mPosition;

        other.mBufferSize = other.mMsgSize = other.mPosition = 0;
    }
    return *this;
}

void cCommBufferBase::copyFrom(const cCommBuffer *other)
{
    checkThreadAccess();
    const cCommBufferBase *castOther = static_cast<const cCommBufferBase *>(other);
    castOther->checkThreadAccess();
    allocateAtLeast(castOther->getMessageSize());
    mMsgSize = castOther->getMessageSize();
    mPosition = 0;
    std::copy(castOther->getBuffer(), castOther->getBuffer() + mMsgSize, mBuffer);
}

cCommBufferBase::~cCommBufferBase()
{
    checkThreadAccess();
    delete[] mBuffer;
}

char *cCommBufferBase::getBuffer() const
{
    checkThreadAccess();
    return mBuffer;
}

int cCommBufferBase::getBufferLength() const
{
    checkThreadAccess();
    return mBufferSize;
}

void cCommBufferBase::allocateAtLeast(int size)
{
    checkThreadAccess();
    size += 4;  // allocate a bit more room, for sentry (used in cFileCommBuffer, etc.)
    if (mBufferSize < size) {
        delete[] mBuffer;
        mBuffer = new char[size];
        mBufferSize = size;
    }
}

void cCommBufferBase::setMessageSize(int size)
{
    checkThreadAccess();
    mMsgSize = size;
    mPosition = 0;
}

int cCommBufferBase::getMessageSize() const
{
    checkThreadAccess();
    return mMsgSize;
}

void cCommBufferBase::reset()
{
    checkThreadAccess();
    mMsgSize = 0;
    mPosition = 0;
}

void cCommBufferBase::extendBufferFor(int dataSize)
{
    // TBD move reallocate+copy out of loop (more efficient)
    checkThreadAccess();
    while (mMsgSize+dataSize >= mBufferSize) {
        // increase the size of the buffer while
        // retaining its own existing contents
        char *tempBuffer;
        int i, oldBufferSize = 0;

        oldBufferSize = mBufferSize;
        if (mBufferSize == 0)
            mBufferSize = 1000;
        else
            mBufferSize += mBufferSize;

        tempBuffer = new char[mBufferSize];
        for (i = 0; i < oldBufferSize; i++)
            tempBuffer[i] = mBuffer[i];

        delete[] mBuffer;
        mBuffer = tempBuffer;
    }
}

bool cCommBufferBase::isBufferEmpty() const
{
    checkThreadAccess();
    return mPosition == mMsgSize;
}

void cCommBufferBase::assertBufferEmpty()
{
    checkThreadAccess();
    if (mPosition == mMsgSize)
        return;

    if (mPosition > mMsgSize) {
        throw cRuntimeError("Internal error: cCommBuffer pack/unpack mismatch: "
                             "read %d bytes past end of buffer while unpacking %d bytes",
                             mPosition-mMsgSize, mPosition);
    }
    else {
        throw cRuntimeError("Internal error: cCommBuffer pack/unpack mismatch: "
                            "%d extra bytes remained in buffer after unpacking %d bytes",
                            mMsgSize-mPosition, mPosition);
    }
}

void cCommBufferBase::swap(cCommBufferBase *other)
{
    checkThreadAccess();
    other->checkThreadAccess();
    std::swap(mBuffer, other->mBuffer);
    std::swap(mBufferSize, other->mBufferSize);
    std::swap(mMsgSize, other->mMsgSize);
    std::swap(mPosition, other->mPosition);
}


}  // namespace omnetpp

