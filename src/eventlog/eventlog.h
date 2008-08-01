//=========================================================================
//  EVENTLOG.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

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
        eventnumber_t numParsedEvents;
        eventnumber_t approximateNumberOfEvents;

        long progressCallInterval;
        long lastProgressCall; // as returned by clock()
        ProgressMonitor progressMonitor;

        Event *firstEvent;
        Event *lastEvent;

        typedef std::vector<EventLogEntry *> EventLogEntryList;
        EventLogEntryList initializationLogEntries; // all entries from the beginning of the file to the first event

        SimulationBeginEntry *simulationBeginEntry;

        typedef std::map<int, ModuleCreatedEntry *> ModuleIdToModuleCreatedEntryMap;
        ModuleIdToModuleCreatedEntryMap moduleIdToModuleCreatedEntryMap;

        typedef std::map<std::pair<int, int>, GateCreatedEntry *> ModuleIdAndGateIdToGateCreatedEntryMap;
        ModuleIdAndGateIdToGateCreatedEntryMap moduleIdAndGateIdToGateCreatedEntryMap;

        std::set<const char *> messageClassNames; // message class names seen so far (see Event::parse)
        std::set<const char *> messageNames; // message names seen so far (see Event::parse)

        typedef std::map<eventnumber_t, Event *> EventNumberToEventMap;
        EventNumberToEventMap eventNumberToEventMap; // all parsed events so far

        typedef std::map<file_offset_t, Event *> OffsetToEventMap;
        OffsetToEventMap beginOffsetToEventMap; // all parsed events so far
        OffsetToEventMap endOffsetToEventMap; // all parsed events so far

    public:
        EventLog(FileReader *index);
        ~EventLog();

        virtual ProgressMonitor setProgressMonitor(ProgressMonitor newProgressMonitor);
        virtual void setProgressCallInterval(double seconds) { progressCallInterval = (long)(seconds * CLOCKS_PER_SEC); lastProgressCall = clock(); }
        virtual void progress();

        void parseInitializationLogEntries();

        /**
         * Returns the event exactly starting at the given offset or NULL if there's no such event.
         */
        Event *getEventForBeginOffset(file_offset_t offset);
        /**
         * Returns the event exactly ending at the given offset or NULL if there's no such event.
         */
        Event *getEventForEndOffset(file_offset_t offset);

        // IEventLog interface
        virtual void synchronize(FileReader::FileChangedState change);
        virtual FileReader *getFileReader() { return reader; }
        virtual eventnumber_t getNumParsedEvents() { return numParsedEvents; }
        virtual std::set<const char *>& getMessageNames() { return messageNames; }
        virtual std::set<const char *>& getMessageClassNames() { return messageClassNames; }
        virtual int getNumModuleCreatedEntries() { return moduleIdToModuleCreatedEntryMap.size(); }
        virtual std::vector<ModuleCreatedEntry *> getModuleCreatedEntries();
        virtual ModuleCreatedEntry *getModuleCreatedEntry(int moduleId) { return moduleIdToModuleCreatedEntryMap[moduleId]; }
        virtual GateCreatedEntry *getGateCreatedEntry(int moduleId, int gateId) { std::pair<int, int> key(moduleId, gateId); return moduleIdAndGateIdToGateCreatedEntryMap[key]; }
        virtual SimulationBeginEntry *getSimulationBeginEntry() { return simulationBeginEntry; }

        virtual Event *getFirstEvent();
        virtual Event *getLastEvent();
        virtual Event *getNeighbourEvent(IEvent *event, eventnumber_t distance = 1);
        virtual Event *getEventForEventNumber(eventnumber_t eventNumber, MatchKind matchKind = EXACT);
        virtual Event *getEventForSimulationTime(simtime_t simulationTime, MatchKind matchKind = EXACT);

        virtual EventLogEntry *findEventLogEntry(EventLogEntry *start, const char *search, bool forward, bool caseSensitive);

        virtual eventnumber_t getApproximateNumberOfEvents();
        virtual Event *getApproximateEventAt(double percentage);

        virtual int getNumInitializationLogEntries() { return initializationLogEntries.size(); }
        virtual EventLogEntry *getInitializationLogEntry(int index) { return initializationLogEntries[index]; }
        virtual void printInitializationLogEntries(FILE *file = stdout);

    protected:
        void parseEvent(Event *event, file_offset_t beginOffset);
        void cacheEvent(Event *event);
        void cacheEventLogEntries(Event *event);
        void uncacheEventLogEntries(Event *event);
        void cacheEventLogEntry(EventLogEntry *eventLogEntry);
        void uncacheEventLogEntry(EventLogEntry *eventLogEntry);
        void deleteAllocatedObjects();
        void clearInternalState(FileReader::FileChangedState change = FileReader::OVERWRITTEN);
};

NAMESPACE_END


#endif
