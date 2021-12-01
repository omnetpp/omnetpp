//=========================================================================
//  EVENT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
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
#include <map>
#include "common/filereader.h"
#include "ievent.h"
#include "eventlogentry.h"
#include "eventlogentries.h"
#include "eventlogentrycache.h"
#include "messagedependency.h"

namespace omnetpp {
namespace eventlog {

class EventLog;

/**
 * Manages all eventlog entries for a single event. (all lines belonging to an "E" line)
 * Returned Event*, EventLogEntry*, EventEntry*, MessageSend*, IMessageDependencyList* pointers should not be
 * remembered by callers, because they may get deleted when the event is
 * thrown out of the eventlog cache.
 */
class EVENTLOG_API Event : public IEvent
{
    protected:
        EventLog *eventLog; // the corresponding eventlog
        file_offset_t beginOffset; // file offset where the event starts
        file_offset_t endOffset; // file offset where the event ends (including the following empty line, equals to the begin of the next thunk)
        EventEntry *eventEntry; // the eventlog entry that corresponds to the actual event ("E" line)
        ModuleDescriptionEntry *moduleDescriptionEntry;
        int numEventLogMessages;
        int numBeginSendEntries;
        int numCustomEntries;
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
         * Parse an event starting at the given offset. Calling this function
         * clears the internal state of this event before parsing.
         */
        file_offset_t parse(FileReader *reader, file_offset_t offset);

        /**
         * Parse eventlog entries starting at the given offset. This function
         *
         * This may be called multiple times if the previous parse stopped
         * because it reached the end of the file before the event was completely
         * parsed.
         */
        file_offset_t parseLines(FileReader *reader, file_offset_t offset);

        // IEvent interface
        virtual void synchronize(FileReader::FileChange change) override;
        virtual IEventLog *getEventLog() override;

        virtual file_offset_t getBeginOffset() override { return beginOffset; }
        virtual file_offset_t getEndOffset() override { return endOffset; }

        virtual EventEntry *getEventEntry() override { return eventEntry; }
        virtual ModuleDescriptionEntry *getModuleDescriptionEntry() override;
        virtual int getNumEventLogEntries() override { return eventLogEntries.size(); }
        virtual EventLogEntry *getEventLogEntry(int index) override { return eventLogEntries[index]; }

        virtual int getNumEventLogMessages() override { return numEventLogMessages; }
        virtual int getNumBeginSendEntries() override { return numBeginSendEntries; }
        virtual int getNumCustomEntries() override { return numCustomEntries; }
        virtual EventLogMessageEntry *getEventLogMessage(int index) override;

        virtual eventnumber_t getEventNumber() override { return eventEntry->eventNumber; }
        virtual simtime_t getSimulationTime() override { return eventEntry->simulationTime; }
        virtual int getModuleId() override { return eventEntry->moduleId; }
        virtual msgid_t getMessageId() override { return eventEntry->messageId; }
        virtual eventnumber_t getCauseEventNumber() override { return eventEntry->causeEventNumber; }

        virtual bool isSelfMessage(BeginSendEntry *beginSendEntry) override;
        virtual bool isSelfMessageProcessingEvent() override;
        virtual EndSendEntry *getEndSendEntry(BeginSendEntry *beginSendEntry) override;
        virtual ComponentMethodEndEntry *getComponentMethodEndEntry(ComponentMethodBeginEntry *componentMethodBeginEntry);
        simtime_t getTransmissionDelay(BeginSendEntry *beginSendEntry);
        simtime_t getRemainingDuration(BeginSendEntry *beginSendEntry);
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
        void deleteCauses();
        void deleteConsequences();
        void deleteEventLogEntries();
        void deleteAllocatedObjects();

        /**
         * Searches for the first eventlog entry that is reusing the provided message id.
         * The search starts from the immediately following event and ends at the end of
         * the eventlog. Returns nullptr if no such eventlog entry is found.
         */
        MessageDescriptionEntry *findReuseMessageDescriptionEntry(int messageId);
        MessageDescriptionEntry *findLocalReuseMessageDescriptionEntry(eventnumber_t previousEventNumber, int messageId);
};

}  // namespace eventlog
}  // namespace omnetpp


#endif
