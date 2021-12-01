//========================================================================
//  CMESSAGE.CC - part of
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
#include "omnetpp/cmodule.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {

using std::ostream;

Register_Class(cMessage);

// static members of cMessage
msgid_t cMessage::nextMessageId = 0;
uint64_t cMessage::totalMsgCount = 0;
uint64_t cMessage::liveMsgCount = 0;

cMessage::cMessage(const cMessage& msg) : cEvent(msg)
{
    copy(msg);

    // inherited
    heapIndex = -1;

    messageId = nextMessageId++;

    totalMsgCount++;
    liveMsgCount++;

    cMessage *nonConstMsg = const_cast<cMessage *>(&msg);
    EVCB.messageCloned(nonConstMsg, this);

    // after envir notification
    nonConstMsg->previousEventNumber = previousEventNumber = getSimulation()->getEventNumber();
}

cMessage::cMessage(const char *name, short k) : cEvent(name),
    messageKind(k), creationTime(getSimulation()->getSimTime())
{
    // name pooling is off for messages by default, as unique names are quite common
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

#define MODNAME(modp)    ((modp) ? (modp)->getFullPath().c_str() : "<deleted module>")

std::string cMessage::str() const
{
    if (targetModuleId < 0)
        return std::string("(new msg)");

    std::stringstream out;

    if (isScheduled()) {
        simtime_t t = getArrivalTime();
        simtime_t dt = t - getSimulation()->getSimTime();
        if (getKind() == MK_STARTER) {
            cModule *tomodp = getSimulation()->getModule(targetModuleId);
            out << "starter for " << MODNAME(tomodp) << " for t=" << t.ustr() << " (now+" << dt.ustr() << ")";
        }
        else if (getKind() == MK_TIMEOUT) {
            cModule *tomodp = getSimulation()->getModule(targetModuleId);
            out << "timeoutmsg for " << MODNAME(tomodp) << " for t=" << t.ustr() << " (now+" << dt.ustr() << ")";
        }
        else if (senderModuleId == targetModuleId) {
            cModule *tomodp = getSimulation()->getModule(targetModuleId);
            out << "selfmsg for " << MODNAME(tomodp) << " for t=" << t.ustr() << " (now+" << dt.ustr() << ")";
        }
        else {
            cModule *frommodp = getSimulation()->getModule(senderModuleId);
            cModule *tomodp = getSimulation()->getModule(targetModuleId);
            out << "in transit from " << MODNAME(frommodp) << " to " << MODNAME(tomodp);
            out << ", arrival at t=" << t.ustr() << " (now+" << dt.ustr() << ")";
        }

        if (controlInfo)
           out << "; ";
    }

    if (controlInfo)
        out << "ctrlInfo: " << controlInfo->getClassName();

    return out.str();
}

#undef MODNAME

void cMessage::forEachChild(cVisitor *v)
{
    if (parList)
        v->visit(parList);
    if (controlInfo)
        v->visit(controlInfo);
}

void cMessage::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cEvent::parsimPack(buffer);

    if (contextPointer || controlInfo)
        throw cRuntimeError(this,"parsimPack(): Cannot pack object with contextPointer or controlInfo set");

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

    if (buffer->packFlag(parList != nullptr))
        buffer->packObject(parList);
#endif
}

void cMessage::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
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
        take(parList = (cArray *)buffer->unpackObject());
#endif
}

cMessage& cMessage::operator=(const cMessage& msg)
{
    if (this == &msg)
        return *this;
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
        parList = nullptr;

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
    nextMessageId--;
    totalMsgCount--;
    liveMsgCount--;
    return ret;
}

void cMessage::setControlInfo(cObject *p)
{
    if (!p)
        throw cRuntimeError(this, "setControlInfo(): Pointer is nullptr");
    if (controlInfo)
        throw cRuntimeError(this, "setControlInfo(): Message already has control info attached");
    if (p->isOwnedObject())
        take((cOwnedObject *)p);
    controlInfo = p;
}

cObject *cMessage::removeControlInfo()
{
    cObject *p = controlInfo;
    controlInfo = nullptr;
    if (p && p->isOwnedObject())
        drop((cOwnedObject *)p);
    return p;
}

cObject *cMessage::getTargetObject() const
{
    return getArrivalModule();
}

cMsgPar& cMessage::par(int index)
{
    cArray& parlist = getParList();
    cObject *p = parlist.get(index);
    if (!p)
        throw cRuntimeError(this, "par(int): Has no parameter #%d", index);
    if (!dynamic_cast<cMsgPar *>(p))
        throw cRuntimeError(this, "par(int): Parameter #%d is of type %s, not cMsgPar", index, p->getClassName());
    return *(cMsgPar *)p;
}

cMsgPar& cMessage::par(const char *name)
{
    cArray& parlist = getParList();
    cObject *p = parlist.get(name);
    if (!p)
        throw cRuntimeError(this, "par(const char *): Has no parameter called '%s'", name);
    if (!dynamic_cast<cMsgPar *>(p))
        throw cRuntimeError(this, "par(const char *): Parameter '%s' is of type %s, not cMsgPar", name, p->getClassName());
    return *(cMsgPar *)p;
}

int cMessage::findPar(const char *name) const
{
    return !parList ? -1 : parList->find(name);
}

cGate *cMessage::getSenderGate() const
{
    if (senderModuleId < 0 || senderGateId < 0)
        return nullptr;
    cModule *mod = getSimulation()->getModule(senderModuleId);
    return !mod ? nullptr : mod->gate(senderGateId);
}

cGate *cMessage::getArrivalGate() const
{
    if (targetModuleId < 0 || targetGateId < 0)
        return nullptr;
    cModule *mod = getSimulation()->getModule(targetModuleId);
    return !mod ? nullptr : mod->gate(targetGateId);
}

bool cMessage::arrivedOn(const char *gateName) const
{
    cGate *arrgate = getArrivalGate();
    return arrgate && arrgate->isName(gateName);
}

bool cMessage::arrivedOn(const char *gateName, int gateIndex) const
{
    cGate *arrgate = getArrivalGate();
    return arrgate && arrgate->isName(gateName) && arrgate->getIndex() == gateIndex;
}

const char *cMessage::getDisplayString() const
{
    return "";  // clients may redefine this method to get messages with custom appearance
}

void cMessage::setSentFrom(cModule *module, int gateId, simtime_t_cref t)
{
    senderModuleId = module ? module->getId() : -1;
    senderGateId = gateId;
    sendTime = t;
}

bool cMessage::isStale()
{
    // check that destination module still exists and is alive
    cSimpleModule *module = dynamic_cast<cSimpleModule *>(getSimulation()->getModule(targetModuleId));
    return !module || module->isTerminated();
}

void cMessage::execute()
{
    cSimpleModule *module = static_cast<cSimpleModule *>(getArrivalModule());
    module->doMessageEvent(this);
}

}  // namespace omnetpp

