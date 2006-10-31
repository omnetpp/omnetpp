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

#include "filereader.h"
#include "stringpool.h"
#include "eventlog.h"

StringPool eventLogStringPool;

EventLog::EventLog(FileReader *reader) : EventLogIndex(reader)
{
    approximateNumberOfEvents = -1;
    parseInitializationLogEntries();
}

EventLog::~EventLog()
{
    for (EventLogEntryList::iterator it = initializationLogEntries.begin(); it != initializationLogEntries.end(); it++)
        delete *it;

    for (EventNumberToEventMap::iterator it = eventNumberToEventMap.begin(); it != eventNumberToEventMap.end(); it++)
        delete it->second;
}

long EventLog::getApproximateNumberOfEvents()
{
    if (approximateNumberOfEvents == -1)
    {
        Event *firstEvent = getFirstEvent();
        Event *lastEvent = getLastEvent();

        if (firstEvent == NULL)
            approximateNumberOfEvents = 0;
        else
        {
            long beginOffset = firstEvent->getBeginOffset();
            long endOffset = lastEvent->getEndOffset();
            long sum = 0;
            long count = 0;
            int eventCount = 100;

            for (int i = 0; i < eventCount; i++)
            {
                if (firstEvent) {
                    sum += firstEvent->getEndOffset() - firstEvent->getBeginOffset();
                    count++;
                    firstEvent = firstEvent->getNextEvent();
                }

                if (lastEvent) {
                    sum += lastEvent->getEndOffset() - lastEvent->getBeginOffset();
                    count++;
                    lastEvent = lastEvent->getPreviousEvent();
                }
            }

            double average = (double)sum / count;
            approximateNumberOfEvents = (endOffset - beginOffset) / average;
        }
    }

    return approximateNumberOfEvents;
}

Event *EventLog::getApproximateEventAt(double percentage)
{
    Event *firstEvent = getFirstEvent();
    Event *lastEvent = getLastEvent();

    if (firstEvent == NULL)
        return NULL;
    else {
        long beginOffset = firstEvent->getBeginOffset();
        long endOffset = lastEvent->getBeginOffset();
        long offset = beginOffset + (endOffset - beginOffset) * percentage;

        long eventNumber, lineStartOffset = -1, lineEndOffset;
        simtime_t simulationTime;
        readToEventLine(true, offset, eventNumber, simulationTime, lineStartOffset, lineEndOffset);

        if (lineStartOffset == -1)
            return getLastEvent();
        else
            return getEventForBeginOffset(lineStartOffset);
    }
}

void EventLog::parseInitializationLogEntries()
{
    reader->seekTo(0);

    if (PRINT_DEBUG_MESSAGES) printf("Parsing initialization log entries at: 0\n");

    while (true)
    {
        char *line = reader->readNextLine();

        if (!line)
            break;

        EventLogEntry *eventLogEntry = EventLogEntry::parseEntry(NULL, line, reader->getLastLineLength());

        if (dynamic_cast<EventEntry *>(eventLogEntry))
            break;

        if (eventLogEntry)
            initializationLogEntries.push_back(eventLogEntry);

        ModuleCreatedEntry *moduleCreatedEntry = dynamic_cast<ModuleCreatedEntry *>(eventLogEntry);

        if (moduleCreatedEntry)
            initializationModuleIdToModuleCreatedEntryMap[moduleCreatedEntry->moduleId] = moduleCreatedEntry;
    }
}

void EventLog::printInitializationLogEntries(FILE *file)
{
    for (EventLogEntryList::iterator it = initializationLogEntries.begin(); it != initializationLogEntries.end(); it++)
        (*it)->print(file);
}

Event *EventLog::getEventForEventNumber(long eventNumber, MatchKind matchKind)
{
    EASSERT(eventNumber >= 0);

    // TODO: use matchKind
    if (matchKind == EXACT) {
        EventNumberToEventMap::iterator it = eventNumberToEventMap.find(eventNumber);

        if (it != eventNumberToEventMap.end())
            return it->second;
    }

    long offset = getOffsetForEventNumber(eventNumber, matchKind);

    if (offset == -1)
        return NULL;
    else
        return getEventForBeginOffset(offset);
}

Event *EventLog::getNeighbourEvent(IEvent *event, long distance)
{
    return (Event *)IEventLog::getNeighbourEvent(event, distance);
}

Event *EventLog::getEventForSimulationTime(simtime_t simulationTime, MatchKind matchKind)
{
    // TODO: use matchKind
    EASSERT(simulationTime >= 0);

    long offset = getOffsetForSimulationTime(simulationTime, matchKind);

    if (offset == -1)
        return NULL;
    else
        return getEventForBeginOffset(offset);
}

Event *EventLog::getEventForBeginOffset(long beginOffset)
{
    if (beginOffset < 0)
        throw new Exception("Offset number must be >= 0, %d", beginOffset);

    OffsetToEventMap::iterator it = offsetToEventMap.find(beginOffset);

    long eventNumber, lineStartOffset, lineEndOffset;
    simtime_t simulationTime;

    if (it != offsetToEventMap.end())
        return it->second;
    else if (reader->getFileSize() != beginOffset)
    {
        Event *event = new Event(this);
        event->parse(reader, beginOffset);
        numParsedEvents++;
        return cacheEvent(event);
    }
    else {
        offsetToEventMap[beginOffset] = NULL;
        return NULL;
    }
}

Event *EventLog::getEventForEndOffset(long endOffset)
{
    long beginOffset = getBeginOffsetForEndOffset(endOffset);

    if (beginOffset == -1)
        return NULL;
    else
        return getEventForBeginOffset(beginOffset);
}

Event *EventLog::cacheEvent(Event *event)
{
    eventNumberToEventMap[event->getEventNumber()] = event;
    offsetToEventMap[event->getBeginOffset()] = event;
    return event;
}
