//=========================================================================
//  EVENTLOGTABLEFACADE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "ievent.h"
#include "ieventlog.h"
#include "event.h"
#include "eventlogtablefacade.h"

EventLogTableFacade::EventLogTableFacade(IEventLog *eventLog) : EventLogFacade(eventLog)
{
    approximateNumberOfEntries = -1;
}

EventLogEntry *EventLogTableFacade::getFirstEntry()
{
    IEvent *event = eventLog->getFirstEvent();

    if (!event)
        return NULL;
    else
        return getEntryInEvent(event, 0);
}

EventLogEntry *EventLogTableFacade::getLastEntry()
{
    IEvent *event = eventLog->getLastEvent();

    if (!event)
        return NULL;
    else
        return getEntryInEvent(event, event->getNumEventLogMessages());
}

EventLogEntry *EventLogTableFacade::getEntryAndDistance(EventLogEntry *sourceEventLogEntry, EventLogEntry *targetEventLogEntry, long distance, long& reachedDistance)
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

            if (eventLogEntry)
                reachedDistance++;
        }
        else {
            eventLogEntry = getPreviousEntry(eventLogEntry, index);
            distance++;

            if (eventLogEntry)
                reachedDistance--;
        }
    }

    return eventLogEntry;
}

EventLogEntry *EventLogTableFacade::getPreviousEntry(EventLogEntry *eventLogEntry, int& index)
{
    IEvent *event = eventLogEntry->getEvent();
    index--;

    if (index == -1) {
        event = event->getPreviousEvent();

        if (event)
            index = event->getNumEventLogMessages();
    }

    if (!event)
        return NULL;
    else
        return getEntryInEvent(event, index);
}

EventLogEntry *EventLogTableFacade::getNextEntry(EventLogEntry *eventLogEntry, int& index)
{
    IEvent *event = eventLogEntry->getEvent();
    index++;

    if (index == event->getNumEventLogMessages() + 1) {
        event = event->getNextEvent();
        index = 0;
    }

    if (!event)
        return NULL;
    else
        return getEntryInEvent(event, index);
}

EventLogEntry *EventLogTableFacade::getEntryInEvent(IEvent *event, int index)
{
    Assert(index >= 0 && index <= event->getNumEventLogMessages());

    if (index == 0)
        return event->getEventEntry();
    else
        return event->getEventLogMessage(index - 1);
}

int EventLogTableFacade::getEntryIndexInEvent(EventLogEntry *eventLogEntry)
{
    if (dynamic_cast<EventEntry *>(eventLogEntry))
        return 0;
    else
        return eventLogEntry->getEvent()->getEventLogMessageIndex((EventLogMessage *)eventLogEntry) + 1;
}

long EventLogTableFacade::getDistanceToEntry(EventLogEntry *sourceEventLogEntry, EventLogEntry *targetEventLogEntry, long limit)
{
    long reachedDistance;
    getEntryAndDistance(sourceEventLogEntry, targetEventLogEntry, limit, reachedDistance);
    return reachedDistance;
}

long EventLogTableFacade::getDistanceToFirstEntry(EventLogEntry *eventLogEntry, long limit)
{
    long reachedDistance;
    getEntryAndDistance(eventLogEntry, NULL, -limit, reachedDistance);
    return -reachedDistance;
}

long EventLogTableFacade::getDistanceToLastEntry(EventLogEntry *eventLogEntry, long limit)
{
    long reachedDistance;
    getEntryAndDistance(eventLogEntry, NULL, limit, reachedDistance);
    return reachedDistance;
}

EventLogEntry *EventLogTableFacade::getNeighbourEntry(EventLogEntry *eventLogEntry, long distance)
{
    long reachedDistance;
    return getEntryAndDistance(eventLogEntry, NULL, distance, reachedDistance);
}

double EventLogTableFacade::getApproximatePercentageForEntry(EventLogEntry *eventLogEntry)
{
    return eventLog->getApproximatePercentageForEventNumber(eventLogEntry->getEvent()->getEventNumber());
}

EventLogEntry *EventLogTableFacade::getApproximateEventLogEntryTableAt(double percentage)
{
    if (percentage == 1) {
        IEvent* event = eventLog->getLastEvent();

        if (!event)
            return NULL;
        else if (event->getNumEventLogMessages() != 0)
            return event->getEventLogMessage(event->getNumEventLogMessages() - 1);
        else
            return event->getEventEntry();
    }
    else
        return eventLog->getApproximateEventAt(percentage)->getEventEntry();
}

long EventLogTableFacade::getApproximateNumberOfEntries()
{
    if (approximateNumberOfEntries == -1)
    {
        IEvent *firstEvent = eventLog->getFirstEvent();
        IEvent *lastEvent = eventLog->getLastEvent();

        if (!firstEvent)
            approximateNumberOfEntries = 0;
        else
        {
            long sum = 0;
            long count = 0;
            int eventCount = 100;

            for (int i = 0; i < eventCount; i++)
            {
                if (firstEvent) {
                    sum += firstEvent->getNumEventLogMessages() + 1;
                    count++;
                    firstEvent = firstEvent->getNextEvent();
                }

                if (lastEvent) {
                    sum += lastEvent->getNumEventLogMessages() + 1;
                    count++;
                    lastEvent = lastEvent->getPreviousEvent();
                }
            }

            double average = (double)sum / count;
            approximateNumberOfEntries = eventLog->getApproximateNumberOfEvents() * average;
        }
    }

    return approximateNumberOfEntries;
}
