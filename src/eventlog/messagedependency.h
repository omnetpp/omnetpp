//=========================================================================
//  MESSAGEDEPENDENCY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_EVENTLOG_MESSAGEDEPENDENCY_H
#define __OMNETPP_EVENTLOG_MESSAGEDEPENDENCY_H

#include <sstream>
#include <vector>
#include "common/filereader.h"
#include "eventlogentry.h"
#include "eventlogentries.h"

namespace omnetpp {
namespace eventlog {

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

        virtual MessageDescriptionEntry *getBeginMessageDescriptionEntry() = 0;
        virtual MessageDescriptionEntry *getEndMessageDescriptionEntry() = 0;

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

        virtual eventnumber_t getCauseEventNumber() override { return causeEventNumber; }
        virtual IEvent *getCauseEvent() override;
        virtual simtime_t getCauseSimulationTime() override;

        virtual eventnumber_t getConsequenceEventNumber() override;
        virtual IEvent *getConsequenceEvent() override;
        virtual simtime_t getConsequenceSimulationTime() override;

        virtual MessageDescriptionEntry *getBeginMessageDescriptionEntry() override;
        virtual MessageDescriptionEntry *getEndMessageDescriptionEntry() override;

        virtual MessageSendDependency *duplicate(IEventLog *eventLog) override;
        virtual bool equals(IMessageDependency *other) override;

        virtual void print(FILE *file = stdout) override;
        virtual const char *getClassName() override { return "MessageSendDependency"; }
        virtual int getClassIndex() override { return 0; }
};

/**
 * Represents a message reuse dependency: the message has arrived at "cause", and it was sent out again at "consequence".
 */
class EVENTLOG_API MessageReuseDependency : public IMessageDependency
{
    protected:
        eventnumber_t causeEventNumber; // -1 means not yet calculated from the consequenceEventNumber, -2 means not found in file
        eventnumber_t consequenceEventNumber; // always present
        int eventLogEntryIndex; // refers to an entry of consequenceEvent

    public:
        MessageReuseDependency(IEventLog *eventLog, eventnumber_t eventNumber, int eventLogEntryIndex);
        virtual ~MessageReuseDependency() {}

        virtual eventnumber_t getCauseEventNumber() override;
        virtual IEvent *getCauseEvent() override;
        virtual simtime_t getCauseSimulationTime() override;

        virtual eventnumber_t getConsequenceEventNumber() override { return consequenceEventNumber; }
        virtual IEvent *getConsequenceEvent() override;
        virtual simtime_t getConsequenceSimulationTime() override;

        virtual MessageDescriptionEntry *getBeginMessageDescriptionEntry() override;
        virtual MessageDescriptionEntry *getEndMessageDescriptionEntry() override;

        virtual MessageReuseDependency *duplicate(IEventLog *eventLog) override;
        virtual bool equals(IMessageDependency *other) override;

        virtual void print(FILE *file = stdout) override;
        virtual const char *getClassName() override { return "MessageReuseDependency"; }
        virtual int getClassIndex() override { return 1; }
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

        virtual eventnumber_t getCauseEventNumber() override { return beginMessageDependency->getCauseEventNumber(); }
        virtual IEvent *getCauseEvent() override;
        virtual simtime_t getCauseSimulationTime() override { return beginMessageDependency->getCauseSimulationTime(); };

        virtual eventnumber_t getConsequenceEventNumber() override { return endMessageDependency->getConsequenceEventNumber(); }
        virtual IEvent *getConsequenceEvent() override;
        virtual simtime_t getConsequenceSimulationTime() override { return endMessageDependency->getConsequenceSimulationTime(); };

        virtual MessageDescriptionEntry *getBeginMessageDescriptionEntry() override { return beginMessageDependency->getBeginMessageDescriptionEntry(); }
        virtual MessageDescriptionEntry *getEndMessageDescriptionEntry() override { return beginMessageDependency->getEndMessageDescriptionEntry(); }
        Kind getKind() { return kind; }

        virtual FilteredMessageDependency *duplicate(IEventLog *eventLog) override;
        virtual bool equals(IMessageDependency *other) override;

        virtual void print(FILE *file) override;
        virtual const char *getClassName() override { return "FilteredMessageDependency"; }
        virtual int getClassIndex() override { return 3; }
};

}  // namespace eventlog
}  // namespace omnetpp


#endif
