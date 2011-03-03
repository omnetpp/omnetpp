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

    public:
        IMessageDependency(IEventLog *eventLog);
        virtual ~IMessageDependency() {}

        static bool corresponds(IMessageDependency *dependency1, IMessageDependency *dependency2);

        virtual eventnumber_t getCauseEventNumber() = 0;
        virtual IEvent *getCauseEvent() = 0;
        virtual simtime_t getCauseSimulationTime() = 0;

        virtual eventnumber_t getConsequenceEventNumber() = 0;
        virtual IEvent *getConsequenceEvent() = 0;
        virtual simtime_t getConsequenceSimulationTime() = 0;

        virtual MessageEntry *getMessageEntry() = 0;

        virtual IMessageDependency *duplicate(IEventLog *eventLog) = 0;
        virtual bool equals(IMessageDependency *other);

        virtual void print(FILE *file = stdout) = 0;
        virtual const char *getClassName() = 0;
        virtual int getClassIndex() = 0;
};

typedef std::vector<IMessageDependency *> IMessageDependencyList;

/**
 * Represents a message send dependency: the message was sent out at "cause", and has arrived at "consequence".
 */
class EVENTLOG_API MessageSendDependency : public IMessageDependency
{
    protected:
        eventnumber_t causeEventNumber; // always present
        eventnumber_t consequenceEventNumber; // -2 means not yet calculated from the causeEventNumber, -1 means not found in file
        int eventLogEntryIndex; // refers to an entry of causeEvent

    public:
        MessageSendDependency(IEventLog *eventLog, eventnumber_t eventNumber, int eventLogEntryIndex);
        virtual ~MessageSendDependency() {}

        virtual eventnumber_t getCauseEventNumber() { return causeEventNumber; }
        virtual IEvent *getCauseEvent();
        virtual simtime_t getCauseSimulationTime();

        virtual eventnumber_t getConsequenceEventNumber();
        virtual IEvent *getConsequenceEvent();
        virtual simtime_t getConsequenceSimulationTime();

        virtual MessageEntry *getMessageEntry();

        virtual MessageSendDependency *duplicate(IEventLog *eventLog);
        virtual bool equals(IMessageDependency *other);

        virtual void print(FILE *file = stdout);
        virtual const char *getClassName() { return "MessageSendDependency"; }
        virtual int getClassIndex() { return 0; }
};

/**
 * Represents a message reuse dependency: the message has arrived at "cause", and it was sent out again at "consequence".
 */
class EVENTLOG_API MessageReuseDependency : public IMessageDependency
{
    protected:
        eventnumber_t causeEventNumber; // -2 means not yet calculated from the consequenceEventNumber, -1 means not found in file
        eventnumber_t consequenceEventNumber; // always present
        int eventLogEntryIndex; // refers to an entry of consequenceEvent

    public:
        MessageReuseDependency(IEventLog *eventLog, eventnumber_t eventNumber, int eventLogEntryIndex);
        virtual ~MessageReuseDependency() {}

        virtual eventnumber_t getCauseEventNumber();
        virtual IEvent *getCauseEvent();
        virtual simtime_t getCauseSimulationTime();

        virtual eventnumber_t getConsequenceEventNumber() { return consequenceEventNumber; }
        virtual IEvent *getConsequenceEvent();
        virtual simtime_t getConsequenceSimulationTime();

        virtual MessageEntry *getMessageEntry();

        virtual MessageReuseDependency *duplicate(IEventLog *eventLog);
        virtual bool equals(IMessageDependency *other);

        virtual void print(FILE *file = stdout);
        virtual const char *getClassName() { return "MessageReuseDependency"; }
        virtual int getClassIndex() { return 1; }
};

/**
 * Represents a chain of message dependencies starting with beginMessageDependency and ending with endMessageDependency.
 */
class EVENTLOG_API FilteredMessageDependency : public IMessageDependency
{
    public:
        enum Kind
        {
            UNDEFINED = 0,
            SENDS = 1,
            REUSES = 2,
            MIXED = 3 // this relies on the fact that: 1 | 2 = 3
        };

    protected:
        Kind kind;
        IMessageDependency *beginMessageDependency;
        IMessageDependency *endMessageDependency;

    public:
        FilteredMessageDependency(IEventLog *eventLog, Kind kind, IMessageDependency *beginMessageDependency, IMessageDependency *endMessageDependency);
        virtual ~FilteredMessageDependency();

        IMessageDependency *getBeginMessageDependency() { return beginMessageDependency; }
        IMessageDependency *getEndMessageDependency() { return endMessageDependency; }

        virtual eventnumber_t getCauseEventNumber() { return beginMessageDependency->getCauseEventNumber(); }
        virtual IEvent *getCauseEvent();
        virtual simtime_t getCauseSimulationTime() { return beginMessageDependency->getCauseSimulationTime(); };

        virtual eventnumber_t getConsequenceEventNumber() { return endMessageDependency->getConsequenceEventNumber(); }
        virtual IEvent *getConsequenceEvent();
        virtual simtime_t getConsequenceSimulationTime() { return endMessageDependency->getConsequenceSimulationTime(); };

        virtual MessageEntry *getMessageEntry() { return beginMessageDependency->getMessageEntry(); }
        Kind getKind() { return kind; }

        virtual FilteredMessageDependency *duplicate(IEventLog *eventLog);
        virtual bool equals(IMessageDependency *other);

        virtual void print(FILE *file);
        virtual const char *getClassName() { return "FilteredMessageDependency"; }
        virtual int getClassIndex() { return 3; }
};

NAMESPACE_END


#endif
