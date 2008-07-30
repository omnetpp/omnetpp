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
#include "globals.h"
#include "cpacketqueue.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

USING_NAMESPACE

using std::ostream;


Register_Class(cPacketQueue);


cPacketQueue::cPacketQueue(const char *name) : cQueue(name)
{
    bitlength = 0;
}

cPacketQueue::cPacketQueue(const cPacketQueue& queue)
{
    setName(queue.getName());
    operator=(queue);
}

cPacketQueue& cPacketQueue::operator=(const cPacketQueue& queue)
{
    cQueue::operator=(queue);
    bitlength = queue.bitlength;
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

void cPacketQueue::msgAdd(cOwnedObject *obj)
{
    if (obj) {
        cPacket *msg = dynamic_cast<cPacket *>(obj);
        if (!msg)
            throw cRuntimeError(this, "insert(): (%s)%s is not a cPacket", obj->getClassName(), obj->getFullName());
        bitlength += msg->getBitLength();
    }
}

void cPacketQueue::insert(cOwnedObject *msg)
{
    msgAdd(msg);
    cQueue::insert(msg);
}

void cPacketQueue::insertBefore(cOwnedObject *where, cOwnedObject *msg)
{
    msgAdd(msg);
    cQueue::insertBefore(where, msg);
}

void cPacketQueue::insertAfter(cOwnedObject *where, cOwnedObject *msg)
{
    msgAdd(msg);
    cQueue::insertAfter(where, msg);
}

cPacket *cPacketQueue::remove(cOwnedObject *msg)
{
    cPacket *msg1 = (cPacket *)cQueue::remove(msg);
    if (msg)
        bitlength -= msg1->getBitLength();
    return msg1;
}

cPacket *cPacketQueue::pop()
{
    cPacket *msg = (cPacket *)cQueue::pop();
    if (msg)
        bitlength -= msg->getBitLength();
    return msg;
}

