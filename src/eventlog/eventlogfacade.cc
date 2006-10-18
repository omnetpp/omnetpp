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
    timelineCoordinateSystemVersion = 0;
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
    EASSERT(sourceEventLogEntry);
    EventLogEntry *eventLogEntry = sourceEventLogEntry;
    reachedDistance = 0;

    int index = getEventLogTableEntryIndexInEvent(eventLogEntry);
    EASSERT(index >= 0);

    while (distance && eventLogEntry && eventLogEntry != targetEventLogEntry) {
        if (distance > 0) {
            eventLogEntry = getNextEventLogTableEntry(eventLogEntry, index);
            distance--;

            if (eventLogEntry)
                reachedDistance++;
        }
        else {
            eventLogEntry = getPreviousEventLogTableEntry(eventLogEntry, index);
            distance++;

            if (eventLogEntry)
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
    if (percentage == 1) {
        IEvent* event = eventLog->getLastEvent();
 
        if (!event)
            return NULL;
        else
            return event->getEventLogMessage(event->getNumEventLogMessages() - 1);
    }
    else
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

double EventLogFacade::getTimelineCoordinate(IEvent *event)
{
    long cachedTimelineCoordinateSystemVersion = event->cachedTimelineCoordinateSystemVersion;

    if (this->timelineCoordinateSystemVersion > cachedTimelineCoordinateSystemVersion) {
        double timelineCoordinate;

        switch (timelineMode) {
            case LINEAR:
                timelineCoordinate = event->getSimulationTime();
                break;
            case STEP:
                timelineCoordinate = event->getEventNumber();
                break;
            case NON_LINEAR:
                // TODO: copied from Java code
	        	//double timelineCoordinate = previousTimelineCoordinate + Math.atan((simulationTime - previousSimulationTime) / nonLinearFocus) / Math.PI * 2;
	        	//logFacade.setEvent_i_timelineCoordinate(i, timelineCoordinate);
	        	//previousTimelineCoordinate = timelineCoordinate;
                throw new Exception("Not yet implemented");
                break;
            default:
                throw new Exception("Unknown timeline mode");
        }

        event->cachedTimelineCoordinate = timelineCoordinate;
        event->cachedTimelineCoordinateSystemVersion = timelineCoordinateSystemVersion;
    }

    return event->cachedTimelineCoordinate;
}

IEvent *EventLogFacade::getLastEventBeforeTimelineCoordinate(double timelineCoordinate)
{
    switch (timelineMode) {
        case LINEAR:
            // TODO:
            throw new Exception("Not yet implemented");
            break;
        case STEP:
            return eventLog->getLastEventNotAfterEventNumber((long)floor(timelineCoordinate));
        case NON_LINEAR:
            // TODO:
            throw new Exception("Not yet implemented");
            break;
        default:
            throw new Exception("Unknown timeline mode");
    }
}

IEvent *EventLogFacade::getFirstEventAfterTimelineCoordinate(double timelineCoordinate)
{
    switch (timelineMode) {
        case LINEAR:
            // TODO:
            throw new Exception("Not yet implemented");
            break;
        case STEP:
            return eventLog->getFirstEventNotBeforeEventNumber((long)ceil(timelineCoordinate));
        case NON_LINEAR:
            // TODO:
            throw new Exception("Not yet implemented");
            break;
        default:
            throw new Exception("Unknown timeline mode");
    }
}
	
double EventLogFacade::getSimulationTimeForTimelineCoordinate(double timelineCoordinate)
{
    switch (getTimelineMode())
    {
	    case LINEAR:
		    return timelineCoordinate;
	    case STEP:
	    case NON_LINEAR:
            {
		    IEvent *event = getLastEventBeforeTimelineCoordinate(timelineCoordinate);
		    double eventSimulationTime;
		    double eventTimelineCoordinate;
    		
   		    // if before the first event
		    if (!event) {
			    eventSimulationTime = 0;
			    eventTimelineCoordinate = 0;
                event = eventLog->getFirstEvent();
		    }
		    else {
                eventSimulationTime = event->getSimulationTime();
                eventTimelineCoordinate = getTimelineCoordinate(event);
		    }

		    // linear approximation between two enclosing events
            IEvent *nextEvent = event->getNextEvent();
            double nextEventSimulationTime;
            double nextEventTimelineCoordinate;

            if (nextEvent) {
                nextEventSimulationTime = nextEvent->getSimulationTime();
                nextEventTimelineCoordinate = getTimelineCoordinate(nextEvent);
            }
            else
    		    // after the last event simulationTime and timelineCoordinate are proportional
			    return eventSimulationTime + timelineCoordinate - eventTimelineCoordinate;

            double simulationTimeDelta = nextEventSimulationTime - eventSimulationTime;
		    double timelineCoordinateDelta = nextEventTimelineCoordinate - eventTimelineCoordinate;

		    if (timelineCoordinateDelta == 0) //XXX this can happen in STEP mode when pos==-1, and 1st event is at timeline zero. perhaps getLastEventPositionBeforeTimelineCoordinate() should check "<=" not "<" ?
			    return eventSimulationTime;

		    EASSERT(timelineCoordinateDelta > 0);

		    return eventSimulationTime + simulationTimeDelta * (timelineCoordinate - eventTimelineCoordinate) / timelineCoordinateDelta;
            }
	    default:
		    throw new Exception("Unknown timeline mode");
    }
}

double EventLogFacade::getTimelineCoordinateForSimulationTime(double simulationTime)
{
	switch (getTimelineMode())
	{
    	case LINEAR:
    		return simulationTime;
    		/*
    	case STEP:
    	case NON_LINEAR:
    		int pos = eventLog.getLastEventPositionBefore(simulationTime);
    		double eventSimulationTime;
    		double eventTimelineCoordinate;
    		
    		// if before the first event
    		if (pos == -1) {
    			eventSimulationTime = 0;
    			eventTimelineCoordinate = 0;
    		}
    		else {
    			eventSimulationTime = logFacade.getEvent_i_simulationTime(pos);
    			eventTimelineCoordinate = logFacade.getEvent_i_timelineCoordinate(pos);
			}

    		// after the last event simulationTime and timelineCoordinate are proportional
    		if (pos == eventLog.getNumEvents() - 1)
    			return eventTimelineCoordinate + simulationTime - eventSimulationTime;

			// linear approximation between two enclosing events
    		double simulationTimeDelta = logFacade.getEvent_i_simulationTime(pos + 1) - eventSimulationTime;
    		double timelineCoordinateDelta = logFacade.getEvent_i_timelineCoordinate(pos + 1) - eventTimelineCoordinate;
       		
    		if (simulationTimeDelta == 0) //XXX this can happen in STEP mode when pos==-1, and 1st event is at timeline zero. perhaps getLastEventPositionBeforeTimelineCoordinate() should check "<=" not "<" ?
    			return eventTimelineCoordinate;
    		Assert.isTrue(simulationTimeDelta > 0);

    		return eventTimelineCoordinate + timelineCoordinateDelta * (simulationTime - eventSimulationTime) / simulationTimeDelta;
	*/
    	default:
    		throw new Exception("Unknown timeline mode");
	}
}
