//=========================================================================
//  FILTEREDEVENT.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include "event.h"
#include "filteredevent.h"
#include "filteredeventlog.h"

namespace omnetpp {
namespace eventlog {

static FilteredMessageDependency::Kind getMessageDependencyKind(IMessageDependency *messageDependency)
{
    if (dynamic_cast<MessageSendDependency *>(messageDependency))
        return FilteredMessageDependency::SENDS;
    else if (dynamic_cast<MessageReuseDependency *>(messageDependency))
        return FilteredMessageDependency::REUSES;
    else
        return FilteredMessageDependency::MIXED;
}

FilteredEvent::FilteredEvent(FilteredEventLog *filteredEventLog, eventnumber_t eventNumber):
    filteredEventLog(filteredEventLog), eventNumber(eventNumber)
{
    clearInternalState();
}

void FilteredEvent::clearInternalState()
{
    causeEventNumber = -1;
    cause = nullptr;
    causes = nullptr;
    consequences = nullptr;
}

FilteredEvent::~FilteredEvent()
{
    deleteAllocatedObjects();
}

void FilteredEvent::deleteConsequences()
{
    if (consequences) {
        for (auto & consequence : *consequences)
            delete consequence;
        delete consequences;
        consequences = nullptr;
    }
}

void FilteredEvent::deleteAllocatedObjects()
{
    if (cause && !causes) {
        delete cause;
        cause = nullptr;
    }
    if (causes) {
        for (auto & cause : *causes)
            delete cause;
        delete causes;
        causes = nullptr;
    }
    deleteConsequences();
}

void FilteredEvent::synchronize(FileReader::FileChange change)
{
    if (change != FileReader::UNCHANGED) {
        switch (change) {
            case FileReader::OVERWRITTEN:
                deleteAllocatedObjects();
                clearInternalState();
                break;
            case FileReader::APPENDED:
                deleteConsequences();
                break;
            default:
                throw opp_runtime_error("Unknown file change");
        }
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
    if (!previousEvent && filteredEventLog->getFirstEvent() != this) {
        previousEvent = filteredEventLog->getMatchingEventInDirection(getEvent()->getPreviousEvent(), false);

        if (previousEvent)
            IEvent::linkEvents(previousEvent, this);
    }

    return (FilteredEvent *)previousEvent;
}

FilteredEvent *FilteredEvent::getNextEvent()
{
    if (!nextEvent && filteredEventLog->getLastEvent() != this) {
        nextEvent = filteredEventLog->getMatchingEventInDirection(getEvent()->getNextEvent(), true);

        if (nextEvent)
            Event::linkEvents(this, nextEvent);
    }

    return (FilteredEvent *)nextEvent;
}

FilteredEvent *FilteredEvent::getCauseEvent()
{
    if (causeEventNumber == -1) {
        IEvent *causeEvent = getEvent()->getCauseEvent();

        // LONG RUNNING OPERATION
        // walk backwards on the cause chain until we find an event matched by the filter
        // this might read all events backward if none of the causes matches the filter
        while (causeEvent) {
            filteredEventLog->progress();

            if (causeEvent->getEventNumber() < filteredEventLog->getFirstConsideredEventNumber())
                return nullptr;

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
    if (cause) {
        MessageDescriptionEntry *messageDescriptionEntry = cause->getBeginMessageDescriptionEntry();
        return dynamic_cast<BeginSendEntry *>(messageDescriptionEntry);
    }
    else
        return nullptr;
}

IMessageDependency *FilteredEvent::getCause()
{
    if (cause == nullptr) {
        IEvent *causeEvent = getEvent();
        IMessageDependency *causeMessageDependency = causeEvent->getCause();

        if (causeMessageDependency) {
            IMessageDependency *messageDependency;

            // LONG RUNNING OPERATION
            // this might read all events backward if none of the causes matches the filter
            while (causeEvent && (messageDependency = causeEvent->getCause())) {
                filteredEventLog->progress();

                if (causeEvent->getEventNumber() < filteredEventLog->getFirstConsideredEventNumber())
                    return nullptr;

                if (filteredEventLog->matchesFilter(messageDependency->getCauseEvent())) {
                    if (messageDependency == causeMessageDependency)
                        cause = messageDependency->duplicate(filteredEventLog);
                    else
                        cause = new FilteredMessageDependency(filteredEventLog, FilteredMessageDependency::SENDS,
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
    if (causes == nullptr) {
        // returns a list of dependencies, where the consequence is this event,
        // and the other end is no further away than getMaximumCauseDepth() and
        // no events in between match the filter
        long begin = clock();
        causes = new IMessageDependencyList();
        std::list<BreadthSearchItem> todoList;
        todoList.push_back(BreadthSearchItem(getEvent(), nullptr, FilteredMessageDependency::UNDEFINED, 0));

        // LONG RUNNING OPERATION
        // this is recursive and might take some time
        while (!todoList.empty()) {
            filteredEventLog->progress();
            if ((long)clock() - begin >= filteredEventLog->getMaximumCauseCollectionTime() * CLOCKS_PER_SEC / 1000)
                break;

            BreadthSearchItem searchItem = todoList.front();
            todoList.pop_front();

            // unpack
            IEvent *currentEvent = searchItem.event;
            IMessageDependency *endMessageDependency = searchItem.firstSeenMessageDependency;
            FilteredMessageDependency::Kind currentKind = searchItem.effectiveKind;
            int level = searchItem.level;

            IMessageDependencyList *eventCauses = currentEvent->getCauses();
            for (auto messageDependency : *eventCauses) {
                IEvent *causeEvent = messageDependency->getCauseEvent();

                if (causeEvent && (filteredEventLog->getCollectMessageReuses() || !dynamic_cast<MessageReuseDependency *>(messageDependency))) {
                    // printf("*** Checking at level %d for cause event number %ld\n", level, causeEvent->getEventNumber());
                    FilteredMessageDependency::Kind effectiveKind = (FilteredMessageDependency::Kind)((int)currentKind | (int)getMessageDependencyKind(messageDependency));
                    if (filteredEventLog->matchesFilter(causeEvent) &&
                        (level == 0 || IMessageDependency::corresponds(messageDependency, endMessageDependency)))
                    {
                        if (level == 0)
                            causes->push_back(messageDependency->duplicate(filteredEventLog));
                        else
                            pushNewFilteredMessageDependency(causes, effectiveKind, messageDependency, endMessageDependency);

                        if (countFilteredMessageDependencies(causes) == filteredEventLog->getMaximumNumberOfCauses())
                            return causes;
                    }
                    else if (level < filteredEventLog->getMaximumCauseDepth())
                        todoList.push_back(BreadthSearchItem(causeEvent, level == 0 ? messageDependency : endMessageDependency, effectiveKind, level + 1));
                }
            }
        }
    }

    return causes;
}

IMessageDependencyList *FilteredEvent::getConsequences()
{
    if (consequences == nullptr) {
        // similar to getCause
        long begin = clock();
        consequences = new IMessageDependencyList();
        std::list<BreadthSearchItem> todoList;
        todoList.push_back(BreadthSearchItem(getEvent(), nullptr, FilteredMessageDependency::UNDEFINED, 0));

        // LONG RUNNING OPERATION
        // this is recursive and might take some time
        while (!todoList.empty()) {
            filteredEventLog->progress();
            if ((long)clock() - begin >= filteredEventLog->getMaximumConsequenceCollectionTime() * CLOCKS_PER_SEC / 1000)
                break;

            BreadthSearchItem searchItem = todoList.front();
            todoList.pop_front();

            // unpack
            IEvent *currentEvent = searchItem.event;
            IMessageDependency *beginMessageDependency = searchItem.firstSeenMessageDependency;
            FilteredMessageDependency::Kind currentKind = searchItem.effectiveKind;
            int level = searchItem.level;

            IMessageDependencyList *eventConsequences = currentEvent->getConsequences();
            for (auto messageDependency : *eventConsequences) {
                IEvent *consequenceEvent = messageDependency->getConsequenceEvent();

                if (consequenceEvent && (filteredEventLog->getCollectMessageReuses() || !dynamic_cast<MessageReuseDependency *>(messageDependency))) {
                    // printf("*** Checking at level %d for consequence event number %ld\n", level, consequenceEvent->getEventNumber());
                    FilteredMessageDependency::Kind effectiveKind = (FilteredMessageDependency::Kind)((int)currentKind | (int)getMessageDependencyKind(messageDependency));
                    if (filteredEventLog->matchesFilter(consequenceEvent) &&
                        (level == 0 || IMessageDependency::corresponds(beginMessageDependency, messageDependency)))
                    {
                        if (level == 0)
                            consequences->push_back(messageDependency->duplicate(filteredEventLog));
                        else
                            pushNewFilteredMessageDependency(consequences, effectiveKind, beginMessageDependency, messageDependency);

                        if (countFilteredMessageDependencies(consequences) == filteredEventLog->getMaximumNumberOfConsequences())
                            return consequences;
                    }
                    else if (level < filteredEventLog->getMaximumConsequenceDepth())
                        todoList.push_back(BreadthSearchItem(consequenceEvent, level == 0 ? messageDependency : beginMessageDependency, effectiveKind, level + 1));
                }
            }
        }
    }

    return consequences;
}

int FilteredEvent::countFilteredMessageDependencies(IMessageDependencyList *messageDependencies)
{
    int count = 0;
    for (auto & messageDependencie : *messageDependencies)
        if (dynamic_cast<FilteredMessageDependency *>(messageDependencie))
            count++;

    return count;
}

void FilteredEvent::pushNewFilteredMessageDependency(IMessageDependencyList *messageDependencies, FilteredMessageDependency::Kind kind, IMessageDependency *beginMessageDependency, IMessageDependency *endMessageDependency)
{
    FilteredMessageDependency *newMessageDependency = new FilteredMessageDependency(filteredEventLog, kind,
            beginMessageDependency->duplicate(filteredEventLog->getEventLog()),
            endMessageDependency->duplicate(filteredEventLog->getEventLog()));

    for (auto & messageDependencie : *messageDependencies) {
        if (messageDependencie->equals(newMessageDependency)) {
            delete newMessageDependency;
            return;
        }
    }

    messageDependencies->push_back(newMessageDependency);
}

}  // namespace eventlog
}  // namespace omnetpp

