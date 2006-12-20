//=========================================================================
//  EVENTLOGINDEX.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
#include "eventlogindex.h"
#include "exception.h"

EventLogIndex::EventLogIndex(FileReader *reader)
{
    this->reader = reader;
    firstEventNumber = EVENT_NOT_YET_CALCULATED;
    lastEventNumber = EVENT_NOT_YET_CALCULATED;
    firstEventOffset = -1;
    lastEventOffset = -1;
}

EventLogIndex::~EventLogIndex()
{
    delete reader;
}

long EventLogIndex::getFirstEventNumber()
{
    if (firstEventNumber == EVENT_NOT_YET_CALCULATED)
    {
        file_offset_t lineEndOffset;
        readToEventLine(true, 0, firstEventNumber, firstSimulationTime, firstEventOffset, lineEndOffset);
    }

    return firstEventNumber;
}

long EventLogIndex::getLastEventNumber()
{
    if (lastEventNumber == EVENT_NOT_YET_CALCULATED)
    {
        file_offset_t lineEndOffset;
        readToEventLine(false, reader->getFileSize(), lastEventNumber, lastSimulationTime, lastEventOffset, lineEndOffset);
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

bool EventLogIndex::needsToBeStored(long eventNumber)
{
    if (eventNumber%EVENTNUM_INDEX_DENSITY==0)
        return true;

    // find eventNumber in map (lower_bound() finds equal or first greater key)
    EventNumberToOffsetMap::iterator it = eventNumberToOffsetMap.lower_bound(eventNumber);
    if (it!=eventNumberToOffsetMap.end() && (it->first - eventNumber) < EVENTNUM_INDEX_DENSITY)
        return false;  // too close to next stored one
    if (it!=eventNumberToOffsetMap.begin() && (eventNumber - (--it)->first) < EVENTNUM_INDEX_DENSITY)
        return false;  // too close to previous stored one
    return true;
}

void EventLogIndex::addPosition(long eventNumber, simtime_t simulationTime, file_offset_t offset)
{
    if (needsToBeStored(eventNumber))
    {
        eventNumberToOffsetMap[eventNumber] = offset;
        simulationTimeToOffsetMap[simulationTime] = offset;
    }
}

file_offset_t EventLogIndex::getBeginOffsetForEndOffset(file_offset_t endOffset)
{
    long eventNumber;
    file_offset_t lineStartOffset, lineEndOffset;
    simtime_t simulationTime;

    if (readToEventLine(false, endOffset, eventNumber, simulationTime, lineStartOffset, lineEndOffset))
        return lineStartOffset;
    else
        return -1;
}

file_offset_t EventLogIndex::getEndOffsetForBeginOffset(file_offset_t beginOffset)
{
    long eventNumber;
    file_offset_t lineStartOffset, lineEndOffset;
    simtime_t simulationTime;

    if (readToEventLine(true, beginOffset + 1, eventNumber, simulationTime, lineStartOffset, lineEndOffset))
        return lineStartOffset;
    else
        return -1;
}

bool EventLogIndex::positionToEventNumber(long eventNumber, MatchKind matchKind)
{
    file_offset_t offset = getOffsetForEventNumber(eventNumber, matchKind);

    if (offset == -1)
        return false; // eventNumber not found

    reader->seekTo(offset);
    return true;
}

bool EventLogIndex::positionToSimulationTime(simtime_t simulationTime, MatchKind matchKind)
{
    reader->seekTo(getOffsetForSimulationTime(simulationTime, matchKind));
    return true;
}

file_offset_t EventLogIndex::getOffsetForEventNumber(long eventNumber, MatchKind matchKind)
{
    file_offset_t offset = binarySearchForOffset(true, &eventNumberToOffsetMap, eventNumber, matchKind);

    if (PRINT_DEBUG_MESSAGES) printf("Found event number: %ld at offset: %lld\n", eventNumber, offset);

    return offset;
}

file_offset_t EventLogIndex::getOffsetForSimulationTime(simtime_t simulationTime, MatchKind matchKind)
{
    file_offset_t offset = binarySearchForOffset(false, &simulationTimeToOffsetMap, simulationTime, matchKind);

    if (PRINT_DEBUG_MESSAGES) printf("*** Found simulation time: %.*g at offset: %lld\n", 12, simulationTime, offset);

    return offset;
}

template <typename T> file_offset_t EventLogIndex::binarySearchForOffset(bool eventNumberBased, std::map<T, file_offset_t> *keyToOffsetMap, T key, MatchKind matchKind)
{
    file_offset_t foundOffset = -1;
    file_offset_t lowerOffset, upperOffset;

    // find key in map (lower_bound() finds equal or first greater key)
    typename std::map<T, file_offset_t>::iterator it = keyToOffsetMap->lower_bound(key);

    // exact match in cache?
    if (it != keyToOffsetMap->end() && it->first == key)
        foundOffset = it->second;
    else
    {
        long lowerKey;

        // figure out start positions for binary search
        if (it == keyToOffsetMap->end())
            upperOffset = reader->getFileSize();
        else
            upperOffset = it->second;

        if (it == keyToOffsetMap->begin()) {
            lowerKey = 0;
            lowerOffset = 0;
        }
        else {
            --it; // note: --it seems to be valid even when it==end()
            lowerKey = it->first;
            lowerOffset = it->second;
        }

        // binary search
        //
        // IMPORTANT NOTE: lowerOffset will always be exactly on an "E" line
        // (that of lowerKey), but upperOffset will NOT! This is necessary
        // for the distance between lowerOffset and upperOffset to shrink properly.
        //
        int stepCount = 0;

        while (true)
        {
            stepCount++;
            file_offset_t midOffset = (upperOffset + lowerOffset) / 2;
            //printf("step %d: offsets: lo=%ld, up=%ld, mid=%ld \t\tkey#: lo=#%ld\n",
            //       stepCount, lowerOffset, upperOffset, midOffset, lowerKey);
            long midEventNumber;
            file_offset_t midEventStartOffset, midEventEndOffset;
            simtime_t midSimulationTime;

            if (readToEventLine(true, midOffset, midEventNumber, midSimulationTime, midEventStartOffset, midEventEndOffset))
            {
                //printf("  found event #%ld at offset=%ld\n", midEventNumber, midEventStartOffset);
                T midKey;

                if (eventNumberBased)
                    midKey = midEventNumber;
                else
                    midKey = midSimulationTime;

                // store the mid position
                addPosition(midEventNumber, midSimulationTime, midEventStartOffset);

                // assign "mid" to "lower" or "upper"
                if (midKey < key)
                {
                    lowerKey = midKey;
                    lowerOffset = lowerOffset == midEventStartOffset ? midEventStartOffset + 1 : midEventStartOffset;
                }
                else if (midKey > key)
                {
                    upperOffset = upperOffset == midOffset ? midOffset - 1 : midOffset;
                }
                // stopping condition
                else if (midKey == key)
                {
                    foundOffset = midEventStartOffset;
                    break;
                }
            }
            else
            {
                //printf("  NOTHING found, decreasing upperOffset\n");

                // no key found -- we must be at the very end of the file.
                // try again finding an "E" line from a bit earlier point in the file.
                upperOffset = midOffset;
            }

            if (lowerOffset >= upperOffset)
            {
                foundOffset = -1;
                break;
            }
        }
    }

    //printf("  binary search steps: %d\n", stepCount);

    if (matchKind == EXACT && (foundOffset == -1 || eventNumberBased))
        return foundOffset;
    else if (matchKind == EXACT && !eventNumberBased)
    {
        file_offset_t firstOffset = linearSearchForOffset(eventNumberBased, foundOffset, key, FIRST_OR_PREVIOUS, true);
        file_offset_t lastOffset = linearSearchForOffset(eventNumberBased, foundOffset, key, LAST_OR_NEXT, true);

        if (foundOffset == lastOffset && foundOffset == firstOffset)
            return foundOffset;
        else
            throw opp_runtime_error("Found non unique simulation time when exact match is requested");
    }
    else
    {
        // non exact match requested
        bool exactMatchFound = foundOffset != -1;
        file_offset_t searchOffset = exactMatchFound ? foundOffset : (matchKind == FIRST_OR_PREVIOUS ? lowerOffset : upperOffset);

        MatchKind linearMatchKind;
        if (matchKind == FIRST_OR_NEXT)
            linearMatchKind = LAST_OR_NEXT;
        else if (matchKind == LAST_OR_PREVIOUS)
            linearMatchKind = FIRST_OR_PREVIOUS;
        else
            linearMatchKind = matchKind;

        return linearSearchForOffset(eventNumberBased, searchOffset, key, linearMatchKind, exactMatchFound);
    }
}

template <typename T> file_offset_t EventLogIndex::linearSearchForOffset(bool eventNumberBased, file_offset_t offset, T key, MatchKind matchKind, bool exactMatchFound)
{
    // this is a search either forward or backward based on matchKind
    if (matchKind == EXACT)
        throw opp_runtime_error("Invalid match kind");

    file_offset_t previousOffset = offset;
    long eventNumber;
    file_offset_t lineStartOffset, lineEndOffset;
    simtime_t simulationTime;

    while (offset != -1)
    {
        bool readEventLine = readToEventLine(matchKind == LAST_OR_NEXT, offset, eventNumber, simulationTime, lineStartOffset, lineEndOffset);

        if (!readEventLine) {
            if (exactMatchFound)
                return previousOffset;
            else
                return -1;
        }

        T readKey;

        if (eventNumberBased)
            readKey = eventNumber;
        else
            readKey = simulationTime;

        if (!exactMatchFound) {
            if (matchKind == LAST_OR_NEXT && readKey > key)
                return lineStartOffset;

            if (matchKind == FIRST_OR_PREVIOUS && readKey < key)
                return lineStartOffset;
        }
        else if (readKey != key)
            return previousOffset;

        previousOffset = lineStartOffset;

        if (matchKind == FIRST_OR_PREVIOUS)
            offset = lineStartOffset;

        if (matchKind == LAST_OR_NEXT)
            offset = lineEndOffset;
    }

    return -1;
}

bool EventLogIndex::readToEventLine(bool forward, file_offset_t readStartOffset, long& eventNumber, simtime_t& simulationTime, file_offset_t& lineStartOffset, file_offset_t& lineEndOffset)
{
    eventNumber = -1;
    simulationTime = -1;
    reader->seekTo(readStartOffset);

    char *line;

    if (PRINT_DEBUG_MESSAGES) printf("Reading to first event line from offset: %lld in direction: %s\n", readStartOffset, forward ? "forward" : "backward");

    // find first "E" line, return false if none found
    while (true)
    {
        if (forward)
            line = reader->readNextLine();
        else
            line = reader->readPreviousLine();

        if (!line)
            return false;

        if (line[0] == 'E' && line[1] == ' ')
            break;
    }

    // find event number and simulation time in line ("# 12345 t 1.2345")
    tokenizer.tokenize(line, reader->getLastLineLength());
    lineStartOffset = reader->getLastLineStartOffset();
    lineEndOffset = reader->getLastLineEndOffset();

    for (int i = 1; i <tokenizer.numTokens() - 1; i += 2)
    {
        if (!strcmp(tokenizer.tokens()[i], "#"))
            eventNumber = atol(tokenizer.tokens()[i+1]);

        if (!strcmp(tokenizer.tokens()[i], "t"))
            simulationTime = atof(tokenizer.tokens()[i+1]);
    }

    if (eventNumber != -1)
        return true;

    // bad luck
    throw opp_runtime_error("Wrong file format: no event number in 'E' line, line %d", reader->getNumReadLines());
}

void EventLogIndex::dumpTable()
{
    printf("Stored eventNumberToOffsetMap:\n");

    for (EventNumberToOffsetMap::iterator it = eventNumberToOffsetMap.begin(); it!=eventNumberToOffsetMap.end(); ++it)
        printf("  #%ld --> offset %lld (0x%llx)\n", it->first, it->second, it->second);
}
