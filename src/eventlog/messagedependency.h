//=========================================================================
//  MESSAGEDEPENDENCY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

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

NAMESPACE_BEGIN

class IEvent;
class IEventLog;

class EVENTLOG_API IMessageDependency
{
    protected:
        IEventLog *eventLog;
        bool isReuse;

    public:
        IMessageDependency(IEventLog *eventLog, bool isReuse);
        virtual ~IMessageDependency() {}

        static bool corresponds(IMessageDependency *dependency1, IMessageDependency *dependency2);
        bool getIsReuse() { return isReuse; }
        virtual bool isSelfMessageReuse() = 0;
        virtual bool isStoredMessageReuse() = 0;

        virtual IMessageDependency *duplicate(IEventLog *eventLog) = 0;

        virtual eventnumber_t getCauseEventNumber() = 0;
        virtual IEvent *getCauseEvent() = 0;

        virtual eventnumber_t getConsequenceEventNumber() = 0;
        virtual IEvent *getConsequenceEvent() = 0;

        virtual simtime_t getCauseSimulationTime() = 0;
        virtual simtime_t getConsequenceSimulationTime() = 0;
        virtual BeginSendEntry *getBeginSendEntry() = 0;
        virtual BeginSendEntry *getCauseBeginSendEntry() = 0;
        virtual BeginSendEntry *getConsequenceBeginSendEntry() = 0;

        virtual bool equals(IMessageDependency *other);

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
        eventnumber_t causeEventNumber; // -2 means not yet calculated from the consequenceEventNumber, -1 means not found in file
        int causeBeginSendEntryNumber; // optional (-1) and refers to an entry of causeEvent

        eventnumber_t consequenceEventNumber; // -2 means not yet calculated from the causeEventNumber, -1 means not found in file
        int consequenceBeginSendEntryNumber; // optional (-1) and refers to an entry of consequenceEvent

    public:
        MessageDependency(IEventLog *eventLog, bool isReuse, eventnumber_t eventNumber, int beginSendEntryNumber);
        virtual ~MessageDependency() {}

        virtual bool isSelfMessageReuse();
        virtual bool isStoredMessageReuse();

        virtual MessageDependency *duplicate(IEventLog *eventLog);

        virtual eventnumber_t getCauseEventNumber();
        virtual IEvent *getCauseEvent();

        virtual eventnumber_t getConsequenceEventNumber();
        virtual IEvent *getConsequenceEvent();

        int getCauseBeginSendEntryNumber() { return causeBeginSendEntryNumber; }
        virtual BeginSendEntry *getCauseBeginSendEntry();

        int getConsequenceBeginSendEntryNumber() { return consequenceBeginSendEntryNumber; }
        virtual BeginSendEntry *getConsequenceBeginSendEntry();

        virtual BeginSendEntry *getBeginSendEntry() { return isReuse ? getConsequenceBeginSendEntry() : getCauseBeginSendEntry(); }

        virtual simtime_t getCauseSimulationTime();
        virtual simtime_t getConsequenceSimulationTime();

        long getCauseMessageId() { return getCauseBeginSendEntry()->messageId; }
        long getConsequenceMessageId() { return getConsequenceBeginSendEntry()->messageId; }

        virtual bool equals(IMessageDependency *other);

        void printCause(FILE *file = stdout);
        void printConsequence(FILE *file = stdout);
        virtual void print(FILE *file = stdout);
        virtual const char *getClassName() { return "MessageDependency"; }
        virtual int getClassIndex() { return 0; }
};

class EVENTLOG_API FilteredMessageDependency : public IMessageDependency
{
    protected:
        IMessageDependency *beginMessageDependency;
        IMessageDependency *endMessageDependency;

    public:
        FilteredMessageDependency(IEventLog *eventLog, bool isReuse, IMessageDependency *beginMessageDependency, IMessageDependency *endMessageDependency);
        ~FilteredMessageDependency();

        virtual bool isSelfMessageReuse() { return false; }
        virtual bool isStoredMessageReuse() { return false; }

        virtual FilteredMessageDependency *duplicate(IEventLog *eventLog);

        IMessageDependency *getBeginMessageDependency() { return beginMessageDependency; }
        IMessageDependency *getEndMessageDependency() { return endMessageDependency; }

        virtual eventnumber_t getCauseEventNumber() { return beginMessageDependency->getCauseEventNumber(); }
        virtual IEvent *getCauseEvent();

        virtual eventnumber_t getConsequenceEventNumber() { return endMessageDependency->getConsequenceEventNumber(); }
        virtual IEvent *getConsequenceEvent();

        virtual simtime_t getCauseSimulationTime() { return beginMessageDependency->getCauseSimulationTime(); };
        virtual simtime_t getConsequenceSimulationTime() { return endMessageDependency->getConsequenceSimulationTime(); };
        virtual BeginSendEntry *getBeginSendEntry() { return isReuse ? getConsequenceBeginSendEntry() : getCauseBeginSendEntry(); }
        virtual BeginSendEntry *getCauseBeginSendEntry();
        virtual BeginSendEntry *getConsequenceBeginSendEntry();

        virtual bool equals(IMessageDependency *other);

        virtual void print(FILE *file);
        virtual const char *getClassName() { return "FilteredMessageDependency"; }
        virtual int getClassIndex() { return 1; }
};

NAMESPACE_END


#endif
