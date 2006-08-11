//=========================================================================
//  EVENTENTRY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTLOGENTRIES_H_
#define __EVENTLOGENTRIES_H_

#include "eventlogdefs.h"
#include "exception.h"


//BU
class BubbleEntry
{
    int moduleId;      // "id"
    const char *text;  // "txt"
};

//MM
class ModuleMethodCalledEntry
{
    int fromModuleId;    // "sm"
    int toModuleId;      // "tm"
    const char *method;  // "m"
};

//MC
class ModuleCreatedEntry
{
    int moduleId;                 // "id"
    const char *moduleClassName;  // "c"
    int parentModuleId;           // "pid" (-1 if none)
    const char *fullName;         // "n"
}

//MD
class ModuleDeletedEntry
{
    int moduleId;  // "id"
}

//MR
class ModuleReparentedEntry
{
    int moduleId;           // "id"
    int newParentModuleId;  // "p"
}

//CC
class ConnectionCreatedEntry
{
    int sourceModuleId;     // "sm"
    int sourceGateId;       // "sg"
    int sourceGateFullName; // "sn"
    int destModuleId;       // "dm"
    int destGateId;         // "dg"
    int destGateFullName;   // "dn"
    //XXX channel, channel attributes, etc
}

//CD
class ConnectionRemovedEntry
{
    int sourceModuleId;     // "sm"
    int sourceGateId;       // "sg"
}

//CS
class ConnectionDisplayStringChangedEntry
{
    int sourceModuleId;        // "sm"
    int sourceGateId;          // "sg"
    const char *displayString; // "d"
}

//DS
class ModuleDisplayStringChangedEntry
{
    int moduleId;              // "id"
    const char *displayString; // "d"
}

//DG
class BackgroundDisplayStringChangedEntry //(cModule *parentmodule)
{
    int moduleId;              // "id"
    const char *displayString; // "d"
}

//E
class MessageDeliveredEntry
{
    long eventNumber;          // "#"
    simtime_t simulationTime;  // "t"
    int moduleId;              // "m"
    long messageId;            // "msg"
}

//SA
class MessageScheduledEntry
{
    long messageId;
    long messageTreeId;
    //long messageEncapsulationId;     FIXME these two are not currently recorded
    //long messageEncapsulationTreeId;

    long messageId;
        ::fprintf(fmsglog, "SA id %ld tid %ld c %s n %s sm %ld t %.*g\n",
                           msg->id(),
                           msg->treeId(),
                           msg->className(),
                           msg->fullName(),
                           msg->senderModuleId(),
                           12, msg->arrivalTime());
    }
}

class MessageCancelledEntry //(cMessage *msg)
{
    if (fmsglog)
    {
        ::fprintf(fmsglog, "CE id %ld\n",
                           msg->id());
    }
}

class BeginSendEntry //(cMessage *msg)
{
    if (fmsglog)
    {
        ::fprintf(fmsglog, "BS id %ld tid %ld eid %ld etid %ld c %s n \"%s\" k %d l %ld\n",
                           msg->id(),
                           msg->treeId(), //XXX only if differs from id()
                           msg->encapsulationId(), //XXX only if differs from id()
                           msg->encapsulationTreeId(), //XXX only if differs from id()
                           msg->className(),
                           msg->fullName(),
                           msg->kind(),
                           msg->length());  //XXX plus many other fields...
    }
}

class MessageSendDirectEntry //(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    if (fmsglog)
    {
        ::fprintf(fmsglog, "SD id %ld sm %ld dm %ld dg %d pd %.*g td %.*g\n",
                           msg->id(),
                           msg->senderModuleId(),
                           toGate->ownerModule()->id(),
                           toGate->id(),
                           12, propagationDelay, //XXX if nonzero
                           12, transmissionDelay);  //XXX if nonzero
    }
}

class MessageSendHopEntry //(cMessage *msg, cGate *srcGate, simtime_t propagationDelay)
{
    if (fmsglog)
    {
        if (propagationDelay==0)
            ::fprintf(fmsglog, "SH id %ld sm %ld sg %d\n",
                               msg->id(),
                               srcGate->ownerModule()->id(),
                               srcGate->id());
        else
            ::fprintf(fmsglog, "SH id %ld sm %ld sg %d pd %.*g\n",
                               msg->id(),
                               srcGate->ownerModule()->id(),
                               srcGate->id(),
                               12, propagationDelay);
    }
}

class MessageSendHopEntry //(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay, simtime_t transmissionStartTime)
{
    if (fmsglog)
    {
        if (transmissionStartTime<0)
          ::fprintf(fmsglog, "SH id %ld sm %ld sg %d td %.*g pd %.*g\n",
                             msg->id(),
                             srcGate->ownerModule()->id(),
                             srcGate->id(),
                             12, transmissionDelay,  //XXX only if nonzero
                             12, propagationDelay);  //XXX only if nonzero
        else
          ::fprintf(fmsglog, "SH id %ld sm %ld sg %d ts %.*g td %.*g pd %.*g\n",
                             msg->id(),
                             srcGate->ownerModule()->id(),
                             srcGate->id(),
                             12, transmissionStartTime,
                             12, transmissionDelay,  //XXX only if nonzero
                             12, propagationDelay);  //XXX only if nonzero
    }
}

#endif


