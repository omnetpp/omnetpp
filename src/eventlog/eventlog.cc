//=========================================================================
//  EVENTLOG.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "filereader.h"
#include "stringpool.h"
#include "eventlog.h"

NAMESPACE_BEGIN

CommonStringPool eventLogStringPool;

EventLog::EventLog(FileReader *reader) : EventLogIndex(reader)
{
    reader->setSynchronizeWhenAppended(false);

    clearInternalState();
    parseInitializationLogEntries();
}

EventLog::~EventLog()
{
    deleteAllocatedObjects();
}

void EventLog::deleteAllocatedObjects()
{
    for (EventLogEntryList::iterator it = initializationLogEntries.begin(); it != initializationLogEntries.end(); it++)
        delete *it;

    for (EventNumberToEventMap::iterator it = eventNumberToEventMap.begin(); it != eventNumberToEventMap.end(); it++)
        delete it->second;

    clearInternalState();
}

void EventLog::clearInternalState(FileReader::FileChangedState change)
{
    Assert(change != FileReader::UNCHANGED);
    approximateNumberOfEvents = -1;

    // lastEvent should be set to null if the last event becomes a different one
    if (change == FileReader::OVERWRITTEN) {
        numParsedEvents = 0;

        progressCallInterval = CLOCKS_PER_SEC;
        lastProgressCall = -1;

        firstEvent = NULL;
        lastEvent = NULL;

        messageNames.clear();
        messageClassNames.clear();

        initializationLogEntries.clear();
        moduleIdToModuleCreatedEntryMap.clear();
        moduleIdAndGateIdToGateCreatedEntryMap.clear();

        simulationBeginEntry = NULL;

        eventNumberToEventMap.clear();
        beginOffsetToEventMap.clear();
        endOffsetToEventMap.clear();
    }
    else if (lastEvent && lastEvent->getBeginOffset() != getLastEventOffset())
        lastEvent = NULL;
}

void EventLog::synchronize(FileReader::FileChangedState change)
{
    if (change != FileReader::UNCHANGED) {
        if (change == FileReader::OVERWRITTEN)
            deleteAllocatedObjects();

        Event *lastEvent = this->lastEvent;

        IEventLog::synchronize(change);
        EventLogIndex::synchronize(change);

        if (change == FileReader::APPENDED) {
            clearInternalState(change);

            // always update the old last event because it might have been incomplete
            if (lastEvent)
                parseEvent(lastEvent, lastEvent->getBeginOffset());

            for (EventNumberToEventMap::iterator it = eventNumberToEventMap.begin(); it != eventNumberToEventMap.end(); it++)
                it->second->synchronize();
        }
        else
            parseInitializationLogEntries();
    }
}

ProgressMonitor EventLog::setProgressMonitor(ProgressMonitor newProgressMonitor)
{
    ProgressMonitor oldProgressMonitor = progressMonitor;
    progressMonitor = newProgressMonitor;
    return oldProgressMonitor;
}

void EventLog::progress()
{
    if (lastProgressCall + progressCallInterval < clock()) {
        progressMonitor.progress(this);
        lastProgressCall = clock();
    }
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
            file_offset_t beginOffset = firstEvent->getBeginOffset();
            file_offset_t endOffset = lastEvent->getEndOffset();
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
            approximateNumberOfEvents = (long)((endOffset - beginOffset) / average);
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
        file_offset_t beginOffset = firstEvent->getBeginOffset();
        file_offset_t endOffset = lastEvent->getBeginOffset();
        file_offset_t offset = beginOffset + (file_offset_t)((endOffset - beginOffset) * percentage);

        long eventNumber;
        file_offset_t lineStartOffset = -1, lineEndOffset;
        simtime_t simulationTime;
        readToEventLine(true, offset, eventNumber, simulationTime, lineStartOffset, lineEndOffset);

        Event *event = NULL;

        if (lineStartOffset == -1)
            event = getLastEvent();
        else
            event = getEventForBeginOffset(lineStartOffset);

        Assert(event);

        return event;
    }
}

void EventLog::parseInitializationLogEntries()
{
    int index = 0;
    reader->seekTo(0);

    if (PRINT_DEBUG_MESSAGES) printf("Parsing initialization log entries at: 0\n");

    while (true)
    {
        char *line = reader->getNextLineBufferPointer();

        if (!line)
            break;

        EventLogEntry *eventLogEntry = EventLogEntry::parseEntry(NULL, index, line, reader->getCurrentLineLength());

        if (!eventLogEntry)
            continue;
        else
            index++;

        if (dynamic_cast<EventEntry *>(eventLogEntry)) {
            delete eventLogEntry;
            break;
        }

        initializationLogEntries.push_back(eventLogEntry);
        cacheEventLogEntry(eventLogEntry);
    }
}

void EventLog::printInitializationLogEntries(FILE *file)
{
    for (EventLogEntryList::iterator it = initializationLogEntries.begin(); it != initializationLogEntries.end(); it++)
        (*it)->print(file);
}

std::vector<ModuleCreatedEntry *> EventLog::getModuleCreatedEntries()
{
    std::vector<ModuleCreatedEntry *> moduleCreatedEntries;

    for (ModuleIdToModuleCreatedEntryMap::iterator it = moduleIdToModuleCreatedEntryMap.begin(); it != moduleIdToModuleCreatedEntryMap.end(); it++)
        moduleCreatedEntries.push_back(it->second);

    return moduleCreatedEntries;
}

Event *EventLog::getFirstEvent()
{
    if (!firstEvent) {
        file_offset_t offset = getFirstEventOffset();

        if (offset != -1)
            firstEvent = getEventForBeginOffset(offset);
    }

    return firstEvent;
}

Event *EventLog::getLastEvent()
{
    if (!lastEvent) {
        file_offset_t offset = getLastEventOffset();

        if (offset != -1)
            lastEvent = getEventForBeginOffset(offset);
    }

    return lastEvent;
}

Event *EventLog::getEventForEventNumber(long eventNumber, MatchKind matchKind)
{
    Assert(eventNumber >= 0);

    if (matchKind == EXACT) {
        EventNumberToEventMap::iterator it = eventNumberToEventMap.find(eventNumber);
        if (it != eventNumberToEventMap.end())
            return it->second;

        // the following two are still faster than binary searching
        it = eventNumberToEventMap.find(eventNumber - 1);
        if (it != eventNumberToEventMap.end())
            return it->second->getNextEvent();

        it = eventNumberToEventMap.find(eventNumber + 1);
        if (it != eventNumberToEventMap.end())
            return it->second->getPreviousEvent();
    }

    // TODO: maybe cache result
    file_offset_t offset = getOffsetForEventNumber(eventNumber, matchKind);

    if (offset == -1)
        return NULL;
    else
        return getEventForBeginOffset(offset);
}

Event *EventLog::getNeighbourEvent(IEvent *event, long distance)
{
    Assert(event);
    return (Event *)IEventLog::getNeighbourEvent(event, distance);
}

Event *EventLog::getEventForSimulationTime(simtime_t simulationTime, MatchKind matchKind)
{
    Assert(simulationTime >= 0);

    file_offset_t offset = getOffsetForSimulationTime(simulationTime, matchKind);

    if (offset == -1)
        return NULL;
    else
        return getEventForBeginOffset(offset);
}

EventLogEntry *EventLog::findEventLogEntry(EventLogEntry *start, const char *search, bool forward, bool caseSensitive)
{
    char *line;
    reader->seekTo(start->getEvent()->getBeginOffset());
    int index = start->getEntryIndex();

    for (int i = 0; i < index + forward ? 1 : 0; i++)
        reader->getNextLineBufferPointer();

    if (forward)
        line = reader->findNextLineBufferPointer(search, caseSensitive);
    else
        line = reader->findPreviousLineBufferPointer(search, caseSensitive);

    if (line) {
        if (forward)
            line = reader->getPreviousLineBufferPointer();

        index = 0;

        do {
            if (line[0] == 'E' && line[1] == ' ')
                return getEventForBeginOffset(reader->getCurrentLineStartOffset())->getEventLogEntry(index);
            else if (line[0] != '\r' && line[0] != '\n')
                index++;
        }
        while ((line = reader->getPreviousLineBufferPointer()));
    }

    return NULL;
}

Event *EventLog::getEventForBeginOffset(file_offset_t beginOffset)
{
    Assert(beginOffset >= 0);
    OffsetToEventMap::iterator it = beginOffsetToEventMap.find(beginOffset);

    if (it != beginOffsetToEventMap.end())
        return it->second;
    else if (reader->getFileSize() != beginOffset)
    {
        Event *event = new Event(this);
        parseEvent(event, beginOffset);
        cacheEvent(event);
        return event;
    }
    else {
        beginOffsetToEventMap[beginOffset] = NULL;
        return NULL;
    }
}

Event *EventLog::getEventForEndOffset(file_offset_t endOffset)
{
    Assert(endOffset >= 0);
    OffsetToEventMap::iterator it = endOffsetToEventMap.find(endOffset);

    if (it != endOffsetToEventMap.end())
        return it->second;
    else {
        file_offset_t beginOffset = getBeginOffsetForEndOffset(endOffset);

        if (beginOffset == -1) {
            endOffsetToEventMap[endOffset] = NULL;
            return NULL;
        }
        else
            return getEventForBeginOffset(beginOffset);
    }
}

void EventLog::parseEvent(Event *event, file_offset_t beginOffset)
{
    event->parse(reader, beginOffset);
    cacheEntry(event->getEventNumber(), event->getSimulationTime(), event->getBeginOffset(), event->getEndOffset());
    cacheEventLogEntries(event);
    numParsedEvents++;
}

void EventLog::cacheEventLogEntries(Event *event)
{
    for (int i = 0; i < event->getNumEventLogEntries(); i++)
        cacheEventLogEntry(event->getEventLogEntry(i));
}

void EventLog::uncacheEventLogEntries(Event *event)
{
    for (int i = 0; i < event->getNumEventLogEntries(); i++)
        uncacheEventLogEntry(event->getEventLogEntry(i));
}

void EventLog::cacheEventLogEntry(EventLogEntry *eventLogEntry)
{
    // simulation begin entry
    SimulationBeginEntry *simulationBeginEntry = dynamic_cast<SimulationBeginEntry *>(eventLogEntry);

    if (simulationBeginEntry)
        this->simulationBeginEntry = simulationBeginEntry;

    // collect module created entries
    ModuleCreatedEntry *moduleCreatedEntry = dynamic_cast<ModuleCreatedEntry *>(eventLogEntry);

    if (moduleCreatedEntry)
        moduleIdToModuleCreatedEntryMap[moduleCreatedEntry->moduleId] = moduleCreatedEntry;

    // collect gate created entries
    GateCreatedEntry *gateCreatedEntry = dynamic_cast<GateCreatedEntry *>(eventLogEntry);

    if (gateCreatedEntry) {
        std::pair<int, int> key(gateCreatedEntry->moduleId, gateCreatedEntry->gateId);
        moduleIdAndGateIdToGateCreatedEntryMap[key] = gateCreatedEntry;
    }

    // colllect begin send entry
    BeginSendEntry *beginSendEntry = dynamic_cast<BeginSendEntry *>(eventLogEntry);
    if (beginSendEntry) {
        messageNames.insert(beginSendEntry->messageFullName);
        messageClassNames.insert(beginSendEntry->messageClassName);
    }
}

void EventLog::uncacheEventLogEntry(EventLogEntry *eventLogEntry)
{
    // collect module created entries
    ModuleCreatedEntry *moduleCreatedEntry = dynamic_cast<ModuleCreatedEntry *>(eventLogEntry);

    if (moduleCreatedEntry)
        moduleIdToModuleCreatedEntryMap.erase(moduleCreatedEntry->moduleId);

    // collect gate created entries
    GateCreatedEntry *gateCreatedEntry = dynamic_cast<GateCreatedEntry *>(eventLogEntry);

    if (gateCreatedEntry) {
        std::pair<int, int> key(gateCreatedEntry->moduleId, gateCreatedEntry->gateId);
        moduleIdAndGateIdToGateCreatedEntryMap.erase(key);
    }
}

void EventLog::cacheEvent(Event *event)
{
    int eventNumber = event->getEventNumber();
    Assert(!lastEvent || eventNumber <= lastEvent->getEventNumber());
    Assert(eventNumberToEventMap.find(eventNumber) == eventNumberToEventMap.end());

    eventNumberToEventMap[eventNumber] = event;
    beginOffsetToEventMap[event->getBeginOffset()] = event;
    endOffsetToEventMap[event->getEndOffset()] = event;
}

NAMESPACE_END
