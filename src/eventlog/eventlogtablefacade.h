//=========================================================================
//  EVENTLOGTABLEFACADE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_EVENTLOG_EVENTLOGTABLEFACADE_H
#define __OMNETPP_EVENTLOG_EVENTLOGTABLEFACADE_H

#include "common/matchexpression.h"
#include "ievent.h"
#include "ieventlog.h"
#include "eventlogfacade.h"

namespace omnetpp {
namespace eventlog {

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
    typedef omnetpp::common::MatchExpression MatchExpression;
    protected:
        eventnumber_t approximateNumberOfEntries; // cached value, -1 means not yet calculated
        eventnumber_t lastMatchedEventNumber; // -1 means unused
        int lastNumMatchingEventLogEntries; // -1 means unused
        EventLogTableFilterMode filterMode; // can be set with the public API
        std::string customFilter; // can be set with the public API
        MatchExpression matchExpression; // cached expression

    public:
        EventLogTableFacade(IEventLog *eventLog);
        virtual ~EventLogTableFacade() {}

        virtual void synchronize(FileReader::FileChangedState change) override;

        EventLogTableFilterMode getFilterMode() { return filterMode; }
        void setFilterMode(EventLogTableFilterMode filterMode);
        bool matchesFilter(EventLogEntry *eventLogEntry);
        int getNumMatchingEventLogEntries(IEvent *event);
        void setCustomFilter(const char *pattern);
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
        EventLogEntry *getApproximateEventLogEntryAt(double percentage);
        eventnumber_t getApproximateNumberOfEntries();

    protected:
        EventLogEntry *getPreviousEntry(EventLogEntry *eventLogEntry, int& index);
        EventLogEntry *getNextEntry(EventLogEntry *eventLogEntry, int& index);
        EventLogEntry *getEntryAndDistance(EventLogEntry *sourceEventLogEntry, EventLogEntry *targetEventLogEntry, eventnumber_t distance, eventnumber_t& reachedDistance);
        void clearInternalState();
};

} // namespace eventlog
}  // namespace omnetpp


#endif
