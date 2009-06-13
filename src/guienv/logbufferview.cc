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

void LogBufferViewInput::addModuleTree(int moduleId, const std::vector<int>& excludedModuleIds)
{
    v.push_back(ModuleTree());
    v.back().rootModuleId = moduleId;
    v.back().excludedModuleIds = excludedModuleIds;
}

//---

LogBufferView::LogBufferView(LogBuffer *log, const LogBufferViewInput& f)
{
    this->log = log;

    for (int i=0; i<f.getNumModuleTrees(); i++)
    {
        filter.push_back(ModuleTree());
        filter.back().rootModuleId = f.getModuleId(i);
        const std::vector<int>& v = f.getExcludedModuleIds(i);
        for (int j=0; j<(int)v.size(); j++)
            filter.back().excludedModuleIds.insert(v[j]);
    }

    countLines();
    currentPosValid = false;

    log->addListener(this);
}

LogBufferView::~LogBufferView()
{
    log->removeListener(this);
}

void LogBufferView::countLines()
{
    totalLines = 0;
    totalChars = 0;
    const std::list<LogBuffer::Entry>& entries = log->getEntries();
    for (std::list<LogBuffer::Entry>::const_iterator it=entries.begin(); it!=entries.end(); it++)
    {
        const LogBuffer::Entry& entry = *it;
        if (isGoodEntry(entry))
        {
            totalLines += entry.getNumLines();
            totalChars += entry.getNumChars();
        }
    }
}

void LogBufferView::entryAdded(const LogBuffer::Entry& entry)
{
    if (isGoodEntry(entry))
    {
        totalLines += entry.getNumLines();
        totalChars += entry.getNumChars();
    }
    //verifyTotals(); -- put it back when debugging
}

void LogBufferView::lineAdded(const LogBuffer::Entry& entry, size_t numLineChars)
{
    if (isGoodEntry(entry))
    {
        totalLines++;
        totalChars += numLineChars;
    }
    //verifyTotals(); -- put it back when debugging
}

void LogBufferView::discardingEntry(const LogBuffer::Entry& entry)
{
    if (isGoodEntry(entry))
    {
        totalLines -= entry.getNumLines();
        totalChars -= entry.getNumChars();

        if (currentPosValid)
        {
            if (currentLineIndex < entry.getNumLines())
                currentPosValid = false;
            else {
                currentLineIndex -= entry.getNumLines();
                currentLineOffset -= entry.getNumChars();
            }
        }
    }

    // note: cannot call verifyTotals() here because entry hasn't been actually
    // discarded from the underlying log yet
}

void LogBufferView::verifyTotals()
{
    size_t oldTotalLines = totalLines;
    size_t oldTotalChars = totalChars;
    countLines();

    Assert(oldTotalLines==totalLines);
    Assert(oldTotalChars==totalChars);
}

bool LogBufferView::isGoodEntry(const LogBuffer::Entry& entry) const
{
    if (filter.empty())
        return true;  // no filtering

    int entryModuleId = entry.getModuleId();

    // "info" lines (no module ID) are always included
    if (entryModuleId == 0)
        return true;

    // otherwise, it must be under one of our filter modules, and must not be excluded
    for (int i=0; i<(int)filter.size(); i++) {
        const ModuleTree& f = filter[i];
        if (entry.isFromTreeOf(f.rootModuleId) && f.excludedModuleIds.find(entryModuleId)==f.excludedModuleIds.end())
            return true;
    }
    return false;
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
        if (isGoodEntry(*currentEntry))
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
        if (isGoodEntry(*currentEntry))
            break;
    }
}

void LogBufferView::gotoNextLineInEntry()
{
    Assert(currentPosValid && entryLineNo < currentEntry->getNumLines()-1);
    size_t lineLength = currentEntry->getLineLength(entryLineNo);
    currentLineIndex++;
    currentLineOffset += lineLength;
    entryLineNo++;
    entryLineOffset += lineLength;
}

void LogBufferView::gotoPreviousLineInEntry()
{
    Assert(currentPosValid && entryLineNo > 0);
    entryLineNo--;
    size_t lineLength = currentEntry->getLineLength(entryLineNo);
    currentLineOffset -= lineLength;
    currentLineIndex--;
    entryLineOffset -= lineLength;
}

void LogBufferView::gotoBeginning()
{
    currentEntry = log->getEntries().begin();
    while (!isGoodEntry(*currentEntry) && currentEntry!=log->getEntries().end())
        ++currentEntry;
    if (currentEntry == log->getEntries().end())
        currentPosValid = false;
    else {
        currentPosValid = true;
        entryLineNo = 0;
        entryLineOffset = 0;
        currentLineIndex = 0;
        currentLineOffset = 0;
    }
}

void LogBufferView::gotoEnd()
{
    currentEntry = log->getEntries().end();
    --currentEntry;
    while (!isGoodEntry(*currentEntry) && currentEntry!=log->getEntries().begin())
        --currentEntry;
    if (!isGoodEntry(*currentEntry))
        currentPosValid = false;
    else {
        currentPosValid = true;
        entryLineNo = currentEntry->getNumLines() - 1;
        size_t lineLength = currentEntry->getLineLength(entryLineNo);
        entryLineOffset = currentEntry->getNumChars() - lineLength;
        currentLineIndex = getNumLines() - 1;
        currentLineOffset = getNumChars() - lineLength;
    }
}

inline size_t distance(size_t a, size_t b) { return a<b ? b-a : a-b; }

void LogBufferView::gotoLine(size_t lineIndex)
{
    if (lineIndex<0 || lineIndex>=getNumLines())
        throw opp_runtime_error("LogBufferView: line index %ld is out of range 0..%ld", (long)lineIndex, (long)getNumLines()-1);

    // find suitable starting point
    size_t distanceToBeg = lineIndex;
    size_t distanceToEnd = getNumLines() - lineIndex;
    size_t distanceToCurrent = currentPosValid ? distance(currentLineIndex,lineIndex) : getNumLines();
    if (distanceToCurrent < distanceToBeg && distanceToCurrent < distanceToEnd)
        ; // nop - use current position
    else if (distanceToBeg < distanceToEnd)
        gotoBeginning();
    else
        gotoEnd();

    if (!currentPosValid)
        return;

    // position INTO the required entry. First, go backward whole entries if needed
    if (lineIndex < currentLineIndex - entryLineNo) {
        // seek to the beginning of this entry
        while (entryLineNo > 0)
            gotoPreviousLineInEntry();
        Assert(entryLineOffset == 0);

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
        Assert(entryLineOffset == 0);

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
    if (offset<0 || offset>=getNumChars())
        throw opp_runtime_error("LogBufferView: offset %ld is out of range 0..%ld", (long)offset, (long)getNumChars()-1);

    // find suitable starting point
    size_t distanceToBeg = offset;
    size_t distanceToEnd = getNumChars() - offset;
    size_t distanceToCurrent = currentPosValid ? distance(currentLineOffset,offset) : getNumChars();
    if (distanceToBeg < distanceToEnd && distanceToBeg < distanceToCurrent)
        gotoBeginning();
    else if (distanceToEnd < distanceToBeg && distanceToEnd < distanceToCurrent)
        gotoEnd();

    if (!currentPosValid)
        return;

    // position INTO the required entry. First, go backward whole entries if needed
    if (offset < currentLineOffset - entryLineOffset) {
        // seek to the beginning of this entry
        while (entryLineNo > 0)
            gotoPreviousLineInEntry();
        Assert(entryLineOffset == 0);

        // go backward a few entries
        while (offset < currentLineOffset) {
            decCurrentEntry();
            if (!currentPosValid)
                return;
            currentLineIndex -= currentEntry->getNumLines();
            currentLineOffset -= currentEntry->getNumChars();
        }

    }

    // go forward whole entries if needed
    if (offset >= currentLineOffset - entryLineOffset + currentEntry->getNumChars()) {
        // seek to the beginning of this entry
        while (entryLineNo > 0)
            gotoPreviousLineInEntry();
        Assert(entryLineOffset == 0);

        // go forward a few entries
        while (offset >= currentLineOffset + currentEntry->getNumChars()) {
            currentLineIndex += currentEntry->getNumLines();
            currentLineOffset += currentEntry->getNumChars();
            incCurrentEntry();
            if (!currentPosValid)
                return;
        }
    }

    // position within the entry
    while (offset >= currentLineOffset + currentEntry->getLineLength(entryLineNo))
        gotoNextLineInEntry();
    while (offset < currentLineOffset)
        gotoPreviousLineInEntry();
}

const char *LogBufferView::getLine(size_t lineIndex)
{
    gotoLine(lineIndex);
    Assert(currentPosValid);
    return currentEntry->getLine(entryLineNo);
}

int LogBufferView::getLineType(size_t lineIndex)
{
    gotoLine(lineIndex);
    Assert(currentPosValid);
    return currentEntry->getLineType(entryLineNo);
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
