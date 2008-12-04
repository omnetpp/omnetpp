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
#include "tklib.h"
#include "logbuffer.h"

USING_NAMESPACE

LogBuffer::Entry::Entry()
{
    eventNumber = 0;
    simtime = 0;
    moduleId = 0;
    banner = NULL;
    numChars = 0;
}

LogBuffer::Entry::Entry(eventnumber_t e, simtime_t t, int modId, const char *bannerText)
{
    eventNumber = e;
    simtime = t;
    moduleId = modId;
    banner = bannerText;
    numChars = banner ? strlen(banner) : 0;
}

LogBuffer::Entry::~Entry()
{
    delete[] banner;
    for (int i=0; i<(int)lines.size(); i++)
        delete[] lines[i];
}

//----

LogBuffer::LogBuffer(int memoryLimit)
{
    memLimit = memoryLimit;
    totalChars = 0;
}

LogBuffer::~LogBuffer()
{
}

void LogBuffer::addEvent(eventnumber_t e, simtime_t t, int moduleId, const char *banner)
{
    entries.push_back(Entry(e,t,moduleId,banner));
    totalStrings++;
    totalChars += entries.back().numChars;
    discardIfMemoryLimitExceeded();
}

void LogBuffer::addLogLine(const char *buffer, int n)
{
    char *line = new char[n+1];
    memcpy(line, buffer, n);
    line[n] = 0;

    Entry& entry = entries.back();
    entry.lines.push_back(line);
    totalStrings++;
    totalChars += n;

    discardIfMemoryLimitExceeded();
}

void LogBuffer::addInfo(const char *text)
{
    entries.push_back(Entry(0,0,0,text));
    totalStrings++;
    totalChars += entries.back().numChars;
    discardIfMemoryLimitExceeded();
}

void LogBuffer::setMemoryLimit(size_t limit)
{
    memLimit = limit;
    discardIfMemoryLimitExceeded();
}

void LogBuffer::discardIfMemoryLimitExceeded()
{
    while (estimatedMemUsage() > memLimit)
    {
        // discard first entry
        Entry& entry = entries.front();
        totalChars -= entry.numChars;
        totalStrings -= entry.lines.size()+1;
        entries.pop_front();
    }
}


