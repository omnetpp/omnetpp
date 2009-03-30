//==========================================================================
//  LOGBUFFER.CC - part of
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
#include "logbuffer.h"
#include "cmodule.h"

USING_NAMESPACE

LogBuffer::Entry::~Entry()
{
    delete[] moduleIds;
    delete[] banner;
    for (int i=0; i<(int)lines.size(); i++)
        delete[] lines[i];
}

bool LogBuffer::Entry::isFromTreeOf(int moduleId) const
{
    // check that moduleId is contained in our module path up to the root
    for (int *p = moduleIds; *p; p++)
        if (*p == moduleId)
            return true;
    return false;
}

//----

LogBuffer::LogBuffer(int memoryLimit)
{
    numEntries =0;
    memLimit = memoryLimit;
    totalChars = 0;
    totalLines = 0;
}

LogBuffer::~LogBuffer()
{
}

void LogBuffer::fillEntry(Entry& entry, eventnumber_t e, simtime_t t, cModule *mod, const char *banner)
{
    entry.eventNumber = e;
    entry.simtime = t;
    entry.banner = opp_strdup(banner);
    entry.numChars = banner ? strlen(banner)+1: 1;

    // store all moduleIds up to the root
    if (mod)
    {
        int depth = 0;
        for (cModule *p=mod; p; p=p->getParentModule())
            depth++;
        entry.moduleIds = new int[depth+1];
        int i = 0;
        for (cModule *p=mod; p; p=p->getParentModule(), i++)
            entry.moduleIds[i] = p->getId();
         entry.moduleIds[depth] = 0;
    }
}

void LogBuffer::addEvent(eventnumber_t e, simtime_t t, cModule *mod, const char *banner)
{
    entries.push_back(Entry());
    numEntries++;
    fillEntry(entries.back(), e, t, mod, banner);
    totalLines++;
    totalChars += entries.back().numChars;

    linesAdded(1, entries.back().getNumChars());

    discardIfMemoryLimitExceeded();
}

void LogBuffer::addLogLine(const char *text)
{
    if (entries.empty())
    {
        // this is likely the initialize() phase -- hence no banner
        addEvent(0, 0, NULL, "-");
        Entry& entry = entries.back();
        entry.moduleIds = new int[1]; // add empty array, to distinguish entry from an info entry
        entry.moduleIds[0] = 0;
    }

    //FIXME if last line is "info" then we cannot append to it! create new entry with empty banner?

    Entry& entry = entries.back();
    entry.lines.push_back(opp_strdup(text));
    size_t lineLength = strlen(text); //FIXME opp_strdup() already counted that!!!
    entry.numChars += lineLength + 1;
    totalLines++;
    totalChars += lineLength + 1;

    linesAdded(1, lineLength + 1);

    discardIfMemoryLimitExceeded();
}

void LogBuffer::addInfo(const char *text)
{
    entries.push_back(Entry());
    numEntries++;
    fillEntry(entries.back(), 0, 0, NULL, text);
    totalLines++;
    totalChars += entries.back().getNumChars();

    linesAdded(1, entries.back().getNumChars());

    discardIfMemoryLimitExceeded();
}

void LogBuffer::setMemoryLimit(size_t limit)
{
    memLimit = limit;
    discardIfMemoryLimitExceeded();
}

void LogBuffer::discardIfMemoryLimitExceeded()
{
    size_t numLinesDiscarded = 0;
    size_t numCharsDiscarded = 0;

    while (estimatedMemUsage() > memLimit && numEntries > 1)  // leave at least 1 entry
    {
        // discard first entry
        Entry& entry = entries.front();
        size_t numLines = entry.getNumLines();
        size_t numChars = entry.getNumChars();
        entries.pop_front();
        numEntries--;

        totalChars -= numChars;
        totalLines -= numLines;
        numLinesDiscarded += numLines;
        numCharsDiscarded += numChars;
    }
    linesDiscarded(numLinesDiscarded, numCharsDiscarded);
}

void LogBuffer::addListener(IListener *listener)
{
    listeners.push_back(listener);
}

void LogBuffer::removeListener(IListener *listener)
{
    for (int i=0; i<(int)listeners.size(); i++) {
        if (listeners[i] == listener) {
            listeners.erase(listeners.begin() + i);
            return;
        }
    }
}

void LogBuffer::linesAdded(size_t numLines, size_t numChars)
{
    for (int i=0; i<(int)listeners.size(); i++)
        listeners[i]->linesAdded(numLines, numChars);
}

void LogBuffer::linesDiscarded(size_t numLines, size_t numChars)
{
    for (int i=0; i<(int)listeners.size(); i++)
        listeners[i]->linesDiscarded(numLines, numChars);
}

#define LL  INT64_PRINTF_FORMAT

void LogBuffer::dump() const
{
    printf("\nLogBuffer dump (%d entries):\n", numEntries);

    int k=0;
    for (std::list<Entry>::const_iterator it=entries.begin(); it!=entries.end(); it++)
    {
        const LogBuffer::Entry& entry = *it;
        printf("[%d] event #%ld T=%s moduleId=%d numLines=%d numChars=%d:\n",
               k, (long)entry.getEventNumber(), SIMTIME_STR(entry.getSimtime()), entry.getModuleId(),
               (int)entry.getNumLines(), (int)entry.getNumChars());
        for (int i=0; i<(int)entry.getNumLines(); i++)
            printf("\tlen=%d >>%s<<\n", (int)entry.getLineLength(i), entry.getLine(i));
        k++;
    }
}


