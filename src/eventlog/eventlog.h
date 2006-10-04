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

#include <sstream>
#include <set>
#include <map>
#include "stringpool.h"
#include "filereader.h"
#include "event.h"
#include "ieventlog.h"
#include "eventlogindex.h"

extern StringPool eventLogStringPool;

class Event;
class EventLogEntry;

/**
 * Manages an event log file in memory. Caches some events. Clients should not
 * store pointers to Events or EventLogEntries, because this class may
 * thow them out of the cache any time.
 */
class EventLog : public IEventLog, public EventLogIndex
{
    protected:
        long numEventsApproximation;

        typedef std::vector<EventLogEntry *> EventLogEntryList;
        EventLogEntryList initializationLogEntries; // all entries from the beginning of the file to the first event

        typedef std::map<int, ModuleCreatedEntry *> ModuleIdToModuleCreatedEntryMap;
        ModuleIdToModuleCreatedEntryMap initializationModuleIdToModuleCreatedEntryMap;

        typedef std::map<long, Event *> EventNumberToEventMap;
        EventNumberToEventMap eventNumberToEventMap; // all parsed events so far

        typedef std::map<long, Event *> OffsetToEventMap;
        OffsetToEventMap offsetToEventMap; // all parsed events so far

    public:
        EventLog(FileReader *index);
        ~EventLog();

        void parseInitializationLogEntries();

        Event *getEventForBeginOffset(long offset);
        Event *getEventForEndOffset(long offset);

        // IEventLog interface
        virtual ModuleCreatedEntry *getModuleCreatedEntry(int moduleId) { return initializationModuleIdToModuleCreatedEntryMap[moduleId]; }
        virtual int getNumModuleCreatedEntries() { return initializationModuleIdToModuleCreatedEntryMap.size(); }

        virtual Event *getFirstEvent() { return getEventForEventNumber(getFirstEventNumber()); }
        virtual Event *getLastEvent() { return getEventForEventNumber(getLastEventNumber()); }
        virtual Event *getEventForEventNumber(long eventNumber, MatchKind matchKind = EXACT);
        virtual Event *getEventForSimulationTime(simtime_t simulationTime, MatchKind matchKind = EXACT);

        virtual long getNumEventsApproximation();

        virtual void printInitializationLogEntries(FILE *file = stdout);

    protected:
        Event *cacheEvent(Event *event);
};

#endif
