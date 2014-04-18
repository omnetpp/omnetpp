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
#include "simtime_t.h"
#include "tkutil.h"

NAMESPACE_BEGIN

class cModule;
class LogBuffer;

class ILogBufferListener
{
  public:
    virtual ~ILogBufferListener() {}
    virtual void logEntryAdded() = 0;
    virtual void logLineAdded() = 0;
    virtual void messageSendAdded() = 0;
};

/**
 * Stores textual debug output from modules.
 */
class TKENV_API LogBuffer
{
  public:
    struct Line {
        const char *prefix;
        const char *line;  // including newline
        Line(const char *prefix, const char *line) : prefix(prefix), line(line) {}
    };
    struct MessageSend {
        cMessage *msg;
        //TODO instead of hopModuleIds: int srcModuleId, int destModuleId, int* intermediateModuleIds
        //TODO also: txStartTime, propagationDelay, duration for each hop
        std::vector<int> hopModuleIds;
    };
    struct Entry {
        eventnumber_t eventNumber;
        simtime_t simtime;
        int *moduleIds;  // from this module up to the root; zero-terminated; NULL for info messages
        const char *banner;
        std::vector<Line> lines;
        int numChars; // banner plus lines

        std::vector<MessageSend> msgs;

        Entry() {eventNumber=0; simtime=0; moduleIds=NULL; banner=NULL; numChars=0;}
        ~Entry();
    };

  protected:
    std::vector<ILogBufferListener*> listeners;

    size_t memLimit;  //TODO currently the stored msg lines are not counted!!!!
    size_t totalChars;
    size_t totalStrings;
    std::list<Entry> entries;
    size_t numEntries;  // gcc's list::size() is O(n)...

  protected:
    void discardIfMemoryLimitExceeded();
    size_t estimatedMemUsage() {return totalChars + 8*totalStrings + numEntries*(8+2*sizeof(void*)+sizeof(Entry)+32); }
    void fillEntry(Entry& entry, eventnumber_t e, simtime_t t, cModule *mod, const char *banner);

  public:
    LogBuffer(int memLimit=10*1024*1024);  // 10MB
    ~LogBuffer();

    void addListener(ILogBufferListener *l);
    void removeListener(ILogBufferListener *l);

    void addEvent(eventnumber_t e, simtime_t t, cModule *moduleIds, const char *banner);
    void addLogLine(const char *prefix, const char *text);
    void addInfo(const char *text);

    void beginSend(cMessage *msg);
    void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay);
    void messageSendHop(cMessage *msg, cGate *srcGate);
    void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay);
    void endSend(cMessage *msg);

    void setMemoryLimit(size_t limit);
    size_t getMemoryLimit()  {return memLimit;}

    const std::list<Entry>& getEntries() const {return entries;}
    size_t getNumEntries() const {return numEntries;}

    void clear();

    void dump() const;
};

NAMESPACE_END


#endif


