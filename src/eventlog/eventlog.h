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
#include "eventlogindex.h"

extern StringPool eventLogStringPool;

class Event;
class EventLogEntry;

/**
 * Manages an event log file in memory.
 */
class EventLog : public EventLogIndex
{
    protected:
        typedef std::vector<EventLogEntry *> EventLogEntryList;
        EventLogEntryList initializationLogEntries; // all entries from the beginning of the file to the first event

        typedef std::map<long, Event *> EventNumberToEventMap;
        EventNumberToEventMap eventNumberToEventMap; // all parsed events so far

        typedef std::map<long, Event *> OffsetToEventMap;
        OffsetToEventMap offsetToEventMap; // all parsed events so far

    public:
        EventLog(FileReader *index);
        ~EventLog();

        void parseInitializationLogEntries();
        void parse(long fromEventNumber, long toEventNumber);

        void printInitializationLogEntries(FILE *file);
        void printEvents(FILE *file);
        void print(FILE *file);

        Event *getEventForEventNumber(long eventNumber);
        Event *getEventForSimulationTime(simtime_t simulationTime);
        Event *getEventForOffset(long offset);

    protected:
        Event *cacheEvent(Event *event);
};

#endif