//=========================================================================
//  EVENT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_EVENTLOG_EVENT_H
#define __OMNETPP_EVENTLOG_EVENT_H

#include <sstream>
#include <algorithm>
#include <vector>
#include "common/filereader.h"
#include "ievent.h"
#include "eventlogentry.h"
#include "eventlogentries.h"
#include "messagedependency.h"

namespace omnetpp {
namespace eventlog {

class EventLog;

/**
 * Manages all event log entries for a single event. (All lines belonging to an "E" line.)
 * Returned Event*, EventLogEntry*, EventEntry*, MessageSend*, IMessageDependencyList* pointers should not be
 * remembered by callers, because they may get deleted when the event is
 * thrown out of the eventlog cache.
 */
class EVENTLOG_API Event : public IEvent
{
    protected:
        EventLog *eventLog; // the corresponding event log
        file_offset_t beginOffset; // file offset where the event starts
        file_offset_t endOffset; // file offset where the event ends (ie. begin of next event)
        EventEntry *eventEntry; // the event log entry that corresponds to the actual event ("E" line)
        ModuleCreatedEntry *moduleCreatedEntry;
        int numEventLogMessages;
        int numBeginSendEntries;

        typedef std::vector<EventLogEntry *> EventLogEntryList;
        EventLogEntryList eventLogEntries; // all entries parsed from the file (lines below "E" line)

        /**
         * A is a cause of B if and only if B is a consequence of A.
         */
        MessageSendDependency *cause; // the message send which is processed in this event
        IMessageDependencyList *causes; // the arrival message sends of messages which we send in this event
        IMessageDependencyList *consequences; // message sends in this event

    public:
        Event(EventLog *eventLog);
        virtual ~Event();

        /**
         * Parse an event starting at the given offset.
         */
        file_offset_t parse(FileReader *reader, file_offset_t offset);

        // IEvent interface
        virtual void synchronize(FileReader::FileChangedState change) override;
        virtual IEventLog *getEventLog() override;

        virtual ModuleCreatedEntry *getModuleCreatedEntry() override;

        virtual file_offset_t getBeginOffset() override { return beginOffset; }
        virtual file_offset_t getEndOffset() override { return endOffset; }

        virtual EventEntry *getEventEntry() override { return eventEntry; }
        virtual int getNumEventLogEntries() override { return eventLogEntries.size(); }
        virtual EventLogEntry *getEventLogEntry(int index) override { return eventLogEntries[index]; }

        virtual int getNumEventLogMessages() override { return numEventLogMessages; }
        virtual int getNumBeginSendEntries() override { return numBeginSendEntries; }
        virtual EventLogMessageEntry *getEventLogMessage(int index) override;

        virtual eventnumber_t getEventNumber() override { return eventEntry->eventNumber; }
        virtual simtime_t getSimulationTime() override { return eventEntry->simulationTime; }
        virtual int getModuleId() override { return eventEntry->moduleId; }
        virtual long getMessageId() override { return eventEntry->messageId; }
        virtual eventnumber_t getCauseEventNumber() override { return eventEntry->causeEventNumber; }

        virtual bool isSelfMessage(BeginSendEntry *beginSendEntry) override;
        virtual bool isSelfMessageProcessingEvent() override;
        virtual EndSendEntry *getEndSendEntry(BeginSendEntry *beginSendEntry) override;
        simtime_t getTransmissionDelay(BeginSendEntry *beginSendEntry);
        virtual Event *getPreviousEvent() override;
        virtual Event *getNextEvent() override;

        virtual Event *getCauseEvent() override;
        virtual BeginSendEntry *getCauseBeginSendEntry() override;
        virtual MessageSendDependency *getCause() override;
        virtual IMessageDependencyList *getCauses() override;
        virtual IMessageDependencyList *getConsequences() override;

        virtual void print(FILE *file = stdout, bool outputEventLogMessages = true) override;

    protected:
        void clearInternalState();
        void deleteConsequences();
        void deleteAllocatedObjects();
};

} // namespace eventlog
}  // namespace omnetpp


#endif
