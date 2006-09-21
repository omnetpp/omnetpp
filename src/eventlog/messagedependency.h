//=========================================================================
//  MESSAGEDEPENDENCY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __MESSAGEDEPENDENCY_H_
#define __MESSAGEDEPENDENCY_H_

#include <sstream>
#include <vector>
#include "filereader.h"
#include "eventlogentry.h"
#include "eventlogentries.h"

class Event;
class EventLog;

class MessageDependency
{
    protected:
        EventLog *eventLog;
        long causeEventNumber;
        long consequenceEventNumber;
        int messageSendEntryNumber;

    public:
        MessageDependency(EventLog *eventLog, long causeEventNumber, long consequenceEventNumber, int messageSendEntryNumber);

        long getCauseEventNumber();
        Event *getCauseEvent();

        long getConsequenceEventNumber();
        Event *getConsequenceEvent();

        long getMessageSendEntryNumber() { return messageSendEntryNumber; };
        EventLogEntry *getMessageSendEntry();

        simtime_t getCauseTime();
        simtime_t getConsequenceTime();

        long getMessageId() { return getMessageSendEntry()->getMessageId(); };
};

class MessageReuse : public MessageDependency
{
    public:
        MessageReuse(EventLog *eventLog, long senderEventNumber, int messageSendEntryNumber);
};

/**
 * Represents a single message send.
 */
class MessageSend : public MessageDependency
{
    public:
        MessageSend(EventLog *eventLog, long senderEventNumber, int messageSendEntryNumber);

        long getSenderEventNumber() { return getCauseEventNumber(); };
        Event *getSenderEvent() { return getCauseEvent(); };

        long getArrivalEventNumber() { return getConsequenceEventNumber(); };
        Event *getArrivalEvent() { return getConsequenceEvent(); };

        simtime_t getSenderTime() { return getCauseTime(); };
        simtime_t getArrivalTime() { return getConsequenceTime(); };
};

#endif