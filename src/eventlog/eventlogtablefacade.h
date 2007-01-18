//=========================================================================
//  EVENTLOGTABLEFACADE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTLOGTABLEFACADE_H_
#define __EVENTLOGTABLEFACADE_H_

#include "ievent.h"
#include "ieventlog.h"
#include "eventlogfacade.h"

/**
 * A class that makes it possible to extract info about events, without
 * returning objects. (Wherever a C++ method returns an object pointer,
 * SWIG-generated wrapper creates a corresponding Java object with the
 * pointer value inside. This has disastrous effect on performance
 * when dealing with huge amounts of data).
 */
class EventLogTableFacade : EventLogFacade
{
    protected:
        long approximateNumberOfEntries;

    public:
        EventLogTableFacade(IEventLog *eventLog);
        virtual ~EventLogTableFacade() {}

		EventLogEntry *getFirstEntry();
		EventLogEntry *getLastEntry();
        int getEntryIndexInEvent(EventLogEntry *eventLogEntry);
        EventLogEntry *getEntryInEvent(IEvent *event, int index);
		long getDistanceToEntry(EventLogEntry *sourceEventLogEntry, EventLogEntry *targetEventLogEntry, long limit);
		long getDistanceToFirstEntry(EventLogEntry *eventLogEntry, long limit);
		long getDistanceToLastEntry(EventLogEntry *eventLogEntry, long limit);
		EventLogEntry *getNeighbourEntry(EventLogEntry *eventLogEntry, long distance);
        double getApproximatePercentageForEntry(EventLogEntry *eventLogEntry);
		EventLogEntry *getApproximateEventLogEntryTableAt(double percentage);
		long getApproximateNumberOfEntries();

    protected:
        EventLogEntry *getPreviousEntry(EventLogEntry *eventLogEntry, int& index);
        EventLogEntry *getNextEntry(EventLogEntry *eventLogEntry, int& index);
        EventLogEntry *getEntryAndDistance(EventLogEntry *sourceEventLogEntry, EventLogEntry *targetEventLogEntry, long distance, long& reachedDistance);
};

#endif