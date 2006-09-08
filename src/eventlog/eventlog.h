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

/**
 * Manages an event log file in memory.
 */
class EventLog : public EventLogIndex
{
    protected:
        typedef std::vector<EventLogEntry *> EventLogEntryList;
        EventLogEntryList initializationLogEntries;

        typedef std::map<long, Event> EventNumberToEventMap;
        EventNumberToEventMap eventNumberToEventMap;

    public:
        EventLog(FileReader *index);

        void parseInitializationLogEntries();
        void parse(long fromEventNumber, long toEventNumber);

        void printInitializationLogEntries(FILE *file);
        void printEvents(FILE *file);
        void print(FILE *file);

        Event *getEvent(long eventNumber);
};

#endif