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

#include <vector>
#include "eventlogdefs.h"
#include "event.h"

class FilteredEventLog;

/**
 * Events stored in the FilteredEventLog.
 *
 * Filtered events are in a lazy double-linked list based on event numbers.
 */
class FilteredEvent
{
    public:
        typedef std::vector<FilteredMessageDependency *> FilteredMessageDependencyList;

    protected:
        FilteredEventLog *filteredEventLog;

        long eventNumber; // the corresponding event number
        long causeEventNumber; // the event number from which the message was sent that is being processed in this event
        FilteredMessageDependency *cause; // the message send which is processed in this event
        FilteredMessageDependencyList *causes; // the arrival message sends of messages which we send in this even and are in the filtered set
        FilteredMessageDependencyList *consequences; // the message sends and arrivals from this event to another in the filtered set

        long previousFilteredEventNumber; // the event number of the previous matching filtered event or -1 if unknown
        long nextFilteredEventNumber; // the event number of the next matching filtered event or -1 if unknown

    public:
        FilteredEvent(FilteredEventLog *filteredEventLog, long eventNumber);
        ~FilteredEvent();
        static void linkFilteredEvents(FilteredEvent *previousFilteredEvent, FilteredEvent *nextFilteredEvent);

        Event *getEvent();
        long getEventNumber() { return eventNumber; };
        long getPreviousFilteredEventNumber() { return previousFilteredEventNumber; };
        long getNextFilteredEventNumber() { return nextFilteredEventNumber; };

        FilteredEvent *getCauseFilteredEvent();
        FilteredMessageDependency *getCause();
        FilteredMessageDependencyList *getCauses();
        FilteredMessageDependencyList *getConsequences();

    protected:
        FilteredMessageDependencyList *getCauses(Event *event, int consequenceMessageSendEntryNumber, int level);
        FilteredMessageDependencyList *getConsequences(Event *event, int causeMessageSendEntryNumber, int level);
};

#endif