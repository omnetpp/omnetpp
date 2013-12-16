//=========================================================================
//  CPACKETQUEUE.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <sstream>
#include "globals.h"
#include "cpacketqueue.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

NAMESPACE_BEGIN

using std::ostream;


Register_Class(cPacketQueue);


cPacketQueue::cPacketQueue(const char *name, CompareFunc cmp) : cQueue(name,cmp)
{
    bitlength = 0;
}

cPacketQueue::cPacketQueue(const cPacketQueue& queue) : cQueue(queue)
{
    copy(queue);
}

void cPacketQueue::copy(const cPacketQueue& queue)
{
    bitlength = queue.bitlength;
}

cPacketQueue& cPacketQueue::operator=(const cPacketQueue& queue)
{
    if (this==&queue) return *this;
    cQueue::operator=(queue);
    copy(queue);
    return *this;
}

std::string cPacketQueue::info() const
{
    if (empty())
        return std::string("empty");
    std::stringstream out;
    out << "len=" << getLength() << ", " << getBitLength() << " bits (" << getByteLength() << " bytes)";
    return out.str();
}

void cPacketQueue::parsimPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cQueue::parsimPack(buffer);
    buffer->pack(bitlength);
#endif
}

void cPacketQueue::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cQueue::parsimUnpack(buffer);
    buffer->unpack(bitlength);
#endif
}

void cPacketQueue::addLength(cPacket *pkt)
{
    bitlength += pkt->getBitLength();
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
        bitlength -= pkt->getBitLength();
    return pkt;
}

void cPacketQueue::clear()
{
    cQueue::clear();
    bitlength = 0;
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
        bitlength -= pkt->getBitLength();
    return pkt;
}

NAMESPACE_END

