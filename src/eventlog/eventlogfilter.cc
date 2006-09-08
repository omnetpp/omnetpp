//=========================================================================
//  EVENTLOGFILTER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "eventlogfilter.h"

FilteredEvent::FilteredEvent(EventLogFilter *eventLogFilter, long eventNumber)
{
    this->eventNumber = eventNumber;
    this->eventLogFilter = eventLogFilter;
    causeEventNumber = -1;
}

Event *FilteredEvent::getEvent()
{
    return eventLogFilter->eventLog->getEvent(eventNumber);
}

FilteredEvent *FilteredEvent::getCause()
{
    return NULL;
}

FilteredEventList *FilteredEvent::getCauses()
{
    return NULL;
}

FilteredEventList *FilteredEvent::getConsequences()
{
    return NULL;
}

/**************************************************/

EventLogFilter::EventLogFilter(
    EventLog *eventLog,
    long tracedEventNumber,
    std::set<int> *includeModuleIds,
    bool includeCauses,
    bool includeConsequences,
    bool includetNonDeliveryMessages)
{
    this->eventLog = eventLog;
    this->tracedEventNumber = tracedEventNumber;
    this->includeModuleIds = includeModuleIds;
    this->includeCauses = includeCauses;
    this->includeConsequences = includeConsequences;
    this->includetNonDeliveryMessages = includetNonDeliveryMessages;
}

void EventLogFilter::print(FILE *file)
{
    FilteredEvent *filteredEvent = getFirstFilteredEvent();

    do
    {
        if (filteredEvent)
            filteredEvent->getEvent()->print(file);
    }
    while (filteredEvent = getNextFilteredEvent(filteredEvent));
}

bool EventLogFilter::matchesFilter(Event *event)
{
    return true;
}

FilteredEvent* EventLogFilter::getFirstFilteredEvent()
{
    long eventNumber = 0;
    Event *event;

    while (event = eventLog->getEvent(eventNumber))
    {
        if (matchesFilter(event))
            return new FilteredEvent(this, eventNumber);
    }

    return NULL;
}

FilteredEvent* EventLogFilter::getLastFilteredEvent()
{
    return NULL;
}

FilteredEvent* EventLogFilter::getNextFilteredEvent(FilteredEvent *filteredEvent)
{
    return NULL;
}

FilteredEvent* EventLogFilter::getPreviousFilteredEvent(FilteredEvent *filteredEvent)
{
    return NULL;
}

