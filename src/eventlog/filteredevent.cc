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

        // TODO: LONG RUNNING OPERATION
        // walk backwards on the cause chain until we find an event matched by the filter
        // this might read all events backward if none of the causes matches the filter
        while (causeEvent)
        {
            if (causeEvent->getEventNumber() < filteredEventLog->getFirstEventNumber())
                return NULL;

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

            // TODO: LONG RUNNING OPERATION
            // this might read all events backward if none of the causes matches the filter
            while (causeEvent && (messageDependency = causeEvent->getCause()))
            {
                if (causeEvent->getEventNumber() < filteredEventLog->getFirstEventNumber())
                    return NULL;

                if (filteredEventLog->matchesFilter(messageDependency->getCauseEvent()))
                {
                    if (messageDependency == causeMessageDependency)
                        cause = messageDependency->duplicate(filteredEventLog);
                    else
                        cause = new FilteredMessageDependency(filteredEventLog, false,
                                    messageDependency->duplicate(filteredEventLog->getEventLog()),
                                    causeMessageDependency->duplicate(filteredEventLog->getEventLog()));
                    break;
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

// TODO: LONG RUNNING OPERATION
// this is recursive and might take some time
void FilteredEvent::getCauses(IEvent *event, IMessageDependency *endMessageDependency, bool isReuse, int level)
{
    // returns a list of dependencies, where the consequence is this event,
    // and the other end is no further away than getMaximumCauseDepth() and
    // no events in between match the filter
    IMessageDependencyList *eventCauses = event->getCauses();

    for (IMessageDependencyList::iterator it = eventCauses->begin(); it != eventCauses->end(); it++)
    {
        IMessageDependency *messageDependency = *it;
        IEvent *causeEvent = messageDependency->getCauseEvent();

        if (causes->size() == filteredEventLog->getMaximumNumberOfCauses())
            return;

        if (causeEvent && (filteredEventLog->getCollectMessageReuses() || !messageDependency->getIsReuse())) {
            //printf("*** Checking at level %d for cause event number %ld\n", level, causeEvent->getEventNumber());
            // TODO: take care about maximum number of dependencies
            bool effectiveIsReuse = isReuse || messageDependency->getIsReuse();
            if (filteredEventLog->matchesFilter(causeEvent) &&
                (level == 0 || IMessageDependency::corresponds(messageDependency, endMessageDependency)))
            {
                if (level == 0)
                    causes->push_back(messageDependency->duplicate(filteredEventLog));
                else
                    pushNewFilteredMessageDependency(causes, effectiveIsReuse, messageDependency, endMessageDependency);
            }
            else if (level < filteredEventLog->getMaximumCauseDepth())
                getCauses(causeEvent,
                    level == 0 ? messageDependency : endMessageDependency,
                    effectiveIsReuse,
                    level + 1);
        }
    }
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

// TODO: LONG RUNNING OPERATION
// this is recursive and might take some time
void FilteredEvent::getConsequences(IEvent *event, IMessageDependency *beginMessageDependency, bool isReuse, int level)
{
    // similar to getCause
    IMessageDependencyList *eventConsequences = event->getConsequences();

    for (IMessageDependencyList::iterator it = eventConsequences->begin(); it != eventConsequences->end(); it++)
    {
        IMessageDependency *messageDependency = *it;
        IEvent *consequenceEvent = messageDependency->getConsequenceEvent();

        if (consequences->size() == filteredEventLog->getMaximumNumberOfConsequences())
            return;

        if (consequenceEvent && (filteredEventLog->getCollectMessageReuses() || !messageDependency->getIsReuse())) {
            //printf("*** Checking at level %d for consequence event number %ld\n", level, consequenceEvent->getEventNumber());
            // TODO: take care about maximum number of dependencies
            bool effectiveIsReuse = isReuse | messageDependency->getIsReuse();
            if (filteredEventLog->matchesFilter(consequenceEvent) &&
                (level == 0 || IMessageDependency::corresponds(beginMessageDependency, messageDependency)))
            {
                if (level == 0)
                    consequences->push_back(messageDependency->duplicate(filteredEventLog));
                else
                    pushNewFilteredMessageDependency(consequences, effectiveIsReuse, beginMessageDependency, messageDependency);
            }
            else if (level < filteredEventLog->getMaximumConsequenceDepth())
                getConsequences(consequenceEvent,
                    level == 0 ? messageDependency : beginMessageDependency,
                    effectiveIsReuse,
                    level + 1);
        }
    }
}

void FilteredEvent::pushNewFilteredMessageDependency(IMessageDependencyList *messageDependencies, bool isReuse, IMessageDependency *beginMessageDependency, IMessageDependency *endMessageDependency)
{
    FilteredMessageDependency *newMessageDependency = new FilteredMessageDependency(filteredEventLog, isReuse,
            beginMessageDependency->duplicate(filteredEventLog->getEventLog()),
            endMessageDependency->duplicate(filteredEventLog->getEventLog()));

    for (IMessageDependencyList::iterator it = messageDependencies->begin(); it != messageDependencies->end(); it++) {
        if ((*it)->equals(newMessageDependency)) {
            delete newMessageDependency;
            return;
        }
    }

    messageDependencies->push_back(newMessageDependency);
}


