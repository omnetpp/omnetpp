//=========================================================================
//  FILTEREDEVENT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_EVENTLOG_FILTEREDEVENT_H
#define __OMNETPP_EVENTLOG_FILTEREDEVENT_H

#include <vector>
#include "eventlogdefs.h"
#include "ievent.h"
#include "event.h"
#include "messagedependency.h"

namespace omnetpp {
namespace eventlog {

class FilteredEventLog;

/**
 * Events stored in the FilteredEventLog. This class uses the Event class by delegation so that multiple
 * filtered events can share the same event object.
 *
 * Filtered events are in a lazy double-linked list based on event numbers.
 */
// TODO: cache event
class EVENTLOG_API FilteredEvent : public IEvent
{
    protected:
        FilteredEventLog *filteredEventLog;

        eventnumber_t eventNumber; // the corresponding event number
        eventnumber_t causeEventNumber; // the event number from which the message was sent that is being processed in this event
        IMessageDependency *cause; // the message send which is processed in this event
        IMessageDependencyList *causes; // the arrival message sends of messages which we send in this even and are in the filtered set
        IMessageDependencyList *consequences; // the message sends and arrivals from this event to another in the filtered set

        struct BreadthSearchItem {
            IEvent *event;
            IMessageDependency *firstSeenMessageDependency;
            FilteredMessageDependency::Kind effectiveKind;
            int level;

            BreadthSearchItem(IEvent *event, IMessageDependency *firstSeenMessageDependency, FilteredMessageDependency::Kind effectiveKind, int level)
            {
                this->event = event;
                this->firstSeenMessageDependency = firstSeenMessageDependency;
                this->effectiveKind = effectiveKind;
                this->level = level;
            }
        };

    public:
        FilteredEvent(FilteredEventLog *filteredEventLog, eventnumber_t eventNumber);
        virtual ~FilteredEvent();

        IEvent *getEvent();

        // IEvent interface
        virtual void synchronize(FileReader::FileChange change) override;
        virtual IEventLog *getEventLog() override;

        virtual file_offset_t getBeginOffset() override { return getEvent()->getBeginOffset(); }
        virtual file_offset_t getEndOffset() override { return getEvent()->getEndOffset(); }
        virtual EventEntry *getEventEntry() override { return getEvent()->getEventEntry(); }
        virtual ModuleDescriptionEntry *getModuleDescriptionEntry() override { return getEvent()->getModuleDescriptionEntry(); }
        virtual int getNumEventLogEntries() override { return getEvent()->getNumEventLogEntries(); }
        virtual EventLogEntry *getEventLogEntry(int index) override { return getEvent()->getEventLogEntry(index); }

        virtual int getNumEventLogMessages() override { return getEvent()->getNumEventLogMessages(); }
        virtual int getNumBeginSendEntries() override { return getEvent()->getNumBeginSendEntries(); }
        virtual int getNumCustomEntries() override { return getEvent()->getNumCustomEntries(); }
        virtual EventLogMessageEntry *getEventLogMessage(int index) override { return getEvent()->getEventLogMessage(index); }

        virtual eventnumber_t getEventNumber() override { return eventNumber; }
        virtual simtime_t getSimulationTime() override { return getEvent()->getSimulationTime(); }
        virtual int getModuleId() override { return getEvent()->getModuleId(); }
        virtual msgid_t getMessageId() override { return getEvent()->getMessageId(); }
        virtual eventnumber_t getCauseEventNumber() override { return getEvent()->getCauseEventNumber(); }

        virtual bool isSelfMessage(BeginSendEntry *beginSendEntry) override { return getEvent()->isSelfMessage(beginSendEntry); }
        virtual bool isSelfMessageProcessingEvent() override { return getEvent()->isSelfMessageProcessingEvent(); }
        virtual EndSendEntry *getEndSendEntry(BeginSendEntry *beginSendEntry) override { return getEvent()->getEndSendEntry(beginSendEntry); };
        virtual FilteredEvent *getPreviousEvent() override;
        virtual FilteredEvent *getNextEvent() override;

        virtual FilteredEvent *getCauseEvent() override;
        virtual IMessageDependency *getCause() override;
        virtual BeginSendEntry *getCauseBeginSendEntry() override;
        virtual IMessageDependencyList *getCauses() override;
        virtual IMessageDependencyList *getConsequences() override;

        virtual void print(FILE *file = stdout, bool outputEventLogMessages = true) override { getEvent()->print(file, outputEventLogMessages); }

    protected:
        void clearInternalState();
        void deleteConsequences();
        void deleteAllocatedObjects();

        void getCauses(IEvent *event, IMessageDependency *endMessageDependency, bool IsMixed, int level);
        void getConsequences(IEvent *event, IMessageDependency *beginMessageDependency, bool IsMixed, int level);
        int countFilteredMessageDependencies(IMessageDependencyList *messageDependencies);
        void pushNewFilteredMessageDependency(IMessageDependencyList *messageDependencies, FilteredMessageDependency::Kind kind, IMessageDependency *beginMessageDependency, IMessageDependency *endMessageDependency);
};

}  // namespace eventlog
}  // namespace omnetpp


#endif
