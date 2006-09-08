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

// all the event log entries for a single event
class Event
{
    protected:
       EventEntry *eventEntry;

       typedef std::vector<EventLogEntry *> EventLogEntryList;
       EventLogEntryList eventLogEntries;

       int numLogMessages; // total number of log messages for this event (sum of its causes[]' log messages)

    public:
       Event();
       long eventNumber();
       long parse(FileReader *index, long offset);
       void print(FILE *file);
};

#endif