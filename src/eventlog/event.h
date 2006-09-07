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

#include <vector>
#include "EventLogEntry.h"

// event log entries for a single event
class Event
{
    protected:
       typedef std::vector<EventLogEntry *> EventLogEntryList;
       EventLogEntryList eventLogEntries;

       int numLogMessages; // total number of log messages for this event (sum of its causes[]' log messages)

    public:
       Event();
};

#endif