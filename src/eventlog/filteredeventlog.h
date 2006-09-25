//=========================================================================
//  FILTEREDEVENTLOG.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FILTEREDEVENTLOG_H_
#define __FILTEREDEVENTLOG_H_

#include <sstream>
#include "eventlogdefs.h"
#include "eventlog.h"
#include "filteredevent.h"

/**
 * This is a "view" of the EventLog, showing only a subset of events and relationships
 */
class FilteredEventLog
{
    protected:
        EventLog *eventLog;

        // filter parameters
        long tracedEventNumber; // the event number from which causes and consequences are followed or -1
        long firstEventNumber; // the first event to be considered by the filter or -1
        long lastEventNumber; // the last event to be considered by the filter or -1
        std::set<int> *includeModuleIds; // events outside these modules will be filtered out, NULL means include all
        bool includeCauses; // only when tracedEventNumber is given, includes events which cause the traced event even if through a chain of filtered events
        bool includeConsequences; // only when tracedEventNumber is given
        int maxCauseDepth; // maximum number of message dependencies considered when collecting causes
        int maxConsequenceDepth; // maximum number of message dependencies considered when collecting consequences

        // state
        typedef std::map<long, FilteredEvent *> EventNumberToFilteredEventMap;
        EventNumberToFilteredEventMap eventNumberToFilteredEventMap;

        typedef std::map<long, bool> EventNumberToFilterMatchesMap;
        EventNumberToFilterMatchesMap eventNumberToFilterMatchesMap; // a cache of whether the given event number matches the filter or not

        long firstMatchingEventNumber; // the event number of the first matching event
        long lastMatchingEventNumber; // the event number of the last matching event

    public:
        FilteredEventLog(EventLog *eventLog,
                         std::set<int> *includeModuleIds,
                         long tracedEventNumber = -1,
                         bool includeCauses = false,
                         bool includeConsequences = false,
                         long firstEventNumber = -1,
                         long lastEventNumber = -1);
        ~FilteredEventLog();

    public:
        EventLog *getEventLog() { return eventLog; };
        FilteredEvent* getFilteredEvent(long eventNumber);
        int getMaxCauseDepth() { return maxCauseDepth; };
        int getMaxConsequenceDepth() { return maxConsequenceDepth; };

        FilteredEvent* getFirstFilteredEvent();
        FilteredEvent* getLastFilteredEvent();
        FilteredEvent* getNextFilteredEvent(FilteredEvent *filteredEvent);
        FilteredEvent* getPreviousFilteredEvent(FilteredEvent *filteredEvent);
        FilteredEvent* getNextFilteredEvent(long eventNumber);
        FilteredEvent* getPreviousFilteredEvent(long eventNumber);

        bool matchesFilter(Event *event);
        void print(FILE *file = stdout, long fromEventNumber = -1, long toEventNumber = -1);

    protected:
        FilteredEvent *cacheFilteredEvent(long eventNumber);
        FilteredEvent *cacheFilteredEvent(FilteredEvent *filteredEvent);
        FilteredEvent* getFilteredEventInDirection(long filteredEventNumber, long eventNumber, bool forward);
        bool matchesEvent(Event *event);
        bool matchesDependency(Event *event);
        bool causesEvent(Event *cause, Event *consequence);
        bool consequencesEvent(Event *cause, Event *consequence);
};

#endif
