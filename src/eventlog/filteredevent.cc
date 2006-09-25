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

FilteredEvent::~FilteredEvent()
{
    delete cause;

    if (causes)
    {
        for (FilteredMessageDependencyList::iterator it = causes->begin(); it != causes->end(); it++)
            delete *it;
        delete causes;
    }

    if (consequences)
    {
        for (FilteredMessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++)
            delete *it;
        delete consequences;
    }
}

Event *FilteredEvent::getEvent()
{
    return filteredEventLog->getEventLog()->getEventForEventNumber(eventNumber);
}

FilteredEvent *FilteredEvent::getCauseFilteredEvent()
{
    if (causeEventNumber == -1)
    {
        Event *causeEvent = getEvent()->getCauseEvent();

        // walk backwards on the cause chain until we find an event matched by the filter
        while (causeEvent)
        {
            if (filteredEventLog->matchesFilter(causeEvent))
                return filteredEventLog->getFilteredEvent(causeEvent->getEventNumber());

            causeEvent = causeEvent->getCauseEvent();
        }
    }

    return filteredEventLog->getFilteredEvent(causeEventNumber);
}

FilteredMessageDependency *FilteredEvent::getCause()
{
    if (cause == NULL)
    {
        Event *causeEvent = getEvent();
        MessageDependency *causeMessageDependency = causeEvent->getCause();

        while (causeEvent)
        {
            MessageDependency *messageDependency = causeEvent->getCause();

            if (filteredEventLog->matchesFilter(messageDependency->getCauseEvent()))
            {
                cause = new FilteredMessageDependency(filteredEventLog->getEventLog(),
                    messageDependency->getCauseEventNumber(), messageDependency->getCauseMessageSendEntryNumber(),
                    causeMessageDependency->getCauseEventNumber(), causeMessageDependency->getCauseMessageSendEntryNumber(),
                    getEventNumber(), -1);
            }

            causeEvent = causeEvent->getCauseEvent();
        }
    }

    return cause;
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
    // returns a list of dependencies, where the consequence is this event,
    // and the other end is no further away than getMaxCauseDepth() and
    // no events in between match the filter
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
    // similar to getCause
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
    // used to build the linked list
    previousFilteredEvent->nextFilteredEventNumber = nextFilteredEvent->getEventNumber();
    nextFilteredEvent->previousFilteredEventNumber = previousFilteredEvent->getEventNumber();
}
