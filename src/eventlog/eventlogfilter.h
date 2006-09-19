//=========================================================================
//  EVENTLOGFILTER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTLOGFILTER_H_
#define __EVENTLOGFILTER_H_

#include <sstream>
#include <vector>
#include <deque>
#include "eventlogdefs.h"
#include "eventlog.h"

class EventLogFilter;

class FilteredEvent
{
    friend EventLogFilter;

    public:
        typedef std::vector<FilteredEvent *> FilteredEventList;
        typedef std::vector<long> EventNumberList;

    protected:
        EventLogFilter *eventLogFilter;

        long eventNumber; // the corresponding event number
        long causeEventNumber; // the event number from which the message was sent that is being processed in this event
        EventNumberList causeEventNumbers; // the arrival event of messages which we send in this event
        EventNumberList consequenceEventNumbers; // a set of events which process messages sent in this event

        long nextFilteredEventNumber; // the event number of the next matching filtered event or -1 if unknown
        long previousFilteredEventNumber; // the event number of the previous matching filtered event or -1 if unknown

        // the following fields are for the convenience of the GUI
        double timelineCoordinate;
        int64 cachedX;
        int64 cachedY;
        bool isExpandedInTree;
        int tableRowIndex;

    public:
        FilteredEvent(EventLogFilter *eventLogFilter, long eventNumber);

    public:
        long getEventNumber() { return eventNumber; };
        Event *getEvent();

        FilteredEvent *getCause();
        FilteredEventList *getCauses(); // the returned FilteredEventList must be deleted
        FilteredEventList *getConsequences(); // the returned FilteredEventList must be deleted
};

class EventLogFilter
{
    friend FilteredEvent;

    protected:
        EventLog *eventLog;
        long tracedEventNumber; // the event number from which causes and consequences are followed or -1
        long firstEventNumber; // the first event to be considered by the filter or -1
        long lastEventNumber; // the last event to be considered by the filter or -1
        std::set<int> *includeModuleIds; // events outside these modules will be filtered out, NULL means include all
        bool includeCauses; // only when tracedEventNumber is given
        bool includeConsequences; // only when tracedEventNumber is given

        typedef std::map<long, FilteredEvent *> EventNumberToFilteredEventMap;
        EventNumberToFilteredEventMap eventNumberToFilteredEventMap;

        typedef std::map<long, bool> EventNumberToFilterMatchesMap;
        EventNumberToFilterMatchesMap eventNumberToFilterMatchesMap; // a cache of whether the given event number matches the filter or not

        long firstMatchingEventNumber; // the event number of the first matching event
        long lastMatchingEventNumber; // the event number of the last matching event

    public:
        EventLogFilter(EventLog *eventLog,
                       std::set<int> *includeModuleIds,
                       long tracedEventNumber = -1,
                       bool includeCauses = false,
                       bool includeConsequences = false,
                       long firstEventNumber = -1,
                       long lastEventNumber = -1);
        ~EventLogFilter();

    public:
        FilteredEvent* getFirstFilteredEvent();
        FilteredEvent* getLastFilteredEvent();
        FilteredEvent* getNextFilteredEvent(FilteredEvent *filteredEvent);
        FilteredEvent* getPreviousFilteredEvent(FilteredEvent *filteredEvent);

        void print(FILE *file);

    protected:
        FilteredEvent* cacheFilteredEvent(long eventNumber);
        FilteredEvent* getFilteredEventInDirection(long filteredEventNumber, long eventNumber, bool forward);
        bool matchesFilter(Event *event);
        bool matchesFilterNonCached(Event *event);
        void linkFilteredEvents(FilteredEvent *previousFilteredEvent, FilteredEvent *nextFilteredEvent);
};

#endif
