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
    nextFilteredEventNumber = -1;
    previousFilteredEventNumber = -1;
}

Event *FilteredEvent::getEvent()
{
    return eventLogFilter->eventLog->getEvent(eventNumber);
}


FilteredEvent *FilteredEvent::getCause()
{
    Event *cause = getEvent()->getCauseEvent();

    while (cause)
    {
        if (eventLogFilter->matchesFilter(cause))
            return eventLogFilter->cacheFilteredEvent(cause->getEventNumber());

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

/**************************************************/

EventLogFilter::EventLogFilter(EventLog *eventLog,
                               std::set<int> *includeModuleIds,
                               long tracedEventNumber,
                               bool includeCauses,
                               bool includeConsequences,
                               long firstEventNumber,
                               long lastEventNumber)
{
    this->eventLog = eventLog;
    this->includeModuleIds = includeModuleIds;
    this->tracedEventNumber = tracedEventNumber;
    this->includeCauses = includeCauses;
    this->includeConsequences = includeConsequences;
    this->firstEventNumber = firstEventNumber;
    this->lastEventNumber = lastEventNumber;
    firstMatchingEventNumber = -1;
    lastMatchingEventNumber = -1;
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
    eventLog->printInitializationLogEntries(file);

    FilteredEvent *filteredEvent = getFirstFilteredEvent();

    while (filteredEvent)
    {
        filteredEvent->getEvent()->print(file);
        filteredEvent = getNextFilteredEvent(filteredEvent);
    }
}

bool EventLogFilter::matchesFilter(Event *event)
{
    EventNumberToFilterMatchesMap::iterator it = eventNumberToFilterMatchesMap.find(event->getEventNumber());
    
    if (it != eventNumberToFilterMatchesMap.end())
        return it->second;

    bool matches = matchesFilterNonCached(event);
    eventNumberToFilterMatchesMap[event->getEventNumber()] = matches;
    return matches;
}

bool EventLogFilter::matchesFilterNonCached(Event *event)
{
    if (tracedEventNumber != -1 && event->getEventNumber() == tracedEventNumber)
        return true;

    if (includeModuleIds != NULL &&
        includeModuleIds->find(event->getEventEntry()->moduleId) == includeModuleIds->end())
        return false;

    if ((firstEventNumber != -1 && event->getEventNumber() < firstEventNumber) ||
        (lastEventNumber != -1 && event->getEventNumber() > lastEventNumber))
        return false;
    
    if (tracedEventNumber != -1 && tracedEventNumber > event->getEventNumber() && includeCauses)
    {
        Event::MessageSendList *consequences = event->getConsequences();

        for (Event::MessageSendList::iterator it = consequences->begin(); it != consequences->end(); it++)
        {
            MessageSend *messageSend = *it;

            if (matchesFilter(messageSend->getArrivalEvent()))
                return true;
        }
    }

    if (tracedEventNumber != -1 && tracedEventNumber < event->getEventNumber() && includeConsequences)
    {
        Event::MessageSendList *causes = event->getCauses();

        for (Event::MessageSendList::iterator it = causes->begin(); it != causes->end(); it++)
        {
            MessageSend *messageSend = *it;

            if (matchesFilter(messageSend->getSenderEvent()))
                return true;
        }
    }

    return false;
}

FilteredEvent* EventLogFilter::getFilteredEventInDirection(long filteredEventNumber, long eventNumber, bool forward)
{
    if (filteredEventNumber != -1)
        return cacheFilteredEvent(filteredEventNumber);
    {
        Event *event;

        while (event = eventLog->getEvent(eventNumber))
        {
            if (matchesFilter(event))
                return cacheFilteredEvent(eventNumber);

            if (forward)
                eventNumber++;
            else
                eventNumber--;
        }

        return NULL; 
    }
}

FilteredEvent* EventLogFilter::getFirstFilteredEvent()
{
    FilteredEvent *firstMatchingFilteredEvent = getFilteredEventInDirection(firstMatchingEventNumber, eventLog->getFirstEventNumber(), true);

    if (firstMatchingFilteredEvent != NULL)
    {
        firstMatchingEventNumber = firstMatchingFilteredEvent->eventNumber;
    }

    return firstMatchingFilteredEvent;
}

FilteredEvent* EventLogFilter::getLastFilteredEvent()
{
    FilteredEvent *lastMatchingFilteredEvent = getFilteredEventInDirection(lastMatchingEventNumber, eventLog->getLastEventNumber(), false);

    if (lastMatchingFilteredEvent != NULL)
    {
        lastMatchingEventNumber = lastMatchingFilteredEvent->eventNumber;
    }

    return lastMatchingFilteredEvent;
}

FilteredEvent* EventLogFilter::getNextFilteredEvent(FilteredEvent *filteredEvent)
{
    FilteredEvent *nextFilteredEvent = getFilteredEventInDirection(filteredEvent->nextFilteredEventNumber, filteredEvent->eventNumber + 1, true);

    if (nextFilteredEvent != NULL)
    {
        linkFilteredEvents(filteredEvent, nextFilteredEvent);
    }

    return nextFilteredEvent;
}

FilteredEvent* EventLogFilter::getPreviousFilteredEvent(FilteredEvent *filteredEvent)
{
    FilteredEvent *previousFilteredEvent = getFilteredEventInDirection(filteredEvent->previousFilteredEventNumber, filteredEvent->eventNumber - 1, false);

    if (previousFilteredEvent != NULL)
    {
        linkFilteredEvents(previousFilteredEvent, filteredEvent);
    }

    return previousFilteredEvent;
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

void EventLogFilter::linkFilteredEvents(FilteredEvent *previousFilteredEvent, FilteredEvent *nextFilteredEvent)
{
    previousFilteredEvent->nextFilteredEventNumber = nextFilteredEvent->eventNumber;
    nextFilteredEvent->previousFilteredEventNumber = previousFilteredEvent->eventNumber;
}
