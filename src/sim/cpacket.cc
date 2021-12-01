//========================================================================
//  CPACKET.CC - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sstream>
#include "omnetpp/globals.h"
#include "omnetpp/cpacket.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/platdep/platmisc.h"  // PRId64

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

using namespace omnetpp;

using std::ostream;

// comment out to disable reference-counting for encapsulated messages
#define REFCOUNTING

Register_Class(cPacket);

cPacket::cPacket(const cPacket& pkt) : cMessage(pkt)
{
    copy(pkt);
}

cPacket::cPacket(const char *name, short k, int64_t l) : cMessage(name, k), bitLength(l)
{
}

cPacket::~cPacket()
{
    if (encapsulatedPacket)
#ifdef REFCOUNTING
        _deleteEncapMsg();
#else
        dropAndDelete(encapsulatedPacket);
#endif
}

inline void printLen(std::stringstream &out, int64_t bits)
{
    if ((bits & 7) == 0)
        out << bits/8 << "B";
    else
        out << bits << "b";
}

std::string cPacket::str() const
{
    std::stringstream out;
    if (isUpdate())
        out << "update w/ remainingDuration=" << remainingDuration.ustr() << ", ";
    out << "len="; printLen(out, getBitLength());
    if (duration != SimTime::ZERO)
        out << " duration=" << duration.ustr();
    if (encapsulatedPacket) {
        out << " (encapsulates ";
        printLen(out, encapsulatedPacket->getBitLength());
        // or: << encapsulatedPacket->getClassName() << ")" << encapsulatedPacket->getFullName() << ")"; -- but that might be too long
        out << ")";
    }
    std::string msgStr = cMessage::str();
    if (!msgStr.empty())
        out << "; " << msgStr;
    return out.str();
}

void cPacket::forEachChild(cVisitor *v)
{
    cMessage::forEachChild(v);

    if (encapsulatedPacket) {
#ifdef REFCOUNTING
        _detachEncapMsg();  // see method comment why this is needed
#endif
        v->visit(encapsulatedPacket);
    }
}

void cPacket::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cMessage::parsimPack(buffer);
    buffer->pack(bitLength);
    buffer->pack(duration);
    if (buffer->packFlag(encapsulatedPacket != nullptr))
        buffer->packObject(encapsulatedPacket);
    buffer->pack(transmissionId);
    buffer->pack(remainingDuration);
#endif
}

void cPacket::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    ASSERT(shareCount == 0);
    cMessage::parsimUnpack(buffer);
    buffer->unpack(bitLength);
    buffer->unpack(duration);
    if (buffer->checkFlag())
        take(encapsulatedPacket = (cPacket *)buffer->unpackObject());
    buffer->unpack(transmissionId);
    buffer->unpack(remainingDuration);
#endif
}

cPacket& cPacket::operator=(const cPacket& msg)
{
    if (this == &msg)
        return *this;
    cMessage::operator=(msg);
    copy(msg);
    return *this;
}

void cPacket::copy(const cPacket& msg)
{
#ifdef REFCOUNTING
    if (shareCount != 0)
        throw cRuntimeError(this, "operator=(): This message is refcounted (shared between "
                                  "several messages), it is forbidden to change it");
#endif

    bitLength = msg.bitLength;
    duration = msg.duration;

#ifndef REFCOUNTING
    dropAndDelete(encapsulatedPacket);
    if (msg.encapsulatedPacket)
        take(encapsulatedPacket = (cPacket *)msg.encapsulatedPacket->dup());
    else
        encapsulatedPacket = nullptr;
#else
    if (encapsulatedPacket)
        _deleteEncapMsg();
    encapsulatedPacket = msg.encapsulatedPacket;
    if (encapsulatedPacket && ++encapsulatedPacket->shareCount == 0) {  // sharecount overflow
        --encapsulatedPacket->shareCount;
        take(encapsulatedPacket = (cPacket *)encapsulatedPacket->dup());
    }
#endif

    transmissionId = msg.transmissionId;
    remainingDuration = msg.remainingDuration;
}

#ifdef REFCOUNTING
void cPacket::_deleteEncapMsg()
{
    if (encapsulatedPacket->shareCount > 0) {
        encapsulatedPacket->shareCount--;
        if (encapsulatedPacket->owner == this)
            encapsulatedPacket->owner = nullptr;
    }
    else {
        // note: dropAndDelete(encapmsg) cannot be used, because due to sharecounting
        // ownerp is not valid (may be any former owner, possibly deleted since then)
        encapsulatedPacket->owner = nullptr;
        delete encapsulatedPacket;
    }
}

#endif

#ifdef REFCOUNTING
void cPacket::_detachEncapMsg()
{
    if (encapsulatedPacket->shareCount > 0) {
        // "de-share" object - create our own copy
        encapsulatedPacket->shareCount--;
        if (encapsulatedPacket->owner == this)
            encapsulatedPacket->owner = nullptr;
        take(encapsulatedPacket = (cPacket *)encapsulatedPacket->dup());
    }
    else {
        // note: due to sharecounting, ownerp may be pointing to a previous owner -- fix it
        encapsulatedPacket->owner = this;
    }
}

#endif

const char *cPacket::getDisplayString() const
{
    return encapsulatedPacket ? encapsulatedPacket->getDisplayString() : "";
}

void cPacket::setBitLength(int64_t l)
{
    if (l < 0)
        throw cRuntimeError(this, "setBitLength(): Negative length %" PRId64, l);
    bitLength = l;
}

void cPacket::addBitLength(int64_t l)
{
    bitLength += l;
    if (bitLength < 0)
        throw cRuntimeError(this, "addBitLength(): Length became negative (%" PRId64 ") after adding %" PRId64, bitLength, l);
}

void cPacket::encapsulate(cPacket *msg)
{
    if (encapsulatedPacket)
        throw cRuntimeError(this, "encapsulate(): Another message already encapsulated");

    if (msg) {
        if (msg->getOwner() != getSimulation()->getContextSimpleModule())
            throw cRuntimeError(this, "encapsulate(): Not owner of message (%s)%s, owner is (%s)%s",
                    msg->getClassName(), msg->getFullName(),
                    msg->getOwner()->getClassName(), msg->getOwner()->getFullPath().c_str());
        if (msg->getBitLength() < 0)
            throw cRuntimeError(this, "encapsulate(): negative length (%" PRId64 ") in message (%s)%s",
                    msg->getBitLength(), msg->getClassName(), msg->getFullName());
        take(encapsulatedPacket = msg);
#ifdef REFCOUNTING
        ASSERT(encapsulatedPacket->shareCount == 0);
#endif
        bitLength += encapsulatedPacket->getBitLength();
    }
}

cPacket *cPacket::decapsulate()
{
    if (!encapsulatedPacket)
        return nullptr;
    if (bitLength > 0)
        bitLength -= encapsulatedPacket->getBitLength();
    if (bitLength < 0)
        throw cRuntimeError(this, "decapsulate(): Packet length is smaller than encapsulated packet");

#ifdef REFCOUNTING
    if (encapsulatedPacket->shareCount > 0) {
        encapsulatedPacket->shareCount--;
        if (encapsulatedPacket->owner == this)
            encapsulatedPacket->owner = nullptr;
        cPacket *msg = encapsulatedPacket->dup();
        encapsulatedPacket = nullptr;
        return msg;
    }
    encapsulatedPacket->owner = this;
#endif
    cPacket *msg = encapsulatedPacket;
    encapsulatedPacket = nullptr;
    drop(msg);
    return msg;
}

cPacket *cPacket::getEncapsulatedPacket() const
{
#ifdef REFCOUNTING
    // encapmsg may be shared (sharecount>0) -- we'll make our own copy,
    // so that other messages are not affected in case the user modifies
    // the encapsulated message via the returned pointer.
    // Trick: this is a const method, so we can only do changes via a
    // non-const copy of the 'this' pointer.
    if (encapsulatedPacket)
        const_cast<cPacket *>(this)->_detachEncapMsg();
#endif
    return encapsulatedPacket;
}

bool cPacket::hasEncapsulatedPacket() const
{
    return encapsulatedPacket != nullptr;
}

msgid_t cPacket::getEncapsulationId() const
{
    // find innermost msg. Note: don't use getEncapsulatedPacket() because it does copy-on-touch of shared msgs
    const cPacket *msg = this;
    while (msg->encapsulatedPacket)
        msg = msg->encapsulatedPacket;
    return msg->getId();
}

msgid_t cPacket::getEncapsulationTreeId() const
{
    // find innermost msg. Note: don't use getEncapsulatedPacket() because it does copy-on-touch of shared msgs
    const cPacket *msg = this;
    while (msg->encapsulatedPacket)
        msg = msg->encapsulatedPacket;
    return msg->getTreeId();
}

