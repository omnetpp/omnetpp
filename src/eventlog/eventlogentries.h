//=========================================================================
//  EVENTLOGENTRIES.H - part of
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

// FIXME these should probably be generated

// FIXME re-cast BeginSendEntry as something like "MessageDescription"?
//       with its task being to describe the contents of a message identified by an ID?
//       it could occur just before any line where a message is involved.
// FIXME Then MessageScheduledEntry could also be split into two!


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
};

//MD
class ModuleDeletedEntry
{
    int moduleId;  // "id"
};

//MR
class ModuleReparentedEntry
{
    int moduleId;           // "id"
    int newParentModuleId;  // "p"
};

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
};

//CD
class ConnectionRemovedEntry
{
    int sourceModuleId;     // "sm"
    int sourceGateId;       // "sg"
};

//CS
class ConnectionDisplayStringChangedEntry
{
    int sourceModuleId;        // "sm"
    int sourceGateId;          // "sg"
    const char *displayString; // "d"
};

//DS
class ModuleDisplayStringChangedEntry
{
    int moduleId;              // "id"
    const char *displayString; // "d"
};

//DG
class BackgroundDisplayStringChangedEntry //(cModule *parentmodule)
{
    int moduleId;              // "id"
    const char *displayString; // "d"
};

//E
class MessageDeliveredEntry
{
    long eventNumber;          // "#"
    simtime_t simulationTime;  // "t"
    int moduleId;              // "m"
    long messageId;            // "msg"
};

//SA
class MessageScheduledEntry
{
    long messageId;       // "id"
    long messageTreeId;   // "tid"
    //long messageEncapsulationId;     FIXME these two are not currently recorded
    //long messageEncapsulationTreeId;
    const char *messageClassName;  // "c"
    const char *messageFullName;   // "n"
    int senderModuleId;            // "sm"
    simtime_t arrivalTime;         // "t"
};

//CE
class MessageCancelledEntry
{
    long messageId; // "id"
};

//BS
class BeginSendEntry
{
    long messageId;                  // "id"
    long messageTreeId;              // "tid"
    long messageEncapsulationId;     // "eid"
    long messageEncapsulationTreeId; // "etid"
    const char *messageClassName;    // "c"
    const char *messageFullName;     // "n"
    int messageKind;                 // "k"
    long messageLength;              // "l"
    // and many other fields...
};

//SD
class MessageSendDirectEntry
{
    long messageId;                  // "id"
    int senderModuleId;              // "sm"
    int destModuleId;                // "dm"
    int destGateId;                  // "dg"
    simtime_t propagationDelay;      // "pd"
    simtime_t transmissionDelay;     // "td"
};

//SH
//FIXME split into two? it's typical that there's no delay
class MessageSendHopEntry
{
    long messageId;                  // "id"
    int senderModuleId;              // "sm"
    int senderGateId;                // "sg"
    simtime_t transmissionStartId;   // "ts"  --FIXME this should be eliminated from simkernel
    simtime_t propagationDelay;      // "pd"
    simtime_t transmissionDelay;     // "td"
};

#endif


