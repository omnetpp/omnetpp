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
  Copyright (C) 1992-2005 Andras Varga

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


Register_Class(cMessage);

// static members of cMessage
long cMessage::next_msg_id = 0;
long cMessage::total_msgs = 0;
long cMessage::live_msgs = 0;


cMessage::cMessage(const cMessage& msg) : cOwnedObject(msg)
{
    sharecount = 0;
    parlistp = NULL;
    encapmsg = NULL;
    ctrlp = NULL;
    heapindex = -1;
    prev_event_num = -1;
    operator=(msg);

    msg_tree_id = msg.msg_tree_id;
    msg_seq_id = next_msg_id++;
    total_msgs++;
    live_msgs++;
}

cMessage::cMessage(const char *name, short k, int64 ln, short pri, bool err) : cOwnedObject(name,false)
{
    // name pooling is off for messages by default, as unique names are quite common
    msgkind=k; len=ln; prior=pri; error=err;
    parlistp = NULL;
    encapmsg = NULL;
    contextptr = NULL;
    ctrlp = NULL;
    sharecount = 0;
    srcprocid = -1;

    frommod = fromgate = -1;
    tomod = togate = -1;
    created = simulation.simTime();
    sent = delivd = tstamp = 0;
    heapindex = -1;
    prev_event_num = -1;

    msg_tree_id = msg_seq_id = next_msg_id++;
    total_msgs++;
    live_msgs++;
}

cMessage::~cMessage()
{
    EVCB.messageDeleted(this);  //XXX verify this is really needed

    if (parlistp)
        dropAndDelete(parlistp);
    if (encapmsg)
#ifdef REFCOUNTING
        _deleteEncapMsg();
#else
        dropAndDelete(encapmsg);
#endif

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

    if (delivd > simulation.simTime())
    {
        // if it arrived in the past, dt is usually unimportant, don't print it
        out << "at T=" << delivd << ", in dt=" << (delivd - simulation.simTime()) << "; ";
    }

#define MODNAME(modp) ((modp) ? (modp)->fullPath().c_str() : deletedstr)
    if (kind()==MK_STARTER)
    {
        cModule *tomodp = simulation.module(tomod);
        out << "starter for " << MODNAME(tomodp) << " (id=" << tomod << ") ";
    }
    else if (kind()==MK_TIMEOUT)
    {
        cModule *tomodp = simulation.module(tomod);
        out << "timeoutmsg for " << MODNAME(tomodp) << " (id=" << tomod << ") ";
    }
    else if (frommod==tomod)
    {
        cModule *tomodp = simulation.module(tomod);
        out << "selfmsg for " << MODNAME(tomodp) << " (id=" << tomod << ") ";
    }
    else
    {
        cModule *frommodp = simulation.module(frommod);
        cModule *tomodp = simulation.module(tomod);
        out << "src=" << MODNAME(frommodp) << " (id=" << frommod << ") ";
        out << " dest=" << MODNAME(tomodp) << " (id=" << tomod << ") ";
    }
#undef MODNAME

    if (encapmsg)
        // #ifdef REFCOUNTING const_cast<cMessage *>(this)->_detachEncapMsg();  // see _detachEncapMsg() comment why this might be needed
        out << "  encapsulates: (" << encapmsg->className() << ")" << encapmsg->fullName();

    if (ctrlp)
        out << "  control info: (" << ctrlp->className() << ") " << ctrlp->fullName() << "\n";

    return out.str();
}

void cMessage::forEachChild(cVisitor *v)
{
    if (parlistp)
        v->visit(parlistp);
    if (encapmsg)
    {
#ifdef REFCOUNTING
        _detachEncapMsg();  // see method comment why this is needed
#endif
        v->visit(encapmsg);
    }
}

std::string cMessage::detailedInfo() const
{
    return "";  // all fields are available via reflection, no point in repeating them here
}

void cMessage::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cOwnedObject::netPack(buffer);

    if (contextptr || ctrlp)
        throw cRuntimeError(this,"netPack(): cannot pack object with contextPointer or controlInfo set");

    buffer->pack(msgkind);
    buffer->pack(prior);
    buffer->pack(len);
    buffer->pack(error);
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

    if (buffer->packFlag(parlistp!=NULL))
        buffer->packObject(parlistp);

    if (buffer->packFlag(encapmsg!=NULL))
        buffer->packObject(encapmsg);
#endif
}

void cMessage::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cOwnedObject::netUnpack(buffer);

    ASSERT(sharecount==0);
    buffer->unpack(msgkind);
    buffer->unpack(prior);
    buffer->unpack(len);
    bool tmp;
    buffer->unpack(tmp);
    error = tmp;
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

    if (buffer->checkFlag())
        take(encapmsg = (cMessage *) buffer->unpackObject());
#endif
}

cMessage& cMessage::operator=(const cMessage& msg)
{
    if (this==&msg) return *this;

    if (sharecount!=0)
        throw cRuntimeError(this,"operator=(): this message is refcounted (shared between "
                                 "several messages), it is forbidden to change it");

    cOwnedObject::operator=(msg);

    msgkind = msg.msgkind;
    prior = msg.prior;
    len = msg.len;
    error = msg.error;
    tstamp = msg.tstamp;
    srcprocid = msg.srcprocid;

    created = msg.created;

    dropAndDelete(parlistp);
    if (msg.parlistp)
        take(parlistp = (cArray *)msg.parlistp->dup());
    else
        parlistp = NULL;

#ifndef REFCOUNTING
    dropAndDelete(encapmsg);
    if (msg.encapmsg)
        take(encapmsg = (cMessage *)msg.encapmsg->dup());
    else
        encapmsg = NULL;
#else
    if (encapmsg)
        _deleteEncapMsg();
    encapmsg = msg.encapmsg;
    if (encapmsg && ++encapmsg->sharecount==0)   // sharecount overflow
    {
        --encapmsg->sharecount;
        take(encapmsg = (cMessage *)encapmsg->dup());
    }
#endif

    contextptr = msg.contextptr;

    frommod = msg.frommod;
    fromgate = msg.fromgate;
    tomod = msg.tomod;
    togate = msg.togate;

    sent = msg.sent;
    delivd = msg.delivd;

    // NOTE: do not copy msg_tree_id, msg_seq_id, and prev_event_num!

    return *this;
}

void cMessage::_createparlist()
{
    parlistp = new cArray("parameters", 5, 5);
    take(parlistp);
}

#ifdef REFCOUNTING
void cMessage::_deleteEncapMsg()
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
void cMessage::_detachEncapMsg()
{
    if (encapmsg->sharecount>0)
    {
        // "de-share" object - create our own copy
        encapmsg->sharecount--;
        if (encapmsg->ownerp == this)
            encapmsg->ownerp = NULL;
        take(encapmsg = (cMessage *)encapmsg->dup());
    }
    else
    {
        // note: due to sharecounting, ownerp may be pointing to a previous owner -- fix it
        encapmsg->ownerp = this;
    }
}
#endif

void cMessage::setLength(int64 l)
{
    if (l<0)
        throw cRuntimeError(this,"setLength(): negative length %"INT64_PRINTF_FORMAT"d", l);
    len=l;
}

void cMessage::addLength(int64 l)
{
    len += l;
    if (len<0)
        throw cRuntimeError(this,"addLength(): length became negative (%"INT64_PRINTF_FORMAT") after adding %"INT64_PRINTF_FORMAT"d", len, l);
}

void cMessage::encapsulate(cMessage *msg)
{
    if (encapmsg)
        throw cRuntimeError(this,"encapsulate(): another message already encapsulated");

    if (msg)
    {
        if (msg->owner()!=simulation.contextSimpleModule())
            throw cRuntimeError(this,"encapsulate(): not owner of message (%s)%s, owner is (%s)%s",
                                    msg->className(), msg->fullName(),
                                    msg->owner()->className(), msg->owner()->fullPath().c_str());
        take(encapmsg = msg);
#ifdef REFCOUNTING
        ASSERT(encapmsg->sharecount==0);
#endif
        len += encapmsg->len;
    }
}

cMessage *cMessage::decapsulate()
{
    if (!encapmsg)
        return NULL;
    if (len>0)
        len -= encapmsg->len;
    if (len<0)
        throw cRuntimeError(this,"decapsulate(): msg length is smaller than encapsulated msg length");

#ifdef REFCOUNTING
    if (encapmsg->sharecount>0)
    {
        encapmsg->sharecount--;
        if (encapmsg->ownerp == this)
            encapmsg->ownerp = NULL;
        cMessage *msg = (cMessage *)encapmsg->dup();
        encapmsg = NULL;
        return msg;
    }
    encapmsg->ownerp = this;
#endif
    cMessage *msg = encapmsg;
    encapmsg = NULL;
    if (msg) drop(msg);
    return msg;
}

cMessage *cMessage::encapsulatedMsg() const
{
#ifdef REFCOUNTING
    // encapmsg may be shared (sharecount>0) -- we'll make our own copy,
    // so that other messages are not affected in case the user modifies
    // the encapsulated message via the returned pointer.
    // Trick: this is a const method, so we can only do changes via a
    // non-const copy of the 'this' pointer.
    if (encapmsg)
        const_cast<cMessage *>(this)->_detachEncapMsg();
#endif
    return encapmsg;
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

long cMessage::encapsulationId() const
{
    // find innermost msg. Note: don't use encapsulatedMsg() because it does copy-on-touch of shared msgs
    if (!encapmsg)
        return -1;
    const cMessage *msg = this;
    while (msg->encapmsg)
        msg = msg->encapmsg;
    return msg->id();
}

long cMessage::encapsulationTreeId() const
{
    // find innermost msg. Note: don't use encapsulatedMsg() because it does copy-on-touch of shared msgs
    if (!encapmsg)
        return -1;
    const cMessage *msg = this;
    while (msg->encapmsg)
        msg = msg->encapmsg;
    return msg->treeId();
}

cMsgPar& cMessage::par(int n)
{
    cArray& parlist = parList();
    cObject *p = parlist.get(n);
    if (!p)
        throw cRuntimeError(this,"par(int): has no parameter #%d",n);
    if (!dynamic_cast<cMsgPar *>(p))
        throw cRuntimeError(this,"par(int): parameter #%d is of type %s, not cMsgPar",n, p->className());
    return *(cMsgPar *)p;
}

cMsgPar& cMessage::par(const char *s)
{
    cArray& parlist = parList();
    cObject *p = parlist.get(s);
    if (!p)
        throw cRuntimeError(this,"par(const char *): has no parameter called `%s'",s);
    if (!dynamic_cast<cMsgPar *>(p))
        throw cRuntimeError(this,"par(const char *): parameter `%s' is of type %s, not cMsgPar",s, p->className());
    return *(cMsgPar *)p;
}

int cMessage::findPar(const char *s) const
{
    if (!parlistp)
        return -1;
    return parlistp->find(s);
}

cGate *cMessage::senderGate() const
{
    if (frommod<0 || fromgate<0)  return NULL;
    cModule *mod = simulation.module(frommod);
    if (!mod) return NULL;
    return mod->gate( fromgate );
}

cGate *cMessage::arrivalGate() const
{
    if (tomod<0 || togate<0)  return NULL;
    cModule *mod = simulation.module(tomod);
    if (!mod) return NULL;
    return mod->gate(togate);
}

bool cMessage::arrivedOn(const char *s)
{
    cGate *arrgate = arrivalGate();
    if (!arrgate) return false;
    return arrgate->isName(s);
}

bool cMessage::arrivedOn(const char *s, int gateindex)
{
    cGate *arrgate = arrivalGate();
    if (!arrgate) return false;
    return arrgate->isName(s) && arrgate->index()==gateindex;
}

const char *cMessage::displayString() const
{
    // redefine to get messages with custom appearance
    return "";
}


void cMessage::setSentFrom(cModule *module, int gate, simtime_t t)
{
    frommod = module ? module->id() : -1;
    fromgate = gate;
    sent = t;
}

void cMessage::setArrival(cModule *module, int gate, simtime_t t)
{
    tomod = module ? module->id() : -1;
    togate = gate;
    delivd = t;
}

void cMessage::setArrival(cModule *module, int gate)
{
    tomod = module ? module->id() : -1;
    togate = gate;
}

void cMessage::setArrivalTime(simtime_t t)
{
    delivd = t;
}


