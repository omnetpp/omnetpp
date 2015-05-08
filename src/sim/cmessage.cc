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
long cMessage::nextMessageId = 0;
long cMessage::totalMsgCount = 0;
long cMessage::liveMsgCount = 0;


cMessage::cMessage(const cMessage& msg) : cEvent(msg)
{
    parList = NULL;
    controlInfo = NULL;
    heapIndex = -1;
    copy(msg);

    messageId = nextMessageId++;
    totalMsgCount++;
    liveMsgCount++;

    cMessage *nonConstMsg = const_cast<cMessage*>(&msg);
    EVCB.messageCloned(nonConstMsg, this);

    // after envir notification
    nonConstMsg->previousEventNumber = previousEventNumber = getSimulation()->getEventNumber();
}

cMessage::cMessage(const char *name, short k) : cEvent(name)
{
    // name pooling is off for messages by default, as unique names are quite common
    messageKind = k;
    parList = NULL;
    contextPointer = NULL;
    controlInfo = NULL;
    srcProcId = -1;

    senderModuleId = senderGateId = -1;
    targetModuleId = targetGateId = -1;
    creationTime = getSimulation()->getSimTime();
    sendTime = timestamp = 0;

    messageTreeId = messageId = nextMessageId++;
    totalMsgCount++;
    liveMsgCount++;

    previousEventNumber = -1;
    EVCB.messageCreated(this);

    // after envir notification
    previousEventNumber = getSimulation()->getEventNumber();
}

cMessage::~cMessage()
{
    EVCB.messageDeleted(this);

    if (parList)
        dropAndDelete(parList);

    if (controlInfo) {
        if (controlInfo->isOwnedObject())
            dropAndDelete((cOwnedObject *)controlInfo);
        else
            delete controlInfo;
    }

    if ((flags & FL_ISPRIVATECOPY) == 0)
        liveMsgCount--;
}

std::string cMessage::info() const
{
    if (targetModuleId<0)
        return std::string("(new msg)");

    std::stringstream out;
    const char *deletedstr = "<deleted module>";

    simtime_t t = getArrivalTime();
    if (t > getSimulation()->getSimTime())
    {
        // if it arrived in the past, dt is usually unimportant, don't print it
        out << "at T=" << t << ", in dt=" << (t - getSimulation()->getSimTime()) << "; ";
    }

#define MODNAME(modp) ((modp) ? (modp)->getFullPath().c_str() : deletedstr)
    if (getKind()==MK_STARTER)
    {
        cModule *tomodp = getSimulation()->getModule(targetModuleId);
        out << "starter for " << MODNAME(tomodp) << " (id=" << targetModuleId << ") ";
    }
    else if (getKind()==MK_TIMEOUT)
    {
        cModule *tomodp = getSimulation()->getModule(targetModuleId);
        out << "timeoutmsg for " << MODNAME(tomodp) << " (id=" << targetModuleId << ") ";
    }
    else if (senderModuleId==targetModuleId)
    {
        cModule *tomodp = getSimulation()->getModule(targetModuleId);
        out << "selfmsg for " << MODNAME(tomodp) << " (id=" << targetModuleId << ") ";
    }
    else
    {
        cModule *frommodp = getSimulation()->getModule(senderModuleId);
        cModule *tomodp = getSimulation()->getModule(targetModuleId);
        out << "src=" << MODNAME(frommodp) << " (id=" << senderModuleId << ") ";
        out << " dest=" << MODNAME(tomodp) << " (id=" << targetModuleId << ") ";
    }
#undef MODNAME

    if (controlInfo)
        out << "  control info: (" << controlInfo->getClassName() << ") " << controlInfo->getFullName() << "\n";

    return out.str();
}

void cMessage::forEachChild(cVisitor *v)
{
    if (parList)
        v->visit(parList);
    if (controlInfo)
        v->visit(controlInfo);
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

    if (contextPointer || controlInfo)
        throw cRuntimeError(this,"parsimPack(): cannot pack object with contextPointer or controlInfo set");

    buffer->pack(messageKind);
    buffer->pack(timestamp);
    buffer->pack(senderModuleId);
    buffer->pack(senderGateId);
    buffer->pack(targetModuleId);
    buffer->pack(targetGateId);
    buffer->pack(creationTime);
    buffer->pack(sendTime);

    // note: do not pack msgid and treeid, because they'd conflict
    // with ids assigned at the destination partition

    if (buffer->packFlag(parList!=NULL))
        buffer->packObject(parList);
#endif
}

void cMessage::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,E_NOPARSIM);
#else
    cEvent::parsimUnpack(buffer);

    buffer->unpack(messageKind);
    buffer->unpack(timestamp);
    buffer->unpack(senderModuleId);
    buffer->unpack(senderGateId);
    buffer->unpack(targetModuleId);
    buffer->unpack(targetGateId);
    buffer->unpack(creationTime);
    buffer->unpack(sendTime);

    if (buffer->checkFlag())
        take(parList = (cArray *) buffer->unpackObject());
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
    messageKind = msg.messageKind;
    timestamp = msg.timestamp;
    srcProcId = msg.srcProcId;

    creationTime = msg.creationTime;

    dropAndDelete(parList);
    if (msg.parList)
        take(parList = (cArray *)msg.parList->dup());
    else
        parList = NULL;

    contextPointer = msg.contextPointer;

    senderModuleId = msg.senderModuleId;
    senderGateId = msg.senderGateId;
    targetModuleId = msg.targetModuleId;
    targetGateId = msg.targetGateId;

    sendTime = msg.sendTime;

    messageTreeId = msg.messageTreeId;
}

void cMessage::_createparlist()
{
    parList = new cArray("parameters", 5, 5);
    take(parList);
}

cMessage *cMessage::privateDup() const
{
    cMessage *ret = dup();
    ret->messageId = messageId;
    ret->flags |= FL_ISPRIVATECOPY;
    ret->removeFromOwnershipTree();
    totalMsgCount--;
    liveMsgCount--;
    return ret;
}

void cMessage::setControlInfo(cObject *p)
{
    if (!p)
        throw cRuntimeError(this,"setControlInfo(): pointer is NULL");
    if (controlInfo)
        throw cRuntimeError(this,"setControlInfo(): message already has control info attached");
    if (p->isOwnedObject())
        take((cOwnedObject *)p);
    controlInfo = p;
}

cObject *cMessage::removeControlInfo()
{
    cObject *p = controlInfo;
    controlInfo = NULL;
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
    return !parList ? -1 : parList->find(s);
}

cGate *cMessage::getSenderGate() const
{
    if (senderModuleId<0 || senderGateId<0)  return NULL;
    cModule *mod = getSimulation()->getModule(senderModuleId);
    return !mod ? NULL : mod->gate(senderGateId);
}

cGate *cMessage::getArrivalGate() const
{
    if (targetModuleId<0 || targetGateId<0)  return NULL;
    cModule *mod = getSimulation()->getModule(targetModuleId);
    return !mod ? NULL : mod->gate(targetGateId);
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
    senderModuleId = module ? module->getId() : -1;
    senderGateId = gateId;
    sendTime = t;
}

void cMessage::setArrival(cModule *module, int gateId, simtime_t_cref t)
{
    setArrival(module ? module->getId() : -1, gateId, t);
}

bool cMessage::isStale()
{
    // check that destination module still exists and is alive
    cSimpleModule *modp = dynamic_cast<cSimpleModule *>(getSimulation()->getModule(targetModuleId));
    return !modp || modp->isTerminated();
}

void cMessage::execute()
{
    throw new cRuntimeError("illegal call to cMessage::execute()");
}

NAMESPACE_END

