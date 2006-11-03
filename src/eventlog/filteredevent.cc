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

#include "event.h"
#include "filteredevent.h"
#include "filteredeventlog.h"

FilteredEvent::FilteredEvent(FilteredEventLog *filteredEventLog, long eventNumber)
{
    this->eventNumber = eventNumber;
    this->filteredEventLog = filteredEventLog;

    causeEventNumber = -1;
    cause = NULL;
    causes = NULL;
    consequences = NULL;
}

FilteredEvent::~FilteredEvent()
{
    delete cause;

    if (causes)
    {
        for (MessageDependencyList::iterator it = causes->begin(); it != causes->end(); it++)
            delete *it;
        delete causes;
    }

    if (consequences)
    {
        for (MessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++)
            delete *it;
        delete consequences;
    }
}

IEventLog *FilteredEvent::getEventLog()
{
    return filteredEventLog;
}

IEvent *FilteredEvent::getEvent()
{
    return filteredEventLog->getEventLog()->getEventForEventNumber(eventNumber);
}

FilteredEvent *FilteredEvent::getPreviousEvent()
{
    if (!previousEvent && filteredEventLog->getFirstEvent() != this)
    {
        previousEvent = filteredEventLog->getMatchingEventInDirection(eventNumber - 1, false);

        if (previousEvent)
            IEvent::linkEvents(previousEvent, this);
    }

    return (FilteredEvent *)previousEvent;
}

FilteredEvent *FilteredEvent::getNextEvent()
{
    if (!nextEvent && filteredEventLog->getLastEvent() != this)
    {
        nextEvent = filteredEventLog->getMatchingEventInDirection(eventNumber + 1, true);

        if (nextEvent)
            Event::linkEvents(this, nextEvent);
    }

    return (FilteredEvent *)nextEvent;
}

FilteredEvent *FilteredEvent::getCauseEvent()
{
    if (causeEventNumber == -1)
    {
        IEvent *causeEvent = getEvent()->getCauseEvent();

        // walk backwards on the cause chain until we find an event matched by the filter
        while (causeEvent)
        {
            if (filteredEventLog->matchesFilter(causeEvent))
                return filteredEventLog->getEventForEventNumber(causeEvent->getEventNumber());

            causeEvent = causeEvent->getCauseEvent();
        }
    }

    return filteredEventLog->getEventForEventNumber(causeEventNumber);
}

MessageDependency *FilteredEvent::getCause()
{
    if (cause == NULL)
    {
        IEvent *causeEvent = getEvent();
        MessageDependency *causeMessageDependency = causeEvent->getCause();

        if (causeMessageDependency) {
            MessageDependency *messageDependency;

            while (causeEvent && (messageDependency = causeEvent->getCause()))
            {
                if (filteredEventLog->matchesFilter(messageDependency->getCauseEvent()))
                {
                    if (messageDependency == causeMessageDependency)
                        cause = messageDependency->duplicate();
                    else
                        cause = new FilteredMessageDependency(filteredEventLog->getEventLog(),
                            messageDependency->getCauseEventNumber(), messageDependency->getCauseBeginSendEntryNumber(),
                            causeMessageDependency->getCauseEventNumber(), causeMessageDependency->getCauseBeginSendEntryNumber(),
                            getEventNumber(), -1);
                }

                causeEvent = causeEvent->getCauseEvent();
            }
        }
    }

    return cause;
}

MessageDependencyList *FilteredEvent::getCauses()
{
    if (causes == NULL)
    {
        causes = new MessageDependencyList();
        getCauses(getEvent(), -1, 0);
    }

    return causes;
}

MessageDependencyList *FilteredEvent::getCauses(IEvent *event, int consequenceBeginSendEntryNumber, int level)
{
    // returns a list of dependencies, where the consequence is this event,
    // and the other end is no further away than getMaxCauseDepth() and
    // no events in between match the filter
    MessageDependencyList *eventCauses = event->getCauses();

    for (MessageDependencyList::iterator it = eventCauses->begin(); it != eventCauses->end(); it++)
    {
        MessageDependency *messageDependency = *it;
        IEvent *causeEvent = messageDependency->getCauseEvent();

        //printf("*** Checking at level %d for cause event number %ld\n", level, causeEvent->getEventNumber());

        if (filteredEventLog->matchesFilter(causeEvent))
            if (level == 0)
                causes->push_back(messageDependency->duplicate());
            else
                causes->push_back(new FilteredMessageDependency(filteredEventLog->getEventLog(),
                    messageDependency->getCauseEventNumber(), messageDependency->getCauseBeginSendEntryNumber(),
                    messageDependency->getConsequenceEventNumber(), messageDependency->getConsequenceBeginSendEntryNumber(),
                    getEventNumber(), consequenceBeginSendEntryNumber));
        else if (level < filteredEventLog->getMaxCauseDepth())
            getCauses(causeEvent,
                level == 0 ? messageDependency->getConsequenceBeginSendEntryNumber() : consequenceBeginSendEntryNumber,
                level + 1);
    }

    return causes;
}

MessageDependencyList *FilteredEvent::getConsequences()
{
    if (consequences == NULL)
    {
        consequences = new MessageDependencyList();
        getConsequences(getEvent(), -1, 0);
    }

    return consequences;
}

MessageDependencyList *FilteredEvent::getConsequences(IEvent *event, int causeBeginSendEntryNumber, int level)
{
    // similar to getCause
    MessageDependencyList *eventConsequences = event->getConsequences();

    for (MessageDependencyList::iterator it = eventConsequences->begin(); it != eventConsequences->end(); it++)
    {
        MessageDependency *messageDependency = *it;
        IEvent *consequenceEvent = messageDependency->getConsequenceEvent();

        if (consequenceEvent == NULL)
            continue; // skip cancelled self messages

        //printf("*** Checking at level %d for consequence event number %ld\n", level, consequenceEvent->getEventNumber());

        if (filteredEventLog->matchesFilter(consequenceEvent))
            if (level == 0)
                consequences->push_back(messageDependency->duplicate());
            else
                consequences->push_back(new FilteredMessageDependency(filteredEventLog->getEventLog(),
                    getEventNumber(), causeBeginSendEntryNumber,
                    messageDependency->getCauseEventNumber(), messageDependency->getCauseBeginSendEntryNumber(),
                    messageDependency->getConsequenceEventNumber(), messageDependency->getConsequenceBeginSendEntryNumber()));
        else if (level < filteredEventLog->getMaxConsequenceDepth())
            getConsequences(consequenceEvent,
                level == 0 ? messageDependency->getCauseBeginSendEntryNumber() : causeBeginSendEntryNumber,
                level + 1);
    }

    return consequences;
}
