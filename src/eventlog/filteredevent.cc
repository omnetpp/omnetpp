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

    nextFilteredEventNumber = -1;
    previousFilteredEventNumber = -1;

    causeEventNumber = -1;
    causes = NULL;
    consequences = NULL;
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

FilteredEvent::FilteredMessageDependencyList *FilteredEvent::getCauses()
{
    if (causes == NULL)
    {
        causes = new FilteredEvent::FilteredMessageDependencyList();
        getCauses(getEvent(), -1, 0);
    }

    return causes;
}

FilteredEvent::FilteredMessageDependencyList *FilteredEvent::getCauses(Event *event, int consequenceMessageSendEntryNumber, int level)
{
    Event::MessageDependencyList *eventCauses = event->getCauses();

    for (Event::MessageDependencyList::iterator it = eventCauses->begin(); it != eventCauses->end(); it++)
    {
        MessageDependency *messageDependency = *it;
        Event *causeEvent = messageDependency->getCauseEvent();

        //printf("*** Checking at level %d for cause event number %ld\n", level, causeEvent->getEventNumber());

        if (filteredEventLog->matchesFilter(causeEvent))
            causes->push_back(new FilteredMessageDependency(filteredEventLog->getEventLog(),
                causeEvent->getEventNumber(), messageDependency->getCauseMessageSendEntryNumber(),
                messageDependency->getConsequenceEventNumber(), messageDependency->getConsequenceMessageSendEntryNumber(),
                getEventNumber(), consequenceMessageSendEntryNumber));
        else if (level < filteredEventLog->getMaxCauseDepth())
            getCauses(causeEvent,
                consequenceMessageSendEntryNumber == -1 ? messageDependency->getConsequenceMessageSendEntryNumber() : consequenceMessageSendEntryNumber,
                level + 1);
    }

    return causes;
}

FilteredEvent::FilteredMessageDependencyList *FilteredEvent::getConsequences()
{
    if (consequences == NULL)
    {
        consequences = new FilteredEvent::FilteredMessageDependencyList();
        getConsequences(getEvent(), -1, 0);
    }

    return consequences;
}

FilteredEvent::FilteredMessageDependencyList *FilteredEvent::getConsequences(Event *event, int causeMessageSendEntryNumber, int level)
{
    Event::MessageDependencyList *eventConsequences = event->getConsequences();

    for (Event::MessageDependencyList::iterator it = eventConsequences->begin(); it != eventConsequences->end(); it++)
    {
        MessageDependency *messageDependency = *it;
        Event *consequenceEvent = messageDependency->getConsequenceEvent();

        if (consequenceEvent == NULL)
            continue; // skip cancelled self messages

        //printf("*** Checking at level %d for consequence event number %ld\n", level, consequenceEvent->getEventNumber());

        if (filteredEventLog->matchesFilter(consequenceEvent))
            consequences->push_back(new FilteredMessageDependency(filteredEventLog->getEventLog(),
                getEventNumber(), causeMessageSendEntryNumber,
                messageDependency->getCauseEventNumber(), messageDependency->getCauseMessageSendEntryNumber(),
                consequenceEvent->getEventNumber(), messageDependency->getConsequenceMessageSendEntryNumber()));
        else if (level < filteredEventLog->getMaxConsequenceDepth())
            getConsequences(consequenceEvent,
                causeMessageSendEntryNumber == -1 ? messageDependency->getCauseMessageSendEntryNumber() : causeMessageSendEntryNumber,
                level + 1);
    }

    return consequences;
}

void FilteredEvent::linkFilteredEvents(FilteredEvent *previousFilteredEvent, FilteredEvent *nextFilteredEvent)
{
    previousFilteredEvent->nextFilteredEventNumber = nextFilteredEvent->getEventNumber();
    nextFilteredEvent->previousFilteredEventNumber = previousFilteredEvent->getEventNumber();
}
