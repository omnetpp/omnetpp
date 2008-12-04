//==========================================================================
//  LOGBUFFER.H - part of
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

#ifndef __LOGBUFFER_H
#define __LOGBUFFER_H

#include <string>
#include <list>
#include <vector>
#include <set>
#include "tkutil.h"

NAMESPACE_BEGIN


/**
 * Stores textual debug output from modules.
 */
class LogBuffer
{
  public:
    struct Entry {
        eventnumber_t eventNumber;
        simtime_t simtime;
        int moduleId;
        const char *banner;
        std::vector<const char *> lines;
        int numChars; // banner plus lines

        Entry();
        Entry(eventnumber_t e, simtime_t t, int modId, const char *bannerText);
        ~Entry();
    };

  protected:
    size_t memLimit;
    size_t totalChars;
    size_t totalStrings;
    std::list<Entry> entries;

  protected:
    void discardIfMemoryLimitExceeded();
    size_t estimatedMemUsage() {return totalChars + 8*totalStrings + entries.size()*(8+2*sizeof(void*)+sizeof(Entry)); }

  public:
    LogBuffer(int memLimit=10*1024*1024);  // 10MB
    ~LogBuffer();

    void addEvent(eventnumber_t e, simtime_t t, int moduleId, const char *banner);
    void addLogLine(const char *buffer, int n);
    void addInfo(const char *text);

    void setMemoryLimit(size_t limit);
    size_t getMemoryLimit()  {return memLimit;}

    const std::list<Entry>& getEntries() const {return entries;}
};

NAMESPACE_END


#endif


