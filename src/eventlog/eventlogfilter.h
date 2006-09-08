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

class FilteredEvent;
class EventLogFilter;

typedef std::vector<FilteredEvent *> FilteredEventList;
typedef std::vector<long> EventNumberList;

class FilteredEvent
{
    protected:
        EventLogFilter *eventLogFilter;

        long eventNumber;
        long causeEventNumber; // the event number from which the message was sent that is being processed in this event
        EventNumberList causeEventNumbers; // the arrival event of messages which we send in this event
        EventNumberList consequenceEventNumbers; // a set of events which process messages sent in this event

        // the following fields are for the convenience of the GUI
        double timelineCoordinate;
        int64 cachedX;
        int64 cachedY;
        bool isExpandedInTree;
        int tableRowIndex;

    public:
        FilteredEvent(EventLogFilter *eventLogFilter, long eventNumber);

    public:
        Event *getEvent();

        // lazily calculatations
        FilteredEvent *getCause();
        FilteredEventList *getCauses();
        FilteredEventList *getConsequences();
};

class EventLogFilter
{
    friend FilteredEvent;

    protected:
        EventLog *eventLog;
        long tracedEventNumber;
        std::set<int> *includeModuleIds;
        bool includeCauses;
        bool includeConsequences;
        bool includetNonDeliveryMessages;

        typedef std::deque<FilteredEvent> FilteredEventList;
        FilteredEventList filteredEventList;

        long firstEventNumber; // event number of the first considered event
        long lastEventNumber; // event number of the last considered event

    public:
        EventLogFilter(
            EventLog *eventLog,
            long tracedEventNumber,
            std::set<int> *includeModuleIds,
            bool includeCauses,
            bool includeConsequences,
            bool includetNonDeliveryMessages);

    public:
        void print(FILE *file);

        bool matchesFilter(Event *event);

        FilteredEvent* getFirstFilteredEvent();
        FilteredEvent* getLastFilteredEvent();
        FilteredEvent* getNextFilteredEvent(FilteredEvent *filteredEvent);
        FilteredEvent* getPreviousFilteredEvent(FilteredEvent *filteredEvent);
};

#endif
