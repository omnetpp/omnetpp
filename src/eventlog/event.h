//=========================================================================
//  EVENT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENT_H_
#define __EVENT_H_

#include <sstream>
#include <vector>
#include "filereader.h"
#include "eventlogentry.h"
#include "eventlogentries.h"

class EventLog;

/**
 * Manages all event log entries for a single event.
 */
class Event
{
    public:
        typedef std::vector<Event *> EventList;

    protected:
        EventLog *eventLog;
        EventEntry *eventEntry; // the event log entry that corresponds to the actual event

        typedef std::vector<EventLogEntry *> EventLogEntryList;
        EventLogEntryList eventLogEntries;

    public:
        Event(EventLog *eventLog);
        ~Event();

        EventEntry *getEventEntry() { return eventEntry; };
        long getEventNumber() { return eventEntry->eventNumber; };

        Event *getCause();
        EventList *getCauses();
        EventList *getConsequences();

        long parse(FileReader *index, long offset);
        void print(FILE *file);
};

#endif