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

#include <math.h>            // pow
#include <stdio.h>           // sprintf
#include <string.h>          // strcpy
#include "macros.h"
#include "cmodule.h"
#include "csimplemodule.h"
#include "cmessage.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

// comment out to disable reference-counting for encapsulated messages
#define REFCOUNTING

using std::ostream;

//=== registration
Register_Class(cMessage);

// static members of cMessage
long cMessage::total_msgs = 0;
long cMessage::live_msgs = 0;

//=========================================================================
//=== cMessage - member functions

cMessage::cMessage(const cMessage& msg) : cObject()
{
    sharecount = 0;
    parlistp = NULL;
    encapmsg = NULL;
    ctrlp = NULL;
    setName( msg.name() );
    operator=( msg );

    total_msgs++;
    live_msgs++;
}

cMessage::cMessage(const char *name, int k, long ln, int pri, bool err) : cObject(name)
{
    msgkind=k; len=ln; prior=pri; error=err;
    parlistp = NULL;
    encapmsg = NULL;
    contextptr = NULL;
    ctrlp = NULL;
    sharecount = 0;
    srcprocid = -1;

    frommod=fromgate=-1;
    tomod=togate=-1;
    created=simulation.simTime();
    sent=delivd=tstamp=0;
    heapindex=-1;

    total_msgs++;
    live_msgs++;
}

cMessage::~cMessage()
{
    if (parlistp)
        dropAndDelete(parlistp);
    if (encapmsg)
#ifdef REFCOUNTING
        _deleteEncapMsg();
#else
        dropAndDelete(encapmsg);
#endif

    if (dynamic_cast<cObject *>(ctrlp))
        dropAndDelete((cObject *)ctrlp);
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
        out << "at T=" << simtimeToStrShort(delivd);
        out << ", in dt=" << simtimeToStrShort(delivd-simulation.simTime()) << "; ";
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

void cMessage::writeContents(ostream& os)
{
    os << "  sender:    id=" << senderModuleId() << '\n';
    os << "  dest.:     id=" << arrivalModuleId() << '\n';
    os << "  sent:      " << simtimeToStr(sendingTime()) << '\n';
    os << "  arrived:   " << simtimeToStr(arrivalTime()) << '\n';
    os << "  length:    " << length() << '\n';
    os << "  kind:      " << kind() << '\n';
    os << "  priority:  " << priority() << '\n';
    os << "  error:     " << (hasBitError() ? "true" : "false") << '\n';
    os << "  timestamp: " << simtimeToStr(timestamp()) << '\n';
    if (parlistp) {
        os << "  parameter list:\n";
        parlistp->writeContents( os );
    } else {
        os << "  no parameter list\n";
    }
    if (encapmsg) {
        os << "  encapsulated message:\n";
#ifdef REFCOUNTING
        _detachEncapMsg();  // see method comment why this is needed
#endif
        encapmsg->writeContents( os );
    }
    if (ctrlp) {
        os << "  control info: (" << ctrlp->className() << ") " << ctrlp->detailedInfo() << "\n";
    }
}

void cMessage::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    cObject::netPack(buffer);

    if (contextptr || ctrlp)
        throw new cRuntimeError(this,"netPack(): cannot pack object with contextPointer or controlInfo set");

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
    throw new cRuntimeError(this,eNOPARSIM);
#else
    cObject::netUnpack(buffer);

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
        throw new cRuntimeError(this,"operator=(): this message is refcounted (shared between "
                                     "several messages), it is forbidden to change it");

    cObject::operator=(msg);

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

void cMessage::setLength(long l)
{
    if (l<0)
        throw new cRuntimeError(this,"setLength(): negative length %ld",l);
    len=l;
}

void cMessage::addLength(long l)
{
    len+=l;
    if (len<0)
        throw new cRuntimeError(this,"addLength(): length became negative (%ld) after adding %ld",len,l);
}

void cMessage::encapsulate(cMessage *msg)
{
    if (encapmsg)
        throw new cRuntimeError(this,"encapsulate(): another message already encapsulated");

    if (msg)
    {
        if (msg->owner()!=simulation.contextSimpleModule())
            throw new cRuntimeError(this,"encapsulate(): not owner of message (%s)%s, owner is (%s)%s",
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
        throw new cRuntimeError(this,"decapsulate(): msg length is smaller than encapsulated msg length");

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

void cMessage::setControlInfo(cPolymorphic *p)
{
    if (!p)
        throw new cRuntimeError(this,"setControlInfo(): pointer is NULL");
    if (ctrlp)
        throw new cRuntimeError(this,"setControlInfo(): message already has control info attached");
    if (dynamic_cast<cObject *>(p))
        take((cObject *)p);
    ctrlp = p;
}

cPolymorphic *cMessage::removeControlInfo()
{
    cPolymorphic *p = ctrlp;
    ctrlp = NULL;
    if (dynamic_cast<cObject *>(p))
        drop((cObject *)p);
    return p;
}

cPar& cMessage::par(int n)
{
    cArray& parlist = parList();
    cObject *p = parlist.get(n);
    if (!p)
        throw new cRuntimeError(this,"par(int): has no parameter #%d",n);
    if (!dynamic_cast<cPar *>(p))
        throw new cRuntimeError(this,"par(int): parameter #%d is of type %s, not cPar",n, p->className());
    return *(cPar *)p;
}

cPar& cMessage::par(const char *s)
{
    cArray& parlist = parList();
    cObject *p = parlist.get(s);
    if (!p)
        throw new cRuntimeError(this,"par(const char *): has no parameter called `%s'",s);
    if (!dynamic_cast<cPar *>(p))
        throw new cRuntimeError(this,"par(const char *): parameter `%s' is of type %s, not cPar",s, p->className());
    return *(cPar *)p;
}

int cMessage::findPar(const char *s) const
{
    if (!parlistp)
        return -1;
    return parlistp->find(s);
}

bool cMessage::hasPar(const char *s) const
{
    return parlistp ? parlistp->find(s)>=0 : false;
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

int cMessage::cmpbydelivtime(cObject *one, cObject *other)
{
    // compare by delivery time and priority
    cMessage *msg1 = (cMessage*)one,
             *msg2 = (cMessage*)other;
    simtime_t x = msg1->delivd - msg2->delivd;
    return (x!=0) ? (x>0 ? 1 : -1) : (msg1->prior - msg2->prior);
}

int cMessage::cmpbypriority(cObject *one, cObject *other)
{
    int x = ((cMessage*)one)->prior - ((cMessage*)other)->prior;
    return sgn(x);
}


