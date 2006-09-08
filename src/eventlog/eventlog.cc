//=========================================================================
//  EVENTLOG.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>
#include "filereader.h"
#include "event.h"
#include "eventlog.h"

EventLog::EventLog(FileReader *reader) : EventLogIndex(reader)
{
}

void EventLog::parse(long fromEventNumber, long toEventNumber)
{
    long fromOffset = getOffsetForEventNumber(fromEventNumber);
    long toOffset = getOffsetForEventNumber(toEventNumber);
    long offset = fromOffset;

    eventNumberToEventMap.clear();

    while (offset < toOffset)
    {
        Event *event = new Event();
        offset = event->parse(reader, offset);
        eventNumberToEventMap[event->eventNumber()] = *event;
    }
}

void EventLog::print(FILE *file)
{
    for (EventNumberToEventMap::iterator it = eventNumberToEventMap.begin(); it != eventNumberToEventMap.end(); it++)
    {
        it->second.print(file);
    }
}
