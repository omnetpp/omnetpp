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

    eventNumberToEventMap.clear();

    while (offset <= toOffset)
    {
        Event *event = new Event();
        offset = event->parse(reader, offset);
        eventNumberToEventMap[event->eventNumber()] = *event;
    }
}

void EventLog::printInitializationLogEntries(FILE *file)
{
    for (EventLogEntryList::iterator it = initializationLogEntries.begin(); it != initializationLogEntries.end(); it++)
    {
        (*it)->print(file);
    }    
}

void EventLog::print(FILE *file)
{
    printInitializationLogEntries(file);

    for (EventNumberToEventMap::iterator it = eventNumberToEventMap.begin(); it != eventNumberToEventMap.end(); it++)
    {
        it->second.print(file);
    }
}
