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

class cModule;

/**
 * Stores textual debug output from modules.
 */
class LogBuffer
{
  public:
    struct Entry {
        eventnumber_t eventNumber;
        simtime_t simtime;
        int *moduleIds;  // from this module up to the root; zero-terminated; NULL for info messages
        const char *banner;
        std::vector<const char *> lines;
        int numChars; // banner plus lines

        Entry() {eventNumber=0; simtime=0; moduleIds=NULL; banner=NULL; numChars=0;}
        ~Entry();
    };

  protected:
    size_t memLimit;
    size_t totalChars;
    size_t totalStrings;
    std::list<Entry> entries;

  protected:
    void discardIfMemoryLimitExceeded();
    size_t estimatedMemUsage() {return totalChars + 8*totalStrings + entries.size()*(8+2*sizeof(void*)+sizeof(Entry)+32); }
    void fillEntry(Entry& entry, eventnumber_t e, simtime_t t, cModule *mod, const char *banner);

  public:
    LogBuffer(int memLimit=10*1024*1024);  // 10MB
    ~LogBuffer();

    void addEvent(eventnumber_t e, simtime_t t, cModule *moduleIds, const char *banner);
    void addLogLine(const char *text);
    void addInfo(const char *text);

    void setMemoryLimit(size_t limit);
    size_t getMemoryLimit()  {return memLimit;}

    const std::list<Entry>& getEntries() const {return entries;}

    void dump() const;
};

NAMESPACE_END


#endif


