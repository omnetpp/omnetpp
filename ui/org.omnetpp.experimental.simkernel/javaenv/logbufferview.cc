//==========================================================================
//  LOGBUFFERVIEW.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include "commonutil.h"
#include "logbufferview.h"

USING_NAMESPACE

LogBufferView::LogBufferView(LogBuffer *log, int moduleId, const std::set<int>& excludedModuleIds)
{
    this->log = log;
    this->rootModuleId = moduleId;
    this->excludedModuleIds = excludedModuleIds;

    // count lines and chars
    totalLines = 0;
    totalChars = 0;
    const std::list<LogBuffer::Entry>& entries = log->getEntries();
    for (std::list<LogBuffer::Entry>::const_iterator it=entries.begin(); it!=entries.end(); it++)
    {
        const LogBuffer::Entry& entry = *it;
        if (isGood(entry))
        {
            totalLines += entry.getNumLines();
            totalChars += entry.getNumChars();
        }
    }

    // not at a valid position
    currentPosValid = false;
}

LogBufferView::~LogBufferView()
{
}

bool LogBufferView::isGood(const LogBuffer::Entry& entry) const
{
    int entryModuleId = entry.getModuleId();

    // "info" lines (no module ID) are always included
    if (entryModuleId == 0)
        return true;

    // otherwise, it must be under our "root" module, and must not be excluded
    return entry.isFromTreeOf(rootModuleId) && excludedModuleIds.find(entryModuleId)==excludedModuleIds.end();
}

void LogBufferView::incCurrentEntry()
{
    Assert(currentPosValid);
    while (true)
    {
        currentEntry++;
        if (currentEntry == log->getEntries().end()) {
            currentPosValid = false;
            break;
        }
        if (isGood(*currentEntry))
            break;
    }
}

void LogBufferView::decCurrentEntry()
{
    Assert(currentPosValid);
    while (true)
    {
        if (currentEntry == log->getEntries().begin()) {
            currentPosValid = false;
            break;
        }
        currentEntry--;
        if (isGood(*currentEntry))
            break;
    }
}

void LogBufferView::gotoNextLineInEntry()
{
    Assert(currentPosValid && entryLineNo < currentEntry->getNumLines()-1);
    currentLineOffset += currentEntry->getLineLength(entryLineNo);
    currentLineIndex++;
    entryLineNo++;
}

void LogBufferView::gotoPreviousLineInEntry()
{
    Assert(currentPosValid && entryLineNo > 0);
    entryLineNo--;
    currentLineOffset -= currentEntry->getLineLength(entryLineNo);
    currentLineIndex--;
}

inline size_t distance(size_t a, size_t b) { return a<b ? b-a : a-b; }

void LogBufferView::gotoLine(size_t lineIndex)
{
    // find suitable starting point
    size_t distanceToBeg = lineIndex;
    size_t distanceToEnd = getNumLines() - lineIndex;
    size_t distanceToCurrent = currentPosValid ? distance(currentLineIndex,lineIndex) : getNumLines();
    if (distanceToBeg < distanceToEnd && distanceToBeg < distanceToCurrent)
    {
        // go to the beginning
        currentEntry = log->getEntries().begin();
        while (!isGood(*currentEntry) && currentEntry!=log->getEntries().end())
            ++currentEntry;
        if (currentEntry == log->getEntries().end())
            currentPosValid = false;
        else {
            currentPosValid = true;
            entryLineNo = 0;
            currentLineIndex = 0;
            currentLineOffset = 0;
        }
    }
    else if (distanceToEnd < distanceToBeg && distanceToEnd < distanceToCurrent)
    {
        // go to the end
        currentEntry = log->getEntries().end();
        --currentEntry;
        while (!isGood(*currentEntry) && currentEntry!=log->getEntries().begin())
            --currentEntry;
        if (!isGood(*currentEntry))
            currentPosValid = false;
        else {
            currentPosValid = true;
            entryLineNo = currentEntry->getNumLines() - 1;
            currentLineIndex = getNumLines() - 1;
            currentLineOffset = getNumChars() - currentEntry->getLineLength(entryLineNo);
        }
    }

    if (!currentPosValid)
        return;

    // position INTO the required entry. First, go backward whole entries if needed
    if (lineIndex < currentLineIndex - entryLineNo) {
        // seek to the beginning of this entry
        while (entryLineNo > 0)
            gotoPreviousLineInEntry();

        // go backward a few entries
        while (lineIndex < currentLineIndex) {
            decCurrentEntry();
            if (!currentPosValid)
                return;
            currentLineIndex -= currentEntry->getNumLines();
            currentLineOffset -= currentEntry->getNumChars();
        }

    }

    // go forward whole entries if needed
    if (lineIndex >= currentLineIndex - entryLineNo + currentEntry->getNumLines()) {
        // seek to the beginning of this entry
        while (entryLineNo > 0)
            gotoPreviousLineInEntry();

        // go forward a few entries
        while (lineIndex >= currentLineIndex + currentEntry->getNumLines()) {
            currentLineIndex += currentEntry->getNumLines();
            currentLineOffset += currentEntry->getNumChars();
            incCurrentEntry();
            if (!currentPosValid)
                return;
        }
    }

    // position within the entry
    while (lineIndex > currentLineIndex)
        gotoNextLineInEntry();
    while (lineIndex < currentLineIndex)
        gotoPreviousLineInEntry();
}

void LogBufferView::gotoOffset(size_t offset)
{
    // find suitable starting point

    // walk to given offset
    //TODO
}

const char *LogBufferView::getLine(size_t lineIndex)
{
    gotoLine(lineIndex);
    Assert(currentPosValid);
    return currentEntry->getLine(entryLineNo);
}

size_t LogBufferView::getLineAtOffset(size_t offset)
{
    gotoOffset(offset);
    Assert(currentPosValid);
    return currentLineIndex;
}

size_t LogBufferView::getOffsetAtLine(size_t lineIndex)
{
    gotoLine(lineIndex);
    Assert(currentPosValid);
    return currentLineOffset;
}

std::string LogBufferView::getTextRange(int start, int length)
{
    gotoOffset(start);
    Assert(currentPosValid);
    //TODO....
    return "TODO";
}

