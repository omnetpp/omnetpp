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
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>            // pow
#include <stdio.h>           // sprintf
#include <string.h>          // strcpy
#include "macros.h"
#include "cmodule.h"
#include "cmessage.h"
#include "cexception.h"

//=== registration
Register_Class( cMessage )

// static members of cMessage
unsigned long cMessage::total_msgs;
unsigned long cMessage::live_msgs;

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
    // parlistp is deleted by ~cObject()
    live_msgs--;
}

void cMessage::info(char *buf)
{
    cObject::info( buf );

    // find end of string and append a space
    char *b = buf;
    while(*b) b++;
    *b++ = ' '; *b='\0';

    // append useful info
    if (tomod<0)
        sprintf(b,"(new msg)");
    else if (kind()==MK_STARTER)
        sprintf(b,"starter for #%d T=%s",tomod,simtimeToStr(delivd));
    else if (kind()==MK_TIMEOUT)
        sprintf(b,"timeout for #%d T=%s",tomod,simtimeToStr(delivd));
    else if (frommod==tomod)
        sprintf(b,"selfmsg for #%d T=%s",tomod,simtimeToStr(delivd));
    else
        sprintf(b,"src=#%d dest=#%d T=%s", frommod,tomod,simtimeToStr(delivd));
}

void cMessage::forEach( ForeachFunc do_fn )
{
    if (do_fn(this,true))
        if (parlistp) parlistp->forEach( do_fn );
    do_fn(this,false);
}

void cMessage::writeContents(ostream& os)
{
    os << "  #" << senderModuleId() << " --> #" << arrivalModuleId()  << '\n';
    os << "  sent:      " << simtimeToStr(sendingTime()) << '\n';
    os << "  arrived:   " << simtimeToStr(arrivalTime()) << '\n';
    os << "  length:    " << length() << '\n';
    os << "  kind:      " << kind() << '\n';
    os << "  priority:  " << priority() << '\n';
    os << "  error:     " << (hasBitError() ? "true" : "false") << '\n';
    os << "  time stamp:" << simtimeToStr(timestamp()) << '\n';
    if (parlistp) {
        os << "  parameter list:\n";
        parlistp->writeContents( os );
    } else {
        os << "  no parameter list\n";
    }
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

    created = msg.created;  // hmm...

    if (parlistp)
        delete parlistp;
    if (msg.parlistp && msg.parlistp->owner()==const_cast<cMessage*>(&msg))
        take( parlistp = (cArray *)msg.parlistp->dup() );
    else
        parlistp = msg.parlistp;

    if (encapmsg && encapmsg->owner()==this)
        dealloc( encapmsg );
    if (msg.encapmsg && msg.encapmsg->owner()==const_cast<cMessage*>(&msg))
        take( encapmsg = (cMessage *)msg.encapmsg->dup() );
    else
        encapmsg = msg.encapmsg;

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
        throw new cException("(%s)%s: setLength(): negative length %ld",className(),fullName(),l);
    len=l;
}

void cMessage::addLength(long l)
{
    len+=l;
    if (len<0)
        throw new cException("(%s)%s: addLength(): length became negative (%ld)",className(),fullName(),len);
}

void cMessage::encapsulate(cMessage *msg)
{
    if (encapmsg)
        throw new cException("(%s)%s: encapsulate(): another message already encapsulated", className(),fullName());

    if (msg)
    {
        if (msg->owner()!=&(simulation.contextSimpleModule()->locals))
            throw new cException("(%s)%s: encapsulate(): not owner of message", className(),fullName());
        take( encapmsg = msg );
        len += encapmsg->len;
    }
}

cMessage *cMessage::decapsulate()
{
    if ((len>0) && encapmsg)
        len-=encapmsg->len;
    if (len<0)
        throw new cException("(%s)%s: decapsulate(): msg length smaller than encapsulated msg length",className(),fullName());

    cMessage *msg = encapmsg;
    encapmsg = NULL;
    if (msg) drop(msg);
    return msg;
}

cPar& cMessage::par(int n)
{
    cArray& parlist = parList();
    cPar *p = (cPar *)parlist[n];
    if (p)
        return *p;
    else
    {
        opp_warning("(%s)%s: par(): no parameter #%d",className(),fullName(),n);
        return *NO(cPar);
    }
}

cPar& cMessage::par(const char *s)
{
    cArray& parlist = parList();
    int pn = parlist.find( s );
    if (pn!=-1)
        return *(cPar *)parlist[pn];
    else
    {
        opp_warning("(%s)%s: par(): no parameter called `%s'",className(),fullName(),s);
        return *NO(cPar);
    }
}

int cMessage::findPar(const char *s) const
{
    if (!parlistp) return -1;
    return parlistp->find( s );
    // alternatively: return parlistp ? parlistp->find( s ) : -1;

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


