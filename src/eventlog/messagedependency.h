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

        long causeEventNumber; // -2 means not yet calculated from the consequenceEventNumber, -1 means did not found
        int causeMessageSendEntryNumber; // optional (-1) and refers to an entry of causeEvent

        long consequenceEventNumber; // -2 means not yet calculated from the causeEventNumber, -1 means did not found
        int consequenceMessageSendEntryNumber; // optional (-1) and refers to an entry of consequenceEvent

    public:
        MessageDependency(EventLog *eventLog,
            long causeEventNumber, int causeMessageSendEntryNumber,
            long consequenceEventNumber, int consequenceMessageSendEntryNumber);

        long getCauseEventNumber();
        Event *getCauseEvent();

        long getConsequenceEventNumber();
        Event *getConsequenceEvent();

        long getCauseMessageSendEntryNumber() { return causeMessageSendEntryNumber; };
        EventLogEntry *getCauseMessageSendEntry();

        long getConsequenceMessageSendEntryNumber() { return consequenceMessageSendEntryNumber; };
        EventLogEntry *getConsequenceMessageSendEntry();

        simtime_t getCauseTime();
        simtime_t getConsequenceTime();

        long getCauseMessageId() { return getCauseMessageSendEntry()->getMessageId(); };
        long getConsequenceMessageId() { return getConsequenceMessageSendEntry()->getMessageId(); };

        EventLogEntry *getMessageSendEntry();
        void printCause(FILE *file);
        void printConsequence(FILE *file);
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

class FilteredMessageDependency : public MessageDependency
{
    protected:
        long xxxEventNumber;
        int xxxMessageSendEntryNumber; // optional and refers to an entry of xxxEvent

    public:
        FilteredMessageDependency(EventLog *eventLog,
            long causeEventNumber, int causeMessageSendEntryNumber,
            long xxxEventNumber, int xxxMessageSendEntryNumber,
            long consequenceEventNumber, int consequenceMessageSendEntryNumber);

        long getxxxEventNumber() { return xxxEventNumber; };
        Event *getxxxEvent();
        simtime_t getxxxTime();
        int getxxxMessageSendEntryNumber() { return xxxMessageSendEntryNumber; };
        EventLogEntry *getxxxMessageSendEntry();

        void print(FILE *file);
        void printxxx(FILE *file);
};

#endif