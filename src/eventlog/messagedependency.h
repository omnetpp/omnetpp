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

class IEvent;
class IEventLog;

/**
 * Represents two places in the event log file which are associate with the
 * same message object: (cause event number, cause entry number),
 * (consequence event number, consequence entry number). This may represent
 * a message sending (sent at "cause", arrives at "consequence"), a message
 * scheduling (ditto), or a message reuse (arrived at the module at "cause",
 * and sent out again at "consequence").
 *
 * Entry number is the line number after the "E" line.
 *
 */
class MessageDependency
{
    protected:
        IEventLog *eventLog;

        long causeEventNumber; // -2 means not yet calculated from the consequenceEventNumber, -1 means not found in file
        int causeMessageSendEntryNumber; // optional (-1) and refers to an entry of causeEvent

        long consequenceEventNumber; // -2 means not yet calculated from the causeEventNumber, -1 means not found in file
        int consequenceMessageSendEntryNumber; // optional (-1) and refers to an entry of consequenceEvent

    public:
        MessageDependency(IEventLog *eventLog,
                          long causeEventNumber, int causeMessageSendEntryNumber,
                          long consequenceEventNumber, int consequenceMessageSendEntryNumber);

        long getCauseEventNumber();
        IEvent *getCauseEvent();

        long getConsequenceEventNumber();
        IEvent *getConsequenceEvent();

        long getCauseMessageSendEntryNumber() { return causeMessageSendEntryNumber; }
        EventLogEntry *getCauseMessageSendEntry();

        long getConsequenceMessageSendEntryNumber() { return consequenceMessageSendEntryNumber; }
        EventLogEntry *getConsequenceMessageSendEntry();

        simtime_t getCauseTime();
        simtime_t getConsequenceTime();

        long getCauseMessageId() { return getCauseMessageSendEntry()->getMessageId(); }
        long getConsequenceMessageId() { return getConsequenceMessageSendEntry()->getMessageId(); }

        EventLogEntry *getMessageSendEntry();

        void printCause(FILE *file = stdout);
        void printConsequence(FILE *file = stdout);
        virtual void print(FILE *file = stdout);
};

typedef std::vector<MessageDependency *> MessageDependencyList;

class MessageReuse : public MessageDependency
{
    public:
        MessageReuse(IEventLog *eventLog, long senderEventNumber, int messageSendEntryNumber);
};

/**
 * Represents a single message send (or schedule).
 */
class MessageSend : public MessageDependency
{
    public:
        MessageSend(IEventLog *eventLog, long senderEventNumber, int messageSendEntryNumber);

        long getSenderEventNumber() { return getCauseEventNumber(); }
        IEvent *getSenderEvent() { return getCauseEvent(); }

        long getArrivalEventNumber() { return getConsequenceEventNumber(); }
        IEvent *getArrivalEvent() { return getConsequenceEvent(); }

        simtime_t getSenderTime() { return getCauseTime(); }
        simtime_t getArrivalTime() { return getConsequenceTime(); }
};

class FilteredMessageDependency : public MessageDependency
{
    protected:
        long middleEventNumber;
        int middleMessageSendEntryNumber; // optional and refers to an entry of middleEvent

    public:
        FilteredMessageDependency(IEventLog *eventLog,
                                  long causeEventNumber, int causeMessageSendEntryNumber,
                                  long middleEventNumber, int middleMessageSendEntryNumber,
                                  long consequenceEventNumber, int consequenceMessageSendEntryNumber);

        long getMiddleEventNumber() { return middleEventNumber; }
        IEvent *getMiddleEvent();
        simtime_t getMiddleTime();
        int getMiddleMessageSendEntryNumber() { return middleMessageSendEntryNumber; }
        EventLogEntry *getMiddleMessageSendEntry();

        void printMiddle(FILE *file);
        virtual void print(FILE *file);
};

#endif