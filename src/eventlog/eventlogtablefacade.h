//=========================================================================
//  EVENTLOGTABLEFACADE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTLOGTABLEFACADE_H_
#define __EVENTLOGTABLEFACADE_H_

#include "matchexpression.h"
#include "ievent.h"
#include "ieventlog.h"
#include "eventlogfacade.h"

NAMESPACE_BEGIN

enum EventLogTableFilterMode {
    ALL_ENTRIES,
    EVENT_AND_SEND_AND_MESSAGE_ENTRIES,
    EVENT_AND_MESSAGE_ENTRIES,
    EVENT_ENTRIES,
    CUSTOM_ENTRIES
};

/**
 * A class that makes it possible to extract info about events, without
 * returning objects. (Wherever a C++ method returns an object pointer,
 * SWIG-generated wrapper creates a corresponding Java object with the
 * pointer value inside. This has disastrous effect on performance
 * when dealing with huge amounts of data).
 */
class EVENTLOG_API EventLogTableFacade : public EventLogFacade
{
    protected:
        eventnumber_t approximateNumberOfEntries;
        eventnumber_t lastMatchedEventNumber;
        int lastNumMatchingEventLogEntries;
        EventLogTableFilterMode filterMode;
        std::string customFilter;
        MatchExpression matchExpression;

    public:
        EventLogTableFacade(IEventLog *eventLog);
        virtual ~EventLogTableFacade() {}

        virtual void synchronize(FileReader::FileChangedState change);

        EventLogTableFilterMode getFilterMode() { return filterMode; }
        void setFilterMode(EventLogTableFilterMode filterMode);
        bool matchesFilter(EventLogEntry *eventLogEntry);
        int getNumMatchingEventLogEntries(IEvent *event);
        void setCustomFilter(const char *pattern) { customFilter = pattern; matchExpression.setPattern((std::string("E or (") + customFilter + ")").c_str(), false, true, false); }
        const char *getCustomFilter() { return customFilter.c_str(); }

        EventLogEntry *getEventLogEntry(eventnumber_t eventNumber, int eventLogEntryIndex);
        EventLogEntry *getFirstEntry();
        EventLogEntry *getLastEntry();
        int getEntryIndexInEvent(EventLogEntry *eventLogEntry);
        EventLogEntry *getEntryInEvent(IEvent *event, int index);
        eventnumber_t getDistanceToEntry(EventLogEntry *sourceEventLogEntry, EventLogEntry *targetEventLogEntry, eventnumber_t limit);
        EventLogEntry *getClosestEntryInEvent(EventLogEntry *eventLogEntry);
        eventnumber_t getDistanceToFirstEntry(EventLogEntry *eventLogEntry, eventnumber_t limit);
        eventnumber_t getDistanceToLastEntry(EventLogEntry *eventLogEntry, eventnumber_t limit);
        EventLogEntry *getNeighbourEntry(EventLogEntry *eventLogEntry, eventnumber_t distance);
        double getApproximatePercentageForEntry(EventLogEntry *eventLogEntry);
        EventLogEntry *getApproximateEventLogEntryTableAt(double percentage);
        eventnumber_t getApproximateNumberOfEntries();

    protected:
        EventLogEntry *getPreviousEntry(EventLogEntry *eventLogEntry, int& index);
        EventLogEntry *getNextEntry(EventLogEntry *eventLogEntry, int& index);
        EventLogEntry *getEntryAndDistance(EventLogEntry *sourceEventLogEntry, EventLogEntry *targetEventLogEntry, eventnumber_t distance, eventnumber_t& reachedDistance);
};

NAMESPACE_END


#endif
