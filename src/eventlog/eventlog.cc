//=========================================================================
//  EVENTLOG.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>
#include "filereader.h"
#include "stringpool.h"
#include "event.h"
#include "eventlog.h"

StringPool eventLogStringPool;

EventLog::EventLog(FileReader *reader) : EventLogIndex(reader)
{
    parseInitializationLogEntries();
}

EventLog::~EventLog()
{
    for (EventLogEntryList::iterator it = initializationLogEntries.begin(); it != initializationLogEntries.end(); it++)
    {
        delete *it;
    }

    for (EventNumberToEventMap::iterator it = eventNumberToEventMap.begin(); it != eventNumberToEventMap.end(); it++)
    {
        delete it->second;
    }

    for (MessageIdToEventMap::iterator it = messageIdToSenderEventMap.begin(); it != messageIdToSenderEventMap.end(); it++)
    {
        delete it->second;
    }
}

void EventLog::parseInitializationLogEntries()
{
    long firstOffset = getOffsetForEventNumber(0);
    reader->seekTo(0);

    do
    {
        char *line = reader->readLine();

        if (!line)
            break;

        EventLogEntry *eventLogEntry = EventLogEntry::parseEntry(line);

        if (eventLogEntry && !dynamic_cast<EventEntry *>(eventLogEntry))
            initializationLogEntries.push_back(eventLogEntry);
    }
    while (reader->lineStartOffset() < firstOffset);
}

void EventLog::parse(long fromEventNumber, long toEventNumber)
{
    long fromOffset = getOffsetForEventNumber(fromEventNumber);
    long toOffset = getOffsetForEventNumber(toEventNumber);
    long offset = fromOffset;

    if (fromOffset == -1 || toOffset == -1)
        throw new Exception("Could not find requested events");

    while (offset <= toOffset)
    {
        Event *event = new Event();
        offset = event->parse(reader, offset);

        if (eventNumberToEventMap.find(event->getEventNumber()) ==  eventNumberToEventMap.end())
        {
            eventNumberToEventMap[event->getEventNumber()] = event;
        }
    }
}

void EventLog::printInitializationLogEntries(FILE *file)
{
    for (EventLogEntryList::iterator it = initializationLogEntries.begin(); it != initializationLogEntries.end(); it++)
    {
        (*it)->print(file);
    }
}

void EventLog::printEvents(FILE *file)
{
    for (EventNumberToEventMap::iterator it = eventNumberToEventMap.begin(); it != eventNumberToEventMap.end(); it++)
    {
        it->second->print(file);
    }
}

void EventLog::print(FILE *file)
{
    printInitializationLogEntries(file);
    printEvents(file);
}

Event *EventLog::getEvent(long eventNumber)
{
    EventNumberToEventMap::iterator it = eventNumberToEventMap.find(eventNumber);

    if (it != eventNumberToEventMap.end())
        return it->second;
    else
    {
        long offset = getOffsetForEventNumber(eventNumber);

        if (offset != -1)
        {
            Event *event = new Event();
            event->parse(reader, offset);

            eventNumberToEventMap[eventNumber] = event;

            return event;
        }
        else
            return NULL;
    }
}

Event *EventLog::getCause(Event *event)
{
    MessageIdToEventMap::iterator it = messageIdToSenderEventMap.find(event->getEventEntry()->messageId);

    if (it != messageIdToSenderEventMap.end())
        return it->second;

    // TODO: read event based on the message's sending time or store sender event id directly?
    return NULL;
}
