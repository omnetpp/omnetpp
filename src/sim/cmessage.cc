//========================================================================
//  CMESSAGE.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//   Member functions of
//    cMessage       : the message class
//
//  Author: Andras Varga
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>  // sprintf
#include "globals.h"
#include "cmodule.h"
#include "csimplemodule.h"
#include "cmessage.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

USING_NAMESPACE

// comment out to disable reference-counting for encapsulated messages
#define REFCOUNTING

using std::ostream;

#ifdef WITHOUT_CPACKET
#undef cMessage
#endif

Register_Class(cMessage);
Register_Class(cPacket);

// static members of cMessage
long cMessage::next_id = 0;
long cMessage::total_msgs = 0;
long cMessage::live_msgs = 0;


cMessage::cMessage(const cMessage& msg) : cOwnedObject(msg)
{
    parlistp = NULL;
    ctrlp = NULL;
    heapindex = -1;
    prev_event_num = -1;
    operator=(msg);

    msgid = next_id++;
    total_msgs++;
    live_msgs++;
}

cMessage::cMessage(const char *name, short k) : cOwnedObject(name, false)
{
    // name pooling is off for messages by default, as unique names are quite common
    msgkind = k;
    prior = 0;
    parlistp = NULL;
    contextptr = NULL;
    ctrlp = NULL;
    srcprocid = -1;

    frommod = fromgate = -1;
    tomod = togate = -1;
    created = simulation.getSimTime();
    sent = delivd = tstamp = 0;
    heapindex = -1;
    prev_event_num = -1;

    msgtreeid = msgid = next_id++;
    total_msgs++;
    live_msgs++;
}

cMessage::~cMessage()
{
#ifndef WITHOUT_CPACKET
    EVCB.messageDeleted(this);
#endif

    if (parlistp)
        dropAndDelete(parlistp);

    if (dynamic_cast<cOwnedObject *>(ctrlp))
        dropAndDelete((cOwnedObject *)ctrlp);
    else
        delete ctrlp;
    live_msgs--;
}

std::string cMessage::info() const
{
    if (tomod<0)
        return std::string("(new msg)");

    std::stringstream out;
    const char *deletedstr = "<deleted module>";

    if (delivd > simulation.getSimTime())
    {
        // if it arrived in the past, dt is usually unimportant, don't print it
        out << "at T=" << delivd << ", in dt=" << (delivd - simulation.getSimTime()) << "; ";
    }

#define MODNAME(modp) ((modp) ? (modp)->getFullPath().c_str() : deletedstr)
    if (getKind()==MK_STARTER)
    {
        cModule *tomodp = simulation.getModule(tomod);
        out << "starter for " << MODNAME(tomodp) << " (id=" << tomod << ") ";
    }
    else if (getKind()==MK_TIMEOUT)
    {
        cModule *tomodp = simulation.getModule(tomod);
        out << "timeoutmsg for " << MODNAME(tomodp) << " (id=" << tomod << ") ";
    }
    else if (frommod==tomod)
    {
        cModule *tomodp = simulation.getModule(tomod);
        out << "selfmsg for " << MODNAME(tomodp) << " (id=" << tomod << ") ";
    }
    else
    {
        cModule *frommodp = simulation.getModule(frommod);
        cModule *tomodp = simulation.getModule(tomod);
        out << "src=" << MODNAME(frommodp) << " (id=" << frommod << ") ";
        out << " dest=" << MODNAME(tomodp) << " (id=" << tomod << ") ";
    }
#undef MODNAME

    if (ctrlp)
        out << "  control info: (" << ctrlp->getClassName() << ") " << ctrlp->getFullName() << "\n";

    return out.str();
}

void cMessage::forEachChild(cVisitor *v)
{
    if (parlistp)
        v->visit(parlistp);
}

std::string cMessage::detailedInfo() const
{
    return "";  // all fields are available via reflection, no point in repeating them here
}

void cMessage::parsimPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cOwnedObject::parsimPack(buffer);

    if (contextptr || ctrlp)
        throw cRuntimeError(this,"parsimPack(): cannot pack object with contextPointer or controlInfo set");

    buffer->pack(msgkind);
    buffer->pack(prior);
    buffer->pack(tstamp);

    buffer->pack(frommod);
    buffer->pack(fromgate);
    buffer->pack(tomod);
    buffer->pack(togate);
    buffer->pack(created);
    buffer->pack(sent);
    buffer->pack(delivd);
    buffer->pack(heapindex);
    buffer->pack(insertordr);

    // note: do not pack msgid and treeid, because they'd conflict
    // with ids assigned at the destination partition

    if (buffer->packFlag(parlistp!=NULL))
        buffer->packObject(parlistp);
#endif
}

void cMessage::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cOwnedObject::parsimUnpack(buffer);

    buffer->unpack(msgkind);
    buffer->unpack(prior);
    buffer->unpack(tstamp);

    buffer->unpack(frommod);
    buffer->unpack(fromgate);
    buffer->unpack(tomod);
    buffer->unpack(togate);
    buffer->unpack(created);
    buffer->unpack(sent);
    buffer->unpack(delivd);
    buffer->unpack(heapindex);
    buffer->unpack(insertordr);

    if (buffer->checkFlag())
        take(parlistp = (cArray *) buffer->unpackObject());
#endif
}

cMessage& cMessage::operator=(const cMessage& msg)
{
    if (this==&msg) return *this;

    cOwnedObject::operator=(msg);

    msgkind = msg.msgkind;
    prior = msg.prior;
    tstamp = msg.tstamp;
    srcprocid = msg.srcprocid;

    created = msg.created;

    dropAndDelete(parlistp);
    if (msg.parlistp)
        take(parlistp = (cArray *)msg.parlistp->dup());
    else
        parlistp = NULL;

    contextptr = msg.contextptr;

    frommod = msg.frommod;
    fromgate = msg.fromgate;
    tomod = msg.tomod;
    togate = msg.togate;

    sent = msg.sent;
    delivd = msg.delivd;

    msgtreeid = msg.msgtreeid;

    return *this;
}

void cMessage::_createparlist()
{
    parlistp = new cArray("parameters", 5, 5);
    take(parlistp);
}

void cMessage::setControlInfo(cObject *p)
{
    if (!p)
        throw cRuntimeError(this,"setControlInfo(): pointer is NULL");
    if (ctrlp)
        throw cRuntimeError(this,"setControlInfo(): message already has control info attached");
    if (dynamic_cast<cOwnedObject *>(p))
        take((cOwnedObject *)p);
    ctrlp = p;
}

cObject *cMessage::removeControlInfo()
{
    cObject *p = ctrlp;
    ctrlp = NULL;
    if (dynamic_cast<cOwnedObject *>(p))
        drop((cOwnedObject *)p);
    return p;
}

cMsgPar& cMessage::par(int n)
{
    cArray& parlist = getParList();
    cObject *p = parlist.get(n);
    if (!p)
        throw cRuntimeError(this,"par(int): has no parameter #%d",n);
    if (!dynamic_cast<cMsgPar *>(p))
        throw cRuntimeError(this,"par(int): parameter #%d is of type %s, not cMsgPar",n, p->getClassName());
    return *(cMsgPar *)p;
}

cMsgPar& cMessage::par(const char *s)
{
    cArray& parlist = getParList();
    cObject *p = parlist.get(s);
    if (!p)
        throw cRuntimeError(this,"par(const char *): has no parameter called `%s'",s);
    if (!dynamic_cast<cMsgPar *>(p))
        throw cRuntimeError(this,"par(const char *): parameter `%s' is of type %s, not cMsgPar",s, p->getClassName());
    return *(cMsgPar *)p;
}

int cMessage::findPar(const char *s) const
{
    return !parlistp ? -1 : parlistp->find(s);
}

cGate *cMessage::getSenderGate() const
{
    if (frommod<0 || fromgate<0)  return NULL;
    cModule *mod = simulation.getModule(frommod);
    return !mod ? NULL : mod->gate(fromgate);
}

cGate *cMessage::getArrivalGate() const
{
    if (tomod<0 || togate<0)  return NULL;
    cModule *mod = simulation.getModule(tomod);
    return !mod ? NULL : mod->gate(togate);
}

bool cMessage::arrivedOn(const char *gatename) const
{
    cGate *arrgate = getArrivalGate();
    return arrgate && arrgate->isName(gatename);
}

bool cMessage::arrivedOn(const char *gatename, int gateindex) const
{
    cGate *arrgate = getArrivalGate();
    return arrgate && arrgate->isName(gatename) && arrgate->getIndex()==gateindex;
}

const char *cMessage::getDisplayString() const
{
    return ""; // clients may redefine this method to get messages with custom appearance
}


void cMessage::setSentFrom(cModule *module, int gateId, simtime_t t)
{
    frommod = module ? module->getId() : -1;
    fromgate = gateId;
    sent = t;
}

void cMessage::setArrival(cModule *module, int gateId, simtime_t t)
{
    tomod = module ? module->getId() : -1;
    togate = gateId;
    delivd = t;
}

void cMessage::setArrival(cModule *module, int gateId)
{
    tomod = module ? module->getId() : -1;
    togate = gateId;
}

void cMessage::setArrivalTime(simtime_t t)
{
    delivd = t;
}

//----

cPacket::cPacket(const cPacket& msg) : cMessage(msg)
{
    encapmsg = NULL;
}

cPacket::cPacket(const char *name, short k, int64 len) : cMessage(name, k)
{
    len = 0;
    encapmsg = NULL;
    duration = 0;
}

cPacket::~cPacket()
{
#ifdef WITHOUT_CPACKET
    EVCB.messageDeleted(this);
#endif

    if (encapmsg)
#ifdef REFCOUNTING
        _deleteEncapMsg();
#else
        dropAndDelete(encapmsg);
#endif
}

std::string cPacket::info() const  //FIXME revise
{
//    if (tomod<0)
//        return std::string("(new msg)");

    std::stringstream out;
//    const char *deletedstr = "<deleted module>";
//
//    if (delivd > simulation.getSimTime())
//    {
//        // if it arrived in the past, dt is usually unimportant, don't print it
//        out << "at T=" << delivd << ", in dt=" << (delivd - simulation.getSimTime()) << "; ";
//    }
//
//#define MODNAME(modp) ((modp) ? (modp)->getFullPath().c_str() : deletedstr)
//    if (getKind()==MK_STARTER)
//    {
//        cModule *tomodp = simulation.getModule(tomod);
//        out << "starter for " << MODNAME(tomodp) << " (id=" << tomod << ") ";
//    }
//    else if (getKind()==MK_TIMEOUT)
//    {
//        cModule *tomodp = simulation.getModule(tomod);
//        out << "timeoutmsg for " << MODNAME(tomodp) << " (id=" << tomod << ") ";
//    }
//    else if (frommod==tomod)
//    {
//        cModule *tomodp = simulation.getModule(tomod);
//        out << "selfmsg for " << MODNAME(tomodp) << " (id=" << tomod << ") ";
//    }
//    else
//    {
//        cModule *frommodp = simulation.getModule(frommod);
//        cModule *tomodp = simulation.getModule(tomod);
//        out << "src=" << MODNAME(frommodp) << " (id=" << frommod << ") ";
//        out << " dest=" << MODNAME(tomodp) << " (id=" << tomod << ") ";
//    }
//#undef MODNAME
//
//    if (encapmsg)
//        // #ifdef REFCOUNTING const_cast<cPacket *>(this)->_detachEncapMsg();  // see _detachEncapMsg() comment why this might be needed
//        out << "  encapsulates: (" << encapmsg->getClassName() << ")" << encapmsg->getFullName();
//
//    if (ctrlp)
//        out << "  control info: (" << ctrlp->getClassName() << ") " << ctrlp->getFullName() << "\n";
//
    return out.str();
}

void cPacket::forEachChild(cVisitor *v)
{
    cMessage::forEachChild(v);

    if (encapmsg)
    {
#ifdef REFCOUNTING
        _detachEncapMsg();  // see method comment why this is needed
#endif
        v->visit(encapmsg);
    }
}

std::string cPacket::detailedInfo() const
{
    return "";  // all fields are available via reflection, no point in repeating them here
}

void cPacket::parsimPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cMessage::parsimPack(buffer);
    buffer->pack(len);
    buffer->pack(duration);
    if (buffer->packFlag(encapmsg!=NULL))
        buffer->packObject(encapmsg);
#endif
}

void cPacket::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    ASSERT(sharecount==0);
    cMessage::parsimUnpack(buffer);
    buffer->unpack(len);
    buffer->unpack(duration);
    if (buffer->checkFlag())
        take(encapmsg = (cPacket *) buffer->unpackObject());
#endif
}

cPacket& cPacket::operator=(const cPacket& msg)
{
    if (this==&msg) return *this;

    cMessage::operator=(msg);

#ifdef REFCOUNTING
    if (sharecount!=0)
        throw cRuntimeError(this,"operator=(): this message is refcounted (shared between "
                                 "several messages), it is forbidden to change it");
#endif

    len = msg.len;
    duration = msg.duration;

#ifndef REFCOUNTING
    dropAndDelete(encapmsg);
    if (msg.encapmsg)
        take(encapmsg = (cPacket *)msg.encapmsg->dup());
    else
        encapmsg = NULL;
#else
    if (encapmsg)
        _deleteEncapMsg();
    encapmsg = msg.encapmsg;
    if (encapmsg && ++encapmsg->sharecount==0)   // sharecount overflow
    {
        --encapmsg->sharecount;
        take(encapmsg = (cPacket *)encapmsg->dup());
    }
#endif

    return *this;
}

#ifdef REFCOUNTING
void cPacket::_deleteEncapMsg()
{
    if (encapmsg->sharecount>0)
    {
        encapmsg->sharecount--;
        if (encapmsg->ownerp == this)
            encapmsg->ownerp = NULL;
    }
    else
    {
        // note: dropAndDelete(encapmsg) cannot be used, because due to sharecounting
        // ownerp is not valid (may be any former owner, possibly deleted since then)
        encapmsg->ownerp = NULL;
        delete encapmsg;
    }
}
#endif

#ifdef REFCOUNTING
void cPacket::_detachEncapMsg()
{
    if (encapmsg->sharecount>0)
    {
        // "de-share" object - create our own copy
        encapmsg->sharecount--;
        if (encapmsg->ownerp == this)
            encapmsg->ownerp = NULL;
        take(encapmsg = (cPacket *)encapmsg->dup());
    }
    else
    {
        // note: due to sharecounting, ownerp may be pointing to a previous owner -- fix it
        encapmsg->ownerp = this;
    }
}
#endif

void cPacket::setBitLength(int64 l)
{
    if (l<0)
        throw cRuntimeError(this,"setBitLength(): negative length %"INT64_PRINTF_FORMAT"d", l);
    len = l;
}

void cPacket::addBitLength(int64 l)
{
    len += l;
    if (len<0)
        throw cRuntimeError(this,"addBitLength(): length became negative (%"INT64_PRINTF_FORMAT") after adding %"INT64_PRINTF_FORMAT"d", len, l);
}

void cPacket::encapsulate(cPacket *msg)
{
    if (encapmsg)
        throw cRuntimeError(this,"encapsulate(): another message already encapsulated");

    if (msg)
    {
        if (msg->getOwner()!=simulation.getContextSimpleModule())
            throw cRuntimeError(this,"encapsulate(): not owner of message (%s)%s, owner is (%s)%s",
                                msg->getClassName(), msg->getFullName(),
                                msg->getOwner()->getClassName(), msg->getOwner()->getFullPath().c_str());
        take(encapmsg = msg);
#ifdef REFCOUNTING
        ASSERT(encapmsg->sharecount==0);
#endif
        len += encapmsg->len;
    }
}

cPacket *cPacket::decapsulate()
{
    if (!encapmsg)
        return NULL;
    if (len>0)
        len -= encapmsg->len;
    if (len<0)
        throw cRuntimeError(this,"decapsulate(): packet length is smaller than encapsulated packet");

#ifdef REFCOUNTING
    if (encapmsg->sharecount>0)
    {
        encapmsg->sharecount--;
        if (encapmsg->ownerp == this)
            encapmsg->ownerp = NULL;
        cPacket *msg = encapmsg->dup();
        encapmsg = NULL;
        return msg;
    }
    encapmsg->ownerp = this;
#endif
    cPacket *msg = encapmsg;
    encapmsg = NULL;
    if (msg) drop(msg);
    return msg;
}

cPacket *cPacket::getEncapsulatedMsg() const
{
#ifdef REFCOUNTING
    // encapmsg may be shared (sharecount>0) -- we'll make our own copy,
    // so that other messages are not affected in case the user modifies
    // the encapsulated message via the returned pointer.
    // Trick: this is a const method, so we can only do changes via a
    // non-const copy of the 'this' pointer.
    if (encapmsg)
        const_cast<cPacket *>(this)->_detachEncapMsg();
#endif
    return encapmsg;
}

long cPacket::getEncapsulationId() const
{
    // find innermost msg. Note: don't use getEncapsulatedMsg() because it does copy-on-touch of shared msgs
    const cPacket *msg = this;
    while (msg->encapmsg)
        msg = msg->encapmsg;
    return msg->getId();
}

long cPacket::getEncapsulationTreeId() const
{
    // find innermost msg. Note: don't use getEncapsulatedMsg() because it does copy-on-touch of shared msgs
    const cPacket *msg = this;
    while (msg->encapmsg)
        msg = msg->encapmsg;
    return msg->getTreeId();
}





