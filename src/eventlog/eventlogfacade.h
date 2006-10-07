//=========================================================================
//  EVENTLOGFACADE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTLOGFACADE_H_
#define __EVENTLOGFACADE_H_

#include "ievent.h"
#include "ieventlog.h"

/**
 * A class that makes it possible to extract info about events, without
 * returning objects. (Wherever a C++ method returns an object pointer,
 * SWIG-generated wrapper creates a corresponding Java object with the
 * pointer value inside. This has disastrous effect on performance
 * when dealing with huge amounts of data).
 *
 * IMPORTANT! Java code MUST NOT ask members of objects that don't exist!
 * Otherwise there will be a crash. For example, if getEvent_i_hasCause(i)
 * returns false, then getEvent_i_cause_messageClassName(i) will CRASH!
 */
class EventLogFacade
{
    protected:
        IEventLog *eventLog;
        long approximateNumberOfEventLogTableEntries;

    public:
        EventLogFacade(IEventLog *eventLog);

        IEvent *getNeighbourEvent(IEvent *event, long distance = 1);

        // EventLogTable interface
		EventLogEntry *getFirstEventLogTableEntry();
		EventLogEntry *getLastEventLogTableEntry();
        EventLogEntry *getPreviousEventLogTableEntry(EventLogEntry *eventLogEntry, int& index);
        EventLogEntry *getNextEventLogTableEntry(EventLogEntry *eventLogEntry, int& index);
        int getEventLogTableEntryIndexInEvent(EventLogEntry *eventLogEntry);
        EventLogEntry *getEventLogTableEntryInEvent(IEvent *event, int index);
        EventLogEntry *getEventLogTableEntryAndDistance(EventLogEntry *sourceEventLogEntry, EventLogEntry *targetEventLogEntry, long distance, long& reachedDistance);
		long getDistanceToEventLogTableEntry(EventLogEntry *sourceEventLogEntry, EventLogEntry *targetEventLogEntry, long limit);
		long getDistanceToFirstEventLogTableEntry(EventLogEntry *eventLogEntry, long limit);
		long getDistanceToLastEventLogTableEntry(EventLogEntry *eventLogEntry, long limit);
		EventLogEntry *getNeighbourEventLogTableEntry(EventLogEntry *eventLogEntry, long distance);
        double getApproximatePercentageForEventLogTableEntry(EventLogEntry *eventLogEntry);
		EventLogEntry *getApproximateEventLogEntryTableAt(double percentage);
		long getApproximateNumberOfEventLogTableEntries();
};

#endif