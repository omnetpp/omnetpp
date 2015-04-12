//========================================================================
//  CMESSAGE.CC - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sstream>
#include "omnetpp/globals.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

NAMESPACE_BEGIN

using std::ostream;

Register_Class(cMessage);

// static members of cMessage
long cMessage::next_id = 0;
long cMessage::total_msgs = 0;
long cMessage::live_msgs = 0;


cMessage::cMessage(const cMessage& msg) : cEvent(msg)
{
    parlistp = NULL;
    ctrlp = NULL;
    heapindex = -1;
    copy(msg);

    msgid = next_id++;
    total_msgs++;
    live_msgs++;

    cMessage *nonConstMsg = const_cast<cMessage*>(&msg);
    EVCB.messageCloned(nonConstMsg, this);

    // after envir notification
    nonConstMsg->prev_event_num = prev_event_num = simulation.getEventNumber();
}

cMessage::cMessage(const char *name, short k) : cEvent(name)
{
    // name pooling is off for messages by default, as unique names are quite common
    msgkind = k;
    parlistp = NULL;
    contextptr = NULL;
    ctrlp = NULL;
    srcprocid = -1;

    frommod = fromgate = -1;
    tomod = togate = -1;
    created = simulation.getSimTime();
    sent = tstamp = 0;

    msgtreeid = msgid = next_id++;
    total_msgs++;
    live_msgs++;

    prev_event_num = -1;
    EVCB.messageCreated(this);

    // after envir notification
    prev_event_num = simulation.getEventNumber();
}

cMessage::~cMessage()
{
    EVCB.messageDeleted(this);

    if (parlistp)
        dropAndDelete(parlistp);

    if (ctrlp) {
        if (ctrlp->isOwnedObject())
            dropAndDelete((cOwnedObject *)ctrlp);
        else
            delete ctrlp;
    }

    if ((flags & FL_ISPRIVATECOPY) == 0)
        live_msgs--;
}

std::string cMessage::info() const
{
    if (tomod<0)
        return std::string("(new msg)");

    std::stringstream out;
    const char *deletedstr = "<deleted module>";

    simtime_t t = getArrivalTime();
    if (t > simulation.getSimTime())
    {
        // if it arrived in the past, dt is usually unimportant, don't print it
        out << "at T=" << t << ", in dt=" << (t - simulation.getSimTime()) << "; ";
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
    if (ctrlp)
        v->visit(ctrlp);
}

std::string cMessage::detailedInfo() const
{
    return "";  // all fields are available via reflection, no point in repeating them here
}

void cMessage::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,E_NOPARSIM);
#else
    cEvent::parsimPack(buffer);

    if (contextptr || ctrlp)
        throw cRuntimeError(this,"parsimPack(): cannot pack object with contextPointer or controlInfo set");

    buffer->pack(msgkind);
    buffer->pack(tstamp);
    buffer->pack(frommod);
    buffer->pack(fromgate);
    buffer->pack(tomod);
    buffer->pack(togate);
    buffer->pack(created);
    buffer->pack(sent);

    // note: do not pack msgid and treeid, because they'd conflict
    // with ids assigned at the destination partition

    if (buffer->packFlag(parlistp!=NULL))
        buffer->packObject(parlistp);
#endif
}

void cMessage::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,E_NOPARSIM);
#else
    cEvent::parsimUnpack(buffer);

    buffer->unpack(msgkind);
    buffer->unpack(tstamp);
    buffer->unpack(frommod);
    buffer->unpack(fromgate);
    buffer->unpack(tomod);
    buffer->unpack(togate);
    buffer->unpack(created);
    buffer->unpack(sent);

    if (buffer->checkFlag())
        take(parlistp = (cArray *) buffer->unpackObject());
#endif
}

cMessage& cMessage::operator=(const cMessage& msg)
{
    if (this==&msg) return *this;
    cEvent::operator=(msg);
    copy(msg);
    return *this;
}

void cMessage::copy(const cMessage& msg)
{
    msgkind = msg.msgkind;
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

    msgtreeid = msg.msgtreeid;
}

void cMessage::_createparlist()
{
    parlistp = new cArray("parameters", 5, 5);
    take(parlistp);
}

cMessage *cMessage::privateDup() const
{
    cMessage *ret = dup();
    ret->msgid = msgid;
    ret->flags |= FL_ISPRIVATECOPY;
    ret->removeFromOwnershipTree();
    total_msgs--;
    live_msgs--;
    return ret;
}

void cMessage::setControlInfo(cObject *p)
{
    if (!p)
        throw cRuntimeError(this,"setControlInfo(): pointer is NULL");
    if (ctrlp)
        throw cRuntimeError(this,"setControlInfo(): message already has control info attached");
    if (p->isOwnedObject())
        take((cOwnedObject *)p);
    ctrlp = p;
}

cObject *cMessage::removeControlInfo()
{
    cObject *p = ctrlp;
    ctrlp = NULL;
    if (p && p->isOwnedObject())
        drop((cOwnedObject *)p);
    return p;
}

cObject *cMessage::getTargetObject() const
{
    return getArrivalModule();
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

void cMessage::setSentFrom(cModule *module, int gateId, simtime_t_cref t)
{
    frommod = module ? module->getId() : -1;
    fromgate = gateId;
    sent = t;
}

void cMessage::setArrival(cModule *module, int gateId, simtime_t_cref t)
{
    setArrival(module ? module->getId() : -1, gateId, t);
}

bool cMessage::isStale()
{
    // check that destination module still exists and is alive
    cSimpleModule *modp = dynamic_cast<cSimpleModule *>(simulation.getModule(tomod));
    return !modp || modp->isTerminated();
}

void cMessage::execute()
{
    throw new cRuntimeError("illegal call to cMessage::execute()");
}

NAMESPACE_END

