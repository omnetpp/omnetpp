//==========================================================================
//  CPACKETQUEUE.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CPACKETQUEUE_H
#define __OMNETPP_CPACKETQUEUE_H

#include "cqueue.h"
#include "cpacket.h"

namespace omnetpp {


/**
 * @brief A queue class specialized for cPacket objects.
 *
 * The added value is that this class keeps track of the total queue length
 * in bits as well.
 *
 * @ingroup SimProgr
 */
class SIM_API cPacketQueue : public cQueue
{
  private:
    int64_t bitLength=0;

  private:
    void copy(const cPacketQueue& other);
    virtual void insert(cObject *obj) override;
    virtual void insertBefore(cObject *where, cObject *obj) override;
    virtual void insertAfter(cObject *where, cObject *obj) override;
    virtual cObject *remove(cObject *obj) override;

  protected:
    // internal
    void addLength(cPacket *pkt);
    cPacket *checkPacket(cObject *obj);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor. When comparator argument is nullptr, the queue will
     * act as FIFO, otherwise as priority queue.
     */
    cPacketQueue(const char *name=nullptr, Comparator *cmp=nullptr) : cQueue(name, cmp) {}

    /**
     * Constructor. Sets up cPacketQueue as a priority queue.
     */
    cPacketQueue(const char *name, CompareFunc cmp) : cQueue(name, cmp) {}

    /**
     * Copy constructor. Contained objects that are owned by the queue
     * will be duplicated so that the new queue will have its own copy
     * of them.
     */
    cPacketQueue(const cPacketQueue& queue);

    /**
     * Assignment operator. Contained objects that are owned by the queue
     * will be duplicated so that the new queue will have its own copy of them.
     *
     * The name member is not copied; see cNamedObject::operator=() for details.
     */
    cPacketQueue& operator=(const cPacketQueue& queue);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * Contained objects that are owned by the queue will be duplicated
     * so that the new queue will have its own copy of them.
     */
    virtual cPacketQueue *dup() const override  {return new cPacketQueue(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string str() const override;

    /**
     * Serializes the object into an MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimPack(cCommBuffer *buffer) const override;

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimUnpack(cCommBuffer *buffer) override;
    //@}

    /** @name Setup, insertion and removal functions. */
    //@{
    /**
     * Adds an element to the back of the queue. Trying to insert
     * nullptr is an error (throws cRuntimeError). The given
     * object must be an instance of cPacket.
     */
    virtual void insert(cPacket *pkt);

    /**
     * Inserts exactly before the given object. If the given position
     * does not exist or if you try to insert nullptr, cRuntimeError
     * is thrown. The given object must be an instance of cPacket.
     */
    virtual void insertBefore(cPacket *where, cPacket *pkt);

    /**
     * Inserts exactly after the given object. If the given position
     * does not exist or if you try to insert nullptr,
     * cRuntimeError is thrown. The given object must be an instance
     * of cPacket.
     */
    virtual void insertAfter(cPacket *where, cPacket *pkt);

    /**
     * Unlinks and returns the object given. If the object is not in the
     * queue, nullptr is returned. The given object must be an instance
     * of cPacket.
     */
    virtual cPacket *remove(cPacket *pkt);

    /**
     * Unlinks and returns the front element in the queue. If the queue
     * is empty, cRuntimeError is thrown.
     */
    virtual cPacket *pop() override;

    /**
     * Empties the container. Contained objects that were owned by the
     * queue (see getTakeOwnership()) will be deleted.
     */
    virtual void clear() override;
    //@}

    /** @name Query functions. */
    //@{
    /**
     * Returns the total size of the messages in the queue, in bits.
     * This is the sum of the message bit lengths; see cPacket::getBitLength().
     */
    int64_t getBitLength() const  {return bitLength;}

    /**
     * Returns the sum of the message lengths in bytes, that is, getBitLength()/8.
     * If getBitLength() is not a multiple of 8, the result is rounded up.
     */
    int64_t getByteLength() const  {return (bitLength+7)>>3;}

    /**
     * Returns pointer to the object at the front of the queue.
     * This is the element to be return by pop().
     * Returns nullptr if the queue is empty.
     */
    virtual cPacket *front() const override  {return (cPacket *)cQueue::front();}

    /**
     * Returns pointer to the last (back) element in the queue.
     * This is the element most recently added by insert().
     * Returns nullptr if the queue is empty.
     */
    virtual cPacket *back() const override  {return (cPacket *)cQueue::back();}

    /**
     * Returns the ith element in the queue, or nullptr if i is out of range.
     * get(0) returns the front element. This method performs linear
     * search.
     */
    virtual cPacket *get(int i) const override {return (cPacket *)cQueue::get(i);}
    //@}
};

}  // namespace omnetpp


#endif

