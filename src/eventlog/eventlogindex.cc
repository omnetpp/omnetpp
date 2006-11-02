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
        long lineEndOffset;
        readToEventLine(true, 0, firstEventNumber, firstSimulationTime, firstEventOffset, lineEndOffset);
    }

    return firstEventNumber;
}

long EventLogIndex::getLastEventNumber()
{
    if (lastEventNumber == EVENT_NOT_YET_CALCULATED)
    {
        long lineEndOffset;
        readToEventLine(false, reader->getFileSize(), lastEventNumber, lastSimulationTime, lastEventOffset, lineEndOffset);
    }

    return lastEventNumber;
}

long EventLogIndex::getFirstEventOffset()
{
    getFirstEventNumber();

    return firstEventOffset;
}

long EventLogIndex::getLastEventOffset()
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

void EventLogIndex::addPosition(long eventNumber, simtime_t simulationTime, long offset)
{
    if (needsToBeStored(eventNumber))
    {
        eventNumberToOffsetMap[eventNumber] = offset;
        simulationTimeToOffsetMap[simulationTime] = offset;
    }
}

long EventLogIndex::getBeginOffsetForEndOffset(long endOffset)
{
    long eventNumber, lineStartOffset, lineEndOffset;
    simtime_t simulationTime;

    if (readToEventLine(false, endOffset, eventNumber, simulationTime, lineStartOffset, lineEndOffset))
        return lineStartOffset;
    else
        return -1;
}

long EventLogIndex::getEndOffsetForBeginOffset(long beginOffset)
{
    long eventNumber, lineStartOffset, lineEndOffset;
    simtime_t simulationTime;

    if (readToEventLine(true, beginOffset + 1, eventNumber, simulationTime, lineStartOffset, lineEndOffset))
        return lineStartOffset;
    else
        return -1;
}

bool EventLogIndex::positionToEventNumber(long eventNumber, MatchKind matchKind)
{
    long offset = getOffsetForEventNumber(eventNumber, matchKind);

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

long EventLogIndex::getOffsetForEventNumber(long eventNumber, MatchKind matchKind)
{
    long offset = binarySearchForOffset(true, &eventNumberToOffsetMap, eventNumber, matchKind);

    if (PRINT_DEBUG_MESSAGES) printf("Found event number: %ld at offset: %ld\n", eventNumber, offset);

    return offset;
}

long EventLogIndex::getOffsetForSimulationTime(simtime_t simulationTime, MatchKind matchKind)
{
    long offset = binarySearchForOffset(false, &simulationTimeToOffsetMap, simulationTime, matchKind);

    if (PRINT_DEBUG_MESSAGES) printf("*** Found simulation time: %.*g at offset: %ld\n", 12, simulationTime, offset);

    return offset;
}

template <typename T> long EventLogIndex::binarySearchForOffset(bool eventNumberBased, std::map<T, long> *keyToOffsetMap, T key, MatchKind matchKind)
{
    long foundOffset = -1;
    long lowerOffset, upperOffset;

    // find key in map (lower_bound() finds equal or first greater key)
    typename std::map<T, long>::iterator it = keyToOffsetMap->lower_bound(key);

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
            long midOffset = (upperOffset + lowerOffset) / 2;
            //printf("step %d: offsets: lo=%ld, up=%ld, mid=%ld \t\tkey#: lo=#%ld\n", 
            //       stepCount, lowerOffset, upperOffset, midOffset, lowerKey);
            long midEventNumber, midEventStartOffset, midEventEndOffset;
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
        long firstOffset = linearSearchForOffset(eventNumberBased, foundOffset, key, FIRST, true);
        long lastOffset = linearSearchForOffset(eventNumberBased, foundOffset, key, LAST, true);

        if (foundOffset == lastOffset && foundOffset == firstOffset)
            return foundOffset;
        else
            throw new Exception("Found non unique simulation time when exact match is requested");
    }
    else
    {
        // non exact match requested
        bool exactMatchFound = foundOffset != -1;
        long searchOffset = exactMatchFound ? foundOffset : (matchKind == FIRST ? lowerOffset : upperOffset);
        return linearSearchForOffset(eventNumberBased, searchOffset, key, matchKind, exactMatchFound);
    }
}

template <typename T> long EventLogIndex::linearSearchForOffset(bool eventNumberBased, long offset, T key, MatchKind matchKind, bool exactMatchFound)
{
    // this is a search either forward or backward based on matchKind
    if (matchKind == EXACT)
        throw new Exception("Invalid match kind");

    long previousOffset = offset;
    long eventNumber, lineStartOffset, lineEndOffset;
    simtime_t simulationTime;

    while (offset != -1)
    {
        bool readEventLine = readToEventLine(matchKind == LAST, offset, eventNumber, simulationTime, lineStartOffset, lineEndOffset);

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
            if (matchKind == LAST && readKey > key)
                return lineStartOffset;

            if (matchKind == FIRST && readKey < key)
                return lineStartOffset;
        }
        else if (readKey != key)
            return previousOffset;

        previousOffset = lineStartOffset;

        if (matchKind == FIRST)
            offset = lineStartOffset;

        if (matchKind == LAST)
            offset = lineEndOffset;
    }

    return -1;
}

bool EventLogIndex::readToEventLine(bool forward, long readStartOffset, long& eventNumber, simtime_t& simulationTime, long& lineStartOffset, long& lineEndOffset)
{
    eventNumber = -1;
    simulationTime = -1;
    reader->seekTo(readStartOffset);

    char *line;

    if (PRINT_DEBUG_MESSAGES) printf("Reading to first event line from offset: %ld in direction: %s\n", readStartOffset, forward ? "forward" : "backward");

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
    lineEndOffset = lineStartOffset + strlen(line);

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
    throw new Exception("Wrong file format: no event number in 'E' line, line %d", reader->getNumReadLines());
}

void EventLogIndex::dumpTable()
{
    printf("Stored eventNumberToOffsetMap:\n");

    for (EventNumberToOffsetMap::iterator it = eventNumberToOffsetMap.begin(); it!=eventNumberToOffsetMap.end(); ++it)
        printf("  #%ld --> offset %ld (0x%lx)\n", it->first, it->second, it->second);
}
