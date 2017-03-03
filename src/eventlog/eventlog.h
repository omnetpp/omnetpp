//=========================================================================
//  EVENTLOG.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_EVENTLOG_EVENTLOG_H
#define __OMNETPP_EVENTLOG_EVENTLOG_H

#include <ctime>
#include <sstream>
#include <set>
#include <map>
#include "common/stringpool.h"
#include "common/filereader.h"
#include "event.h"
#include "ieventlog.h"
#include "eventlogindex.h"

namespace omnetpp {
namespace eventlog {

extern EVENTLOG_API omnetpp::common::StringPool eventLogStringPool;

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

        SimulationBeginEntry *simulationBeginEntry;
        SimulationEndEntry *simulationEndEntry;

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

        int keyframeBlockSize;
        std::vector<eventnumber_t> consequenceLookaheadLimits;
        std::map<eventnumber_t, std::vector<MessageEntry *> > previousEventNumberToMessageEntriesMap;

    public:
        EventLog(FileReader *index);
        virtual ~EventLog();

        virtual ProgressMonitor setProgressMonitor(ProgressMonitor newProgressMonitor) override;
        virtual void setProgressCallInterval(double seconds) override { progressCallInterval = (long)(seconds * CLOCKS_PER_SEC); lastProgressCall = clock(); }
        virtual void progress() override;

        int getKeyframeBlockSize() override { return keyframeBlockSize; }
        eventnumber_t getConsequenceLookahead(eventnumber_t eventNumber) { return consequenceLookaheadLimits[eventNumber / keyframeBlockSize]; }
        std::vector<MessageEntry *> getMessageEntriesWithPreviousEventNumber(eventnumber_t eventNumber);

        /**
         * Returns the event exactly starting at the given offset or nullptr if there is no such event.
         */
        Event *getEventForBeginOffset(file_offset_t offset);
        /**
         * Returns the event exactly ending at the given offset or nullptr if there is no such event.
         */
        Event *getEventForEndOffset(file_offset_t offset);

        // IEventLog interface
        virtual void synchronize(FileReader::FileChangedState change) override;
        virtual FileReader *getFileReader() override { return reader; }
        virtual eventnumber_t getNumParsedEvents() override { return numParsedEvents; }
        virtual std::set<const char *>& getMessageNames() override { return messageNames; }
        virtual std::set<const char *>& getMessageClassNames() override { return messageClassNames; }
        virtual int getNumModuleCreatedEntries() override { return moduleIdToModuleCreatedEntryMap.size(); }
        virtual std::vector<ModuleCreatedEntry *> getModuleCreatedEntries() override;
        virtual ModuleCreatedEntry *getModuleCreatedEntry(int moduleId) override;
        virtual GateCreatedEntry *getGateCreatedEntry(int moduleId, int gateId) override;
        virtual SimulationBeginEntry *getSimulationBeginEntry() override { getFirstEvent(); return simulationBeginEntry; }
        virtual SimulationEndEntry *getSimulationEndEntry() { getLastEvent(); return simulationEndEntry; }

        virtual Event *getFirstEvent() override;
        virtual Event *getLastEvent() override;
        virtual Event *getNeighbourEvent(IEvent *event, eventnumber_t distance = 1) override;
        virtual Event *getEventForEventNumber(eventnumber_t eventNumber, MatchKind matchKind = EXACT, bool useCacheOnly = false) override;
        virtual Event *getEventForSimulationTime(simtime_t simulationTime, MatchKind matchKind = EXACT, bool useCacheOnly = false) override;

        virtual EventLogEntry *findEventLogEntry(EventLogEntry *start, const char *search, bool forward, bool caseSensitive) override;

        virtual eventnumber_t getApproximateNumberOfEvents() override;
        virtual Event *getApproximateEventAt(double percentage) override;

        virtual void print(FILE *file = stdout, eventnumber_t fromEventNumber = -1, eventnumber_t toEventNumber = -1, bool outputEventLogMessages = true) override;

    protected:
        void parseEvent(Event *event, file_offset_t beginOffset);
        void cacheEvent(Event *event);
        void cacheEventLogEntries(Event *event);
        void uncacheEventLogEntries(Event *event);
        void cacheEventLogEntry(EventLogEntry *eventLogEntry);
        void uncacheEventLogEntry(EventLogEntry *eventLogEntry);
        void clearInternalState();
        void deleteAllocatedObjects();
        void parseKeyframes();
};

} // namespace eventlog
}  // namespace omnetpp


#endif
