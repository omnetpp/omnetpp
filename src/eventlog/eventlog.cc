//=========================================================================
//  EVENTLOG.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include "common/filereader.h"
#include "common/stringpool.h"
#include "eventlog.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace eventlog {

StringPool eventLogStringPool;

EventLog::EventLog(FileReader *reader) : EventLogIndex(reader)
{
    reader->setIgnoreAppendChanges(false);
    clearInternalState();
    parseKeyframes();
}

EventLog::~EventLog()
{
    deleteAllocatedObjects();
}

void EventLog::clearInternalState()
{
    numParsedEvents = 0;
    approximateNumberOfEvents = -1;
    progressCallInterval = CLOCKS_PER_SEC;
    lastProgressCall = -1;
    firstEvent = nullptr;
    lastEvent = nullptr;
    messageNames.clear();
    messageClassNames.clear();
    moduleIdToModuleCreatedEntryMap.clear();
    moduleIdAndGateIdToGateCreatedEntryMap.clear();
    previousEventNumberToMessageEntriesMap.clear();
    simulationBeginEntry = nullptr;
    simulationEndEntry = nullptr;
    eventNumberToEventMap.clear();
    beginOffsetToEventMap.clear();
    endOffsetToEventMap.clear();
    consequenceLookaheadLimits.clear();
    previousEventNumberToMessageEntriesMap.clear();
}

void EventLog::deleteAllocatedObjects()
{
    for (auto & it : eventNumberToEventMap)
        delete it.second;
}

void EventLog::synchronize(FileReader::FileChangedState change)
{
    if (change != FileReader::UNCHANGED) {
        IEventLog::synchronize(change);
        EventLogIndex::synchronize(change);
        switch (change) {
            case FileReader::UNCHANGED:  // just to avoid unused enumeration value warnings
                break;
            case FileReader::OVERWRITTEN:
                deleteAllocatedObjects();
                clearInternalState();
                parseKeyframes();
                break;
            case FileReader::APPENDED:
                for (auto & it : eventNumberToEventMap)
                    it.second->synchronize(change);
                if (lastEvent) {
                    IEvent::unlinkNeighbourEvents(lastEvent);
                    eventNumberToEventMap.erase(lastEvent->getEventNumber());
                    eventNumberToCacheEntryMap.erase(lastEvent->getEventNumber());
                    beginOffsetToEventMap.erase(lastEvent->getBeginOffset());
                    endOffsetToEventMap.erase(lastEvent->getEndOffset());
                    if (firstEvent == lastEvent) {
                        firstEvent = nullptr;
                        simulationBeginEntry = nullptr;
                    }
                    delete lastEvent;
                    lastEvent = nullptr;
                }
                // TODO: we could do this incrementally
                parseKeyframes();
                break;
        }
    }
}

void EventLog::print(FILE *file, eventnumber_t fromEventNumber, eventnumber_t toEventNumber, bool outputEventLogMessages)
{
    IEvent *event = fromEventNumber == -1 ? getFirstEvent() : getFirstEventNotBeforeEventNumber(fromEventNumber);

    while (event != nullptr && (toEventNumber == -1 || event->getEventNumber() <= toEventNumber)) {
        event->print(file, outputEventLogMessages);
        event = event->getNextEvent();
        if (event)
            fprintf(file, "\n");
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
    if (lastProgressCall + progressCallInterval < (long)clock()) {
        progressMonitor.progress(this);
        lastProgressCall = clock();
    }
}

eventnumber_t EventLog::getApproximateNumberOfEvents()
{
    if (approximateNumberOfEvents == -1) {
        Event *firstEvent = getFirstEvent();
        Event *lastEvent = getLastEvent();

        if (firstEvent == nullptr)
            approximateNumberOfEvents = 0;
        else {
            file_offset_t beginOffset = firstEvent->getBeginOffset();
            file_offset_t endOffset = lastEvent->getEndOffset();
            long sum = 0;
            long count = 0;
            int eventCount = 100;

            for (int i = 0; i < eventCount; i++) {
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

    if (firstEvent == nullptr)
        return nullptr;
    else {
        file_offset_t beginOffset = firstEvent->getBeginOffset();
        file_offset_t endOffset = lastEvent->getEndOffset();
        file_offset_t offset = beginOffset + (file_offset_t)((endOffset - beginOffset) * percentage);

        eventnumber_t eventNumber;
        file_offset_t lineStartOffset = -1, lineEndOffset;
        simtime_t simulationTime;
        readToEventLine(false, offset, eventNumber, simulationTime, lineStartOffset, lineEndOffset);

        Event *event = nullptr;

        if (lineStartOffset == -1)
            event = getFirstEvent();
        else
            event = getEventForBeginOffset(lineStartOffset);

        Assert(event);

        return event;
    }
}

void EventLog::parseKeyframes()
{
    // NOTE: optimized for performance
    char *line;
    SimulationBeginEntry *simulationBeginEntry = getSimulationBeginEntry();
    if (simulationBeginEntry) {
        keyframeBlockSize = simulationBeginEntry->keyframeBlockSize;
        consequenceLookaheadLimits.resize(getLastEventNumber() / keyframeBlockSize + 1, 0);
        reader->seekTo(reader->getFileSize());
        while ((line = reader->getPreviousLineBufferPointer())) {
            EventLogEntry *eventLogEntry = (EventLogEntry *)EventLogEntry::parseEntry(this, nullptr, 0, reader->getCurrentLineStartOffset(), line, reader->getCurrentLineLength());
            if (KeyframeEntry *keyframeEntry = dynamic_cast<KeyframeEntry *>(eventLogEntry)) {
                // store consequenceLookaheadLimits from the keyframe
                char *s = const_cast<char *>(keyframeEntry->consequenceLookaheadLimits);
                while (*s != '\0') {
                    eventnumber_t eventNumber = strtol(s, &s, 10);
                    eventnumber_t keyframeIndex = eventNumber / keyframeBlockSize;
                    s++;
                    eventnumber_t consequenceLookaheadLimit = strtol(s, &s, 10);
                    s++;
                    consequenceLookaheadLimits[keyframeIndex] = std::max(consequenceLookaheadLimits[keyframeIndex], consequenceLookaheadLimit);
                }
                // TODO: store simulation state data from the keyframe
                // jump to previous keyframe
                reader->seekTo(keyframeEntry->previousKeyframeFileOffset + 1);
                reader->getNextLineBufferPointer();
            }
            else if (MessageEntry *messageEntry = dynamic_cast<MessageEntry *>(eventLogEntry)) {
                if (messageEntry->previousEventNumber != -1) {
                    int blockIndex = messageEntry->previousEventNumber / keyframeBlockSize;
                    // NOTE: the last event number is a reasonable approximation here
                    consequenceLookaheadLimits[blockIndex] = std::max(consequenceLookaheadLimits[blockIndex], getLastEventNumber() - messageEntry->previousEventNumber);
                }
            }
            delete eventLogEntry;
            progress();
        }
    }
}

std::vector<ModuleCreatedEntry *> EventLog::getModuleCreatedEntries()
{
    std::vector<ModuleCreatedEntry *> moduleCreatedEntries;

    for (ModuleIdToModuleCreatedEntryMap::iterator it = moduleIdToModuleCreatedEntryMap.begin(); it != moduleIdToModuleCreatedEntryMap.end(); ++it) {
        Assert(it->second);
        moduleCreatedEntries.push_back(it->second);
    }

    return moduleCreatedEntries;
}

ModuleCreatedEntry *EventLog::getModuleCreatedEntry(int moduleId)
{
    ModuleIdToModuleCreatedEntryMap::iterator it = moduleIdToModuleCreatedEntryMap.find(moduleId);

    if (it == moduleIdToModuleCreatedEntryMap.end())
        return nullptr;
    else
        return it->second;
}

GateCreatedEntry *EventLog::getGateCreatedEntry(int moduleId, int gateId)
{
    std::pair<int, int> key(moduleId, gateId);
    ModuleIdAndGateIdToGateCreatedEntryMap::iterator it = moduleIdAndGateIdToGateCreatedEntryMap.find(key);

    if (it == moduleIdAndGateIdToGateCreatedEntryMap.end())
        return nullptr;
    else
        return it->second;
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

Event *EventLog::getEventForEventNumber(eventnumber_t eventNumber, MatchKind matchKind, bool useCacheOnly)
{
    Assert(eventNumber >= 0);
    if (matchKind == EXACT) {
        EventNumberToEventMap::iterator it = eventNumberToEventMap.find(eventNumber);
        if (it != eventNumberToEventMap.end())
            return it->second;
        else if (useCacheOnly)
            return nullptr;
        else {
            // the following two are still faster than binary searching
            // but this may access the disk
            it = eventNumberToEventMap.find(eventNumber - 1);
            if (it != eventNumberToEventMap.end()) {
                Event *event = it->second->getNextEvent();
                // the file might be filtered
                return event && event->getEventNumber() == eventNumber ? event : nullptr;
            }

            it = eventNumberToEventMap.find(eventNumber + 1);
            if (it != eventNumberToEventMap.end()) {
                Event *event = it->second->getPreviousEvent();
                // the file might be filtered
                return event && event->getEventNumber() == eventNumber ? event : nullptr;
            }
        }
    }
    if (useCacheOnly)
        return nullptr;
    else {
        file_offset_t offset = getOffsetForEventNumber(eventNumber, matchKind);
        if (offset == -1)
            return nullptr;
        else
            return getEventForBeginOffset(offset);
    }
}

Event *EventLog::getNeighbourEvent(IEvent *event, eventnumber_t distance)
{
    Assert(event);
    return (Event *)IEventLog::getNeighbourEvent(event, distance);
}

Event *EventLog::getEventForSimulationTime(simtime_t simulationTime, MatchKind matchKind, bool useCacheOnly)
{
    if (useCacheOnly)
        return nullptr;
    else {
        Assert(simulationTime >= 0);
        file_offset_t offset = getOffsetForSimulationTime(simulationTime, matchKind);

        if (offset == -1)
            return nullptr;
        else
            return getEventForBeginOffset(offset);
    }
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
        } while ((line = reader->getPreviousLineBufferPointer()));
    }

    return nullptr;
}

Event *EventLog::getEventForBeginOffset(file_offset_t beginOffset)
{
    Assert(beginOffset >= 0);
    OffsetToEventMap::iterator it = beginOffsetToEventMap.find(beginOffset);

    if (it != beginOffsetToEventMap.end())
        return it->second;
    else if (reader->getFileSize() != beginOffset) {
        Event *event = new Event(this);
        parseEvent(event, beginOffset);
        cacheEvent(event);
        return event;
    }
    else {
        beginOffsetToEventMap[beginOffset] = nullptr;
        return nullptr;
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
            endOffsetToEventMap[endOffset] = nullptr;
            return nullptr;
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
    Assert(event->getEventEntry());
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

    // simulation begin entry
    SimulationEndEntry *simulationEndEntry = dynamic_cast<SimulationEndEntry *>(eventLogEntry);

    if (simulationEndEntry)
        this->simulationEndEntry = simulationEndEntry;

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
        messageNames.insert(beginSendEntry->messageName);
        messageClassNames.insert(beginSendEntry->messageClassName);
    }
}

void EventLog::uncacheEventLogEntry(EventLogEntry *eventLogEntry)
{
    // collect module created entries
//    ModuleCreatedEntry *moduleCreatedEntry = dynamic_cast<ModuleCreatedEntry *>(eventLogEntry);
//    if (moduleCreatedEntry)
//        moduleIdToModuleCreatedEntryMap.erase(moduleCreatedEntry->moduleId);

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

std::vector<MessageEntry *> EventLog::getMessageEntriesWithPreviousEventNumber(eventnumber_t previousEventNumber)
{
    std::map<eventnumber_t, std::vector<MessageEntry *> >::iterator it = previousEventNumberToMessageEntriesMap.find(previousEventNumber);
    if (it != previousEventNumberToMessageEntriesMap.end())
        return it->second;
    else {
        eventnumber_t keyframeBlockIndex = previousEventNumber / keyframeBlockSize;
        eventnumber_t beginEventNumber = (eventnumber_t)keyframeBlockIndex * keyframeBlockSize;
        eventnumber_t endEventNumber = beginEventNumber + keyframeBlockSize;
        eventnumber_t consequenceLookahead = getConsequenceLookahead(previousEventNumber);
        for (eventnumber_t i = beginEventNumber; i < endEventNumber; i++)
            previousEventNumberToMessageEntriesMap[i] = std::vector<MessageEntry *>();
        eventnumber_t eventNumber = beginEventNumber;
        Event *event = getEventForEventNumber(beginEventNumber);
        while (eventNumber < endEventNumber + consequenceLookahead) {
            if (event) {
                for (int i = 0; i < (int)event->getNumEventLogEntries(); i++) {
                    MessageEntry *messageEntry = dynamic_cast<MessageEntry *>(event->getEventLogEntry(i));
                    if (messageEntry) {
                        eventnumber_t messageEntryPreviousEventNumber = messageEntry->previousEventNumber;
                        if (beginEventNumber <= messageEntryPreviousEventNumber && messageEntryPreviousEventNumber < endEventNumber && messageEntryPreviousEventNumber != event->getEventNumber()) {
                            it = previousEventNumberToMessageEntriesMap.find(messageEntryPreviousEventNumber);
                            it->second.push_back(messageEntry);
                        }
                    }
                }
                eventNumber++;
                event = event->getNextEvent();
            }
            else {
                eventNumber++;
                event = getEventForEventNumber(eventNumber);
            }
        }
        return previousEventNumberToMessageEntriesMap.find(previousEventNumber)->second;
    }
}

} // namespace eventlog
}  // namespace omnetpp

