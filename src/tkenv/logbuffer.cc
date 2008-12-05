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
#include "cmodule.h"

USING_NAMESPACE

LogBuffer::Entry::~Entry()
{
    delete[] moduleIds;
    delete[] banner;
    for (int i=0; i<(int)lines.size(); i++)
        delete[] lines[i];
}

//----

LogBuffer::LogBuffer(int memoryLimit)
{
    memLimit = memoryLimit;
    totalChars = 0;
    totalStrings = 0;
}

LogBuffer::~LogBuffer()
{
}

void LogBuffer::fillEntry(Entry& entry, eventnumber_t e, simtime_t t, cModule *mod, const char *banner)
{
    entry.eventNumber = e;
    entry.simtime = t;
    entry.banner = opp_strdup(banner);
    entry.numChars = banner ? strlen(banner) : 0;

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
    fillEntry(entries.back(), e, t, mod, banner);
    totalStrings++;
    totalChars += entries.back().numChars;
    discardIfMemoryLimitExceeded();
}

void LogBuffer::addLogLine(const char *text)
{
    if (entries.empty())
    {
        // this is likely the initialize() phase -- hence no banner
        addEvent(0, 0, NULL, "{}");
        Entry& entry = entries.back();
        entry.moduleIds = new int[1]; // add empty array, to distinguish entry from an info entry
        entry.moduleIds[0] = 0;
    }

    //FIXME if last line is "info" then we cannot append to it! create new entry with empty banner?

    Entry& entry = entries.back();
    entry.lines.push_back(opp_strdup(text));
    totalStrings++;
    totalChars += strlen(text);

    discardIfMemoryLimitExceeded();
}

void LogBuffer::addInfo(const char *text)
{
    entries.push_back(Entry());
    fillEntry(entries.back(), 0, 0, NULL, text);
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
    while (estimatedMemUsage() > memLimit && entries.size()>1)  // leave at least 1 entry
    {
        // discard first entry
        Entry& entry = entries.front();
        totalChars -= entry.numChars;
        totalStrings -= entry.lines.size()+1;
        entries.pop_front();
    }
}

void LogBuffer::dump() const
{
    printf("LogBuffer: %d entries\n", entries.size());

    int k=0;
    for (std::list<Entry>::const_iterator it=entries.begin(); it!=entries.end(); it++)
    {
        const LogBuffer::Entry& entry = *it;
        printf("[%d] #%ld t=%s moduleId=%d: %s", k, entry.eventNumber, SIMTIME_STR(entry.simtime), entry.moduleIds?entry.moduleIds[0]:-1, entry.banner);
        for (int i=0; i<(int)entry.lines.size(); i++)
            printf("\t[l%d]:%s", k, entry.lines[i]);
        k++;
    }
}


