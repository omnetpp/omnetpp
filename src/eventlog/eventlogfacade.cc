//=========================================================================
//  EVENTLOGFACADE.CC - part of
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
#include "eventlogfacade.h"

EventLogFacade::EventLogFacade(IEventLog *eventLog)
{
    EASSERT(eventLog);
    this->eventLog = eventLog;

    approximateNumberOfEventLogTableEntries = -1;
}

IEvent *EventLogFacade::getNeighbourEvent(IEvent *event, long distance)
{
    while (distance != 0)
    {
        if (distance > 0) {
            distance--;
            event = event->getNextEvent();
        }
        else {
            distance++;
            event = event->getPreviousEvent();
        }
    }

    return event;
}

EventLogEntry *EventLogFacade::getFirstEventLogTableEntry()
{
	IEvent *event = eventLog->getFirstEvent();
	
	if (!event)
		return NULL;
	else
		return getEventLogTableEntryInEvent(event, 0);
}

EventLogEntry *EventLogFacade::getLastEventLogTableEntry()
{
	IEvent *event = eventLog->getLastEvent();
	
	if (!event)
		return NULL;
	else
		return getEventLogTableEntryInEvent(event, event->getNumEventLogMessages());
}

EventLogEntry *EventLogFacade::getEventLogTableEntryAndDistance(EventLogEntry *sourceEventLogEntry, EventLogEntry *targetEventLogEntry, long distance, long& reachedDistance)
{
    EventLogEntry *eventLogEntry = sourceEventLogEntry;
    reachedDistance = 0;

    int index = getEventLogTableEntryIndexInEvent(eventLogEntry);
    while (distance && eventLogEntry != targetEventLogEntry) {
        if (distance > 0) {
            eventLogEntry = getNextEventLogTableEntry(eventLogEntry, index);
            distance--;
            reachedDistance++;
        }
        else {
            eventLogEntry = getPreviousEventLogTableEntry(eventLogEntry, index);
            distance++;
            reachedDistance--;
        }
    }

    return eventLogEntry;
}

EventLogEntry *EventLogFacade::getPreviousEventLogTableEntry(EventLogEntry *eventLogEntry, int& index)
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
        return getEventLogTableEntryInEvent(event, index);
}

EventLogEntry *EventLogFacade::getNextEventLogTableEntry(EventLogEntry *eventLogEntry, int& index)
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
        return getEventLogTableEntryInEvent(event, index);
}

EventLogEntry *EventLogFacade::getEventLogTableEntryInEvent(IEvent *event, int index)
{
    EASSERT(index >= 0 && index <= event->getNumEventLogMessages());

    if (index == 0)
        return event->getEventEntry();
    else
        return event->getEventLogMessage(index - 1);
}

int EventLogFacade::getEventLogTableEntryIndexInEvent(EventLogEntry *eventLogEntry)
{
    if (dynamic_cast<EventEntry *>(eventLogEntry))
        return 0;
    else
        return eventLogEntry->getEvent()->getEventLogMessageIndex((EventLogMessage *)eventLogEntry) + 1;
}

long EventLogFacade::getDistanceToEventLogTableEntry(EventLogEntry *sourceEventLogEntry, EventLogEntry *targetEventLogEntry, long limit)
{
    long reachedDistance;
    getEventLogTableEntryAndDistance(sourceEventLogEntry, targetEventLogEntry, limit, reachedDistance);
    return reachedDistance;
}

long EventLogFacade::getDistanceToFirstEventLogTableEntry(EventLogEntry *eventLogEntry, long limit)
{
    long reachedDistance;
    getEventLogTableEntryAndDistance(eventLogEntry, NULL, -limit, reachedDistance);
    return -reachedDistance;
}

long EventLogFacade::getDistanceToLastEventLogTableEntry(EventLogEntry *eventLogEntry, long limit)
{
    long reachedDistance;
    getEventLogTableEntryAndDistance(eventLogEntry, NULL, limit, reachedDistance);
    return reachedDistance;
}

EventLogEntry *EventLogFacade::getNeighbourEventLogTableEntry(EventLogEntry *eventLogEntry, long distance)
{
    long reachedDistance;
    return getEventLogTableEntryAndDistance(eventLogEntry, NULL, distance, reachedDistance);
}

double EventLogFacade::getApproximatePercentageForEventLogTableEntry(EventLogEntry *eventLogEntry)
{
    return eventLog->getApproximatePercentageForEventNumber(eventLogEntry->getEvent()->getEventNumber());
}

EventLogEntry *EventLogFacade::getApproximateEventLogEntryTableAt(double percentage)
{
    return eventLog->getApproximateEventAt(percentage)->getEventEntry();
}

long EventLogFacade::getApproximateNumberOfEventLogTableEntries()
{
    if (approximateNumberOfEventLogTableEntries == -1)
    {
        IEvent *firstEvent = eventLog->getFirstEvent();
        IEvent *lastEvent = eventLog->getLastEvent();

        if (!firstEvent)
            approximateNumberOfEventLogTableEntries = 0;
        else
        {
            long sum = 0;
            long count = 0;

            for (int i = 0; i < 100; i++)
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

            double average = sum / count;
            approximateNumberOfEventLogTableEntries = eventLog->getApproximateNumberOfEvents() * average;
        }
    }

    return approximateNumberOfEventLogTableEntries;
}
