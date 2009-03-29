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
            totalLines += 1 + entry.lines.size();  //+1=banner
            totalChars += entry.numChars; //XXX does this include newlines???
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
    // "info" lines (no module ID) are always included
    if (!entry.moduleIds)
        return true;

    // check that this module is covered in entry.moduleIds[] (module path up to the root)
    bool found = false;
    for (int *p = entry.moduleIds; !found && *p; p++)
        if (*p == rootModuleId)
            found = true;
    // also, it must not be excluded
    return found && excludedModuleIds.find(entry.moduleIds[0])==excludedModuleIds.end();
}

const char *LogBufferView::currentLine() const
{
    return entryLineNo==0 ? currentEntry->banner : currentEntry->lines[entryLineNo-1];
}

void LogBufferView::gotoNextLine()
{
    if (entryLineNo < 1 + currentEntry->lines.size())
    {
        currentLineOffset += strlen(currentLine()) + 1;  //+1=LF
        currentLineIndex++;
        entryLineNo++;
    }
    else
    {
        currentLineOffset += strlen(currentLine()) + 1;  //+1=LF
        currentLineIndex++;
        currentEntry++;
        entryLineNo = 0;
        if (currentEntry == log->lines().end())
            XXX;  //invalidpos?
    }
}

void LogBufferView::gotoPreviousLine()
{
}

inline size_t distance(size_t a, size_t b) { return a<b ? b-a : a-b; }

void LogBufferView::gotoLine(size_t lineIndex)
{
    // find suitable starting point
    //TODO if (!currentPosValid || distance(currentLineIndex,lineIndex)

    // walk forward to given line index
    while (lineIndex > currentLineIndex)
    {
        size_t entryLinesRemaining = currentEntry->lines.size() - entryLineNo; // counting +1 banner too
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
            currentLineIndex += 1+currentEntry->lines.size();
            currentLineOffset += currentEntry->numChars;
            currentEntry++; //XXX what if end()
        }
    }

    // walk backward to given line index
    while (lineIndex < currentLineIndex)
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
            currentEntry--; //XXX what if begin()?
            currentLineIndex -= 1+currentEntry->lines.size();
            currentLineOffset -= currentEntry->numChars;
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
    return currentLine();
}

size_t LogBufferView::getLineAtOffset(size_t offset)
{
    gotoOffset(offset);
    return currentLineIndex;
}

size_t LogBufferView::getOffsetAtLine(size_t lineIndex)
{
    gotoLine(lineIndex);
    return currentLineOffset;
}

std::string LogBufferView::getTextRange(int start, int length)
{
    gotoOffset(start);
    //TODO....
    return "TODO";
}

