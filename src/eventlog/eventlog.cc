//=========================================================================
//  EVENTLOG.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
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
#include "common/stringutil.h"
#include "eventlog.h"
#include "index.h"
#include "snapshot.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace eventlog {

StaticStringPool eventLogStringPool;

EventLog::EventLog(FileReader *reader) : EventLogIndex(reader)
{
    reader->setFileLocking(true);
    clearInternalState();
    parseIndex();
    if (reader->getFileSize() < 10E+6)
        parseAll();
    else {
        parseBegin(1E+6);
        parseEnd(1E+6);
    }
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
    eventLogEntryCache.clearCache();
    simulationBeginEntry = nullptr;
    simulationEndEntry = nullptr;
    eventNumberToEventMap.clear();
    eventNumberToIndexMap.clear();
    eventNumberToSnapshotMap.clear();
    beginOffsetToEventMap.clear();
    endOffsetToEventMap.clear();
}

void EventLog::deleteAllocatedObjects()
{
    delete simulationBeginEntry;
    delete simulationEndEntry;
    for (EventNumberToEventMap::iterator it = eventNumberToEventMap.begin(); it != eventNumberToEventMap.end(); it++)
        delete it->second;
    for (EventNumberToIndexMap::iterator it = eventNumberToIndexMap.begin(); it != eventNumberToIndexMap.end(); it++)
        delete it->second;
    for (EventNumberToSnapshotMap::iterator it = eventNumberToSnapshotMap.begin(); it != eventNumberToSnapshotMap.end(); it++)
        delete it->second;
}

void EventLog::synchronize(FileReader::FileChange change)
{
    if (change != FileReader::UNCHANGED) {
        IEventLog::synchronize(change);
        EventLogIndex::synchronize(change);
        switch (change) {
            case FileReader::OVERWRITTEN:
                deleteAllocatedObjects();
                clearInternalState();
                parseIndex();
                break;
            case FileReader::APPENDED:
                approximateNumberOfEvents = -1;
                if (lastEvent) {
                    lastEvent->parseLines(reader, lastEvent->getEndOffset());
                    eventNumberToCacheEntryMap.erase(lastEvent->getEventNumber());
                    endOffsetToEventMap.erase(lastEvent->getEndOffset());
                    lastEventNumber = EVENT_NOT_YET_CALCULATED;
                    lastSimulationTime = simtime_nil;
                    lastEventOffset = -1;
                    lastEvent = nullptr;
                }
                for (EventNumberToEventMap::iterator it = eventNumberToEventMap.begin(); it != eventNumberToEventMap.end(); it++)
                    it->second->synchronize(change);
                parseIndex();
                break;
            default:
                throw opp_runtime_error("Unknown file change");
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
        file_offset_t lineBeginOffset = -1, lineEndOffset;
        simtime_t simulationTime;
        readToEventLine(false, offset, eventNumber, simulationTime, lineBeginOffset, lineEndOffset);

        Event *event = nullptr;

        if (lineBeginOffset == -1)
            event = getFirstEvent();
        else
            event = getEventForBeginOffset(lineBeginOffset);

        Assert(event);

        return event;
    }
}

SimulationBeginEntry *EventLog::getSimulationBeginEntry()
{
    if (!simulationBeginEntry) {
        reader->seekTo(0);
        char *line = reader->getNextLineBufferPointer();
        if (line) {
            EventLogEntry *eventLogEntry = (EventLogEntry *)EventLogEntry::parseEntry(this, nullptr, 0, reader->getCurrentLineStartOffset(), line, reader->getCurrentLineLength());
            SimulationBeginEntry *simulationBeginEntry = dynamic_cast<SimulationBeginEntry *>(eventLogEntry);
            if (simulationBeginEntry)
                this->simulationBeginEntry = simulationBeginEntry;
        }
    }
    return simulationBeginEntry;
}

SimulationEndEntry *EventLog::getSimulationEndEntry()
{
    if (!simulationEndEntry) {
        reader->seekTo(reader->getFileSize());
        char *line = reader->getPreviousLineBufferPointer();
        if (line) {
            EventLogEntry *eventLogEntry = (EventLogEntry *)EventLogEntry::parseEntry(this, nullptr, 0, reader->getCurrentLineStartOffset(), line, reader->getCurrentLineLength());
            SimulationEndEntry *simulationEndEntry = dynamic_cast<SimulationEndEntry *>(eventLogEntry);
            if (simulationEndEntry)
                this->simulationEndEntry = simulationEndEntry;
        }
    }
    return simulationEndEntry;
}

void EventLog::parseIndex()
{
    // this function is optimized for performance
    // the idea is to read indices backwards starting from the end of file
    // file offsets must be shifted to be able to read truncated files
    std::map<file_offset_t, Snapshot *> snapshotFileOffsetsToSnapshotMap;
    reader->seekTo(reader->getFileSize());
    char *line = reader->getPreviousLineBufferPointer();
    std::vector<Index *> indices;
    while (line) {
        EventLogEntry *eventLogEntry = (EventLogEntry *)EventLogEntry::parseEntry(this, nullptr, 0, reader->getCurrentLineStartOffset(), line, reader->getCurrentLineLength());
        IndexEntry *indexEntry = dynamic_cast<IndexEntry *>(eventLogEntry);
        if (indexEntry) {
            // check if we already have this index
            EventNumberToIndexMap::iterator it = eventNumberToIndexMap.find(indexEntry->eventNumber);
            if (it != eventNumberToIndexMap.end()) {
                indices.push_back(it->second);
                delete eventLogEntry;
                break;
            }
            // jump to previous snapshot entry
            Snapshot *snapshot = nullptr;
            std::map<file_offset_t, Snapshot *>::iterator jt = snapshotFileOffsetsToSnapshotMap.find(indexEntry->previousSnapshotFileOffset);
            if (indexEntry->previousSnapshotFileOffset != -1 && jt == snapshotFileOffsetsToSnapshotMap.end()) {
                file_offset_t realFileOffset = indexEntry->getOffset() - indexEntry->fileOffset + indexEntry->previousSnapshotFileOffset;
                if (realFileOffset >= 0) {
                    reader->seekTo(indexEntry->getOffset() - indexEntry->fileOffset + indexEntry->previousSnapshotFileOffset);
                    line = reader->getNextLineBufferPointer();
                    EventLogEntry *eventLogEntry = (EventLogEntry *)EventLogEntry::parseEntry(this, nullptr, 0, reader->getCurrentLineStartOffset(), line, reader->getCurrentLineLength());
                    SnapshotEntry *snapshotEntry = dynamic_cast<SnapshotEntry *>(eventLogEntry);
                    Assert(snapshotEntry);
                    EventNumberToSnapshotMap::iterator kt = eventNumberToSnapshotMap.find(snapshotEntry->eventNumber);
                    if (kt == eventNumberToSnapshotMap.end()) {
                        snapshot = new Snapshot(this, reader->getCurrentLineStartOffset());
                        eventNumberToSnapshotMap[snapshotEntry->eventNumber] = snapshot;
                    }
                    else
                        snapshot = kt->second;
                    snapshotFileOffsetsToSnapshotMap[indexEntry->previousSnapshotFileOffset] = snapshot;
                    delete snapshotEntry;
                }
            }
            else if (jt != snapshotFileOffsetsToSnapshotMap.end())
                snapshot = jt->second;
            // create index
            Index *index = new Index(this, indexEntry->getOffset(), snapshot);
            indices.push_back(index);
            eventNumberToIndexMap[indexEntry->eventNumber] = index;
            // jump to previous index entry
            file_offset_t realFileOffset = index->getBeginOffset() - indexEntry->fileOffset + indexEntry->previousIndexFileOffset;
            if (realFileOffset >= 0) {
                reader->seekTo(realFileOffset);
                line = reader->getNextLineBufferPointer();
            }
            else
                line = nullptr;
        }
        else
            line = reader->getPreviousLineBufferPointer();
        delete eventLogEntry;
        progress();
    }
    for (int i = 1; i < (int)indices.size(); i++)
        // indices is in reverse order
        Index::linkIndices(indices[i], indices[i - 1]);
}

void EventLog::parseBegin(uint64_t limit)
{
    uint64_t initialReadBytes = reader->getNumReadBytes();
    IEvent *event = getFirstEvent();
    while (event) {
        event = event->getNextEvent();
        if (reader->getNumReadBytes() - initialReadBytes > limit)
            break;
    }
}

void EventLog::parseEnd(uint64_t limit)
{
    uint64_t initialReadBytes = reader->getNumReadBytes();
    IEvent *event = getLastEvent();
    while (event) {
        event = event->getPreviousEvent();
        if (reader->getNumReadBytes() - initialReadBytes > limit)
            break;
    }
}

void EventLog::parseAll()
{
    IEvent *event = getFirstEvent();
    while (event)
        event = event->getNextEvent();
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
    if (opp_isempty(search))
        return forward ? start->getNextEventLogEntry() : start->getPreviousEventLogEntry();
    else {
        char *line;
        file_offset_t eventBeginOffset = -1;
        file_offset_t matchOffset = -1;
        Event *matchEvent = nullptr;
        reader->seekTo(start->getOffset());
        if (forward) {
            reader->getNextLineBufferPointer();
            while ((line = reader->getNextLineBufferPointer()) != nullptr) {
                if (line[0] == 'E' && line[1] == ' ')
                    eventBeginOffset = reader->getCurrentLineStartOffset();
                if (opp_strnistr(line, search, reader->getCurrentLineLength(), caseSensitive)) {
                    file_offset_t currentLineStartOffset = reader->getCurrentLineStartOffset();
                    Event *event = eventBeginOffset == -1 ? start->getEvent() : getEventForBeginOffset(eventBeginOffset);
                    if (event && event->getBeginOffset() <= currentLineStartOffset && currentLineStartOffset <= event->getEndOffset()) {
                        matchEvent = event;
                        matchOffset = currentLineStartOffset;
                        break;
                    }
                }
                progress();
            }
        }
        else {
            while ((line = reader->getPreviousLineBufferPointer()) != nullptr) {
                if (opp_strnistr(line, search, reader->getCurrentLineLength(), caseSensitive)) {
                    file_offset_t currentLineStartOffset = reader->getCurrentLineStartOffset();
                    Event *event = eventBeginOffset == -1 ? (start->getEntryIndex() == 0 ? start->getEvent()->getPreviousEvent() : start->getEvent()) : getEventForBeginOffset(eventBeginOffset)->getPreviousEvent();
                    if (event && event->getBeginOffset() <= currentLineStartOffset && currentLineStartOffset <= event->getEndOffset()) {
                        matchEvent = event;
                        matchOffset = currentLineStartOffset;
                        break;
                    }
                }
                if (line[0] == 'E' && line[1] == ' ')
                    eventBeginOffset = reader->getCurrentLineStartOffset();
                progress();
            }
        }
        if (matchEvent && matchOffset != -1) {
            for (int i = 0; i < matchEvent->getNumEventLogEntries(); i++) {
                EventLogEntry *eventLogEntry = matchEvent->getEventLogEntry(i);
                if (eventLogEntry->getOffset() == matchOffset)
                    return eventLogEntry;
            }
        }
        return nullptr;
    }
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

Index *EventLog::getIndex(eventnumber_t eventNumber, MatchKind matchKind)
{
    EventNumberToIndexMap::iterator it;
    switch (matchKind) {
        case EXACT:
            it = eventNumberToIndexMap.find(eventNumber);
            break;
        case FIRST_OR_PREVIOUS:
        case LAST_OR_PREVIOUS:
            it = eventNumberToIndexMap.upper_bound(eventNumber);
            if (it == eventNumberToIndexMap.begin())
                it = eventNumberToIndexMap.end();
            else if (!eventNumberToIndexMap.empty())
                it--;
            break;
        case FIRST_OR_NEXT:
        case LAST_OR_NEXT:
            it = eventNumberToIndexMap.lower_bound(eventNumber);
            if (it != eventNumberToIndexMap.end() && it->first == eventNumber)
                it++;
            break;
        default:
            throw opp_runtime_error("Unknown match kind");
    }
    if (it == eventNumberToIndexMap.end())
        return nullptr;
    else
        return it->second;
}

Snapshot *EventLog::getSnapshot(eventnumber_t eventNumber, MatchKind matchKind)
{
    EventNumberToSnapshotMap::iterator it;
    switch (matchKind) {
        case EXACT:
            it = eventNumberToSnapshotMap.find(eventNumber);
            break;
        case FIRST_OR_PREVIOUS:
        case LAST_OR_PREVIOUS:
            it = eventNumberToSnapshotMap.upper_bound(eventNumber);
            if (it == eventNumberToSnapshotMap.begin())
                it = eventNumberToSnapshotMap.end();
            else if (!eventNumberToSnapshotMap.empty())
                it--;
            break;
        case FIRST_OR_NEXT:
        case LAST_OR_NEXT:
            it = eventNumberToSnapshotMap.lower_bound(eventNumber);
            if (it != eventNumberToSnapshotMap.end() && it->first == eventNumber)
                it++;
            break;
        default:
            throw opp_runtime_error("Unknown match kind");
    }
    if (it == eventNumberToSnapshotMap.end())
        return nullptr;
    else
        return it->second;
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

void EventLog::cacheEventLogEntry(EventLogEntry *eventLogEntry)
{
    eventLogEntryCache.cacheEventLogEntry(eventLogEntry);
    // collect message description entries
    MessageDescriptionEntry *messageDescriptionEntry = dynamic_cast<MessageDescriptionEntry *>(eventLogEntry);
    if (messageDescriptionEntry) {
        messageNames.insert(messageDescriptionEntry->messageName);
        messageClassNames.insert(messageDescriptionEntry->messageClassName);
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

}  // namespace eventlog
}  // namespace omnetpp

