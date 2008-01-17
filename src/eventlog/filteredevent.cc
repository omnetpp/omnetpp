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

USING_NAMESPACE

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
        for (IMessageDependencyList::iterator it = causes->begin(); it != causes->end(); it++)
            delete *it;
        delete causes;
    }

    if (consequences)
    {
        for (IMessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++)
            delete *it;
        delete consequences;
    }
}

void FilteredEvent::synchronize()
{
    if (consequences)
    {
        for (IMessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++)
            delete *it;
        delete consequences;

        consequences = NULL;
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
        previousEvent = filteredEventLog->getMatchingEventInDirection(getEvent()->getPreviousEvent(), false);

        if (previousEvent)
            IEvent::linkEvents(previousEvent, this);
    }

    return (FilteredEvent *)previousEvent;
}

FilteredEvent *FilteredEvent::getNextEvent()
{
    if (!nextEvent && filteredEventLog->getLastEvent() != this)
    {
        nextEvent = filteredEventLog->getMatchingEventInDirection(getEvent()->getNextEvent(), true);

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

BeginSendEntry *FilteredEvent::getCauseBeginSendEntry()
{
    IMessageDependency *cause = getCause();
    if (cause)
        return cause->getBeginSendEntry();
    else
        return NULL;
}

IMessageDependency *FilteredEvent::getCause()
{
    if (cause == NULL)
    {
        IEvent *causeEvent = getEvent();
        IMessageDependency *causeMessageDependency = causeEvent->getCause();

        if (causeMessageDependency) {
            IMessageDependency *messageDependency;

            while (causeEvent && (messageDependency = causeEvent->getCause()))
            {
                if (filteredEventLog->matchesFilter(messageDependency->getCauseEvent()))
                {
                    if (messageDependency == causeMessageDependency)
                        cause = messageDependency->duplicate(filteredEventLog);
                    else
                        cause = new FilteredMessageDependency(filteredEventLog, false,
                                    messageDependency->duplicate(filteredEventLog->getEventLog()),
                                    causeMessageDependency->duplicate(filteredEventLog->getEventLog()));
                }

                causeEvent = causeEvent->getCauseEvent();
            }
        }
    }

    return cause;
}

IMessageDependencyList *FilteredEvent::getCauses()
{
    if (causes == NULL)
    {
        causes = new IMessageDependencyList();
        getCauses(getEvent(), NULL, false, 0);
    }

    return causes;
}

IMessageDependencyList *FilteredEvent::getCauses(IEvent *event, IMessageDependency *endMessageDependency, bool isReuse, int level)
{
    // returns a list of dependencies, where the consequence is this event,
    // and the other end is no further away than getMaximumCauseDepth() and
    // no events in between match the filter
    IMessageDependencyList *eventCauses = event->getCauses();

    for (IMessageDependencyList::iterator it = eventCauses->begin(); it != eventCauses->end(); it++)
    {
        IMessageDependency *messageDependency = *it;
        IEvent *causeEvent = messageDependency->getCauseEvent();

        //printf("*** Checking at level %d for cause event number %ld\n", level, causeEvent->getEventNumber());

        // TODO: take care about maximum number of dependencies
        if (filteredEventLog->matchesFilter(causeEvent)) {
            if (level == 0)
                causes->push_back(messageDependency->duplicate(filteredEventLog));
            else
                causes->push_back(
                    new FilteredMessageDependency(filteredEventLog, isReuse,
                        messageDependency->duplicate(filteredEventLog->getEventLog()),
                        endMessageDependency->duplicate(filteredEventLog->getEventLog())));
        }
        else if (level < filteredEventLog->getMaximumCauseDepth() && !messageDependency->getIsReuse())
            getCauses(causeEvent,
                level == 0 ? messageDependency : endMessageDependency,
                isReuse | messageDependency->getIsReuse(),
                level + 1);
    }

    return causes;
}

IMessageDependencyList *FilteredEvent::getConsequences()
{
    if (consequences == NULL)
    {
        consequences = new IMessageDependencyList();
        getConsequences(getEvent(), NULL, false, 0);
    }

    return consequences;
}

IMessageDependencyList *FilteredEvent::getConsequences(IEvent *event, IMessageDependency *beginMessageDependency, bool isReuse, int level)
{
    // similar to getCause
    IMessageDependencyList *eventConsequences = event->getConsequences();

    for (IMessageDependencyList::iterator it = eventConsequences->begin(); it != eventConsequences->end(); it++)
    {
        IMessageDependency *messageDependency = *it;
        IEvent *consequenceEvent = messageDependency->getConsequenceEvent();

        if (consequenceEvent == NULL)
            continue; // skip cancelled self messages

        //printf("*** Checking at level %d for consequence event number %ld\n", level, consequenceEvent->getEventNumber());

        // TODO: take care about maximum number of dependencies
        if (filteredEventLog->matchesFilter(consequenceEvent)) {
            if (level == 0)
                consequences->push_back(messageDependency->duplicate(filteredEventLog));
            else
                consequences->push_back(
                    new FilteredMessageDependency(filteredEventLog, isReuse,
                        beginMessageDependency->duplicate(filteredEventLog->getEventLog()),
                        messageDependency->duplicate(filteredEventLog->getEventLog())));
        }
        else if (level < filteredEventLog->getMaximumConsequenceDepth() && !messageDependency->getIsReuse())
            getConsequences(consequenceEvent,
                level == 0 ? messageDependency : beginMessageDependency,
                isReuse | messageDependency->getIsReuse(),
                level + 1);
    }

    return consequences;
}
