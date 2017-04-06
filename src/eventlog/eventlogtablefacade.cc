//=========================================================================
//  EVENTLOGTABLEFACADE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include "ievent.h"
#include "ieventlog.h"
#include "event.h"
#include "eventlogtablefacade.h"

namespace omnetpp {
namespace eventlog {

EventLogTableFacade::EventLogTableFacade(IEventLog *eventLog) : EventLogFacade(eventLog)
{
    setCustomFilter("*");
    setFilterMode(ALL_ENTRIES);
}

void EventLogTableFacade::clearInternalState()
{
    approximateNumberOfEntries = -1;
    lastMatchedEventNumber = -1;
    lastNumMatchingEventLogEntries = -1;
}

void EventLogTableFacade::synchronize(FileReader::FileChangedState change)
{
    if (change != FileReader::UNCHANGED) {
        EventLogFacade::synchronize(change);
        clearInternalState();
    }
}

void EventLogTableFacade::setCustomFilter(const char *pattern)
{
    customFilter = pattern;
    matchExpression.setPattern((std::string("E or (") + customFilter + ")").c_str(), false, true, false);
    clearInternalState();
}

void EventLogTableFacade::setFilterMode(EventLogTableFilterMode filterMode)
{
    this->filterMode = filterMode;
    clearInternalState();
}

bool EventLogTableFacade::matchesFilter(EventLogEntry *eventLogEntry)
{
    switch (filterMode) {
        case ALL_ENTRIES:
            return true;

        case EVENT_AND_SEND_AND_MESSAGE_ENTRIES:
            return dynamic_cast<EventEntry *>(eventLogEntry) ||
                   dynamic_cast<BeginSendEntry *>(eventLogEntry) ||
                   dynamic_cast<EventLogMessageEntry *>(eventLogEntry);

        case EVENT_AND_MESSAGE_ENTRIES:
            return dynamic_cast<EventEntry *>(eventLogEntry) ||
                   dynamic_cast<EventLogMessageEntry *>(eventLogEntry);

        case EVENT_ENTRIES:
            return dynamic_cast<EventEntry *>(eventLogEntry);

        case CUSTOM_ENTRIES:
            return matchExpression.matches(eventLogEntry);

        default:
            throw opp_runtime_error("Unknown event log table filter");
    }
}

int EventLogTableFacade::getNumMatchingEventLogEntries(IEvent *event)
{
    if (lastMatchedEventNumber == event->getEventNumber())
        return lastNumMatchingEventLogEntries;
    else {
        lastMatchedEventNumber = event->getEventNumber();

        switch (filterMode) {
            case ALL_ENTRIES:
                lastNumMatchingEventLogEntries = event->getNumEventLogEntries();
                break;

            case EVENT_AND_SEND_AND_MESSAGE_ENTRIES:
                lastNumMatchingEventLogEntries = event->getNumBeginSendEntries() + event->getNumEventLogMessages() + 1;
                break;

            case EVENT_AND_MESSAGE_ENTRIES:
                lastNumMatchingEventLogEntries = event->getNumEventLogMessages() + 1;
                break;

            case EVENT_ENTRIES:
                lastNumMatchingEventLogEntries = 1;
                break;

            case CUSTOM_ENTRIES: {
                int count = 0;
                int num = event->getNumEventLogEntries();

                for (int i = 0; i < num; i++)
                    if (matchesFilter(event->getEventLogEntry(i)))
                        count++;


                lastNumMatchingEventLogEntries = count;
                break;
            }

            default:
                throw opp_runtime_error("Unknown event log table filter");
        }

        return lastNumMatchingEventLogEntries;
    }
}

EventLogEntry *EventLogTableFacade::getEventLogEntry(eventnumber_t eventNumber, int eventLogEntryIndex)
{
    IEvent *event = eventLog->getEventForEventNumber(eventNumber);
    Assert(event);

    return event->getEventLogEntry(eventLogEntryIndex);
}

EventLogEntry *EventLogTableFacade::getFirstEntry()
{
    IEvent *event = eventLog->getFirstEvent();

    if (!event)
        return nullptr;
    else
        return getEntryInEvent(event, 0);
}

EventLogEntry *EventLogTableFacade::getLastEntry()
{
    IEvent *event = eventLog->getLastEvent();

    if (!event)
        return nullptr;
    else
        return getEntryInEvent(event, getNumMatchingEventLogEntries(event) - 1);
}

EventLogEntry *EventLogTableFacade::getEntryAndDistance(EventLogEntry *sourceEventLogEntry, EventLogEntry *targetEventLogEntry, eventnumber_t distance, eventnumber_t& reachedDistance)
{
    Assert(sourceEventLogEntry);
    EventLogEntry *eventLogEntry = sourceEventLogEntry;
    reachedDistance = 0;

    int index = getEntryIndexInEvent(eventLogEntry);
    Assert(index >= 0);

    while (distance && eventLogEntry && eventLogEntry != targetEventLogEntry) {
        if (distance > 0) {
            eventLogEntry = getNextEntry(eventLogEntry, index);
            distance--;
            reachedDistance++;
        }
        else {
            eventLogEntry = getPreviousEntry(eventLogEntry, index);
            distance++;
            reachedDistance--;
        }
    }

    if (!eventLogEntry)
        reachedDistance += distance;

    return eventLogEntry;
}

EventLogEntry *EventLogTableFacade::getClosestEntryInEvent(EventLogEntry *eventLogEntry)
{
    Assert(eventLogEntry);
    IEvent *event = eventLogEntry->getEvent();

    for (int i = eventLogEntry->getEntryIndex(); i >= 0; i--) {
        eventLogEntry = event->getEventLogEntry(i);

        if (matchesFilter(eventLogEntry))
            return eventLogEntry;
    }

    throw opp_runtime_error("EventEntry should always match the filter");
}

EventLogEntry *EventLogTableFacade::getPreviousEntry(EventLogEntry *eventLogEntry, int& index)
{
    IEvent *event = eventLog->getEventForEventNumber(eventLogEntry->getEvent()->getEventNumber());
    index--;

    if (index == -1) {
        event = event->getPreviousEvent();

        if (event)
            index = getNumMatchingEventLogEntries(event) - 1;
    }

    if (!event)
        return nullptr;
    else
        return getEntryInEvent(event, index);
}

EventLogEntry *EventLogTableFacade::getNextEntry(EventLogEntry *eventLogEntry, int& index)
{
    IEvent *event = eventLog->getEventForEventNumber(eventLogEntry->getEvent()->getEventNumber());
    index++;

    if (index == getNumMatchingEventLogEntries(event)) {
        event = event->getNextEvent();
        index = 0;
    }

    if (!event)
        return nullptr;
    else
        return getEntryInEvent(event, index);
}

EventLogEntry *EventLogTableFacade::getEntryInEvent(IEvent *event, int index)
{
    Assert(index >= 0 && index < getNumMatchingEventLogEntries(event));

    if (filterMode == ALL_ENTRIES)
        return event->getEventLogEntry(index);
    else {
        int num = event->getNumEventLogEntries();

        for (int i = 0; i < num; i++) {
            EventLogEntry *eventLogEntry = event->getEventLogEntry(i);

            if (matchesFilter(eventLogEntry)) {
                if (index == 0)
                    return eventLogEntry;
                else
                    index--;
            }
        }
    }

    throw opp_runtime_error("No event log entry with index: %d in event: %" PRId64, index, event->getEventNumber());
}

int EventLogTableFacade::getEntryIndexInEvent(EventLogEntry *eventLogEntry)
{
    IEvent *event = eventLogEntry->getEvent();
    int index = 0;
    int num = event->getNumEventLogEntries();

    for (int i = 0; i < num; i++) {
        EventLogEntry *currentEventLogEntry = event->getEventLogEntry(i);

        if (matchesFilter(currentEventLogEntry)) {
            if (currentEventLogEntry == eventLogEntry)
                return index;
            else
                index++;
        }
    }

    throw opp_runtime_error("No event log entry found in event: %" PRId64, event->getEventNumber());
}

eventnumber_t EventLogTableFacade::getDistanceToEntry(EventLogEntry *sourceEventLogEntry, EventLogEntry *targetEventLogEntry, eventnumber_t limit)
{
    eventnumber_t reachedDistance;
    getEntryAndDistance(sourceEventLogEntry, targetEventLogEntry, limit, reachedDistance);
    return reachedDistance;
}

eventnumber_t EventLogTableFacade::getDistanceToFirstEntry(EventLogEntry *eventLogEntry, eventnumber_t limit)
{
    eventnumber_t reachedDistance;
    getEntryAndDistance(eventLogEntry, getFirstEntry(), -limit, reachedDistance);
    return -reachedDistance;
}

eventnumber_t EventLogTableFacade::getDistanceToLastEntry(EventLogEntry *eventLogEntry, eventnumber_t limit)
{
    eventnumber_t reachedDistance;
    getEntryAndDistance(eventLogEntry, getLastEntry(), limit, reachedDistance);
    return reachedDistance;
}

EventLogEntry *EventLogTableFacade::getNeighbourEntry(EventLogEntry *eventLogEntry, eventnumber_t distance)
{
    eventnumber_t reachedDistance;
    return getEntryAndDistance(eventLogEntry, nullptr, distance, reachedDistance);
}

double EventLogTableFacade::getApproximatePercentageForEntry(EventLogEntry *eventLogEntry)
{
    IEvent *beforeEvent = eventLogEntry->getEvent();
    IEvent *afterEvent = beforeEvent->getNextEvent();
    double beforePercentage = eventLog->getApproximatePercentageForEventNumber(beforeEvent->getEventNumber());
    double afterPercentage = afterEvent ? eventLog->getApproximatePercentageForEventNumber(afterEvent->getEventNumber()) : 1.0;
    int index = getEntryIndexInEvent(eventLogEntry);

    return beforePercentage + (afterPercentage - beforePercentage) * index / getNumMatchingEventLogEntries(beforeEvent);
}

EventLogEntry *EventLogTableFacade::getApproximateEventLogEntryAt(double percentage)
{
    Assert(0.0 <= percentage && percentage <= 1.0);
    if (percentage == 1) {
        IEvent *event = eventLog->getLastEvent();

        if (!event)
            return nullptr;
        else
            return getEntryInEvent(event, getNumMatchingEventLogEntries(event) - 1);
    }
    else {
        IEvent *beforeEvent = eventLog->getApproximateEventAt(percentage);

        if (!beforeEvent)
            return nullptr;
        else {
            double beforePercentage = eventLog->getApproximatePercentageForEventNumber(beforeEvent->getEventNumber());
            double afterPercentage = 1;
            IEvent *afterEvent = beforeEvent->getNextEvent();

            if (afterEvent)
                afterPercentage = eventLog->getApproximatePercentageForEventNumber(afterEvent->getEventNumber());

            if (percentage < beforePercentage || percentage > afterPercentage)
                return beforeEvent->getEventEntry();
            else {
                double entryPercentage = (percentage - beforePercentage) / (afterPercentage - beforePercentage);
                Assert(0.0 <= entryPercentage && entryPercentage <= 1.0);
                int index = (int)floor((getNumMatchingEventLogEntries(beforeEvent) - 1) * entryPercentage);
                return getEntryInEvent(beforeEvent, index);
            }
        }
    }
}

eventnumber_t EventLogTableFacade::getApproximateNumberOfEntries()
{
    if (approximateNumberOfEntries == -1) {
        IEvent *firstEvent = eventLog->getFirstEvent();
        IEvent *lastEvent = eventLog->getLastEvent();

        if (!firstEvent)
            approximateNumberOfEntries = 0;
        else {
            long sum = 0;
            long count = 0;
            int eventCount = 100;

            for (int i = 0; i < eventCount; i++) {
                if (firstEvent) {
                    sum += getNumMatchingEventLogEntries(firstEvent);
                    count++;
                    firstEvent = firstEvent->getNextEvent();
                }

                if (lastEvent) {
                    sum += getNumMatchingEventLogEntries(lastEvent);
                    count++;
                    lastEvent = lastEvent->getPreviousEvent();
                }
            }

            double average = (double)sum / count;
            approximateNumberOfEntries = (long)(eventLog->getApproximateNumberOfEvents() * average);
        }
    }

    return approximateNumberOfEntries;
}

} // namespace eventlog
}  // namespace omnetpp

