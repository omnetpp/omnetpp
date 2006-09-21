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
    return filteredEventLog->eventLog->getEventForEventNumber(eventNumber);
}


FilteredEvent *FilteredEvent::getCause()
{
    Event *cause = getEvent()->getCauseEvent();

    while (cause)
    {
        if (filteredEventLog->matchesFilter(cause))
            return filteredEventLog->cacheFilteredEvent(cause->getEventNumber());

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

FilteredEventLog::FilteredEventLog(EventLog *eventLog,
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

FilteredEventLog::~FilteredEventLog()
{
    for (EventNumberToFilteredEventMap::iterator it = eventNumberToFilteredEventMap.begin(); it != eventNumberToFilteredEventMap.end(); it++)
        delete it->second;

    if (includeModuleIds)
        delete includeModuleIds;
}

void FilteredEventLog::print(FILE *file, long fromEventNumber, long toEventNumber)
{
    eventLog->printInitializationLogEntries(file);

    FilteredEvent *filteredEvent = fromEventNumber == -1 ? getFirstFilteredEvent() : getNextFilteredEvent(fromEventNumber);

    while (filteredEvent != NULL && (toEventNumber == -1 || filteredEvent->getEventNumber() < toEventNumber))
    {
        filteredEvent->getEvent()->print(file);
        filteredEvent = getNextFilteredEvent(filteredEvent);
    }
}

bool FilteredEventLog::matchesFilter(Event *event)
{
    EventNumberToFilterMatchesMap::iterator it = eventNumberToFilterMatchesMap.find(event->getEventNumber());
    
    if (it != eventNumberToFilterMatchesMap.end())
        return it->second;

    //printf("*** Matching filter to event: %ld\n", event->getEventNumber());

    bool matches = matchesEvent(event);
    matches &= matchesDependency(event);
    eventNumberToFilterMatchesMap[event->getEventNumber()] = matches;
    return matches;
}

bool FilteredEventLog::matchesEvent(Event *event)
{
    // the traced event
    if (tracedEventNumber != -1 && event->getEventNumber() == tracedEventNumber)
        return true;

    // event not in considered modules
    if (includeModuleIds != NULL &&
        includeModuleIds->find(event->getEventEntry()->moduleId) == includeModuleIds->end())
        return false;

    // event outside of considered range
    if ((firstEventNumber != -1 && event->getEventNumber() < firstEventNumber) ||
        (lastEventNumber != -1 && event->getEventNumber() > lastEventNumber))
        return false;
}

bool FilteredEventLog::matchesDependency(Event *event)
{
    // event is cause of traced event
    if (tracedEventNumber != -1 && tracedEventNumber > event->getEventNumber() && includeCauses)
        return consequencesEvent(event, eventLog->getEventForEventNumber(tracedEventNumber));

    // event is consequence of traced event
    if (tracedEventNumber != -1 && tracedEventNumber < event->getEventNumber() && includeConsequences)
        return causesEvent(eventLog->getEventForEventNumber(tracedEventNumber), event);

    return false;
}

bool FilteredEventLog::consequencesEvent(Event *cause, Event *consequence)
{
    Event::MessageSendList *consequences = cause->getConsequences();

    for (Event::MessageSendList::iterator it = consequences->begin(); it != consequences->end(); it++)
    {
        MessageDependency *messageDependency = *it;
        Event *consequenceEvent = messageDependency->getConsequenceEvent();

        if (consequenceEvent != NULL)
        {
            if (consequence->getEventNumber() == consequenceEvent->getEventNumber())
                return true;
            
            if (consequence->getEventNumber() > consequenceEvent->getEventNumber() &&
                consequencesEvent(consequenceEvent, consequence))
                return true;
        }
    }

    return false;
}

bool FilteredEventLog::causesEvent(Event *cause, Event *consequence)
{
    Event::MessageSendList *causes = consequence->getCauses();

    for (Event::MessageSendList::iterator it = causes->begin(); it != causes->end(); it++)
    {
        MessageDependency *messageDependency = *it;
        Event *causeEvent = messageDependency->getCauseEvent();

        if (causeEvent != NULL)
        {
            if (cause->getEventNumber() == causeEvent->getEventNumber())
                return true;
            
            if (cause->getEventNumber() < causeEvent->getEventNumber() &&
                causesEvent(cause, causeEvent))
                return true;
        }
    }

    return false;
}

FilteredEvent* FilteredEventLog::getFilteredEventInDirection(long filteredEventNumber, long eventNumber, bool forward)
{
    if (filteredEventNumber != -1)
        return cacheFilteredEvent(filteredEventNumber);
    {
        Event *event;

        while (event = eventLog->getEventForEventNumber(eventNumber))
        {
            if (matchesFilter(event))
                return cacheFilteredEvent(eventNumber);

            if (forward)
            {
                eventNumber++;

                if (lastEventNumber != -1 && eventNumber > lastEventNumber)
                    return NULL;
            }
            else {
                eventNumber--;

                if (firstEventNumber != -1 && eventNumber < firstEventNumber)
                    return NULL;
            }
        }

        return NULL; 
    }
}

FilteredEvent* FilteredEventLog::getFirstFilteredEvent()
{
    long startEventNumber = firstEventNumber == -1 ? eventLog->getFirstEventNumber() : std::max(eventLog->getFirstEventNumber(), firstEventNumber);
    FilteredEvent *firstMatchingFilteredEvent = getFilteredEventInDirection(firstMatchingEventNumber, startEventNumber, true);

    if (firstMatchingFilteredEvent != NULL)
        firstMatchingEventNumber = firstMatchingFilteredEvent->eventNumber;


    return firstMatchingFilteredEvent;
}

FilteredEvent* FilteredEventLog::getLastFilteredEvent()
{
    long startEventNumber = lastEventNumber == -1 ? eventLog->getLastEventNumber() : std::min(eventLog->getLastEventNumber(), lastEventNumber);
    FilteredEvent *lastMatchingFilteredEvent = getFilteredEventInDirection(lastMatchingEventNumber, startEventNumber, false);

    if (lastMatchingFilteredEvent != NULL)
        lastMatchingEventNumber = lastMatchingFilteredEvent->eventNumber;

    return lastMatchingFilteredEvent;
}

FilteredEvent* FilteredEventLog::getNextFilteredEvent(FilteredEvent *filteredEvent)
{
    FilteredEvent *nextFilteredEvent = getFilteredEventInDirection(filteredEvent->nextFilteredEventNumber, filteredEvent->eventNumber + 1, true);

    if (nextFilteredEvent != NULL)
        linkFilteredEvents(filteredEvent, nextFilteredEvent);

    return nextFilteredEvent;
}

FilteredEvent* FilteredEventLog::getPreviousFilteredEvent(FilteredEvent *filteredEvent)
{
    FilteredEvent *previousFilteredEvent = getFilteredEventInDirection(filteredEvent->previousFilteredEventNumber, filteredEvent->eventNumber - 1, false);

    if (previousFilteredEvent != NULL)
        linkFilteredEvents(previousFilteredEvent, filteredEvent);

    return previousFilteredEvent;
}

FilteredEvent* FilteredEventLog::getNextFilteredEvent(long eventNumber)
{
    return getFilteredEventInDirection(-1, eventNumber, true);
}

FilteredEvent* FilteredEventLog::getPreviousFilteredEvent(long eventNumber)
{
    return getFilteredEventInDirection(-1, eventNumber, false);
}

FilteredEvent* FilteredEventLog::cacheFilteredEvent(long eventNumber)
{
    EventNumberToFilteredEventMap::iterator it = eventNumberToFilteredEventMap.find(eventNumber);

    if (it != eventNumberToFilteredEventMap.end())
        return it->second;

    FilteredEvent *filteredEvent = new FilteredEvent(this, eventNumber);
    eventNumberToFilteredEventMap[eventNumber] = filteredEvent;
    return filteredEvent;
}

void FilteredEventLog::linkFilteredEvents(FilteredEvent *previousFilteredEvent, FilteredEvent *nextFilteredEvent)
{
    previousFilteredEvent->nextFilteredEventNumber = nextFilteredEvent->eventNumber;
    nextFilteredEvent->previousFilteredEventNumber = previousFilteredEvent->eventNumber;
}
