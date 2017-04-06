//=========================================================================
//  SEQUENCECHARTFACADE.CC - part of
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
#include <algorithm>
#include <set>
#include <cmath>
#include "common/lcgrandom.h"
#include "ievent.h"
#include "ieventlog.h"
#include "event.h"
#include "messagedependency.h"
#include "sequencechartfacade.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace eventlog {

SequenceChartFacade::SequenceChartFacade(IEventLog *eventLog) : EventLogFacade(eventLog)
{
    nonLinearFocus = -1;
    nonLinearMinimumTimelineCoordinateDelta = 0.1;
    timelineMode = NONLINEAR;
    timelineCoordinateSystemVersion = -1;
    undefineTimelineCoordinateSystem();
}

void SequenceChartFacade::synchronize(FileReader::FileChangedState change)
{
    if (change != FileReader::UNCHANGED) {
        EventLogFacade::synchronize(change);
        nonLinearFocus = -1;
        switch (change) {
            case FileReader::UNCHANGED:  // cannot be reached. just to avoid warnings of unhandled enum value
                break;
            case FileReader::OVERWRITTEN:
                undefineTimelineCoordinateSystem();
                break;
            case FileReader::APPENDED:
                IEvent *event = eventLog->getEventForEventNumber(timelineCoordinateOriginEventNumber, EXACT, true);
                if (event)
                    relocateTimelineCoordinateSystem(event);
                else
                    undefineTimelineCoordinateSystem();
                break;
        }
    }
}

double SequenceChartFacade::calculateNonLinearFocus()
{
    if (!eventLog->isEmpty()) {
        double lastEventSimulationTime = eventLog->getLastEvent()->getSimulationTime().dbl();
        double firstEventSimulationTime = eventLog->getFirstEvent()->getSimulationTime().dbl();
        double totalSimulationTimeDelta = lastEventSimulationTime - firstEventSimulationTime;

        if (totalSimulationTimeDelta == 0)
            totalSimulationTimeDelta = firstEventSimulationTime;

        if (totalSimulationTimeDelta == 0)
            return 1;
        else
            return totalSimulationTimeDelta / eventLog->getApproximateNumberOfEvents() / 10;
    }
    else
        return 1;
}

void SequenceChartFacade::setNonLinearMinimumTimelineCoordinateDelta(double value)
{
    Assert(value >= 0);
    nonLinearMinimumTimelineCoordinateDelta = value;
}

void SequenceChartFacade::setNonLinearFocus(double nonLinearFocus)
{
    Assert(nonLinearFocus >= 0);
    this->nonLinearFocus = nonLinearFocus;
}

double SequenceChartFacade::getNonLinearFocus()
{
    if (nonLinearFocus == -1)
        nonLinearFocus = calculateNonLinearFocus();
    return nonLinearFocus;
}

void SequenceChartFacade::undefineTimelineCoordinateSystem()
{
    timelineCoordinateSystemVersion++;
    timelineCoordinateOriginEventNumber = timelineCoordinateRangeStartEventNumber = timelineCoordinateRangeEndEventNumber = -1;
    timelineCoordinateOriginSimulationTime = simtime_nil;
}

void SequenceChartFacade::relocateTimelineCoordinateSystem(IEvent *event)
{
    Assert(event);
    timelineCoordinateSystemVersion++;
    timelineCoordinateOriginEventNumber = timelineCoordinateRangeStartEventNumber = timelineCoordinateRangeEndEventNumber = event->getEventNumber();
    timelineCoordinateOriginSimulationTime = event->getSimulationTime();
    event->cachedTimelineCoordinate = 0;
    event->cachedTimelineCoordinateSystemVersion = timelineCoordinateSystemVersion;
}

void SequenceChartFacade::setTimelineMode(TimelineMode timelineMode)
{
    this->timelineMode = timelineMode;

    if (timelineCoordinateOriginEventNumber != -1)
        relocateTimelineCoordinateSystem(eventLog->getEventForEventNumber(timelineCoordinateOriginEventNumber));
}

double SequenceChartFacade::IEvent_getTimelineCoordinate(ptr_t ptr)
{
    IEVENT_PTR(ptr);
    return getTimelineCoordinate((IEvent *)ptr);
}

double SequenceChartFacade::getTimelineCoordinateDelta(double simulationTimeDelta)
{
    Assert(getNonLinearFocus() > 0);

    if (timelineMode == STEP)
        return 1;
    else
        return nonLinearMinimumTimelineCoordinateDelta + (1 - nonLinearMinimumTimelineCoordinateDelta) * atan(fabs(simulationTimeDelta) / getNonLinearFocus()) / PI * 2;
}

double SequenceChartFacade::getTimelineCoordinate(ptr_t ptr, double lowerTimelineCoordinateCalculationLimit, double upperTimelineCoordinateCalculationLimit)
{
    IEVENT_PTR(ptr);
    return getTimelineCoordinate((IEvent *)ptr, lowerTimelineCoordinateCalculationLimit, upperTimelineCoordinateCalculationLimit);
}

double SequenceChartFacade::getTimelineCoordinate(IEvent *event, double lowerTimelineCoordinateCalculationLimit, double upperTimelineCoordinateCalculationLimit)
{
    Assert(event);
    Assert(event->getEventLog() == eventLog);
    Assert(timelineCoordinateSystemVersion != -1);
    Assert(timelineCoordinateOriginEventNumber != -1);

    if (this->timelineCoordinateSystemVersion > event->cachedTimelineCoordinateSystemVersion) {
        double timelineCoordinate;

        switch (timelineMode) {
            case SIMULATION_TIME:
                timelineCoordinate = (event->getSimulationTime() - timelineCoordinateOriginSimulationTime).dbl();
                break;

            case EVENT_NUMBER:
                timelineCoordinate = event->getEventNumber() - timelineCoordinateOriginEventNumber;
                break;

            case STEP:
            case NONLINEAR: {
                IEvent *previousEvent = nullptr;
                // do we go forward from end or backward from start of known range
                bool forward = event->getEventNumber() > timelineCoordinateRangeEndEventNumber;
                IEvent *currentEvent = eventLog->getEventForEventNumber(forward ? timelineCoordinateRangeEndEventNumber : timelineCoordinateRangeStartEventNumber);

                Assert(event->getEventNumber() < timelineCoordinateRangeStartEventNumber || timelineCoordinateRangeEndEventNumber < event->getEventNumber());

                // TODO: LONG RUNNING OPERATION
                // does a linear search towards the event to calculate the non linear timeline coordinate
                do {
                    eventLog->progress();

                    Assert(currentEvent);
                    previousEvent = currentEvent;
                    currentEvent = forward ? currentEvent->getNextEvent() : currentEvent->getPreviousEvent();
                    Assert(currentEvent);

                    simtime_t previousSimulationTime = previousEvent->getSimulationTime();
                    double previousTimelineCoordinate = previousEvent->cachedTimelineCoordinate;
                    simtime_t simulationTime = currentEvent->getSimulationTime();
                    double timelineCoordinateDelta = getTimelineCoordinateDelta((simulationTime - previousSimulationTime).dbl());

                    if (forward) {
                        timelineCoordinate = previousTimelineCoordinate + timelineCoordinateDelta;

                        if (timelineCoordinate > upperTimelineCoordinateCalculationLimit)
                            return NaN;
                    }
                    else {
                        timelineCoordinate = previousTimelineCoordinate - timelineCoordinateDelta;

                        if (timelineCoordinate < lowerTimelineCoordinateCalculationLimit)
                            return NaN;
                    }

                    currentEvent->cachedTimelineCoordinate = timelineCoordinate;
                    currentEvent->cachedTimelineCoordinateSystemVersion = timelineCoordinateSystemVersion;
                } while (currentEvent != event);

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

double SequenceChartFacade::getCachedTimelineCoordinate(IEvent *event)
{
    Assert(event);
    Assert(timelineCoordinateSystemVersion != -1);
    Assert(timelineCoordinateOriginEventNumber != -1);

    if (this->timelineCoordinateSystemVersion > event->cachedTimelineCoordinateSystemVersion)
        return -1;
    else
        return event->cachedTimelineCoordinate;
}

IEvent *SequenceChartFacade::getEventForNonLinearTimelineCoordinate(double timelineCoordinate, bool& forward)
{
    Assert(timelineCoordinateOriginEventNumber != -1);
    IEvent *timelineCoordinateRangeStartEvent = eventLog->getEventForEventNumber(timelineCoordinateRangeStartEventNumber);
    IEvent *timelineCoordinateRangeEndEvent = eventLog->getEventForEventNumber(timelineCoordinateRangeEndEventNumber);
    IEvent *currentEvent;

    Assert(timelineCoordinateRangeStartEvent && timelineCoordinateRangeEndEvent);

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

    // TODO: LONG RUNNING OPERATION
    // does a linear search towards requested non linear timeline coordinate
    while (currentEvent && (forward ? getTimelineCoordinate(currentEvent) < timelineCoordinate :
                            timelineCoordinate <= getTimelineCoordinate(currentEvent)))
    {
        eventLog->progress();
        currentEvent = forward ? currentEvent->getNextEvent() : currentEvent->getPreviousEvent();
    }

    return currentEvent;
}

IEvent *SequenceChartFacade::getLastEventNotAfterTimelineCoordinate(double timelineCoordinate)
{
    if (eventLog->isEmpty())
        return nullptr;

    switch (timelineMode) {
        case SIMULATION_TIME:
            return eventLog->getLastEventNotAfterSimulationTime(getSimulationTimeForTimelineCoordinate(timelineCoordinate));

        case EVENT_NUMBER: {
            eventnumber_t eventNumber = (eventnumber_t)floor(timelineCoordinate) + timelineCoordinateOriginEventNumber;
            if (eventNumber < 0)
                return nullptr;
            else
                return eventLog->getLastEventNotAfterEventNumber(eventNumber);
        }

        case STEP:
        case NONLINEAR: {
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
    if (eventLog->isEmpty())
        return nullptr;

    switch (timelineMode) {
        case SIMULATION_TIME:
            return eventLog->getFirstEventNotBeforeSimulationTime(getSimulationTimeForTimelineCoordinate(timelineCoordinate));

        case EVENT_NUMBER: {
            eventnumber_t eventNumber = (eventnumber_t)floor(timelineCoordinate) + timelineCoordinateOriginEventNumber;
            if (eventNumber < 0)
                return eventLog->getFirstEvent();
            else
                return eventLog->getFirstEventNotBeforeEventNumber(eventNumber);
        }

        case STEP:
        case NONLINEAR: {
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

void SequenceChartFacade::extractSimulationTimesAndTimelineCoordinates(IEvent *event, IEvent *& nextEvent,
        simtime_t& eventSimulationTime, double& eventTimelineCoordinate,
        simtime_t& nextEventSimulationTime, double& nextEventTimelineCoordinate,
        simtime_t& simulationTimeDelta, double& timelineCoordinateDelta)
{
    // if before the first event
    if (event) {
        eventSimulationTime = event->getSimulationTime();
        eventTimelineCoordinate = getTimelineCoordinate(event);
    }
    else {
        eventSimulationTime = BigDecimal::Zero;
        IEvent *firstEvent = eventLog->getFirstEvent();
        eventTimelineCoordinate = getTimelineCoordinate(firstEvent);

        if (timelineMode == EVENT_NUMBER)
            eventTimelineCoordinate -= 1;
        else
            eventTimelineCoordinate -= getTimelineCoordinateDelta(firstEvent->getSimulationTime().dbl());
    }

    // linear approximation between two enclosing events
    nextEvent = event ? event->getNextEvent() : eventLog->getFirstEvent();

    if (nextEvent) {
        nextEventSimulationTime = nextEvent->getSimulationTime();
        nextEventTimelineCoordinate = getTimelineCoordinate(nextEvent);

        simulationTimeDelta = nextEventSimulationTime - eventSimulationTime;
        timelineCoordinateDelta = nextEventTimelineCoordinate - eventTimelineCoordinate;
    }
}

simtime_t SequenceChartFacade::getSimulationTimeForTimelineCoordinate(double timelineCoordinate, bool upperLimit)
{
    Assert(!isNaN(timelineCoordinate));

    if (eventLog->isEmpty())
        return BigDecimal::Zero;

    simtime_t simulationTime;

    switch (timelineMode) {
        case SIMULATION_TIME: {
            simtime_t lastEventSimulationTime = eventLog->getLastEvent()->getSimulationTime();
            simulationTime = max(BigDecimal::Zero, min(lastEventSimulationTime, timelineCoordinate + timelineCoordinateOriginSimulationTime));
        }
        break;

        case EVENT_NUMBER:
        case STEP:
        case NONLINEAR: {
            IEvent *nextEvent;
            simtime_t eventSimulationTime, nextEventSimulationTime, simulationTimeDelta;
            double eventTimelineCoordinate, nextEventTimelineCoordinate, timelineCoordinateDelta;

            IEvent *event = getLastEventNotAfterTimelineCoordinate(timelineCoordinate);
            extractSimulationTimesAndTimelineCoordinates(event, nextEvent,
                    eventSimulationTime, eventTimelineCoordinate,
                    nextEventSimulationTime, nextEventTimelineCoordinate,
                    simulationTimeDelta, timelineCoordinateDelta);

            if (nextEvent) {
                if (timelineCoordinateDelta == 0) {
                    // IMPORTANT NOTE: this is just an approximation
                    if (upperLimit)
                        simulationTime = nextEventSimulationTime;
                    else
                        simulationTime = eventSimulationTime;
                }
                else {
                    timelineCoordinate = std::max(eventTimelineCoordinate, std::min(nextEventTimelineCoordinate, timelineCoordinate));
                    simulationTime = eventSimulationTime + simulationTimeDelta * (timelineCoordinate - eventTimelineCoordinate) / timelineCoordinateDelta;
                    simulationTime = max(eventSimulationTime, min(nextEventSimulationTime, simulationTime));
                }
            }
            else
                simulationTime = eventSimulationTime;
        }
        break;

        default:
            throw opp_runtime_error("Unknown timeline mode");
    }

    Assert(!simulationTime.isNaN());
    Assert(simulationTime >= BigDecimal::Zero);
    Assert(simulationTime <= eventLog->getLastEvent()->getSimulationTime());

    return simulationTime;
}

double SequenceChartFacade::getTimelineCoordinateForSimulationTime(simtime_t simulationTime, bool upperLimit)
{
    Assert(!simulationTime.isNaN());

    if (eventLog->isEmpty())
        return 0;

    Assert(simulationTime >= BigDecimal::Zero);
    Assert(simulationTime <= eventLog->getLastEvent()->getSimulationTime());

    double timelineCoordinate;

    switch (timelineMode) {
        case SIMULATION_TIME:
            timelineCoordinate = (simulationTime - timelineCoordinateOriginSimulationTime).dbl();
            break;

        case EVENT_NUMBER:
        case STEP:
        case NONLINEAR: {
            IEvent *nextEvent;
            simtime_t eventSimulationTime, nextEventSimulationTime, simulationTimeDelta;
            double eventTimelineCoordinate, nextEventTimelineCoordinate, timelineCoordinateDelta;

            IEvent *event = eventLog->getLastEventNotAfterSimulationTime(simulationTime);
            extractSimulationTimesAndTimelineCoordinates(event, nextEvent,
                    eventSimulationTime, eventTimelineCoordinate,
                    nextEventSimulationTime, nextEventTimelineCoordinate,
                    simulationTimeDelta, timelineCoordinateDelta);

            if (nextEvent) {
                if (simulationTimeDelta == BigDecimal::Zero) {
                    // IMPORTANT NOTE: this is just an approximation
                    if (upperLimit)
                        timelineCoordinate = nextEventTimelineCoordinate;
                    else
                        timelineCoordinate = eventTimelineCoordinate;
                }
                else {
                    simulationTime = max(eventSimulationTime, min(nextEventSimulationTime, simulationTime));
                    timelineCoordinate = eventTimelineCoordinate + timelineCoordinateDelta * (simulationTime - eventSimulationTime).dbl() / simulationTimeDelta.dbl();
                    timelineCoordinate = std::max(eventTimelineCoordinate, std::min(nextEventTimelineCoordinate, timelineCoordinate));
                }
            }
            else
                timelineCoordinate = eventTimelineCoordinate;
        }
        break;

        default:
            throw opp_runtime_error("Unknown timeline mode");
    }

    Assert(!isNaN(timelineCoordinate));

    return timelineCoordinate;
}

double SequenceChartFacade::getTimelineCoordinateForSimulationTimeAndEventInModule(simtime_t simulationTime, int moduleId)
{
    IEvent *event = eventLog->getLastEventNotAfterSimulationTime(simulationTime);

    while (event && event->getSimulationTime() == simulationTime) {
        if (event->getModuleId() == moduleId)
            return getTimelineCoordinate(event);

        event = event->getNextEvent();
    }

    return getTimelineCoordinateForSimulationTime(simulationTime);
}

std::vector<ptr_t> *SequenceChartFacade::getModuleMethodBeginEntries(ptr_t startEventPtr, ptr_t endEventPtr)
{
    IEvent *startEvent = (IEvent *)startEventPtr;
    IEvent *endEvent = (IEvent *)endEventPtr;
    Assert(startEvent);
    Assert(endEvent);
    std::vector<ptr_t> *moduleMethodBeginEntries = new std::vector<ptr_t>();

    for (IEvent *event = startEvent; ; event = event->getNextEvent()) {
        eventLog->progress();

        for (int i = 0; i < event->getNumEventLogEntries(); i++) {
            EventLogEntry *eventLogEntry = event->getEventLogEntry(i);

            if (dynamic_cast<ModuleMethodBeginEntry *>(eventLogEntry))
                moduleMethodBeginEntries->push_back((ptr_t)eventLogEntry);
        }

        if (event == endEvent)
            break;
    }

    return moduleMethodBeginEntries;
}

std::vector<ptr_t> *SequenceChartFacade::getIntersectingMessageDependencies(ptr_t startEventPtr, ptr_t endEventPtr)
{
    IEvent *startEvent = (IEvent *)startEventPtr;
    IEvent *endEvent = (IEvent *)endEventPtr;
    Assert(startEvent);
    Assert(endEvent);
    std::set<ptr_t> messageDependencies;
    eventnumber_t startEventNumber = startEvent->getEventNumber();

    // TODO: LONG RUNNING OPERATION
    // this might take a while if start and end events are far away from each other
    // if not completed then some dependencies will not be included
    for (IEvent *event = startEvent; ; event = event->getNextEvent()) {
        eventLog->progress();
        IMessageDependencyList *causes = event->getCauses();

        for (auto messageDependency : *causes) {
            if (messageDependency->getCauseEventNumber() < startEventNumber)
                messageDependencies.insert((ptr_t)messageDependency);
        }

        IMessageDependencyList *consequences = event->getConsequences();

        for (auto & consequence : *consequences)
            messageDependencies.insert((ptr_t)consequence);

        if (event == endEvent)
            break;
    }

    std::vector<ptr_t> *result = new std::vector<ptr_t>;
    result->resize(messageDependencies.size());
    std::copy(messageDependencies.begin(), messageDependencies.end(), result->begin());

    return result;
}

std::vector<int> SequenceChartFacade::getApproximateMessageDependencyCountAdjacencyMatrix(std::map<int, int> *moduleIdToAxisIndexMap, int numberOfSamples, int messageSendWeight, int messageReuseWeight)
{
    LCGRandom lcgRandom;
    std::vector<int> adjacencyMatrix;
    std::set<int> axisIndexSet;
    std::map<eventnumber_t, IEvent *> eventNumberToEventMap;

    for (auto & it : *moduleIdToAxisIndexMap)
        axisIndexSet.insert(it.second);

    int numberOfAxes = axisIndexSet.size();
    adjacencyMatrix.resize(numberOfAxes * numberOfAxes);

    for (int i = 0; i < numberOfSamples; i++) {
        // draw random
        double percentage = lcgRandom.next01();
        IEvent *event = eventLog->getApproximateEventAt(percentage);
        eventNumberToEventMap[event->getEventNumber()] = event;

        // look before origin
        if (timelineCoordinateOriginEventNumber != -1) {
            if (timelineCoordinateOriginEventNumber - i >= 0) {
                event = eventLog->getEventForEventNumber(timelineCoordinateOriginEventNumber - i);
                if (event)
                    eventNumberToEventMap[event->getEventNumber()] = event;
            }

            // look after origin
            event = eventLog->getEventForEventNumber(timelineCoordinateOriginEventNumber + i);
            if (event)
                eventNumberToEventMap[event->getEventNumber()] = event;
        }
    }

    for (auto & it : eventNumberToEventMap) {
        IEvent *event = it.second;
        IMessageDependencyList *causes = event->getCauses();

        for (auto messageDependency : *causes) {
            IEvent *causeEvent = messageDependency->getCauseEvent();
            IEvent *consequenceEvent = messageDependency->getConsequenceEvent();
            int weight = 0;
            if (dynamic_cast<MessageSendDependency *>(messageDependency))
                weight = messageSendWeight;
            else if (dynamic_cast<MessageReuseDependency *>(messageDependency))
                weight = messageReuseWeight;

            if (causeEvent && consequenceEvent && weight != 0) {
                int causeModuleId = causeEvent->getModuleId();
                int consequenceModuleId = consequenceEvent->getModuleId();

                std::map<int, int>::iterator causeModuleIdIt = moduleIdToAxisIndexMap->find(causeModuleId);
                std::map<int, int>::iterator consequenceModuleIdIt = moduleIdToAxisIndexMap->find(consequenceModuleId);

                if (causeModuleIdIt != moduleIdToAxisIndexMap->end() && consequenceModuleIdIt != moduleIdToAxisIndexMap->end())
                    adjacencyMatrix.at(causeModuleIdIt->second * numberOfAxes + consequenceModuleIdIt->second) += weight;
            }
        }
    }

    return adjacencyMatrix;
}

} // namespace eventlog
}  // namespace omnetpp

