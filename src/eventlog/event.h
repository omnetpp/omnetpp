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
#include <vector>
#include "filereader.h"
#include "eventlogentry.h"
#include "eventlogentries.h"
#include "messagedependency.h"

/**
 * Manages all event log entries for a single event. (All lines belonging to an "E" line.)
 * Returned Event*, EventLogEntry*, EventEntry*, MessageSend*, MessageDependencyList* pointers should not be
 * remembered by callers, because they may get deleted when the event is
 * thrown out of the eventlog cache.
 */
class Event
{
    public:
        typedef std::vector<MessageDependency *> MessageDependencyList;

    protected:
        EventLog *eventLog; // the corresponding event log
        long beginOffset; // file offset where the event starts
        long endOffset; // file offset where the event ends (ie. begin of next event)
        EventEntry *eventEntry; // the event log entry that corresponds to the actual event ("E" line)

        typedef std::vector<EventLogEntry *> EventLogEntryList;
        EventLogEntryList eventLogEntries; // all entries parsed from the file (lines below "E" line)

        MessageSend *cause; // the message send which is processed in this event
        MessageDependencyList *causes; // the arrival message sends of messages which we send in this event
        MessageDependencyList *consequences; // message sends in this event

        static long numParsedEvent; // the number of events parsed so far

    public:
        Event(EventLog *eventLog);
        ~Event();

        long getBeginOffset() { return beginOffset; };
        long getEndOffset() { return endOffset; };

        EventEntry *getEventEntry() { return eventEntry; };
        long getEventNumber() { return eventEntry->eventNumber; };
        simtime_t getSimulationTime() { return eventEntry->simulationTime; };
        long getMessageId() { return eventEntry->messageId; };
        long getCauseEventNumber() { return eventEntry->causeEventNumber; };
        EventLogEntry *getEventLogEntry(int index) { return eventLogEntries[index]; };
        int getNumEventLogEntries() { return eventLogEntries.size(); };

        Event *getCauseEvent();
        MessageSend *getCause();
        MessageDependencyList *getCauses();
        MessageDependencyList *getConsequences();

        long parse(FileReader *index, long offset);
        static long getNumParsedEvent() { return numParsedEvent; };
        void print(FILE *file = stdout);
};

#endif