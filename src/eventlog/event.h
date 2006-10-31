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
 * Returned Event*, EventLogEntry*, EventEntry*, MessageSend*, MessageDependencyList* pointers should not be
 * remembered by callers, because they may get deleted when the event is
 * thrown out of the eventlog cache.
 */
class Event : public IEvent
{
    protected:
        EventLog *eventLog; // the corresponding event log
        long beginOffset; // file offset where the event starts
        long endOffset; // file offset where the event ends (ie. begin of next event)
        EventEntry *eventEntry; // the event log entry that corresponds to the actual event ("E" line)

        typedef std::vector<EventLogEntry *> EventLogEntryList;
        EventLogEntryList eventLogEntries; // all entries parsed from the file (lines below "E" line)

        typedef std::vector<EventLogMessage *> EventLogMessageList;
        EventLogMessageList eventLogMessages;

        /**
         * A is a cause of B if and only if B is a consequence of A.
         */
        MessageSend *cause; // the message send which is processed in this event
        MessageDependencyList *causes; // the arrival message sends of messages which we send in this event
        MessageDependencyList *consequences; // message sends in this event

    public:
        Event(EventLog *eventLog);
        ~Event();

        /**
         * Parse an event starting at the given offset.
         */
        long parse(FileReader *reader, long offset);

        // IEvent interface
        virtual IEventLog *getEventLog();

        virtual long getBeginOffset() { return beginOffset; }
        virtual long getEndOffset() { return endOffset; }

        virtual EventEntry *getEventEntry() { return eventEntry; }
        virtual int getNumEventLogEntries() { return eventLogEntries.size(); }
        virtual EventLogEntry *getEventLogEntry(int index) { return eventLogEntries[index]; }

        virtual int getNumEventLogMessages() { return eventLogMessages.size(); }
        virtual EventLogMessage *getEventLogMessage(int index) { return eventLogMessages[index]; }
        virtual int getEventLogMessageIndex(EventLogMessage *eventLogMessage) { return find(eventLogMessages.begin(), eventLogMessages.end(), eventLogMessage) - eventLogMessages.begin(); }

        virtual long getEventNumber() { return eventEntry->eventNumber; }
        virtual simtime_t getSimulationTime() { return eventEntry->simulationTime; }
        virtual int getModuleId() { return eventEntry->moduleId; }
        virtual long getMessageId() { return eventEntry->messageId; }
        virtual long getCauseEventNumber() { return eventEntry->causeEventNumber; }

        virtual Event *getPreviousEvent();
        virtual Event *getNextEvent();

        virtual Event *getCauseEvent();
        virtual MessageSend *getCause();
        virtual MessageDependencyList *getCauses();
        virtual MessageDependencyList *getConsequences();

        virtual void print(FILE *file = stdout);
};

#endif