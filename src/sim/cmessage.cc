//========================================================================
//
//  CMESSAGE.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
//   Member functions of
//    cMessage       : the message class
//
//  Author: Andras Varga
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

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
#include "parsim/ccommbuffer.h"
#endif

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
    parlistp = NULL;
    encapmsg = NULL;
    setName( msg.name() );
    operator=( msg );

    total_msgs++;
    live_msgs++;
}

cMessage::cMessage(const char *name, int k, long ln, int pri, bool err) : cObject( name )
{
    msgkind=k; len=ln; prior=pri; error=err;
    parlistp = NULL;
    encapmsg = NULL;
    contextptr = NULL;
    refcount = 0;
    srcprocid = 0;

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
    dropAndDelete(parlistp);
    dropAndDelete(encapmsg);
    live_msgs--;
}

void cMessage::info(char *buf)
{
    char *b = buf;
    *b='\0';

    const char *deletedstr = "<deleted module>";

    // append useful info
    if (tomod<0)
    {
        sprintf(b,"(new msg)");
    }
    else if (kind()==MK_STARTER)
    {
        cModule *tomodp = simulation.module(tomod);
        sprintf(b,"T=%s starter for %s (id=%d) ",
                simtimeToStr(delivd),
                tomodp ? tomodp->fullPath() : deletedstr,
                tomod);
    }
    else if (kind()==MK_TIMEOUT)
    {
        cModule *tomodp = simulation.module(tomod);
        sprintf(b,"T=%s timeout for %s (id=%d) ",
                simtimeToStr(delivd),
                tomodp ? tomodp->fullPath() : deletedstr,
                tomod);
    }
    else if (frommod==tomod)
    {
        cModule *tomodp = simulation.module(tomod);
        sprintf(b,"T=%s selfmsg for %s (id=%d) ",
                simtimeToStr(delivd),
                tomodp ? tomodp->fullPath() : deletedstr,
                tomod);
    }
    else
    {
        // 2 sprintfs cannot be merged because of static buffer in fullPath()
        cModule *frommodp = simulation.module(frommod);
        cModule *tomodp = simulation.module(tomod);
        sprintf(b,"T=%s src=%s (id=%d), ",
                simtimeToStr(delivd),
                frommodp ? frommodp->fullPath() : deletedstr,
                frommod);
        while(*b) b++;
        sprintf(b,"dest=%s (id=%d) ",
                tomodp ? tomodp->fullPath() : deletedstr,
                tomod);
    }
}

void cMessage::forEach( ForeachFunc do_fn )
{
    if (do_fn(this,true))
    {
        if (parlistp)
            parlistp->forEach( do_fn );
        if (encapmsg)
            encapmsg->forEach( do_fn );
    }
    do_fn(this,false);
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
        encapmsg->writeContents( os );
    }
}

void cMessage::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cException(this,eNOPARSIM);
#else
// FIXME what about refcount?
    cObject::netPack(buffer);

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

    if (notNull(parlistp, buffer))
        packObject(parlistp,buffer);

    if (notNull(encapmsg, buffer))
        packObject(encapmsg,buffer);
#endif
}

void cMessage::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cException(this,eNOPARSIM);
#else
// FIXME what about refcount?
    cObject::netUnpack(buffer);

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

    if (checkFlag(buffer))
        take(parlistp = (cArray *) unpackObject(buffer));

    if (checkFlag(buffer))
        take(encapmsg = (cMessage *) unpackObject(buffer));
#endif
}

cMessage& cMessage::operator=(const cMessage& msg)
{
    if (this==&msg) return *this;

    cObject::operator=(msg);

    msgkind = msg.msgkind;
    prior = msg.prior;
    len = msg.len;
    error = msg.error;
    tstamp = msg.tstamp;
    srcprocid = msg.srcprocid; // probably redundant
    // TBD refcount!!! should be left alone

    created = msg.created;  // hmm...

    dropAndDelete(parlistp);
    if (msg.parlistp)
        take( parlistp = (cArray *)msg.parlistp->dup() );
    else
        parlistp = NULL;

    dropAndDelete(encapmsg);
    if (msg.encapmsg)
        take( encapmsg = (cMessage *)msg.encapmsg->dup() );
    else
        encapmsg = NULL;

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
    parlistp = new cArray( "parameters", 5, 5 );
    take( parlistp );
}

void cMessage::setLength(long l)
{
    if (l<0)
        throw new cException(this,"setLength(): negative length %ld",l);
    len=l;
}

void cMessage::addLength(long l)
{
    len+=l;
    if (len<0)
        throw new cException(this,"addLength(): length became negative (%ld) after adding %ld",len,l);
}

void cMessage::encapsulate(cMessage *msg)
{
    if (encapmsg)
        throw new cException(this,"encapsulate(): another message already encapsulated");

    if (msg)
    {
        if (msg->owner()!=simulation.contextSimpleModule())
            throw new cException(this,"encapsulate(): not owner of message (%s)%s, owner is (%s)%s",
                                 msg->className(), msg->fullName(),
                                 msg->owner()->className(), msg->owner()->fullPath());
        take( encapmsg = msg );
        len += encapmsg->len;
    }
}

cMessage *cMessage::decapsulate()
{
    if ((len>0) && encapmsg)
        len-=encapmsg->len;
    if (len<0)
        throw new cException(this,"decapsulate(): msg length smaller than encapsulated msg length");

    cMessage *msg = encapmsg;
    encapmsg = NULL;
    if (msg) drop(msg);
    return msg;
}

cPar& cMessage::par(int n)
{
    cArray& parlist = parList();
    cObject *p = parlist.get(n);
    if (!p)
        throw new cException(this,"par(int): has no parameter #%d",n);
    if (!dynamic_cast<cPar *>(p))
        throw new cException(this,"par(int): parameter #%d is of type %s, not cPar",n, p->className());
    return *(cPar *)p;
}

cPar& cMessage::par(const char *s)
{
    cArray& parlist = parList();
    cObject *p = parlist.get(s);
    if (!p)
        throw new cException(this,"par(const char *): has no parameter called `%s'",s);
    if (!dynamic_cast<cPar *>(p))
        throw new cException(this,"par(const char *): parameter `%s' is of type %s, not cPar",s, p->className());
    return *(cPar *)p;
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
    cModule *mod = simulation.module( frommod );
    if (!mod) return NULL;
    return mod->gate( fromgate );
}

cGate *cMessage::arrivalGate() const
{
    if (tomod<0 || togate<0)  return NULL;
    cModule *mod = simulation.module( tomod );
    if (!mod) return NULL;
    return mod->gate( togate );
}

bool cMessage::arrivedOn(const char *s, int g)
{
    return togate==simulation.contextModule()->findGate(s,g);
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


