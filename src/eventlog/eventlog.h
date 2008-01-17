//=========================================================================
//  EVENTLOG.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTLOG_H_
#define __EVENTLOG_H_

#include <time.h>
#include <sstream>
#include <set>
#include <map>
#include "stringpool.h"
#include "filereader.h"
#include "event.h"
#include "ieventlog.h"
#include "eventlogindex.h"

NAMESPACE_BEGIN

extern EVENTLOG_API CommonStringPool eventLogStringPool;

class Event;
class EventLogEntry;

/**
 * Manages an event log file in memory. Caches some events. Clients should not
 * store pointers to Events or EventLogEntries, because this class may
 * thow them out of the cache any time.
 */
class EVENTLOG_API EventLog : public IEventLog, public EventLogIndex
{
    protected:
        long numParsedEvents;
        long approximateNumberOfEvents;

        long progressCallInterval;
        long lastProgressCall; // as returned by clock()
        ProgressMonitor progressMonitor;

        Event *firstEvent;
        Event *lastEvent;

        typedef std::vector<EventLogEntry *> EventLogEntryList;
        EventLogEntryList initializationLogEntries; // all entries from the beginning of the file to the first event

        SimulationBeginEntry *simulationBeginEntry;

        typedef std::map<int, ModuleCreatedEntry *> ModuleIdToModuleCreatedEntryMap;
        ModuleIdToModuleCreatedEntryMap initializationModuleIdToModuleCreatedEntryMap;

        std::set<const char *> messageClassNames; // message class names seen so far (see Event::parse)
        std::set<const char *> messageNames; // message names seen so far (see Event::parse)

        typedef std::map<long, Event *> EventNumberToEventMap;
        EventNumberToEventMap eventNumberToEventMap; // all parsed events so far

        typedef std::map<file_offset_t, Event *> OffsetToEventMap;
        OffsetToEventMap offsetToEventMap; // all parsed events so far

    public:
        EventLog(FileReader *index);
        ~EventLog();

        virtual ProgressMonitor setProgressMonitor(ProgressMonitor newProgressMonitor);
        virtual void setProgressCallInterval(double seconds) { progressCallInterval = (long)(seconds * CLOCKS_PER_SEC); lastProgressCall = clock(); }
        void progress(); // notify monitor

        void parseInitializationLogEntries();

        /**
         * Returns the event exactly starting at the given offset or NULL if there's no such event.
         */
        Event *getEventForBeginOffset(file_offset_t offset);
        /**
         * Returns the event exactly ending at the given offset or NULL if there's no such event.
         */
        Event *getEventForEndOffset(file_offset_t offset);

        void putMessageName(const char *messageName) { messageNames.insert(messageName); }
        void putMessageClassName(const char *messageClassName) { messageClassNames.insert(messageClassName); }

        // IEventLog interface
        virtual void synchronize();
        virtual FileReader *getFileReader() { return reader; }
        virtual long getNumParsedEvents() { return numParsedEvents; }
        virtual std::set<const char *>& getMessageNames() { return messageNames; }
        virtual std::set<const char *>& getMessageClassNames() { return messageClassNames; }
        virtual ModuleCreatedEntry *getModuleCreatedEntry(int moduleId) { return initializationModuleIdToModuleCreatedEntryMap[moduleId]; }
        virtual int getNumModuleCreatedEntries() { return initializationModuleIdToModuleCreatedEntryMap.size(); }
        virtual SimulationBeginEntry *getSimulationBeginEntry() { return simulationBeginEntry; }

        virtual Event *getFirstEvent();
        virtual Event *getLastEvent();
        virtual Event *getNeighbourEvent(IEvent *event, long distance = 1);
        virtual Event *getEventForEventNumber(long eventNumber, MatchKind matchKind = EXACT);
        virtual Event *getEventForSimulationTime(simtime_t simulationTime, MatchKind matchKind = EXACT);

        virtual EventLogEntry *findEventLogEntry(EventLogEntry *start, const char *search, bool forward);

        virtual long getApproximateNumberOfEvents();
        virtual Event *getApproximateEventAt(double percentage);

        virtual void printInitializationLogEntries(FILE *file = stdout);

    protected:
        Event *cacheEvent(Event *event);
        void deleteAllocatedObjects();
        void clearInternalState(FileReader::FileChangedState change = FileReader::OVERWRITTEN);
};

NAMESPACE_END


#endif
