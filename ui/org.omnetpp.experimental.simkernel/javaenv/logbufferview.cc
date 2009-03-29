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

void LogBufferView::gotoNextLine()
{
    Assert(currentPosValid);
    if (entryLineNo < currentEntry->getNumLines()-1)
    {
        currentLineOffset += currentEntry->getLineLength(entryLineNo);
        currentLineIndex++;
        entryLineNo++;
    }
    else
    {
        currentLineOffset += currentEntry->getLineLength(entryLineNo);
        currentLineIndex++;
        entryLineNo = 0;
        currentEntry++;   //XXX and filter???
        if (currentEntry == log->getEntries().end())
            currentPosValid = false;
    }
}

void LogBufferView::gotoPreviousLine()
{
    Assert(currentPosValid);
    if (entryLineNo > 0)
    {
        entryLineNo--;
        currentLineOffset -= currentEntry->getLineLength(entryLineNo);
        currentLineIndex--;
    }
    else
    {
        if (currentEntry == log->getEntries().begin())
            currentPosValid = false;
        else {
            currentEntry--;  //XXX and filter???
            entryLineNo = currentEntry->getNumLines();
            currentLineOffset -= currentEntry->getLineLength(entryLineNo);
            currentLineIndex--;
        }
    }
}

inline size_t distance(size_t a, size_t b) { return a<b ? b-a : a-b; }

void LogBufferView::gotoLine(size_t lineIndex)
{
    // find suitable starting point
    //TODO if (!currentPosValid || distance(currentLineIndex,lineIndex)

    // walk forward to given line index
    while (currentPosValid && lineIndex > currentLineIndex)
    {
        size_t entryLinesRemaining = currentEntry->getNumLines() - entryLineNo;
        size_t distanceToGo = lineIndex - currentLineIndex;
        if (distanceToGo <= entryLinesRemaining)
        {
            // target is within current entry - go there
            for (int i=0; i<distanceToGo; i++)
                gotoNextLine();
        }
        else if (entryLineNo != 0)
        {
            // skip the rest of this entry
            while (entryLineNo != 0)
                gotoNextLine();
        }
        else
        {
            // skip this whole entry at once
            currentLineIndex += currentEntry->getNumLines();
            currentLineOffset += currentEntry->getNumChars();
            incCurrentEntry();
        }
    }

    // walk backward to given line index
    while (currentPosValid && lineIndex < currentLineIndex)
    {
        size_t entryLinesRemaining = entryLineNo;
        size_t distanceToGo = currentLineIndex - lineIndex;
        if (distanceToGo <= entryLinesRemaining)
        {
            // target is within current entry - go there
            for (int i=0; i<distanceToGo; i++)
                gotoPreviousLine();
        }
        else if (entryLineNo != 0)
        {
            // go to the beginning of this entry
            while (entryLineNo != 0)
                gotoPreviousLine();
        }
        else
        {
            // at line 0 of an entry -- skip to the beginning of the previous entry at once
            //XXX isn't that too much???
            decCurrentEntry(); //XXX what if begin()?
            currentLineIndex -= currentEntry->getNumLines();
            currentLineOffset -= currentEntry->getNumChars();
        }
    }
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

