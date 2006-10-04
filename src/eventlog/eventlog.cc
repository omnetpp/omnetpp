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
    parseInitializationLogEntries();
}

EventLog::~EventLog()
{
    for (EventLogEntryList::iterator it = initializationLogEntries.begin(); it != initializationLogEntries.end(); it++)
        delete *it;

    for (EventNumberToEventMap::iterator it = eventNumberToEventMap.begin(); it != eventNumberToEventMap.end(); it++)
        delete it->second;
}

long EventLog::getNumEventsApproximation()
{
    if (numEventsApproximation == -1)
    {
        Event *firstEvent = getFirstEvent();
        Event *lastEvent = getLastEvent();

        if (firstEvent == NULL)
            numEventsApproximation = 0;
        else
        {
            long beginOffset = firstEvent->getBeginOffset();
            long endOffset = lastEvent->getEndOffset();
            long averageSize = 0;
            long averageCount = 0;

            for (int i = 0; i < 1000; i++)
            {
                if (firstEvent) {
                    averageSize += firstEvent->getEndOffset() - firstEvent->getBeginOffset();
                    averageCount++;
                    firstEvent = firstEvent->getNextEvent();
                }

                if (lastEvent) {
                    averageSize += lastEvent->getEndOffset() - lastEvent->getBeginOffset();
                    averageCount++;
                    lastEvent = lastEvent->getPreviousEvent();
                }
            }

            averageSize /= averageCount;
            numEventsApproximation = (endOffset - beginOffset) / averageSize;
        }
    }

    return numEventsApproximation;
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

        ModuleCreatedEntry *moduleCreatedEntry = dynamic_cast<ModuleCreatedEntry *>(eventLogEntry);

        if (eventLogEntry && moduleCreatedEntry)
            initializationModuleIdToModuleCreatedEntryMap[moduleCreatedEntry->moduleId] = moduleCreatedEntry;
    }
    while (reader->lineStartOffset() < firstOffset);
}

void EventLog::printInitializationLogEntries(FILE *file)
{
    for (EventLogEntryList::iterator it = initializationLogEntries.begin(); it != initializationLogEntries.end(); it++)
        (*it)->print(file);
}

Event *EventLog::getEventForEventNumber(long eventNumber, MatchKind matchKind)
{
    // TODO: use matchKind
    EASSERT(eventNumber >= 0);
    EventNumberToEventMap::iterator it = eventNumberToEventMap.find(eventNumber);

    if (it != eventNumberToEventMap.end())
        return it->second;
    else
    {
        long offset = getOffsetForEventNumber(eventNumber);

        if (offset == -1)
            return NULL;
        else
            return getEventForBeginOffset(offset);
    }
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

Event *EventLog::getEventForBeginOffset(long offset)
{
    if (offset < 0)
        throw new Exception("Offset number must be >= 0, %d", offset);

    OffsetToEventMap::iterator it = offsetToEventMap.find(offset);

    if (it != offsetToEventMap.end())
        return it->second;
    else
    {
        Event *event = new Event(this);
        event->parse(reader, offset);
        return cacheEvent(event);
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
