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

class EVENTLOG_API IMessageDependency
{
    protected:
        IEventLog *eventLog;

    public:
        IMessageDependency(IEventLog *eventLog);
        virtual ~IMessageDependency() {}

        virtual IMessageDependency *duplicate(IEventLog *eventLog) = 0;

        virtual long getCauseEventNumber() = 0;
        virtual IEvent *getCauseEvent() = 0;

        virtual long getConsequenceEventNumber() = 0;
        virtual IEvent *getConsequenceEvent() = 0;

        virtual simtime_t getCauseSimulationTime() = 0;
        virtual simtime_t getConsequenceSimulationTime() = 0;
        virtual BeginSendEntry *getBeginSendEntry() = 0;

        virtual void print(FILE *file = stdout) = 0;
        virtual const char *getClassName() = 0;
        virtual int getClassIndex() = 0;
};

typedef std::vector<IMessageDependency *> IMessageDependencyList;

/**
 * Represents two places in the event log file which are associated with the
 * same message object: (cause event number, cause entry number),
 * (consequence event number, consequence entry number). This may represent
 * a message sending (sent at "cause", arrives at "consequence"), a message
 * scheduling (ditto), or a message reuse (arrived at the module at "cause",
 * and sent out again at "consequence").
 *
 * Entry number is the line number after the "E" line.
 *
 */
class EVENTLOG_API MessageDependency : public IMessageDependency
{
    protected:
        long causeEventNumber; // -2 means not yet calculated from the consequenceEventNumber, -1 means not found in file
        int causeBeginSendEntryNumber; // optional (-1) and refers to an entry of causeEvent

        long consequenceEventNumber; // -2 means not yet calculated from the causeEventNumber, -1 means not found in file
        int consequenceBeginSendEntryNumber; // optional (-1) and refers to an entry of consequenceEvent

    public:
        MessageDependency(IEventLog *eventLog,
                          long causeEventNumber, int causeBeginSendEntryNumber,
                          long consequenceEventNumber, int consequenceBeginSendEntryNumber);
        virtual ~MessageDependency() {}

        virtual long getCauseEventNumber();
        virtual IEvent *getCauseEvent();

        virtual long getConsequenceEventNumber();
        virtual IEvent *getConsequenceEvent();

        long getCauseBeginSendEntryNumber() { return causeBeginSendEntryNumber; }
        BeginSendEntry *getCauseBeginSendEntry();

        long getConsequenceBeginSendEntryNumber() { return consequenceBeginSendEntryNumber; }
        BeginSendEntry *getConsequenceBeginSendEntry();

        virtual simtime_t getCauseSimulationTime();
        virtual simtime_t getConsequenceSimulationTime();
        virtual BeginSendEntry *getBeginSendEntry() = 0;

        long getCauseMessageId() { return getCauseBeginSendEntry()->messageId; }
        long getConsequenceMessageId() { return getConsequenceBeginSendEntry()->messageId; }

        void printCause(FILE *file = stdout);
        void printConsequence(FILE *file = stdout);
        virtual void print(FILE *file = stdout);
};

class EVENTLOG_API MessageReuse : public MessageDependency
{
    public:
        MessageReuse(IEventLog *eventLog, long senderEventNumber, int BeginSendEntryNumber);
        virtual MessageReuse *duplicate(IEventLog *eventLog);

        virtual BeginSendEntry *getBeginSendEntry() { return getConsequenceBeginSendEntry(); }
        virtual const char *getClassName() { return "MessageReuse"; }
        virtual int getClassIndex() { return 0; }
};

/**
 * Represents a single message send (or schedule).
 */
class EVENTLOG_API MessageSend : public MessageDependency
{
    public:
        MessageSend(IEventLog *eventLog, long senderEventNumber, int BeginSendEntryNumber);
        virtual MessageSend *duplicate(IEventLog *eventLog);

        virtual BeginSendEntry *getBeginSendEntry() { return getCauseBeginSendEntry(); }

        long getSenderEventNumber() { return getCauseEventNumber(); }
        IEvent *getSenderEvent() { return getCauseEvent(); }

        long getArrivalEventNumber() { return getConsequenceEventNumber(); }
        IEvent *getArrivalEvent() { return getConsequenceEvent(); }

        simtime_t getSenderTime() { return getCauseSimulationTime(); }
        simtime_t getArrivalTime() { return getConsequenceSimulationTime(); }

        virtual const char *getClassName() { return "MessageSend"; }
        virtual int getClassIndex() { return 1; }
};

class EVENTLOG_API FilteredMessageDependency : public IMessageDependency
{
    protected:
        IMessageDependency *beginMessageDependency;
        IMessageDependency *endMessageDependency;

    public:
        FilteredMessageDependency(IEventLog *eventLog, IMessageDependency *beginMessageDependency, IMessageDependency *endMessageDependency);
        ~FilteredMessageDependency();
        virtual FilteredMessageDependency *duplicate(IEventLog *eventLog);

        IMessageDependency *getBeginMessageDependency() { return beginMessageDependency; }
        IMessageDependency *getEndMessageDependency() { return endMessageDependency; }

        virtual long getCauseEventNumber() { return beginMessageDependency->getCauseEventNumber(); }
        virtual IEvent *getCauseEvent();

        virtual long getConsequenceEventNumber() { return endMessageDependency->getConsequenceEventNumber(); }
        virtual IEvent *getConsequenceEvent();

        virtual simtime_t getCauseSimulationTime() { return beginMessageDependency->getCauseSimulationTime(); };
        virtual simtime_t getConsequenceSimulationTime() { return endMessageDependency->getConsequenceSimulationTime(); };
        virtual BeginSendEntry *getBeginSendEntry() { return NULL; }
        virtual void print(FILE *file);
        virtual const char *getClassName() { return "FilteredMessageDependency"; }
        virtual int getClassIndex() { return 2; }
};

#endif
