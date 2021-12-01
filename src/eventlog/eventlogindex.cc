//=========================================================================
//  EVENTLOGINDEX.CC - part of
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
#include <algorithm>
#include <cinttypes>
#include "common/exception.h"
#include "eventlogentry.h"
#include "eventlogindex.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace eventlog {

static bool isEventNumber(eventnumber_t eventNumber)
{
    return true;
}

static bool isEventNumber(simtime_t simulationTime)
{
    return false;
}

static bool isSimulationTime(eventnumber_t eventNumber)
{
    return false;
}

static bool isSimulationTime(simtime_t simulationTime)
{
    return true;
}

static eventnumber_t getKey(eventnumber_t key, eventnumber_t eventNumber, simtime_t simulationTime)
{
    return eventNumber;
}

static simtime_t getKey(simtime_t key, eventnumber_t eventNumber, simtime_t simulationTime)
{
    return simulationTime;
}

EventLogIndex::CacheEntry::CacheEntry(eventnumber_t eventNumber, simtime_t simulationTime, file_offset_t beginOffset, file_offset_t endOffset) :
    simulationTime(simulationTime), beginEventNumber(eventNumber), endEventNumber(eventNumber), beginOffset(beginOffset), endEventBeginOffset(beginOffset), endOffset(endOffset)
{
}

void EventLogIndex::CacheEntry::include(eventnumber_t eventNumber, simtime_t simulationTime, file_offset_t beginOffset, file_offset_t endOffset)
{
    Assert(this->simulationTime == simulationTime);
    this->beginEventNumber = std::min(beginEventNumber, eventNumber);
    this->endEventNumber = std::max(endEventNumber, eventNumber);
    this->beginOffset = std::min(this->beginOffset, beginOffset);
    this->endEventBeginOffset = std::max(this->endEventBeginOffset, beginOffset);
    this->endOffset = std::max(this->endOffset, endOffset);
}

void EventLogIndex::CacheEntry::getBeginKey(eventnumber_t& key)
{
    key = beginEventNumber;
}

void EventLogIndex::CacheEntry::getBeginKey(simtime_t& key)
{
    key = simulationTime;
}

void EventLogIndex::CacheEntry::getEndKey(eventnumber_t& key)
{
    key = endEventNumber;
}

void EventLogIndex::CacheEntry::getEndKey(simtime_t& key)
{
    key = simulationTime;
}

// *************************************************************************************************

EventLogIndex::EventLogIndex(FileReader *reader): reader(reader)
{
    this->tokenizer = new LineTokenizer(reader->getMaxLineSize() + 1);
    clearInternalState();
}

EventLogIndex::~EventLogIndex()
{
    delete reader;
    delete tokenizer;
}

void EventLogIndex::clearInternalState()
{
    firstEventNumber = EVENT_NOT_YET_CALCULATED;
    lastEventNumber = EVENT_NOT_YET_CALCULATED;
    firstSimulationTime = simtime_nil;
    lastSimulationTime = simtime_nil;
    firstEventOffset = -1;
    lastEventOffset = -1;
    eventNumberToCacheEntryMap.clear();
    simulationTimeToCacheEntryMap.clear();
}

void EventLogIndex::synchronize(FileReader::FileChange change)
{
    if (change != FileReader::UNCHANGED) {
        reader->synchronize(change);
        switch (change) {
            case FileReader::OVERWRITTEN:
                clearInternalState();
                break;
            case FileReader::APPENDED:
                eventNumberToCacheEntryMap.erase(lastEventNumber);
                simulationTimeToCacheEntryMap.erase(lastSimulationTime);
                lastEventNumber = EVENT_NOT_YET_CALCULATED;
                lastSimulationTime = simtime_nil;
                lastEventOffset = -1;
                break;
            default:
                throw opp_runtime_error("Unknown file change");
        }
    }
}

eventnumber_t EventLogIndex::getFirstEventNumber()
{
    if (firstEventNumber == EVENT_NOT_YET_CALCULATED) {
        file_offset_t lineEndOffset;
        readToEventLine(true, 0, firstEventNumber, firstSimulationTime, firstEventOffset, lineEndOffset);
    }

    return firstEventNumber;
}

eventnumber_t EventLogIndex::getLastEventNumber()
{
    if (lastEventNumber == EVENT_NOT_YET_CALCULATED) {
        file_offset_t lineEndOffset;
        readToEventLine(false, reader->getFileSize(), lastEventNumber, lastSimulationTime, lastEventOffset, lineEndOffset);
        cacheEntry(lastEventNumber, lastSimulationTime, lastEventOffset, reader->getFileSize());
    }

    return lastEventNumber;
}

file_offset_t EventLogIndex::getFirstEventOffset()
{
    getFirstEventNumber();

    return firstEventOffset;
}

file_offset_t EventLogIndex::getLastEventOffset()
{
    getLastEventNumber();

    return lastEventOffset;
}

simtime_t EventLogIndex::getFirstSimulationTime()
{
    getFirstEventNumber();

    return firstSimulationTime;
}

simtime_t EventLogIndex::getLastSimulationTime()
{
    getLastEventNumber();

    return lastSimulationTime;
}

file_offset_t EventLogIndex::getBeginOffsetForEndOffset(file_offset_t endOffset)
{
    Assert(endOffset >= 0);

    eventnumber_t eventNumber;
    simtime_t simulationTime;
    file_offset_t lineBeginOffset, lineEndOffset;

    if (readToEventLine(false, endOffset, eventNumber, simulationTime, lineBeginOffset, lineEndOffset))
        return lineBeginOffset;
    else
        return -1;
}

file_offset_t EventLogIndex::getEndOffsetForBeginOffset(file_offset_t beginOffset)
{
    Assert(beginOffset >= 0);

    eventnumber_t eventNumber;
    simtime_t simulationTime;
    file_offset_t lineBeginOffset, lineEndOffset;

    if (readToEventLine(true, beginOffset + 1, eventNumber, simulationTime, lineBeginOffset, lineEndOffset))
        return lineBeginOffset;
    else
        return reader->getFileSize();
}

bool EventLogIndex::isEventBeginOffset(file_offset_t offset)
{
    reader->seekTo(offset);
    char *line = reader->getNextLineBufferPointer();

    return line && *line == 'E';
}

file_offset_t EventLogIndex::getOffsetForEventNumber(eventnumber_t eventNumber, MatchKind matchKind)
{
    Assert(eventNumber >= 0);
    file_offset_t offset = searchForOffset(eventNumberToCacheEntryMap, eventNumber, matchKind);

    if (PRINT_DEBUG_MESSAGES)
        printf("Found event number: %" PRId64 " for match kind: %d at offset: %" PRId64 "\n", eventNumber, matchKind, offset);

    return offset;
}

file_offset_t EventLogIndex::getOffsetForSimulationTime(simtime_t simulationTime, MatchKind matchKind)
{
    Assert(simulationTime >= 0);
    file_offset_t offset = searchForOffset(simulationTimeToCacheEntryMap, simulationTime, matchKind);

    if (PRINT_DEBUG_MESSAGES)
        printf("Found simulation time: %.*g for match kind: %d at offset: %" PRId64 "\n", 12, simulationTime.dbl(), matchKind, offset);

    return offset;
}

template<typename T> file_offset_t EventLogIndex::searchForOffset(std::map<T, CacheEntry>& map, T key, MatchKind matchKind)
{
    T lowerKey;
    T upperKey;
    file_offset_t foundOffset;
    file_offset_t lowerOffset;
    file_offset_t upperOffset;
    // first try to look up it the cache, this may result in an exact offset or a range around the offset being searched
    bool found = cacheSearchForOffset(map, key, matchKind, lowerKey, upperKey, foundOffset, lowerOffset, upperOffset);

    // no events in cache (not even the first and last which is always cached)
    // so there are no events at all
    if (lowerKey == -1 && upperKey == -1)
        foundOffset = -1;
    else if (!found) {
        Assert(lowerKey <= key && key <= upperKey);
        Assert(foundOffset == -1 || (lowerOffset <= foundOffset && foundOffset <= upperOffset));

        // if we still have a key range then use a binary search to look up the closest match
        if (foundOffset == -1 || lowerKey != upperKey)
            foundOffset = binarySearchForOffset(key, matchKind, lowerKey, upperKey, lowerOffset, upperOffset);

        bool exactMatchFound = lowerKey == key && upperKey == key;

        // finally use linear search to find the requested offset
        if (matchKind == EXACT) {
            if (foundOffset != -1 && isSimulationTime(key)) {
                // check if there are multiple events with the same simulation time
                file_offset_t firstOffset = linearSearchForOffset(key, foundOffset, false, true);
                file_offset_t lastOffset = linearSearchForOffset(key, foundOffset, true, true);

                if (foundOffset != lastOffset || foundOffset != firstOffset)
                    throw opp_runtime_error("Found non unique simulation time when exact match is requested");
            }
        }
        else {
            bool forward;
            file_offset_t searchOffset;

            if (exactMatchFound) {
                forward = matchKind == LAST_OR_NEXT || matchKind == LAST_OR_PREVIOUS;
                searchOffset = foundOffset;
            }
            else {
                forward = matchKind == FIRST_OR_NEXT || matchKind == LAST_OR_NEXT;
                if (forward)
                    searchOffset = lowerOffset;
                else {
                    reader->seekTo(upperOffset);
                    reader->getNextLineBufferPointer();
                    searchOffset = reader->getCurrentLineStartOffset();
                }
            }

            foundOffset = linearSearchForOffset(key, searchOffset, forward, exactMatchFound);
        }
        Assert(foundOffset == -1 || isEventBeginOffset(foundOffset));
    }

    return foundOffset;
}

template<typename T> bool EventLogIndex::cacheSearchForOffset(std::map<T, CacheEntry>& map, T key, MatchKind matchKind, T& lowerKey, T& upperKey, file_offset_t& foundOffset, file_offset_t& lowerOffset, file_offset_t& upperOffset)
{
    ensureFirstEventAndLastEventCached();
    T keyValue = (T)key;
    typename std::map<T, CacheEntry>::iterator it = map.lower_bound(keyValue);  // greater or equal

    // if exact match found
    if (it != map.end() && it->first == keyValue) {
        CacheEntry& cacheEntry = it->second;

        // for event numbers there can be only one exact match so we can safely return it independently of matchKind
        if (isEventNumber(key)) {
            foundOffset = cacheEntry.beginOffset;
            return true;
        }
        else {
            // for simulation times we must consider whether the cache entry is complete or not by looking around it
            typename std::map<T, CacheEntry>::iterator itUpper = it;
            typename std::map<T, CacheEntry>::iterator itLower = it;

            ++itUpper;
            if (itLower != map.begin())
                --itLower;
            else
                itLower = map.end();

            // subsequent events may or may not have subsequent event numbers in the eventlog file
            // the end offset for the nth event is less than or equal to the begin offset of the nth + 1 event
            bool completeBegin = itLower == map.end() || (itLower->second.endOffset == cacheEntry.beginOffset || itLower->second.endEventNumber + 1 == cacheEntry.beginEventNumber);
            bool completeEnd = itUpper != map.end() && (itUpper->second.beginOffset == cacheEntry.endOffset || itUpper->second.beginEventNumber == cacheEntry.endEventNumber + 1);

            // dispatching on match kind is required
            switch (matchKind) {
                case EXACT:
                    if (completeBegin && completeEnd) {
                        Assert(cacheEntry.beginEventNumber == cacheEntry.endEventNumber);
                        foundOffset = cacheEntry.beginOffset;
                        return true;
                    }
                    break;

                case FIRST_OR_PREVIOUS:
                case FIRST_OR_NEXT:
                    if (completeBegin) {
                        foundOffset = cacheEntry.beginOffset;
                        return true;
                    }
                    break;

                case LAST_OR_PREVIOUS:
                case LAST_OR_NEXT:
                    if (completeEnd) {
                        foundOffset = cacheEntry.endEventBeginOffset;
                        return true;
                    }
                    break;
            }

            // cannot exactly determine from cache
            lowerKey = key;
            lowerOffset = cacheEntry.beginOffset;
            upperKey = key;
            upperOffset = cacheEntry.endOffset;
            // an event's begin offset must be returned
            foundOffset = cacheEntry.beginOffset;
            return false;
        }
    }
    else {
        // upper iterator refers to the closest element after the key
        typename std::map<T, CacheEntry>::iterator itUpper = it;
        if (itUpper != map.end()) {
            CacheEntry& cacheEntry = itUpper->second;
            cacheEntry.getBeginKey(upperKey);
            upperOffset = cacheEntry.beginOffset;
        }
        else {
            upperKey = getKey(key, getLastEventNumber(), getLastSimulationTime());
            upperOffset = reader->getFileSize();  // this has to match last event's end offset
        }

        // lower iterator refers to the closest element before the key
        typename std::map<T, CacheEntry>::iterator itLower = it;
        if (!map.empty() && itLower != map.begin()) {
            --itLower;
            CacheEntry& cacheEntry = itLower->second;
            cacheEntry.getEndKey(lowerKey);
            lowerOffset = cacheEntry.endOffset;
        }
        else {
            lowerKey = getKey(key, getFirstEventNumber(), getFirstSimulationTime());
            lowerOffset = getFirstEventOffset();
        }

        // if the closest element before and after are subsequent elements
        // then the cache is complete around the key so the exact offset can be determined
        if (lowerOffset == upperOffset) {
            switch (matchKind) {
                case EXACT:
                    // we did not get an exact match in the first place (see above)
                    foundOffset = -1;
                    break;

                case FIRST_OR_PREVIOUS:
                case LAST_OR_PREVIOUS:
                    if (itLower == map.end())
                        foundOffset = -1;
                    else
                        foundOffset = itLower->second.endEventBeginOffset;
                    break;

                case FIRST_OR_NEXT:
                case LAST_OR_NEXT:
                    if (itUpper == map.end())
                        foundOffset = -1;
                    else
                        foundOffset = itUpper->second.beginOffset;
                    break;
            }

            return true;
        }
        else {
            // not possible to determine exact offset from cache
            foundOffset = -1;
            return false;
        }
    }
}

template<typename T> file_offset_t EventLogIndex::binarySearchForOffset(T key, MatchKind matchKind, T& lowerKey, T& upperKey, file_offset_t& lowerOffset, file_offset_t& upperOffset)
{
    Assert(key >= 0);
    file_offset_t foundOffset, middleEventBeginOffset, middleEventEndOffset;
    eventnumber_t middleEventNumber;
    simtime_t middleSimulationTime;

    /**
     * Binary search
     *
     * IMPORTANT NOTE: lowerOffset will always be exactly on an "E" line
     * (that of lowerKey), but upperOffset will NOT! This is necessary
     * for the distance between lowerOffset and upperOffset to shrink properly.
     */
    int stepCount = 0;

    while (true) {
        stepCount++;
        file_offset_t middleOffset = (upperOffset + lowerOffset) / 2;
        // printf("step %d: offsets: lo=%ld, up=%ld, middle=%ld \t\tkey#: lo=#%ld\n", stepCount, lowerOffset, upperOffset, middleOffset, lowerKey);

        if (readToEventLine(true, middleOffset, middleEventNumber, middleSimulationTime, middleEventBeginOffset, middleEventEndOffset)) {
            // printf("  found event #%ld at offset=%ld\n", middleEventNumber, middleEventBeginOffset);
            T middleKey = getKey(key, middleEventNumber, middleSimulationTime);

            // assign "middle" to "lower" or "upper"
            if (middleKey < key) {
                lowerKey = middleKey;
                lowerOffset = lowerOffset == middleEventBeginOffset ? middleEventBeginOffset + 1 : middleEventBeginOffset;
            }
            else if (middleKey > key) {
                upperKey = middleKey;
                upperOffset = upperOffset == middleOffset ? middleOffset - 1 : middleOffset;
            }
            // stopping condition
            else if (middleKey == key) {
                lowerKey = upperKey = key;
                foundOffset = lowerOffset = upperOffset = middleEventBeginOffset;
                break;
            }
        }
        else {
            // printf("  NOTHING found, decreasing upperOffset\n");

            // no key found -- we must be at the very end of the file.
            // try again finding an "E" line from a bit earlier point in the file.
            upperOffset = middleOffset;
        }

        if (lowerOffset >= upperOffset) {
            foundOffset = -1;
            break;
        }
    }

    if (PRINT_DEBUG_MESSAGES)
        printf("Binary search steps: %d\n", stepCount);

    return foundOffset;
}

template<typename T> file_offset_t EventLogIndex::linearSearchForOffset(T key, file_offset_t beginOffset, bool forward, bool exactMatchRequired)
{
    Assert(beginOffset >= 0);

    eventnumber_t eventNumber;
    simtime_t simulationTime;
    file_offset_t lineBeginOffset, lineEndOffset;
    file_offset_t previousOffset = beginOffset;

    while (beginOffset != -1) {
        bool readEventLine = readToEventLine(forward, beginOffset, eventNumber, simulationTime, lineBeginOffset, lineEndOffset);

        if (!readEventLine) {
            if (exactMatchRequired)
                return previousOffset;
            else
                return -1;
        }

        T readKey = getKey(key, eventNumber, simulationTime);

        if (!exactMatchRequired) {
            if (forward) {
                if (readKey > key)
                    return lineBeginOffset;
            }
            else {
                if (readKey < key)
                    return lineBeginOffset;
            }
        }
        else if (readKey != key)
            return previousOffset;

        previousOffset = lineBeginOffset;

        if (forward)
            beginOffset = lineEndOffset;
        else
            beginOffset = lineBeginOffset;
    }

    return -1;
}

bool EventLogIndex::readToEventLine(bool forward, file_offset_t readStartOffset, eventnumber_t& eventNumber, simtime_t& simulationTime, file_offset_t& lineStartOffset, file_offset_t& lineEndOffset)
{
    Assert(readStartOffset >= 0);
    eventNumber = -1;
    simulationTime = simtime_nil;
    reader->seekTo(readStartOffset);

    char *line;

    if (PRINT_DEBUG_MESSAGES)
        printf("Reading to first event line from offset: %" PRId64 " in direction: %s\n", readStartOffset, forward ? "forward" : "backward");

    // find first "E" line, return false if none found
    while (true) {
        if (forward)
            line = reader->getNextLineBufferPointer();
        else
            line = reader->getPreviousLineBufferPointer();

        if (!line)
            return false;

        if (line[0] == 'E' && line[1] == ' ')
            break;
    }

    // find event number and simulation time in line ("# 12345 t 1.2345")
    tokenizer->tokenize(line, reader->getCurrentLineLength());
    lineStartOffset = reader->getCurrentLineStartOffset();
    lineEndOffset = reader->getCurrentLineEndOffset();

    int numTokens = tokenizer->numTokens();
    char **tokens = tokenizer->tokens();

    for (int i = 1; i < numTokens - 1; i += 2) {
        const char *token = tokens[i];

        if (token[0] == '#' && token[1] == '\0')
            eventNumber = EventLogEntry::parseEventNumber(tokens[i+1]);
        else if (token[0] == 't' && token[1] == '\0')
            simulationTime = EventLogEntry::parseSimulationTime(tokens[i+1]);
    }

    if (eventNumber != -1) {
        Assert(simulationTime != simtime_nil);
        cacheEntry(eventNumber, simulationTime, lineStartOffset, lineEndOffset);
        return true;
    }

    // bad luck
    throw opp_runtime_error("Wrong file format: No event number in 'E' line, line %" PRId64, reader->getNumReadLines());
}

void EventLogIndex::cacheEntry(eventnumber_t eventNumber, simtime_t simulationTime, file_offset_t beginOffset, file_offset_t endOffset)
{
    EventNumberToCacheEntryMap::iterator itEventNumber = eventNumberToCacheEntryMap.lower_bound(eventNumber);

    if (itEventNumber != eventNumberToCacheEntryMap.end() && itEventNumber->first == eventNumber)
        itEventNumber->second.include(eventNumber, simulationTime, beginOffset, endOffset);
    else
        eventNumberToCacheEntryMap[eventNumber] = CacheEntry(eventNumber, simulationTime, beginOffset, endOffset);

    SimulationTimeToCacheEntryMap::iterator itSimulationTime = simulationTimeToCacheEntryMap.lower_bound(simulationTime);

    if (itSimulationTime != simulationTimeToCacheEntryMap.end() && itSimulationTime->first == simulationTime)
        itSimulationTime->second.include(eventNumber, simulationTime, beginOffset, endOffset);
    else
        simulationTimeToCacheEntryMap[simulationTime] = CacheEntry(eventNumber, simulationTime, beginOffset, endOffset);
}

void EventLogIndex::ensureFirstEventAndLastEventCached()
{
    getFirstEventNumber();
    getLastEventNumber();
}

void EventLogIndex::dump()
{
    printf("eventNumberToCacheEntryMap:\n");

    for (auto & it : eventNumberToCacheEntryMap)
        printf("  #%" PRId64 " --> offset %" PRId64 " (0x%" PRId64 "x)\n", it.first, it.second.beginOffset, it.second.beginOffset);

    printf("simulationTimeToCacheEntryMap:\n");

    for (auto & it : simulationTimeToCacheEntryMap)
        printf("  %.*g --> offset %" PRId64 " (0x%" PRId64 "x)\n", 12, it.first.dbl(), it.second.beginOffset, it.second.beginOffset);
}

}  // namespace eventlog
}  // namespace omnetpp
