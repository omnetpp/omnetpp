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
    entries.push_back(Entry(0,0,0,banner));
    totalStrings++;
    totalChars += entries.back().numChars;
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


/*
void LogBuffer::insertIntoTkText(Tcl_Interp *interp, const char *textWidget, std::set<int>& moduleIds) const
{
    //FIXME zold sorokat is jegyezze meg/irja ki!!!
    for (std::list<Entry>::const_iterator it=entries.begin(); it!=entries.end(); it++)
    {
        const Entry& entry = *it;
        if (moduleIds.find(entry.moduleId)!=moduleIds.end())
        {
            CHK(Tcl_VarEval(interp, textWidget, " insert end {", entry.banner, "} event\n", NULL));
            for (int i=0; i<(int)entry.lines.size(); i++)
                CHK(Tcl_VarEval(interp, textWidget, " insert end {", entry.lines[i], "}\n", NULL));
        }
    }
    CHK(Tcl_VarEval(interp, textWidget, " see end", NULL));
}

void LogBuffer::printBanner(Tcl_Interp *interp, const char *textWidget, const char *banner)
{
    CHK(Tcl_VarEval(interp, textWidget, " insert end {", banner, "} event\n", NULL));
}

void LogBuffer::printLogLine(Tcl_Interp *interp, const char *textWidget, const char *line)
{
    CHK(Tcl_VarEval(interp, textWidget, " insert end {", line, "}\n", NULL));
}

void LogBuffer::printGlobalLogLine(Tcl_Interp *interp, const char *textWidget, const char *line)
{
    CHK(Tcl_VarEval(interp, textWidget, " insert end {", line, "} log\n", NULL));
}

void LogBuffer::seeEnd(Tcl_Interp *interp, const char *textWidget, const char *line)
{
    CHK(Tcl_VarEval(interp, textWidget, " see end", NULL));
}
*/
