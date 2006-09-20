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

class Event;
class EventLog;

/**
 * Represents a single message send.
 */
class MessageSend
{
    protected:
        EventLog *eventLog;
        long senderEventNumber;
        long arrivalEventNumber;
        int messageSendEntryNumber;

    public:
        MessageSend(EventLog *eventLog, long senderEventNumber, int messageSendEntryNumber);

        long getSenderEventNumber() { return senderEventNumber; };
        Event *getSenderEvent();

        long getArrivalEventNumber();
        Event *getArrivalEvent();

        long getMessageSendEntryNumber() { return messageSendEntryNumber; };
        EventLogEntry *getMessageSendEntry();

        static bool isMessageSend(EventLogEntry *eventLogEntry);
        static long getMessageId(EventLogEntry *eventLogEntry);

        long getMessageId();
        simtime_t getArrivalTime();
};

/**
 * Manages all event log entries for a single event.
 */
class Event
{
    public:
        typedef std::vector<MessageSend *> MessageSendList;
        typedef std::vector<Event *> EventList;

    protected:
        EventLog *eventLog; // the corresponding event log
        long beginOffset; // file offset where the event starts
        long endOffset; // file offset where the event ends
        EventEntry *eventEntry; // the event log entry that corresponds to the actual event

        typedef std::vector<EventLogEntry *> EventLogEntryList;
        EventLogEntryList eventLogEntries; // all entries parsed from the file

        MessageSend *cause; // the message send which is processed in this event
        MessageSendList *causes; // the arrival message sends of messages which we send in this event
        MessageSendList *consequences; // message sends in this event

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
        MessageSendList *getCauses(); // the returned EventList must be deleted
        MessageSendList *getConsequences(); // the returned EventList must be deleted

        long parse(FileReader *index, long offset);
        static long getNumParsedEvent() { return numParsedEvent; };
        void print(FILE *file);
};

#endif