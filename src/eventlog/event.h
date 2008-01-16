//=========================================================================
//  EVENT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENT_H_
#define __EVENT_H_

#include <sstream>
#include <algorithm>
#include <vector>
#include "ievent.h"
#include "filereader.h"
#include "eventlogentry.h"
#include "eventlogentries.h"
#include "messagedependency.h"

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
        int numEventLogMessages;
        int numBeginSendEntries;

        typedef std::vector<EventLogEntry *> EventLogEntryList;
        EventLogEntryList eventLogEntries; // all entries parsed from the file (lines below "E" line)

        /**
         * A is a cause of B if and only if B is a consequence of A.
         */
        MessageDependency *cause; // the message send which is processed in this event
        IMessageDependencyList *causes; // the arrival message sends of messages which we send in this event
        IMessageDependencyList *consequences; // message sends in this event

        Event *getReuserEvent(int &beginSendEntryNumber);

    public:
        Event(EventLog *eventLog);
        ~Event();

        /**
         * Parse an event starting at the given offset.
         */
        file_offset_t parse(FileReader *reader, file_offset_t offset);

        // IEvent interface
        virtual void synchronize();
        virtual IEventLog *getEventLog();

        virtual ModuleCreatedEntry *getModuleCreatedEntry();

        virtual file_offset_t getBeginOffset() { return beginOffset; }
        virtual file_offset_t getEndOffset() { return endOffset; }

        virtual EventEntry *getEventEntry() { return eventEntry; }
        virtual int getNumEventLogEntries() { return eventLogEntries.size(); }
        virtual EventLogEntry *getEventLogEntry(int index) { return eventLogEntries[index]; }

        virtual int getNumEventLogMessages() { return numEventLogMessages; }
        virtual int getNumBeginSendEntries() { return numBeginSendEntries; }
        virtual EventLogMessageEntry *getEventLogMessage(int index);

        virtual long getEventNumber() { return eventEntry->eventNumber; }
        virtual simtime_t& getSimulationTime() { return eventEntry->simulationTime; }
        virtual int getModuleId() { return eventEntry->moduleId; }
        virtual long getMessageId() { return eventEntry->messageId; }
        virtual long getCauseEventNumber() { return eventEntry->causeEventNumber; }

        virtual bool isSelfMessageProcessingEvent();
        virtual Event *getPreviousEvent();
        virtual Event *getNextEvent();

        virtual Event *getCauseEvent();
        virtual BeginSendEntry *getCauseBeginSendEntry();
        virtual MessageDependency *getCause();
        virtual IMessageDependencyList *getCauses();
        virtual IMessageDependencyList *getConsequences();

        virtual void print(FILE *file = stdout, bool outputEventLogMessages = true);

    protected:
        void deleteAllocatedObjects();
        void clearInternalState();
};

#endif


