//=========================================================================
//  FILTEREDEVENT.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "filteredevent.h"
#include "filteredeventlog.h"

FilteredEvent::FilteredEvent(FilteredEventLog *filteredEventLog, long eventNumber)
{
    this->eventNumber = eventNumber;
    this->filteredEventLog = filteredEventLog;
    causeEventNumber = -1;
    nextFilteredEventNumber = -1;
    previousFilteredEventNumber = -1;
}

Event *FilteredEvent::getEvent()
{
    return filteredEventLog->getEventLog()->getEventForEventNumber(eventNumber);
}

FilteredEvent *FilteredEvent::getCause()
{
    Event *cause = getEvent()->getCauseEvent();

    while (cause)
    {
        if (filteredEventLog->matchesFilter(cause))
            return filteredEventLog->getFilteredEvent(cause->getEventNumber());

        cause = cause->getCauseEvent();
    }

    return NULL;
}

FilteredEvent::FilteredEventList *FilteredEvent::getCauses()
{
    // TODO: this should be based on getCauses of our event
    return NULL;
}

FilteredEvent::FilteredEventList *FilteredEvent::getConsequences()
{
    // TODO: this should be based on getConsequences of our event
    return NULL;
}

void FilteredEvent::linkFilteredEvents(FilteredEvent *previousFilteredEvent, FilteredEvent *nextFilteredEvent)
{
    previousFilteredEvent->nextFilteredEventNumber = nextFilteredEvent->getEventNumber();
    nextFilteredEvent->previousFilteredEventNumber = previousFilteredEvent->getEventNumber();
}
