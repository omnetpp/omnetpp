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
#include <assert.h>
#include "eventlogindex.h"
#include "exception.h"


EventLogIndex::EventLogIndex(FileReader *reader)
{
    this->reader = reader;
}

EventLogIndex::~EventLogIndex()
{
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

void EventLogIndex::addPositionForEventNumber(long eventNumber, long offset)
{
    if (needsToBeStored(eventNumber))
        eventNumberToOffsetMap[eventNumber] = offset;
}

bool EventLogIndex::positionToEventNumber(long eventNumber)
{
    long offset = getOffsetForEventNumber(eventNumber);
    if (offset==-1)
        return false; // eventNumber not found
    reader->seekTo(offset);
    return true;
}

long EventLogIndex::getOffsetForEventNumber(long eventNumber)
{
    // find eventNumber in map (lower_bound() finds equal or first greater key)
    EventNumberToOffsetMap::iterator it = eventNumberToOffsetMap.lower_bound(eventNumber);

    // exact match?
    if (it != eventNumberToOffsetMap.end() && it->first==eventNumber)
        return it->second;

    // figure out start positions for binary search
    long lowerEventNumber, lowerOffset, upperOffset;
    if (it==eventNumberToOffsetMap.end()) {
        upperOffset = reader->fileSize();
    } else {
        upperOffset = it->second;
    }
    if (it==eventNumberToOffsetMap.begin()) {
        lowerEventNumber = 0;
        lowerOffset = 0;
    } else {
        --it; // note: --it seems to be valid even when it==end()
        lowerEventNumber = it->first;
        lowerOffset = it->second;
    }

    // binary search
    //
    // IMPORTANT NOTE: lowerOffset will always be exactly on an "E" line
    // (that of lowerEventNumber), but upperOffset will NOT! This is necessary
    // for the distance between lowerOffset and upperOffset to shrink properly.
    //
    int stepCount = 0;
    long foundOffset = -1;
    while (true)
    {
        stepCount++;
        long midOffset = (upperOffset + lowerOffset) / 2;
        //printf("step %d: offsets: lo=%ld, up=%ld, mid=%ld \t\tevent#: lo=#%ld\n", 
        //       stepCount, lowerOffset, upperOffset, midOffset, lowerEventNumber);
        long midEventNumber, midEventOffset;
        if (readToFirstEventLine(midOffset, midEventNumber, midEventOffset))
        {
            //printf("  found event #%ld at offset=%ld\n", midEventNumber, midEventOffset);
            
            // stopping condition
            if (midEventNumber==eventNumber || upperOffset-lowerOffset<10)
            {
                foundOffset = midEventNumber==eventNumber ? midEventOffset : -1;
                break;
            }

            // store the mid position
            addPositionForEventNumber(midEventNumber, midEventOffset);

            // assign "mid" to "lower" or "upper"
            if (midEventNumber < eventNumber)
            {
                lowerEventNumber = midEventNumber;
                lowerOffset = midEventOffset;
            }
            else
            {
                upperOffset = midOffset;
            }
        }
        else
        {
            //printf("  NOTHING found, decreasing upperOffset\n");
            
            // no event number found -- we must be at the very end of the file.
            // try again finding an "E" line from a bit earlier point in the file.
            upperOffset -= 10;
        }
    }
    //printf("  binary search steps: %d\n", stepCount);
    return foundOffset;
}

bool EventLogIndex::readToFirstEventLine(long startOffset, long& eventNumber, long& offset)
{
    reader->seekTo(startOffset);

    // find first "E" line, return false if none found
    while (true)
    {
        char *line = reader->readLine();
        if (!line)
            return false;
        tokenizer.tokenize(line);
        if (tokenizer.numTokens()>=3 && !strcmp(tokenizer.tokens()[0], "E"))
            break;
    }

    // find event number in line ("# 2456")
    offset = reader->lineStartOffset();
    for (int i=1; i<tokenizer.numTokens()-1; i+=2)
    {
        if (!strcmp(tokenizer.tokens()[i], "#"))
        {
            eventNumber = atol(tokenizer.tokens()[i+1]);
            return true;
        }
    }

    // bad luck
    throw new Exception("Wrong file format: no event number in 'E' line, line %d", reader->lineNum());
}

void EventLogIndex::dumpTable()
{
    printf("Stored eventNumberToOffsetMap:\n");
    for (EventNumberToOffsetMap::iterator it = eventNumberToOffsetMap.begin(); it!=eventNumberToOffsetMap.end(); ++it)
        printf("  #%ld --> offset %ld (0x%lx)\n", it->first, it->second, it->second);
}


