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
#include <math.h>
#include "lcgrandom.h"
#include "ievent.h"
#include "ieventlog.h"
#include "event.h"
#include "messagedependency.h"
#include "sequencechartfacade.h"

SequenceChartFacade::SequenceChartFacade(IEventLog *eventLog) : EventLogFacade(eventLog)
{
    timelineCoordinateSystemVersion = 0;
    timelineMode = NON_LINEAR;
    relocateTimelineCoordinateSystem(eventLog->getFirstEvent());
}

void SequenceChartFacade::relocateTimelineCoordinateSystem(IEvent *event)
{
    timelineCoordinateSystemVersion++;
    timelineCoordinateOriginEventNumber = timelineCoordinateRangeStartEventNumber = timelineCoordinateRangeEndEventNumber = event->getEventNumber();
    timelineCoordinateOriginSimulationTime = event->getSimulationTime().dbl();

    if (event) {
        event->cachedTimelineCoordinate = 0;
        event->cachedTimelineCoordinateSystemVersion = timelineCoordinateSystemVersion;
    }
}

void SequenceChartFacade::setTimelineMode(TimelineMode timelineMode)
{
    this->timelineMode = timelineMode;
    relocateTimelineCoordinateSystem(eventLog->getEventForEventNumber(timelineCoordinateOriginEventNumber));
}

double SequenceChartFacade::Event_getTimelineCoordinate(int64 ptr)
{
    PTR(ptr);
    return getTimelineCoordinate((IEvent*)ptr);
}

double SequenceChartFacade::getNonLinearTimelineCoordinateDelta(double simulationTimeDelta)
{
    double totalSimulationTimeDelta = eventLog->getLastEvent()->getSimulationTime().dbl() - eventLog->getFirstEvent()->getSimulationTime().dbl();
    double nonLinearFocus = totalSimulationTimeDelta / eventLog->getApproximateNumberOfEvents() / 10;

    return 0.1 + 0.9 * atan(abs(simulationTimeDelta) / nonLinearFocus) / PI * 2;
}

double SequenceChartFacade::getTimelineCoordinate(IEvent *event)
{
    Assert(event);
    Assert(timelineCoordinateOriginEventNumber != -1);
    long cachedTimelineCoordinateSystemVersion = event->cachedTimelineCoordinateSystemVersion;

    if (this->timelineCoordinateSystemVersion > cachedTimelineCoordinateSystemVersion) {
        double timelineCoordinate;

        switch (timelineMode) {
            case LINEAR:
                timelineCoordinate = event->getSimulationTime().dbl() - timelineCoordinateOriginSimulationTime;
                break;
            case STEP:
                timelineCoordinate = event->getEventNumber() - timelineCoordinateOriginEventNumber;
                break;
            case NON_LINEAR:
                {
                    IEvent *previousEvent = NULL;
                    bool forward = event->getEventNumber() > timelineCoordinateRangeEndEventNumber;
                    IEvent *currentEvent = eventLog->getEventForEventNumber(forward ? timelineCoordinateRangeEndEventNumber : timelineCoordinateRangeStartEventNumber);

                    Assert(event->getEventNumber() < timelineCoordinateRangeStartEventNumber || timelineCoordinateRangeEndEventNumber < event->getEventNumber()); 

                    do {
                        previousEvent = currentEvent;
                        currentEvent = forward ? currentEvent->getNextEvent() : currentEvent->getPreviousEvent();

                        double previousSimulationTime = previousEvent->getSimulationTime().dbl();
                        double previousTimelineCoordinate = previousEvent->cachedTimelineCoordinate;
                        double simulationTime = currentEvent->getSimulationTime().dbl();
                        double timelineCoordinateDelta = getNonLinearTimelineCoordinateDelta(simulationTime - previousSimulationTime);

                        if (forward)
                            timelineCoordinate = previousTimelineCoordinate + timelineCoordinateDelta;
                        else
                            timelineCoordinate = previousTimelineCoordinate - timelineCoordinateDelta;

                        currentEvent->cachedTimelineCoordinate = timelineCoordinate;
                        currentEvent->cachedTimelineCoordinateSystemVersion = timelineCoordinateSystemVersion;
                    }
                    while (currentEvent != event);

                    if (forward)
                        timelineCoordinateRangeEndEventNumber = event->getEventNumber();
                    else
                        timelineCoordinateRangeStartEventNumber = event->getEventNumber();
                }
                break;
            default:
                throw opp_runtime_error("Unknown timeline mode");
        }

        event->cachedTimelineCoordinate = timelineCoordinate;
        event->cachedTimelineCoordinateSystemVersion = timelineCoordinateSystemVersion;
    }

    return event->cachedTimelineCoordinate;
}

IEvent *SequenceChartFacade::getEventForNonLinearTimelineCoordinate(double timelineCoordinate, bool &forward)
{
    Assert(timelineCoordinateOriginEventNumber != -1);
    IEvent *timelineCoordinateRangeStartEvent = eventLog->getEventForEventNumber(timelineCoordinateRangeStartEventNumber);
    IEvent *timelineCoordinateRangeEndEvent = eventLog->getEventForEventNumber(timelineCoordinateRangeEndEventNumber);
    IEvent *currentEvent;

    if (timelineCoordinate <= getTimelineCoordinate(timelineCoordinateRangeStartEvent)) {
        forward = false;
        currentEvent = timelineCoordinateRangeStartEvent;
    }
    else if (getTimelineCoordinate(timelineCoordinateRangeEndEvent) <= timelineCoordinate) {
        forward = true;
        currentEvent = timelineCoordinateRangeEndEvent;
    }
    else {
        forward = true;
        currentEvent = timelineCoordinateRangeStartEvent;
    }

    while (currentEvent && (forward ? getTimelineCoordinate(currentEvent) < timelineCoordinate :
                                      timelineCoordinate <= getTimelineCoordinate(currentEvent)))
        currentEvent = forward ? currentEvent->getNextEvent() : currentEvent->getPreviousEvent();

    return currentEvent;
}

IEvent *SequenceChartFacade::getLastEventNotAfterTimelineCoordinate(double timelineCoordinate)
{
    switch (timelineMode) {
        case LINEAR:
            return eventLog->getLastEventNotAfterSimulationTime(getSimulationTimeForTimelineCoordinate(timelineCoordinate));
        case STEP:
            {
                long eventNumber = (long)floor(timelineCoordinate) + timelineCoordinateOriginEventNumber;

                if (eventNumber < 0)
                    return NULL;
                else
                    return eventLog->getLastEventNotAfterEventNumber(eventNumber);
            }
        case NON_LINEAR:
            {
                bool forward;
                IEvent *currentEvent = getEventForNonLinearTimelineCoordinate(timelineCoordinate, forward);
                currentEvent = forward ? (currentEvent ? currentEvent->getPreviousEvent() : eventLog->getLastEvent()) : currentEvent;

                Assert(!currentEvent || getTimelineCoordinate(currentEvent) <= timelineCoordinate);
                return currentEvent;
            }
        default:
            throw opp_runtime_error("Unknown timeline mode");
    }
}

IEvent *SequenceChartFacade::getFirstEventNotBeforeTimelineCoordinate(double timelineCoordinate)
{
    switch (timelineMode) {
        case LINEAR:
            return eventLog->getFirstEventNotBeforeSimulationTime(getSimulationTimeForTimelineCoordinate(timelineCoordinate));
        case STEP:
            {
                long eventNumber = (long)floor(timelineCoordinate) + timelineCoordinateOriginEventNumber;

                if (eventNumber < 0)
                    return eventLog->getFirstEvent();
                else
                    return eventLog->getFirstEventNotBeforeEventNumber(eventNumber);
            }
        case NON_LINEAR:
            {
                bool forward;
                IEvent *currentEvent = getEventForNonLinearTimelineCoordinate(timelineCoordinate, forward);
                currentEvent = forward ? currentEvent : (currentEvent ? currentEvent->getNextEvent() : eventLog->getFirstEvent());

                Assert(!currentEvent || getTimelineCoordinate(currentEvent) >= timelineCoordinate);
                return currentEvent;
            }
        default:
            throw opp_runtime_error("Unknown timeline mode");
    }
}

void SequenceChartFacade::extractSimulationTimesAndTimelineCoordinates(
    IEvent *event, IEvent *&nextEvent,
    double &eventSimulationTime, double &eventTimelineCoordinate,
    double &nextEventSimulationTime, double &nextEventTimelineCoordinate,
    double &simulationTimeDelta, double &timelineCoordinateDelta)
{
    // if before the first event
    if (event) {
        eventSimulationTime = event->getSimulationTime().dbl();
        eventTimelineCoordinate = getTimelineCoordinate(event);
    }
    else {
        eventSimulationTime = 0;
        IEvent *firstEvent = eventLog->getFirstEvent();
        eventTimelineCoordinate = getTimelineCoordinate(firstEvent);

        if (timelineMode == STEP)
            eventTimelineCoordinate -= 1;
        else
            eventTimelineCoordinate -= getNonLinearTimelineCoordinateDelta(firstEvent->getSimulationTime().dbl());
    }

    // linear approximation between two enclosing events
    nextEvent = event ? event->getNextEvent() : eventLog->getFirstEvent();

    if (nextEvent) {
        nextEventSimulationTime = nextEvent->getSimulationTime().dbl();
        nextEventTimelineCoordinate = getTimelineCoordinate(nextEvent);

        simulationTimeDelta = nextEventSimulationTime - eventSimulationTime;
        timelineCoordinateDelta = nextEventTimelineCoordinate - eventTimelineCoordinate;
    }
}

double SequenceChartFacade::getSimulationTimeForTimelineCoordinate(double timelineCoordinate, bool upperBound)
{
    double simulationTime;

    switch (timelineMode)
    {
        case LINEAR:
            {
                double lastEventSimulationTime = eventLog->getLastEvent()->getSimulationTime().dbl();
                simulationTime = std::max(0.0, std::min(lastEventSimulationTime, timelineCoordinate + timelineCoordinateOriginSimulationTime));
            }
            break;
        case STEP:
        case NON_LINEAR:
            {
                IEvent *nextEvent;
                double eventSimulationTime, eventTimelineCoordinate;
                double nextEventSimulationTime, nextEventTimelineCoordinate;
                double simulationTimeDelta, timelineCoordinateDelta;

                IEvent *event = getLastEventNotAfterTimelineCoordinate(timelineCoordinate);
                extractSimulationTimesAndTimelineCoordinates(event, nextEvent, 
                                                             eventSimulationTime, eventTimelineCoordinate,
                                                             nextEventSimulationTime, nextEventTimelineCoordinate,
                                                             simulationTimeDelta, timelineCoordinateDelta);
 
                if (nextEvent) {
                    if (timelineCoordinateDelta == 0) {
                        if (upperBound)
                            simulationTime = nextEventSimulationTime;
                        else
                            simulationTime = eventSimulationTime;
                    }
                    else {
                        timelineCoordinate = std::max(eventTimelineCoordinate, std::min(nextEventTimelineCoordinate, timelineCoordinate));
                        simulationTime = eventSimulationTime + simulationTimeDelta * (timelineCoordinate - eventTimelineCoordinate) / timelineCoordinateDelta;
                    }
                }
                else
                    simulationTime = eventSimulationTime;
            }
            break;
        default:
            throw opp_runtime_error("Unknown timeline mode");
    }

    Assert(simulationTime >= 0);
    Assert(simulationTime <= eventLog->getLastEvent()->getSimulationTime().dbl());

    return simulationTime;
}

double SequenceChartFacade::getTimelineCoordinateForSimulationTime(double simulationTime, bool upperBound)
{
    Assert(simulationTime >= 0);
    Assert(simulationTime <= eventLog->getLastEvent()->getSimulationTime().dbl());

    double timelineCoordinate;

    switch (timelineMode)
    {
        case LINEAR:
            timelineCoordinate = simulationTime - timelineCoordinateOriginSimulationTime;
            break;
        case STEP:
        case NON_LINEAR:
            {
                IEvent *nextEvent;
                double eventSimulationTime, eventTimelineCoordinate;
                double nextEventSimulationTime, nextEventTimelineCoordinate;
                double simulationTimeDelta, timelineCoordinateDelta;

                IEvent *event = eventLog->getLastEventNotAfterSimulationTime(simulationTime);
                extractSimulationTimesAndTimelineCoordinates(event, nextEvent, 
                                                             eventSimulationTime, eventTimelineCoordinate,
                                                             nextEventSimulationTime, nextEventTimelineCoordinate,
                                                             simulationTimeDelta, timelineCoordinateDelta);

                if (nextEvent) {
                    if (simulationTimeDelta == 0) {
                        if (upperBound)
                            timelineCoordinate = nextEventTimelineCoordinate;
                        else
                            timelineCoordinate = eventTimelineCoordinate;
                    }
                    else {
                        simulationTime = std::max(eventSimulationTime, std::min(nextEventSimulationTime, simulationTime));
                        timelineCoordinate = eventTimelineCoordinate + timelineCoordinateDelta * (simulationTime - eventSimulationTime) / simulationTimeDelta;
                    }
                }
                else
                    timelineCoordinate = eventTimelineCoordinate;
            }
            break;
        default:
            throw opp_runtime_error("Unknown timeline mode");
    }

    return timelineCoordinate;
}

std::vector<int64> *SequenceChartFacade::getIntersectingMessageDependencies(int64 startEventPtr, int64 endEventPtr, int lookAroundCount)
{
    int i;
    IEvent *event;
    std::set<int64> messageDependencies;
    IEvent *startEvent = (IEvent *)startEventPtr;
    IEvent *endEvent = (IEvent *)endEventPtr;
    long startEventNumber = startEvent->getEventNumber();
    long endEventNumber = endEvent->getEventNumber();
    long endLookAroundEventNumber = endEventNumber;

    // look after requrested range
    for (event = endEvent, i = 0; i < lookAroundCount && event; event = event->getNextEvent(), i++) {
        MessageDependencyList *causes = event->getCauses();

        for (MessageDependencyList::iterator it = causes->begin(); it != causes->end(); it++) {
            MessageDependency *messageDependency = *it;
            long eventNumber = messageDependency->getCauseEventNumber();

            // store only if refers inside or before the requested range
            if (eventNumber <= endEventNumber)
                messageDependencies.insert((int64)messageDependency);
        }

        endLookAroundEventNumber = event->getEventNumber();
    }

    // look before requrested range
    for (event = startEvent, i = 0; i < lookAroundCount && event; event = event->getPreviousEvent(), i++) {
        MessageDependencyList *consequences = event->getConsequences();

        for (MessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++) {
            MessageDependency *messageDependency = *it;
            long eventNumber = messageDependency->getConsequenceEventNumber();

            // store only if refers outside the requested plus look around range
            if (endLookAroundEventNumber < eventNumber)
                messageDependencies.insert((int64)messageDependency);
        }
    }

    // look at requested range
    for (IEvent *event = startEvent; event != endEvent; event = event->getNextEvent()) {
        MessageDependencyList *causes = event->getCauses();

        for (MessageDependencyList::iterator it = causes->begin(); it != causes->end(); it++)
            messageDependencies.insert((int64)*it);

        MessageDependencyList *consequences = event->getConsequences();

        for (MessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++) {
            MessageDependency *messageDependency = *it;
            long eventNumber = messageDependency->getConsequenceEventNumber();

            // store only if refers outside the requested plus look around range
            if (endLookAroundEventNumber < eventNumber)
                messageDependencies.insert((int64)messageDependency);
        }
    }

    std::vector<int64> *result = new std::vector<int64>;
    result->resize(messageDependencies.size());
    std::copy(messageDependencies.begin(), messageDependencies.end(), result->begin());

    return result;
}

std::vector<int> SequenceChartFacade::getApproximateMessageDependencyCountAdjacencyMatrix(std::map<int, int> *moduleIdToAxisIdMap, int numberOfSamples)
{
    LCGRandom lcgRandom;
    std::vector<int> adjacencyMatrix;
    std::set<int> axisIdSet;
    std::set<long> eventNumbers;

    for (std::map<int, int>::iterator it = moduleIdToAxisIdMap->begin(); it != moduleIdToAxisIdMap->end(); it++)
        axisIdSet.insert(it->second);

    int numberOfAxes = axisIdSet.size();
    adjacencyMatrix.resize(numberOfAxes * numberOfAxes);

    for (int i = 0; i < numberOfSamples; i++)
    {
        double percentage = lcgRandom.next01();
        IEvent *event = eventLog->getApproximateEventAt(percentage);

        if (eventNumbers.find(event->getEventNumber()) == eventNumbers.end()) {
            MessageDependencyList *causes = event->getCauses();

            for (MessageDependencyList::iterator it = causes->begin(); it != causes->end(); it++) {
                MessageDependency *messageDependency = *it;
                IEvent *causeEvent = messageDependency->getCauseEvent();
                IEvent *consequenceEvent = messageDependency->getConsequenceEvent();

                if (causeEvent && consequenceEvent) {
                    int causeModuleId = causeEvent->getModuleId();
                    int consequenceModuleId = consequenceEvent->getModuleId();

                    std::map<int, int>::iterator causeModuleIdIt = moduleIdToAxisIdMap->find(causeModuleId);
                    std::map<int, int>::iterator consequenceModuleIdIt = moduleIdToAxisIdMap->find(consequenceModuleId);

                    if (causeModuleIdIt !=  moduleIdToAxisIdMap->end() && consequenceModuleIdIt !=  moduleIdToAxisIdMap->end())
                        adjacencyMatrix.at(causeModuleIdIt->second * numberOfAxes + consequenceModuleIdIt->second)++;
                }
            }
        }
    }

    return adjacencyMatrix;
}
