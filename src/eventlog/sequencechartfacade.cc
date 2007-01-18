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

#include <algorithm>
#include <set>
#include "ievent.h"
#include "ieventlog.h"
#include "event.h"
#include "messagedependency.h"
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
                throw opp_runtime_error("Not yet implemented");
                break;
            default:
                throw opp_runtime_error("Unknown timeline mode");
        }

        event->cachedTimelineCoordinate = timelineCoordinate;
        event->cachedTimelineCoordinateSystemVersion = timelineCoordinateSystemVersion;
    }

    return event->cachedTimelineCoordinate;
}

IEvent *SequenceChartFacade::getLastEventNotAfterTimelineCoordinate(double timelineCoordinate)
{
    ASSERT(timelineCoordinate >= 0);

    switch (timelineMode) {
        case LINEAR:
            // TODO:
            throw opp_runtime_error("Not yet implemented");
            break;
        case STEP:
            if (timelineCoordinate > LONG_MAX)
                return eventLog->getLastEvent();
            else
                return eventLog->getLastEventNotAfterEventNumber((long)floor(timelineCoordinate));
        case NON_LINEAR:
            // TODO:
            throw opp_runtime_error("Not yet implemented");
            break;
        default:
            throw opp_runtime_error("Unknown timeline mode");
    }
}

IEvent *SequenceChartFacade::getFirstEventNotBeforeTimelineCoordinate(double timelineCoordinate)
{
    ASSERT(timelineCoordinate >= 0);

    switch (timelineMode) {
        case LINEAR:
            // TODO:
            throw opp_runtime_error("Not yet implemented");
            break;
        case STEP:
            if (timelineCoordinate > LONG_MAX)
                return NULL;
            else
                return eventLog->getFirstEventNotBeforeEventNumber((long)ceil(timelineCoordinate));
        case NON_LINEAR:
            // TODO:
            throw opp_runtime_error("Not yet implemented");
            break;
        default:
            throw opp_runtime_error("Unknown timeline mode");
    }
}

double SequenceChartFacade::getSimulationTimeForTimelineCoordinate(double timelineCoordinate)
{
    ASSERT(timelineCoordinate >= 0);

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

                ASSERT(timelineCoordinateDelta > 0);

                return eventSimulationTime + simulationTimeDelta * (timelineCoordinate - eventTimelineCoordinate) / timelineCoordinateDelta;
            }
        default:
            throw opp_runtime_error("Unknown timeline mode");
    }
}

double SequenceChartFacade::getTimelineCoordinateForSimulationTime(double simulationTime)
{
    ASSERT(simulationTime >= 0);

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

                ASSERT(simulationTimeDelta > 0);

                return eventTimelineCoordinate + timelineCoordinateDelta * (simulationTime - eventSimulationTime) / simulationTimeDelta;
            }
        default:
            throw opp_runtime_error("Unknown timeline mode");
    }
}

std::vector<int64> *SequenceChartFacade::getIntersectingMessageDependencies(int64 startEventPtr, int64 endEventPtr)
{
    int i;
    IEvent *event;
    int lookaheadCount = 100;
    std::set<int64> messageDependencies;
    IEvent *startEvent = (IEvent *)startEventPtr;
    IEvent *endEvent = (IEvent *)endEventPtr;
    long startEventNumber = startEvent->getEventNumber();
    long endEventNumber = endEvent->getEventNumber();

    for (event = startEvent, i = 0; i < lookaheadCount && event; event = event->getPreviousEvent(), i++) {
        MessageDependencyList *consequences = event->getCauses();

        for (MessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++) {
            MessageDependency *messageDependency = *it;
            long eventNumber = messageDependency->getConsequenceEventNumber();

            if (endEventNumber < eventNumber)
                messageDependencies.insert((int64)messageDependency);
        }
    }

    for (IEvent *event = startEvent; event != endEvent; event = event->getNextEvent()) {
        MessageDependencyList *causes = event->getCauses();

        for (MessageDependencyList::iterator it = causes->begin(); it != causes->end(); it++)
            messageDependencies.insert((int64)*it);

        MessageDependencyList *consequences = event->getCauses();

        for (MessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++)
            messageDependencies.insert((int64)*it);
    }

    for (event = endEvent, i = 0; i < lookaheadCount && event; event = event->getNextEvent(), i++) {
        MessageDependencyList *causes = event->getCauses();

        for (MessageDependencyList::iterator it = causes->begin(); it != causes->end(); it++) {
            MessageDependency *messageDependency = *it;
            long eventNumber = messageDependency->getCauseEventNumber();

            if (eventNumber < startEventNumber)
                messageDependencies.insert((int64)messageDependency);
        }
    }

    std::vector<int64> *result = new std::vector<int64>;
    result->resize(messageDependencies.size());
    std::copy(messageDependencies.begin(), messageDependencies.end(), result->begin());

    return result;
}
