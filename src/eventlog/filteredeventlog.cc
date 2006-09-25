//=========================================================================
//  FILTEREDEVENTLOG.CC - part of
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
    maxCauseDepth = maxConsequenceDepth = 100;
}

FilteredEventLog::~FilteredEventLog()
{
    for (EventNumberToFilteredEventMap::iterator it = eventNumberToFilteredEventMap.begin(); it != eventNumberToFilteredEventMap.end(); it++)
        delete it->second;

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

FilteredEvent* FilteredEventLog::getFilteredEvent(long eventNumber)
{
    EventNumberToFilteredEventMap::iterator it = eventNumberToFilteredEventMap.find(eventNumber);

    if (it != eventNumberToFilteredEventMap.end())
        return it->second;
    else
    {
        Event *event = eventLog->getEventForEventNumber(eventNumber);

        if (event != NULL && matchesFilter(event))
            return cacheFilteredEvent(eventNumber);
        else
            return NULL;
    }
}

bool FilteredEventLog::matchesFilter(Event *event)
{
    EventNumberToFilterMatchesMap::iterator it = eventNumberToFilterMatchesMap.find(event->getEventNumber());

    // if cached, return it
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

    return true;
}

bool FilteredEventLog::matchesDependency(Event *event)
{
    // if there's no traced event
    if (tracedEventNumber == -1)
        return true;

    // if this is the traced event
    if (event->getEventNumber() == tracedEventNumber)
        return true;

    // event is cause of traced event
    if (tracedEventNumber > event->getEventNumber() && includeCauses)
        return consequencesEvent(event, eventLog->getEventForEventNumber(tracedEventNumber));

    // event is consequence of traced event
    if (tracedEventNumber < event->getEventNumber() && includeConsequences)
        return causesEvent(eventLog->getEventForEventNumber(tracedEventNumber), event);

    return false;
}

bool FilteredEventLog::causesEvent(Event *cause, Event *consequence)
{
    // like consequenceEvent(), but searching from the opposite direction
    Event::MessageDependencyList *causes = consequence->getCauses();

    for (Event::MessageDependencyList::iterator it = causes->begin(); it != causes->end(); it++)
    {
        MessageDependency *messageDependency = *it;
        Event *causeEvent = messageDependency->getCauseEvent();

        if (causeEvent != NULL)
        {
            EventNumberToFilterMatchesMap::iterator it = eventNumberToFilterMatchesMap.find(causeEvent->getEventNumber());

            // check cache
            if (it != eventNumberToFilterMatchesMap.end() && it->second)
                return true;

            // if we reached the cause event, we're done
            if (cause->getEventNumber() == causeEvent->getEventNumber())
                return true;

            // try depth-first search if we haven't passed the cause event yet
            if (cause->getEventNumber() < causeEvent->getEventNumber() &&
                causesEvent(cause, causeEvent))
                return true;
        }
    }

    return false;
}

bool FilteredEventLog::consequencesEvent(Event *cause, Event *consequence)
{
    // returns true if "consequence" can be reached from "cause", using
    // the consequences chain. We use depth-first search.
    Event::MessageDependencyList *consequences = cause->getConsequences();

    for (Event::MessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++)
    {
        MessageDependency *messageDependency = *it;
        Event *consequenceEvent = messageDependency->getConsequenceEvent();

        if (consequenceEvent != NULL)
        {
            EventNumberToFilterMatchesMap::iterator it = eventNumberToFilterMatchesMap.find(consequenceEvent->getEventNumber());

            // check cache
            if (it != eventNumberToFilterMatchesMap.end() && it->second)
                return true;

            // if we reached the consequence event, we're done
            if (consequence->getEventNumber() == consequenceEvent->getEventNumber())
                return true;

            // try depth-first search if we haven't passed the consequence event yet
            if (consequence->getEventNumber() > consequenceEvent->getEventNumber() &&
                consequencesEvent(consequenceEvent, consequence))
                return true;
        }
    }

    return false;
}

FilteredEvent* FilteredEventLog::getFilteredEventInDirection(long filteredEventNumber, long eventNumber, bool forward)
{
    if (filteredEventNumber != -1)
        return cacheFilteredEvent(filteredEventNumber);
    else
    {
        Event *event = eventLog->getEventForEventNumber(eventNumber);

        // TODO: use linear search instead of binary search
        while (event)
        {
            if (matchesFilter(event))
                return cacheFilteredEvent(eventNumber);

            if (forward)
            {
                eventNumber++;
                event = eventLog->getEventForBeginOffset(event->getEndOffset());

                if (lastEventNumber != -1 && eventNumber > lastEventNumber)
                    return NULL;
            }
            else {
                eventNumber--;
                event = eventLog->getEventForEndOffset(event->getBeginOffset());

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
        firstMatchingEventNumber = firstMatchingFilteredEvent->getEventNumber();


    return firstMatchingFilteredEvent;
}

FilteredEvent* FilteredEventLog::getLastFilteredEvent()
{
    long startEventNumber = lastEventNumber == -1 ? eventLog->getLastEventNumber() : std::min(eventLog->getLastEventNumber(), lastEventNumber);
    FilteredEvent *lastMatchingFilteredEvent = getFilteredEventInDirection(lastMatchingEventNumber, startEventNumber, false);

    if (lastMatchingFilteredEvent != NULL)
        lastMatchingEventNumber = lastMatchingFilteredEvent->getEventNumber();

    return lastMatchingFilteredEvent;
}

FilteredEvent* FilteredEventLog::getNextFilteredEvent(FilteredEvent *filteredEvent)
{
    FilteredEvent *nextFilteredEvent = getFilteredEventInDirection(filteredEvent->getNextFilteredEventNumber(), filteredEvent->getEventNumber() + 1, true);

    if (nextFilteredEvent != NULL)
        FilteredEvent::linkFilteredEvents(filteredEvent, nextFilteredEvent);

    return nextFilteredEvent;
}

FilteredEvent* FilteredEventLog::getPreviousFilteredEvent(FilteredEvent *filteredEvent)
{
    FilteredEvent *previousFilteredEvent = getFilteredEventInDirection(filteredEvent->getPreviousFilteredEventNumber(), filteredEvent->getEventNumber() - 1, false);

    if (previousFilteredEvent != NULL)
        FilteredEvent::linkFilteredEvents(previousFilteredEvent, filteredEvent);

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
    FilteredEvent *filteredEvent = new FilteredEvent(this, eventNumber);
    cacheFilteredEvent(filteredEvent);
    return filteredEvent;
}

FilteredEvent* FilteredEventLog::cacheFilteredEvent(FilteredEvent *filteredEvent)
{
    eventNumberToFilteredEventMap[filteredEvent->getEventNumber()] = filteredEvent;
    return filteredEvent;
}
