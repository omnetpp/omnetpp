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

        bool getIsReuse() { return isReuse; }

        virtual IMessageDependency *duplicate(IEventLog *eventLog) = 0;

        virtual long getCauseEventNumber() = 0;
        virtual IEvent *getCauseEvent() = 0;

        virtual long getConsequenceEventNumber() = 0;
        virtual IEvent *getConsequenceEvent() = 0;

        virtual simtime_t& getCauseSimulationTime() = 0;
        virtual simtime_t& getConsequenceSimulationTime() = 0;
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
        MessageDependency(IEventLog *eventLog, bool isReuse, long eventNumber, int beginSendEntryNumber);
        virtual ~MessageDependency() {}

        virtual MessageDependency *duplicate(IEventLog *eventLog);

        virtual long getCauseEventNumber();
        virtual IEvent *getCauseEvent();

        virtual long getConsequenceEventNumber();
        virtual IEvent *getConsequenceEvent();

        long getCauseBeginSendEntryNumber() { return causeBeginSendEntryNumber; }
        BeginSendEntry *getCauseBeginSendEntry();

        long getConsequenceBeginSendEntryNumber() { return consequenceBeginSendEntryNumber; }
        BeginSendEntry *getConsequenceBeginSendEntry();

        virtual BeginSendEntry *getBeginSendEntry() { return isReuse ? getConsequenceBeginSendEntry() : getCauseBeginSendEntry(); }

        virtual simtime_t& getCauseSimulationTime();
        virtual simtime_t& getConsequenceSimulationTime();

        long getCauseMessageId() { return getCauseBeginSendEntry()->messageId; }
        long getConsequenceMessageId() { return getConsequenceBeginSendEntry()->messageId; }

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
        virtual FilteredMessageDependency *duplicate(IEventLog *eventLog);

        IMessageDependency *getBeginMessageDependency() { return beginMessageDependency; }
        IMessageDependency *getEndMessageDependency() { return endMessageDependency; }

        virtual long getCauseEventNumber() { return beginMessageDependency->getCauseEventNumber(); }
        virtual IEvent *getCauseEvent();

        virtual long getConsequenceEventNumber() { return endMessageDependency->getConsequenceEventNumber(); }
        virtual IEvent *getConsequenceEvent();

        virtual simtime_t& getCauseSimulationTime() { return beginMessageDependency->getCauseSimulationTime(); };
        virtual simtime_t& getConsequenceSimulationTime() { return endMessageDependency->getConsequenceSimulationTime(); };
        virtual BeginSendEntry *getBeginSendEntry() { throw opp_runtime_error("getBeginSendEntry is not supported on a FilteredMessageDependency"); }
        virtual void print(FILE *file);
        virtual const char *getClassName() { return "FilteredMessageDependency"; }
        virtual int getClassIndex() { return 1; }
};

NAMESPACE_END


#endif
