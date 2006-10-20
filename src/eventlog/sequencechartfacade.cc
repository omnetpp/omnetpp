//=========================================================================
//  SEQUENCECHARTFACADE.CC - part of
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
#include "sequencechartfacade.h"

SequenceChartFacade::SequenceChartFacade(IEventLog *eventLog) : EventLogFacade(eventLog)
{
    timelineCoordinateSystemVersion = 0;
}

double SequenceChartFacade::Event_getTimelineCoordinate(int64 ptr)
{
    PTR(ptr);
    return getTimelineCoordinate((IEvent*)ptr);
}

double SequenceChartFacade::getTimelineCoordinate(IEvent *event)
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

IEvent *SequenceChartFacade::getLastEventNotAfterTimelineCoordinate(double timelineCoordinate)
{
    EASSERT(timelineCoordinate >= 0);

    switch (timelineMode) {
        case LINEAR:
            // TODO:
            throw new Exception("Not yet implemented");
            break;
        case STEP:
            if (timelineCoordinate > LONG_MAX)
                return eventLog->getLastEvent();
            else
                return eventLog->getLastEventNotAfterEventNumber((long)floor(timelineCoordinate));
        case NON_LINEAR:
            // TODO:
            throw new Exception("Not yet implemented");
            break;
        default:
            throw new Exception("Unknown timeline mode");
    }
}

IEvent *SequenceChartFacade::getFirstEventNotBeforeTimelineCoordinate(double timelineCoordinate)
{
    EASSERT(timelineCoordinate >= 0);

    switch (timelineMode) {
        case LINEAR:
            // TODO:
            throw new Exception("Not yet implemented");
            break;
        case STEP:
            if (timelineCoordinate > LONG_MAX)
                return NULL;
            else
                return eventLog->getFirstEventNotBeforeEventNumber((long)ceil(timelineCoordinate));
        case NON_LINEAR:
            // TODO:
            throw new Exception("Not yet implemented");
            break;
        default:
            throw new Exception("Unknown timeline mode");
    }
}
	
double SequenceChartFacade::getSimulationTimeForTimelineCoordinate(double timelineCoordinate)
{
    EASSERT(timelineCoordinate >= 0);

    switch (getTimelineMode())
    {
	    case LINEAR:
		    return timelineCoordinate;
	    case STEP:
	    case NON_LINEAR:
            {
		        IEvent *event = getLastEventNotAfterTimelineCoordinate(timelineCoordinate);
		        double eventSimulationTime;
		        double eventTimelineCoordinate;
        		
   		        // if before the first event
		        if (event) {
                    eventSimulationTime = event->getSimulationTime();
                    eventTimelineCoordinate = getTimelineCoordinate(event);
		        }
		        else {
			        eventSimulationTime = 0;
			        eventTimelineCoordinate = 0;
		        }

		        // linear approximation between two enclosing events
                IEvent *nextEvent = event ? event->getNextEvent() : eventLog->getFirstEvent();
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

double SequenceChartFacade::getTimelineCoordinateForSimulationTime(double simulationTime)
{
    EASSERT(simulationTime >= 0);

    switch (getTimelineMode())
	{
    	case LINEAR:
    		return simulationTime;
    	case STEP:
    	case NON_LINEAR:
            {
    		    IEvent *event = eventLog->getLastEventNotAfterSimulationTime(simulationTime);
    		    double eventSimulationTime;
    		    double eventTimelineCoordinate;
        		
    		    // if before the first event
    		    if (event) {
                    eventSimulationTime = event->getSimulationTime();
                    eventTimelineCoordinate = getTimelineCoordinate(event);
    		    }
    		    else {
    			    eventSimulationTime = 0;
    			    eventTimelineCoordinate = 0;
			    }

		        // linear approximation between two enclosing events
                IEvent *nextEvent = event ? event->getNextEvent() : eventLog->getFirstEvent();
                double nextEventSimulationTime;
                double nextEventTimelineCoordinate;

                if (nextEvent) {
                    nextEventSimulationTime = nextEvent->getSimulationTime();
                    nextEventTimelineCoordinate = getTimelineCoordinate(nextEvent);
                }
                else
                    // after the last event simulationTime and timelineCoordinate are proportional
    			    return eventTimelineCoordinate + simulationTime - eventSimulationTime;

			    // linear approximation between two enclosing events
    		    double simulationTimeDelta = nextEventSimulationTime - eventSimulationTime;
    		    double timelineCoordinateDelta = nextEventTimelineCoordinate - eventTimelineCoordinate;
           		
    		    if (simulationTimeDelta == 0) //XXX this can happen in STEP mode when pos==-1, and 1st event is at timeline zero. perhaps getLastEventPositionBeforeTimelineCoordinate() should check "<=" not "<" ?
    			    return eventTimelineCoordinate;

    		    EASSERT(simulationTimeDelta > 0);

    		    return eventTimelineCoordinate + timelineCoordinateDelta * (simulationTime - eventSimulationTime) / simulationTimeDelta;
            }
    	default:
    		throw new Exception("Unknown timeline mode");
	}
}
