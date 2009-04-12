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

NAMESPACE_BEGIN

class cModule;

/**
 * Stores textual debug output from modules.
 */
class LogBuffer
{
  public:
    class IListener {
      public:
        /** Called when lines get appended at the end of the buffer */
        virtual void linesAdded(size_t numLines, size_t numChars) = 0;
        /** Called when old lines get discarded at the beginning of the buffer */
        virtual void linesDiscarded(size_t numLines, size_t numChars) = 0;
        /** Virtual dtor */
        virtual ~IListener() {}
    };

    // line type
    enum { LINE_INFO = 0, LINE_BANNER = 1, LINE_LOG = 2 };

    // log entry
    class Entry {
      private:
        friend class LogBuffer;
        eventnumber_t eventNumber;
        simtime_t simtime;
        int *moduleIds;  // from this module up to the root; zero-terminated; NULL for info messages
        const char *banner;
        std::vector<const char *> lines;
        int numChars; // banner plus lines  FIXME should include newlines that are NOT stored in the strings!!!!!

      public:
        Entry() {eventNumber=0; simtime=0; moduleIds=NULL; banner=NULL; numChars=0;}
        ~Entry();
        eventnumber_t getEventNumber() const {return eventNumber;}
        simtime_t getSimtime() const {return simtime;}
        int getModuleId() const {return !moduleIds ? 0 : moduleIds[0];}
        bool isFromTreeOf(int moduleId) const;
        size_t getNumLines() const {return 1+lines.size();} //XXX if banner!=NULL
        size_t getNumChars() const {return numChars;} // incl. newlines
        size_t getLineLength(size_t lineIndex) const {return strlen(getLine(lineIndex))+1;} // incl. newline
        const char *getLine(size_t lineIndex) const {return lineIndex==0 ? banner : lines[lineIndex-1];}
        int getLineType(size_t lineIndex) const {return !moduleIds ? LINE_INFO : lineIndex==0 ? LINE_BANNER : LINE_LOG;}
    };

  protected:
    size_t memLimit;
    size_t totalChars;
    size_t totalLines;
    std::list<Entry> entries;
    size_t numEntries;  // gcc's list::size() is O(n)...
    std::vector<IListener*> listeners;

  protected:
    void discardIfMemoryLimitExceeded();
    size_t estimatedMemUsage() {return totalChars + 8*totalLines + numEntries*(8+2*sizeof(void*)+sizeof(Entry)+32); }
    void fillEntry(Entry& entry, eventnumber_t e, simtime_t t, int *moduleIds, const char *banner);
    void linesAdded(size_t numLines, size_t numChars);
    void linesDiscarded(size_t numLines, size_t numChars);
    static int *extractAncestorModuleIds(cModule *mod);

  public:
    LogBuffer(int memLimit=10*1024*1024);  // 10MB
    ~LogBuffer();

    void addListener(IListener *listener);
    void removeListener(IListener *listener);

    void addEvent(eventnumber_t e, simtime_t t, int *moduleIds, const char *banner);
    void addEvent(eventnumber_t e, simtime_t t, cModule *mod, const char *banner);
    void addLogLine(const char *text);
    void addInfo(const char *text);

    void setMemoryLimit(size_t limit);
    size_t getMemoryLimit()  {return memLimit;}

    const std::list<Entry>& getEntries() const {return entries;}
    size_t getNumEntries() const {return numEntries;}

    void dump() const;
};

NAMESPACE_END

#endif


