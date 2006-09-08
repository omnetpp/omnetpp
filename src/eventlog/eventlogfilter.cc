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
    return eventLogFilter->getCause(this);
}

FilteredEvent::FilteredEventList *FilteredEvent::getCauses()
{
    return eventLogFilter->getCauses(this);
}

FilteredEvent::FilteredEventList *FilteredEvent::getConsequences()
{
    return eventLogFilter->getConsequences(this);
}

/**************************************************/

EventLogFilter::EventLogFilter(EventLog *eventLog,
                               long tracedEventNumber,
                               std::set<int> *includeModuleIds,
                               bool includeCauses,
                               bool includeConsequences)
{
    this->eventLog = eventLog;
    this->tracedEventNumber = tracedEventNumber;
    this->includeModuleIds = includeModuleIds;
    this->includeCauses = includeCauses;
    this->includeConsequences = includeConsequences;
}

EventLogFilter::~EventLogFilter()
{
    for (EventNumberToFilteredEventMap::iterator it = eventNumberToFilteredEventMap.begin(); it != eventNumberToFilteredEventMap.end(); it++)
    {
        delete it->second;
    }

    if (includeModuleIds)
        delete includeModuleIds;
}

void EventLogFilter::print(FILE *file)
{
    FilteredEvent *filteredEvent = getFirstFilteredEvent();

    while (filteredEvent)
    {
        filteredEvent->getEvent()->print(file);
        filteredEvent = getNextFilteredEvent(filteredEvent);
    }
}

bool EventLogFilter::matchesFilter(Event *event)
{
    if (event->getEventNumber() == tracedEventNumber)
        return true;

    if (eventNumberToFilteredEventMap.find(event->getEventNumber()) != eventNumberToFilteredEventMap.end())
        return true;

    if (includeModuleIds != NULL &&
        includeModuleIds->find(event->getEventEntry()->moduleId) == includeModuleIds->end())
        return false;

    if (includeCauses)
    {
        // TODO:
    }

    if (includeConsequences)
    {
        // TODO:
    }

    // this is a temporary hack
    Event *cause = eventLog->getCause(event);

    if (cause)
        return matchesFilter(cause);
    else
        return false;
}

FilteredEvent* EventLogFilter::getFirstFilteredEvent()
{
    long eventNumber = 0;
    Event *event;

    while (event = eventLog->getEvent(eventNumber))
    {
        if (matchesFilter(event))
            return cacheFilteredEvent(eventNumber);
        else
            eventNumber++;
    }

    return NULL;
}

FilteredEvent* EventLogFilter::getLastFilteredEvent()
{
    throw new Exception("NYI");
}

FilteredEvent* EventLogFilter::getNextFilteredEvent(FilteredEvent *filteredEvent)
{
    long eventNumber = filteredEvent->getEventNumber() + 1;
    Event *event;

    while (event = eventLog->getEvent(eventNumber))
    {
        if (matchesFilter(event))
            return cacheFilteredEvent(eventNumber);
        else
            eventNumber++;
    }

    return NULL;
}

FilteredEvent* EventLogFilter::getPreviousFilteredEvent(FilteredEvent *filteredEvent)
{
    long eventNumber = filteredEvent->getEventNumber() - 1;
    Event *event;

    while (event = eventLog->getEvent(eventNumber))
    {
        if (matchesFilter(event))
            return cacheFilteredEvent(eventNumber);
        else
            eventNumber--;
    }

    return NULL;
}

FilteredEvent *EventLogFilter::getCause(FilteredEvent *filteredEvent)
{
    Event *cause = eventLog->getCause(filteredEvent->getEvent());

    while (cause)
    {
        if (matchesFilter(cause))
            return cacheFilteredEvent(cause->getEventNumber());

        cause = cause->getCause();
    }

    return NULL;
}

FilteredEvent::FilteredEventList *EventLogFilter::getCauses(FilteredEvent *filteredEvent)
{
    // TODO: this should be based on getCauses of our event
    return NULL;
}

FilteredEvent::FilteredEventList *EventLogFilter::getConsequences(FilteredEvent *filteredEvent)
{
    // TODO: this should be based on getConsequences of our event
    return NULL;
}

FilteredEvent* EventLogFilter::cacheFilteredEvent(long eventNumber)
{
    EventNumberToFilteredEventMap::iterator it = eventNumberToFilteredEventMap.find(eventNumber);

    if (it != eventNumberToFilteredEventMap.end())
        return it->second;

    FilteredEvent *filteredEvent = new FilteredEvent(this, eventNumber);
    eventNumberToFilteredEventMap[eventNumber] = filteredEvent;
    return filteredEvent;
}
