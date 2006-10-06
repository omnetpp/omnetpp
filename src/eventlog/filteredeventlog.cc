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

FilteredEventLog::FilteredEventLog(IEventLog *eventLog,
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
    firstMatchingEvent = NULL;
    lastMatchingEvent = NULL;
    maxCauseDepth = maxConsequenceDepth = 100;
}

FilteredEventLog::~FilteredEventLog()
{
    for (EventNumberToFilteredEventMap::iterator it = eventNumberToFilteredEventMap.begin(); it != eventNumberToFilteredEventMap.end(); it++)
        delete it->second;

    delete includeModuleIds;

    delete eventLog;
}

long FilteredEventLog::getApproximateNumberOfEvents()
{
    throw new Exception("Not yet implemented");
}

double FilteredEventLog::getApproximatePercentageForEventNumber(long eventNumber)
{
    throw new Exception("Not yet implemented");
}

FilteredEvent *FilteredEventLog::getApproximateEventAt(double percentage)
{
    throw new Exception("Not yet implemented");
}

bool FilteredEventLog::matchesFilter(IEvent *event)
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

bool FilteredEventLog::matchesEvent(IEvent *event)
{
    // the traced event
    if (tracedEventNumber != -1 && event->getEventNumber() == tracedEventNumber)
        return true;

    // event not in considered modules
    if (includeModuleIds &&
        includeModuleIds->find(event->getEventEntry()->moduleId) == includeModuleIds->end())
        return false;

    // event outside of considered range
    if ((firstEventNumber != -1 && event->getEventNumber() < firstEventNumber) ||
        (lastEventNumber != -1 && event->getEventNumber() > lastEventNumber))
        return false;

    return true;
}

bool FilteredEventLog::matchesDependency(IEvent *event)
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

FilteredEvent* FilteredEventLog::getFirstEvent()
{
    if (!firstMatchingEvent)
    {
        long startEventNumber = firstEventNumber == -1 ? eventLog->getFirstEvent()->getEventNumber() : std::max(eventLog->getFirstEvent()->getEventNumber(), firstEventNumber);
        firstMatchingEvent = getMatchingEventInDirection(startEventNumber, true);
    }

    return firstMatchingEvent;
}

FilteredEvent* FilteredEventLog::getLastEvent()
{
    if (!lastMatchingEvent)
    {
        long startEventNumber = lastEventNumber == -1 ? eventLog->getLastEvent()->getEventNumber() : std::min(eventLog->getLastEvent()->getEventNumber(), lastEventNumber);
        lastMatchingEvent = getMatchingEventInDirection(startEventNumber, false);
    }

    return lastMatchingEvent;
}

FilteredEvent *FilteredEventLog::getEventForEventNumber(long eventNumber, MatchKind matchKind)
{
    // TODO: use matchKind
    EventNumberToFilteredEventMap::iterator it = eventNumberToFilteredEventMap.find(eventNumber);

    if (it != eventNumberToFilteredEventMap.end())
        return it->second;
    else
    {
        IEvent *event = eventLog->getEventForEventNumber(eventNumber);

        if (event && matchesFilter(event))
            return cacheFilteredEvent(eventNumber);
        else
            return NULL;
    }
}

FilteredEvent *FilteredEventLog::getEventForSimulationTime(simtime_t simulationTime, MatchKind matchKind)
{
    // TODO:
    return NULL;
}

FilteredEvent* FilteredEventLog::getMatchingEventInDirection(long eventNumber, bool forward)
{
    IEvent *event = eventLog->getEventForEventNumber(eventNumber);

    while (event)
    {
        if (matchesFilter(event))
            return getEventForEventNumber(eventNumber);

        if (forward)
        {
            eventNumber++;
            event = event->getNextEvent();

            if (lastEventNumber != -1 && eventNumber > lastEventNumber)
                return NULL;
        }
        else {
            eventNumber--;
            event = event->getPreviousEvent();

            if (firstEventNumber != -1 && eventNumber < firstEventNumber)
                return NULL;
        }
    }

    return NULL;
}

bool FilteredEventLog::causesEvent(IEvent *cause, IEvent *consequence)
{
    // like consequenceEvent(), but searching from the opposite direction
    MessageDependencyList *causes = consequence->getCauses();

    for (MessageDependencyList::iterator it = causes->begin(); it != causes->end(); it++)
    {
        MessageDependency *messageDependency = *it;
        IEvent *causeEvent = messageDependency->getCauseEvent();

        if (causeEvent)
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

bool FilteredEventLog::consequencesEvent(IEvent *cause, IEvent *consequence)
{
    // returns true if "consequence" can be reached from "cause", using
    // the consequences chain. We use depth-first search.
    MessageDependencyList *consequences = cause->getConsequences();

    for (MessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++)
    {
        MessageDependency *messageDependency = *it;
        IEvent *consequenceEvent = messageDependency->getConsequenceEvent();

        if (consequenceEvent)
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
