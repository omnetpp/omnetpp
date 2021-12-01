//=========================================================================
//  CCOMMBUFFERBASE.H - part of
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

#ifndef __OMNETPP_CCOMMBUFFERBASE_H
#define __OMNETPP_CCOMMBUFFERBASE_H

#include "omnetpp/ccommbuffer.h"

namespace omnetpp {

/**
 * @brief Adds buffer (re)allocation functions to cCommBuffer. This functionality
 * is not always needed, as some libraries manage their pack/unpack buffers internally.
 */
class SIM_API cCommBufferBase : public cCommBuffer
{
  protected:
    char *mBuffer = nullptr; // the buffer
    int mBufferSize = 0;     // size of buffer allocated
    int mMsgSize = 0;        // current msg size (incremented by pack() functions)
    int mPosition = 0;       // current position in buffer for unpacking

  protected:
    void extendBufferFor(int dataSize);

  public:
    /**
     * Constructor.
     */
    cCommBufferBase() {}

    /**
     * Destructor.
     */
    virtual ~cCommBufferBase();

    /** @name Buffer management */
    //@{
    /**
     * Returns the buffer after packing.
     */
    char *getBuffer() const;

    /**
     * Returns the size of the buffer.
     */
    int getBufferLength() const;

    /**
     * Extend buffer to the given size is needed. Existing buffer contents
     * may be lost.
     */
    void allocateAtLeast(int size);

    /**
     * Set message length in the buffer. Used after receiving a message
     * and copying it to the buffer.
     */
    void setMessageSize(int size);

    /**
     * Returns message length in the buffer.
     */
    int getMessageSize() const;

    /**
     * Reset buffer to an empty state.
     */
    void reset();

    /**
     * Returns true if all data in buffer was used up during unpacking.
     * Returns false if there was underflow (too much data unpacked)
     * or still there are unpacked data in the buffer.
     */
    virtual bool isBufferEmpty() const override;

    /**
     * Utility function. To be called after unpacking a communication buffer,
     * it checks whether the buffer is empty. If it is not (i.e. an underflow
     * or overflow occurred), an exception is thrown.
     */
    virtual void assertBufferEmpty() override;

    /**
     * Utility method: swap the contents of the two buffers
     */
    virtual void swap(cCommBufferBase *other);

    //@}
};

}  // namespace omnetpp


#endif
