//=========================================================================
//  CPACKETQUEUE.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstring>
#include <sstream>
#include "omnetpp/globals.h"
#include "omnetpp/cpacketqueue.h"
#include "omnetpp/cexception.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {

using std::ostream;

Register_Class(cPacketQueue);

cPacketQueue::cPacketQueue(const cPacketQueue& queue) : cQueue(queue)
{
    copy(queue);
}

void cPacketQueue::copy(const cPacketQueue& queue)
{
    bitLength = queue.bitLength;
}

cPacketQueue& cPacketQueue::operator=(const cPacketQueue& queue)
{
    if (this == &queue)
        return *this;
    cQueue::operator=(queue);
    copy(queue);
    return *this;
}

std::string cPacketQueue::str() const
{
    if (isEmpty())
        return std::string("empty");
    std::stringstream out;
    out << "len=" << getLength() << ", " << getBitLength() << " bits (" << getByteLength() << " bytes)";
    return out.str();
}

void cPacketQueue::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cQueue::parsimPack(buffer);
    buffer->pack(bitLength);
#endif
}

void cPacketQueue::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cQueue::parsimUnpack(buffer);
    buffer->unpack(bitLength);
#endif
}

void cPacketQueue::addLength(cPacket *pkt)
{
    bitLength += pkt->getBitLength();
}

cPacket *cPacketQueue::checkPacket(cObject *obj)
{
    cPacket *pkt = dynamic_cast<cPacket *>(obj);
    if (!pkt)
        throw cRuntimeError(this, "insert...(): Cannot cast (%s)%s to cPacket", obj->getClassName(), obj->getFullName());
    return pkt;
}

void cPacketQueue::insert(cPacket *pkt)
{
    cQueue::insert(pkt);
    addLength(pkt);
}

void cPacketQueue::insertBefore(cPacket *where, cPacket *pkt)
{
    cQueue::insertBefore(where, pkt);
    addLength(pkt);
}

void cPacketQueue::insertAfter(cPacket *where, cPacket *pkt)
{
    cQueue::insertAfter(where, pkt);
    addLength(pkt);
}

cPacket *cPacketQueue::remove(cPacket *pkt)
{
    return (cPacket *)remove((cObject *)pkt);
}

cPacket *cPacketQueue::pop()
{
    cPacket *pkt = (cPacket *)cQueue::pop();
    if (pkt)
        bitLength -= pkt->getBitLength();
    return pkt;
}

void cPacketQueue::clear()
{
    cQueue::clear();
    bitLength = 0;
}

void cPacketQueue::insert(cObject *obj)
{
    insert(checkPacket(obj));
}

void cPacketQueue::insertBefore(cObject *where, cObject *obj)
{
    insertBefore(checkPacket(where), checkPacket(obj));
}

void cPacketQueue::insertAfter(cObject *where, cObject *obj)
{
    insertAfter(checkPacket(where), checkPacket(obj));
}

cObject *cPacketQueue::remove(cObject *obj)
{
    cPacket *pkt = (cPacket *)cQueue::remove(obj);
    if (pkt)
        bitLength -= pkt->getBitLength();
    return pkt;
}

}  // namespace omnetpp

