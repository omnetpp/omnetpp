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
        int causeBeginSendEntryNumber; // optional (-1) and refers to an entry of causeEvent

        long consequenceEventNumber; // -2 means not yet calculated from the causeEventNumber, -1 means not found in file
        int consequenceBeginSendEntryNumber; // optional (-1) and refers to an entry of consequenceEvent

    public:
        MessageDependency(IEventLog *eventLog,
                          long causeEventNumber, int causeBeginSendEntryNumber,
                          long consequenceEventNumber, int consequenceBeginSendEntryNumber);
        virtual ~MessageDependency() {}
        virtual MessageDependency *duplicate() = 0;

        long getCauseEventNumber();
        IEvent *getCauseEvent();

        long getConsequenceEventNumber();
        IEvent *getConsequenceEvent();

        long getCauseBeginSendEntryNumber() { return causeBeginSendEntryNumber; }
        BeginSendEntry *getCauseBeginSendEntry();

        long getConsequenceBeginSendEntryNumber() { return consequenceBeginSendEntryNumber; }
        BeginSendEntry *getConsequenceBeginSendEntry();

        simtime_t getCauseTime();
        simtime_t getConsequenceTime();

        long getCauseMessageId() { return getCauseBeginSendEntry()->messageId; }
        long getConsequenceMessageId() { return getConsequenceBeginSendEntry()->messageId; }

        void printCause(FILE *file = stdout);
        void printConsequence(FILE *file = stdout);
        virtual void print(FILE *file = stdout);
};

typedef std::vector<MessageDependency *> MessageDependencyList;

class MessageReuse : public MessageDependency
{
    public:
        MessageReuse(IEventLog *eventLog, long senderEventNumber, int BeginSendEntryNumber);
        virtual MessageReuse *duplicate();
};

/**
 * Represents a single message send (or schedule).
 */
class MessageSend : public MessageDependency
{
    public:
        MessageSend(IEventLog *eventLog, long senderEventNumber, int BeginSendEntryNumber);
        virtual MessageSend *duplicate();

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
        int middleBeginSendEntryNumber; // optional and refers to an entry of middleEvent

    public:
        FilteredMessageDependency(IEventLog *eventLog,
                                  long causeEventNumber, int causeBeginSendEntryNumber,
                                  long middleEventNumber, int middleBeginSendEntryNumber,
                                  long consequenceEventNumber, int consequenceBeginSendEntryNumber);
        virtual FilteredMessageDependency *duplicate();

        long getMiddleEventNumber() { return middleEventNumber; }
        IEvent *getMiddleEvent();
        simtime_t getMiddleTime();
        int getMiddleBeginSendEntryNumber() { return middleBeginSendEntryNumber; }
        BeginSendEntry *getMiddleBeginSendEntry();

        void printMiddle(FILE *file);
        virtual void print(FILE *file);
};

#endif



