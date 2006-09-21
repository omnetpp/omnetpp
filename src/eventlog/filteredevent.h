//=========================================================================
//  FILTEREDEVENT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FILTEREDEVENT_H_
#define __FILTEREDEVENT_H_

#include <deque>
#include "eventlogdefs.h"
#include "filteredeventlog.h"

class FilteredEventLog;

class FilteredEvent
{
    friend FilteredEventLog;

    public:
        typedef std::vector<FilteredEvent *> FilteredEventList;
        typedef std::vector<long> EventNumberList;

    protected:
        FilteredEventLog *filteredEventLog;

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
        FilteredEvent(FilteredEventLog *filteredEventLog, long eventNumber);

    public:
        long getEventNumber() { return eventNumber; };
        Event *getEvent();

        FilteredEvent *getCause();
        FilteredEventList *getCauses(); // the returned FilteredEventList must be deleted
        FilteredEventList *getConsequences(); // the returned FilteredEventList must be deleted
};

#endif