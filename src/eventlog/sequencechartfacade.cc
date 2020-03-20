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
                IEvent *event = eventLog->getEventForEventNumber(timelineCoordinateSystemOriginEventNumber, EXACT, true);
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
    timelineCoordinateSystemOriginEventNumber = timelineCoordinateRangeStartEventNumber = timelineCoordinateRangeEndEventNumber = -1;
    timelineCoordinateSystemOriginSimulationTime = simtime_nil;
}

void SequenceChartFacade::relocateTimelineCoordinateSystem(IEvent *event)
{
    Assert(event);
    timelineCoordinateSystemVersion++;
    timelineCoordinateSystemOriginEventNumber = timelineCoordinateRangeStartEventNumber = timelineCoordinateRangeEndEventNumber = event->getEventNumber();
    timelineCoordinateSystemOriginSimulationTime = event->getSimulationTime();
    event->cachedTimelineCoordinateBegin = 0;
    event->cachedTimelineCoordinateEnd = getTimelineCoordinateDelta(event);
    event->cachedTimelineCoordinateSystemVersion = timelineCoordinateSystemVersion;
}

void SequenceChartFacade::setTimelineMode(TimelineMode timelineMode)
{
    this->timelineMode = timelineMode;

    if (timelineCoordinateSystemOriginEventNumber != -1)
        relocateTimelineCoordinateSystem(eventLog->getEventForEventNumber(timelineCoordinateSystemOriginEventNumber));
}

double SequenceChartFacade::IEvent_getTimelineCoordinateBegin(ptr_t ptr)
{
    IEVENT_PTR(ptr);
    return getTimelineCoordinateBegin((IEvent*)ptr);
}

double SequenceChartFacade::IEvent_getTimelineCoordinateEnd(ptr_t ptr)
{
    IEVENT_PTR(ptr);
    return getTimelineCoordinateEnd((IEvent*)ptr);
}

double SequenceChartFacade::EventLogEntry_getTimelineCoordinate(ptr_t ptr)
{
    EVENT_LOG_ENTRY_PTR(ptr);
    EventLogEntry *eventLogEntry = (EventLogEntry *)ptr;
    IEvent *event = eventLogEntry->getEvent();
    if (timelineMode == SIMULATION_TIME || timelineMode == EVENT_NUMBER)
        return getTimelineCoordinateBegin(event);
    else if (timelineMode == STEP || timelineMode == NONLINEAR) {
        double timelineCoordinateDelta = timelineMode == STEP ? 1 : nonLinearMinimumTimelineCoordinateDelta;
        for (int i = 0; i < event->getNumEventLogEntries(); i++) {
            EventLogEntry *currentEventLogEntry = event->getEventLogEntry(i);
            // TODO: do we need different deltas for different entry kinds?
            // TODO: factor with getTimelineCoordinateDelta
            if (eventLogEntry == currentEventLogEntry)
                return getTimelineCoordinateBegin(event) + timelineCoordinateDelta;
            else if (dynamic_cast<ComponentMethodBeginEntry *>(currentEventLogEntry))
                timelineCoordinateDelta += timelineMode == STEP ? 1 : nonLinearMinimumTimelineCoordinateDelta;
            else if (dynamic_cast<ComponentMethodEndEntry *>(currentEventLogEntry))
                timelineCoordinateDelta += timelineMode == STEP ? 1 : nonLinearMinimumTimelineCoordinateDelta;
            else if (dynamic_cast<BeginSendEntry *>(currentEventLogEntry))
                timelineCoordinateDelta += timelineMode == STEP ? 1 : nonLinearMinimumTimelineCoordinateDelta;
        }
        throw opp_runtime_error("EventLogEntry not found");
    }
    else
        throw opp_runtime_error("Unknown timeline mode");
}

double SequenceChartFacade::getTimelineCoordinateDelta(double simulationTimeDelta)
{
    Assert(getNonLinearFocus() > 0);

    if (timelineMode == STEP)
        return 1;
    else
        return nonLinearMinimumTimelineCoordinateDelta + (1 - nonLinearMinimumTimelineCoordinateDelta) * atan(fabs(simulationTimeDelta) / getNonLinearFocus()) / PI * 2;
}

double SequenceChartFacade::getTimelineCoordinateDelta(IEvent *event)
{
    if (timelineMode == SIMULATION_TIME || timelineMode == EVENT_NUMBER)
        return 0;
    else if (timelineMode == STEP || timelineMode == NONLINEAR) {
        double timelineCoordinateDelta = timelineMode == STEP ? 1 : nonLinearMinimumTimelineCoordinateDelta;
        for (int i = 0; i < event->getNumEventLogEntries(); i++) {
            EventLogEntry *eventLogEntry = event->getEventLogEntry(i);
            // TODO: do we need different deltas for different entry kinds?
            if (dynamic_cast<ComponentMethodBeginEntry *>(eventLogEntry))
                timelineCoordinateDelta += timelineMode == STEP ? 1 : nonLinearMinimumTimelineCoordinateDelta;
            else if (dynamic_cast<ComponentMethodEndEntry *>(eventLogEntry))
                timelineCoordinateDelta += timelineMode == STEP ? 1 : nonLinearMinimumTimelineCoordinateDelta;
            else if (dynamic_cast<BeginSendEntry *>(eventLogEntry))
                timelineCoordinateDelta += timelineMode == STEP ? 1 : nonLinearMinimumTimelineCoordinateDelta;
        }
        return timelineCoordinateDelta;
    }
    else
        throw opp_runtime_error("Unknown timeline mode");
}

double SequenceChartFacade::getTimelineCoordinateBegin(ptr_t ptr, double lowerTimelineCoordinateCalculationLimit, double upperTimelineCoordinateCalculationLimit)
{
    IEVENT_PTR(ptr);
    return getTimelineCoordinateBegin((IEvent *)ptr, lowerTimelineCoordinateCalculationLimit, upperTimelineCoordinateCalculationLimit);
}

double SequenceChartFacade::getTimelineCoordinateEnd(ptr_t ptr, double lowerTimelineCoordinateCalculationLimit, double upperTimelineCoordinateCalculationLimit)
{
    IEVENT_PTR(ptr);
    return getTimelineCoordinateEnd((IEvent *)ptr, lowerTimelineCoordinateCalculationLimit, upperTimelineCoordinateCalculationLimit);
}

double SequenceChartFacade::getTimelineCoordinateBegin(IEvent *event, double lowerTimelineCoordinateCalculationLimit, double upperTimelineCoordinateCalculationLimit)
{
    Assert(event);
    Assert(event->getEventLog() == eventLog);
    Assert(timelineCoordinateSystemVersion != -1);
    Assert(timelineCoordinateSystemOriginEventNumber != -1);

    if (this->timelineCoordinateSystemVersion > event->cachedTimelineCoordinateSystemVersion) {
        double timelineCoordinateBegin;

        switch (timelineMode) {
            case SIMULATION_TIME:
                timelineCoordinateBegin = (event->getSimulationTime() - timelineCoordinateSystemOriginSimulationTime).dbl();
                break;

            case EVENT_NUMBER:
                timelineCoordinateBegin = event->getEventNumber() - timelineCoordinateSystemOriginEventNumber;
                break;
            case STEP:
            case NONLINEAR: {
                IEvent *previousEvent = NULL;
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
                    double previousTimelineCoordinateBegin = previousEvent->cachedTimelineCoordinateBegin;
                    simtime_t simulationTime = currentEvent->getSimulationTime();
                    double timelineCoordinateDelta = getTimelineCoordinateDelta((simulationTime - previousSimulationTime).dbl()) + getTimelineCoordinateDelta(forward ? previousEvent : currentEvent);

                    if (forward) {
                        timelineCoordinateBegin = previousTimelineCoordinateBegin + timelineCoordinateDelta;

                        if (timelineCoordinateBegin > upperTimelineCoordinateCalculationLimit)
                            return NaN;
                    }
                    else {
                        timelineCoordinateBegin = previousTimelineCoordinateBegin - timelineCoordinateDelta;

                        if (timelineCoordinateBegin < lowerTimelineCoordinateCalculationLimit)
                            return NaN;
                    }

                    currentEvent->cachedTimelineCoordinateBegin = timelineCoordinateBegin;
                    currentEvent->cachedTimelineCoordinateEnd = timelineCoordinateBegin + getTimelineCoordinateDelta(currentEvent);
                    currentEvent->cachedTimelineCoordinateSystemVersion = timelineCoordinateSystemVersion;
                } while (currentEvent != event);

                if (forward)
                    timelineCoordinateRangeEndEventNumber = event->getEventNumber();
                else
                    timelineCoordinateRangeStartEventNumber = event->getEventNumber();
                break;
            }

            default:
                throw opp_runtime_error("Unknown timeline mode");
        }

        event->cachedTimelineCoordinateBegin = timelineCoordinateBegin;
        event->cachedTimelineCoordinateEnd = timelineCoordinateBegin + getTimelineCoordinateDelta(event);
        event->cachedTimelineCoordinateSystemVersion = timelineCoordinateSystemVersion;
    }

    return event->cachedTimelineCoordinateBegin;
}

double SequenceChartFacade::getTimelineCoordinateEnd(IEvent *event, double lowerTimelineCoordinateCalculationLimit, double upperTimelineCoordinateCalculationLimit)
{
    return getTimelineCoordinateDelta(event) + getTimelineCoordinateBegin(event, lowerTimelineCoordinateCalculationLimit, upperTimelineCoordinateCalculationLimit);
}

double SequenceChartFacade::getCachedTimelineCoordinateBegin(IEvent *event)
{
    Assert(event);
    Assert(timelineCoordinateSystemVersion != -1);
    Assert(timelineCoordinateSystemOriginEventNumber != -1);

    if (this->timelineCoordinateSystemVersion > event->cachedTimelineCoordinateSystemVersion)
        return -1;
    else
        return event->cachedTimelineCoordinateBegin;
}

double SequenceChartFacade::getCachedTimelineCoordinateEnd(IEvent *event)
{
    Assert(event);
    Assert(timelineCoordinateSystemVersion != -1);
    Assert(timelineCoordinateSystemOriginEventNumber != -1);

    if (this->timelineCoordinateSystemVersion > event->cachedTimelineCoordinateSystemVersion)
        return -1;
    else
        return event->cachedTimelineCoordinateEnd;
}

IEvent *SequenceChartFacade::getEventForNonLinearTimelineCoordinate(double timelineCoordinate, bool& forward)
{
    Assert(timelineCoordinateSystemOriginEventNumber != -1);
    IEvent *timelineCoordinateRangeStartEvent = eventLog->getEventForEventNumber(timelineCoordinateRangeStartEventNumber);
    IEvent *timelineCoordinateRangeEndEvent = eventLog->getEventForEventNumber(timelineCoordinateRangeEndEventNumber);
    IEvent *currentEvent;

    Assert(timelineCoordinateRangeStartEvent && timelineCoordinateRangeEndEvent);

    if (timelineCoordinate <= getTimelineCoordinateBegin(timelineCoordinateRangeStartEvent)) {
        forward = false;
        currentEvent = timelineCoordinateRangeStartEvent;
    }
    else if (getTimelineCoordinateBegin(timelineCoordinateRangeEndEvent) <= timelineCoordinate) {
        forward = true;
        currentEvent = timelineCoordinateRangeEndEvent;
    }
    else {
        forward = true;
        currentEvent = timelineCoordinateRangeStartEvent;
    }

    // TODO: LONG RUNNING OPERATION
    // does a linear search towards requested non linear timeline coordinate
    while (currentEvent && (forward ? getTimelineCoordinateBegin(currentEvent) < timelineCoordinate :
                                      timelineCoordinate <= getTimelineCoordinateBegin(currentEvent)))
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
            eventnumber_t eventNumber = (eventnumber_t)floor(timelineCoordinate) + timelineCoordinateSystemOriginEventNumber;
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

            Assert(!currentEvent || getTimelineCoordinateBegin(currentEvent) <= timelineCoordinate);
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
            eventnumber_t eventNumber = (eventnumber_t)floor(timelineCoordinate) + timelineCoordinateSystemOriginEventNumber;
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

            Assert(!currentEvent || getTimelineCoordinateBegin(currentEvent) >= timelineCoordinate);
            return currentEvent;
        }
        default:
            throw opp_runtime_error("Unknown timeline mode");
    }
}

// TODO: extract both begin and end timeline coordinates of events
void SequenceChartFacade::extractSimulationTimesAndTimelineCoordinates(
    IEvent *event, IEvent *&nextEvent,
    simtime_t &eventSimulationTime, double &eventTimelineCoordinateBegin,
    simtime_t &nextEventSimulationTime, double &nextEventTimelineCoordinateBegin,
    simtime_t &simulationTimeDelta, double &timelineCoordinateDelta)
{
    // if before the first event
    if (event) {
        eventSimulationTime = event->getSimulationTime();
        eventTimelineCoordinateBegin = getTimelineCoordinateBegin(event);
    }
    else {
        eventSimulationTime = BigDecimal::Zero;
        IEvent *firstEvent = eventLog->getFirstEvent();
        eventTimelineCoordinateBegin = getTimelineCoordinateBegin(firstEvent);

        if (timelineMode == EVENT_NUMBER)
            eventTimelineCoordinateBegin -= 1;
        else
            eventTimelineCoordinateBegin -= getTimelineCoordinateDelta(firstEvent->getSimulationTime().dbl());
    }

    // linear approximation between two enclosing events
    nextEvent = event ? event->getNextEvent() : eventLog->getFirstEvent();

    if (nextEvent) {
        nextEventSimulationTime = nextEvent->getSimulationTime();
        nextEventTimelineCoordinateBegin = getTimelineCoordinateBegin(nextEvent);

        simulationTimeDelta = nextEventSimulationTime - eventSimulationTime;
        timelineCoordinateDelta = nextEventTimelineCoordinateBegin - eventTimelineCoordinateBegin;
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
            simulationTime = max(BigDecimal::Zero, min(lastEventSimulationTime, timelineCoordinate + timelineCoordinateSystemOriginSimulationTime));
        }
        break;

        case EVENT_NUMBER:
        case STEP:
        case NONLINEAR: {
            IEvent *nextEvent;
            simtime_t eventSimulationTime, nextEventSimulationTime, simulationTimeDelta;
            double eventTimelineCoordinateBegin, nextEventTimelineCoordinateBegin, timelineCoordinateDelta;

            IEvent *event = getLastEventNotAfterTimelineCoordinate(timelineCoordinate);
            extractSimulationTimesAndTimelineCoordinates(event, nextEvent,
                                                         eventSimulationTime, eventTimelineCoordinateBegin,
                                                         nextEventSimulationTime, nextEventTimelineCoordinateBegin,
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
                    timelineCoordinate = std::max(eventTimelineCoordinateBegin, std::min(nextEventTimelineCoordinateBegin, timelineCoordinate));
                    simulationTime = eventSimulationTime + simulationTimeDelta * (timelineCoordinate - eventTimelineCoordinateBegin) / timelineCoordinateDelta;
                    simulationTime = max(eventSimulationTime, min(nextEventSimulationTime, simulationTime));
                }
            }
            break;
        }

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
            timelineCoordinate = (simulationTime - timelineCoordinateSystemOriginSimulationTime).dbl();
            break;

        case EVENT_NUMBER:
        case STEP:
        case NONLINEAR: {
            IEvent *nextEvent;
            simtime_t eventSimulationTime, nextEventSimulationTime, simulationTimeDelta;
            double eventTimelineCoordinateBegin, nextEventTimelineCoordinateBegin, timelineCoordinateDelta;

            IEvent *event = eventLog->getLastEventNotAfterSimulationTime(simulationTime);
            extractSimulationTimesAndTimelineCoordinates(event, nextEvent,
                                                         eventSimulationTime, eventTimelineCoordinateBegin,
                                                         nextEventSimulationTime, nextEventTimelineCoordinateBegin,
                                                         simulationTimeDelta, timelineCoordinateDelta);

            if (nextEvent) {
                if (simulationTimeDelta == BigDecimal::Zero) {
                    // IMPORTANT NOTE: this is just an approximation
                    if (upperLimit)
                        timelineCoordinate = nextEventTimelineCoordinateBegin;
                    else
                        timelineCoordinate = eventTimelineCoordinateBegin;
                }
                else {
                    simulationTime = max(eventSimulationTime, min(nextEventSimulationTime, simulationTime));
                    timelineCoordinate = eventTimelineCoordinateBegin + timelineCoordinateDelta * (simulationTime - eventSimulationTime).dbl() / simulationTimeDelta.dbl();
                    timelineCoordinate = std::max(eventTimelineCoordinateBegin, std::min(nextEventTimelineCoordinateBegin, timelineCoordinate));
                }
            }
            else
                timelineCoordinate = eventTimelineCoordinateBegin;
            break;
        }

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
            return getTimelineCoordinateBegin(event);

        event = event->getNextEvent();
    }

    return getTimelineCoordinateForSimulationTime(simulationTime);
}

std::vector<ptr_t> *SequenceChartFacade::getComponentMethodBeginEntries(ptr_t startEventPtr, ptr_t endEventPtr)
{
    IEvent *startEvent = (IEvent *)startEventPtr;
    IEvent *endEvent = (IEvent *)endEventPtr;
    Assert(startEvent);
    Assert(endEvent);
    std::vector<ptr_t> *componentMethodBeginEntries = new std::vector<ptr_t>();

    for (IEvent *event = startEvent; ; event = event->getNextEvent()) {
        eventLog->progress();

        for (int i = 0; i < event->getNumEventLogEntries(); i++) {
            EventLogEntry *eventLogEntry = event->getEventLogEntry(i);

            if (dynamic_cast<ComponentMethodBeginEntry *>(eventLogEntry))
                componentMethodBeginEntries->push_back((ptr_t)eventLogEntry);
        }

        if (event == endEvent)
            break;
    }

    return componentMethodBeginEntries;
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

std::vector<int> SequenceChartFacade::getMessageDependencyCountAdjacencyMatrix(std::map<int, int> *moduleIdToAxisIndexMap, std::map<eventnumber_t, IEvent *>& eventNumberToEventMap, int messageSendWeight, int messageReuseWeight)
{
    std::vector<int> adjacencyMatrix;
    std::set<int> axisIndexSet;

    for (auto & it : *moduleIdToAxisIndexMap)
        axisIndexSet.insert(it.second);

    int numberOfAxes = axisIndexSet.size();
    adjacencyMatrix.resize(numberOfAxes * numberOfAxes);

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


std::vector<int> SequenceChartFacade::getMessageDependencyCountAdjacencyMatrix(std::map<int, int> *moduleIdToAxisIndexMap, ptr_t startEventPtr, ptr_t endEventPtr, int messageSendWeight, int messageReuseWeight)
{
    IEvent *startEvent = (IEvent *)startEventPtr;
    IEvent *endEvent = (IEvent *)endEventPtr;
    Assert(startEvent);
    Assert(endEvent);
    std::map<eventnumber_t, IEvent *> eventNumberToEventMap;

    for (IEvent *event = startEvent; ; event = event->getNextEvent()) {
        eventLog->progress();
        eventNumberToEventMap[event->getEventNumber()] = event;
        if (event == endEvent)
            break;
    }

    return getMessageDependencyCountAdjacencyMatrix(moduleIdToAxisIndexMap, eventNumberToEventMap, messageSendWeight, messageReuseWeight);
}

std::vector<int> SequenceChartFacade::getApproximateMessageDependencyCountAdjacencyMatrix(std::map<int, int> *moduleIdToAxisIndexMap, int numberOfSamples, int messageSendWeight, int messageReuseWeight)
{
    LCGRandom lcgRandom;
    std::map<eventnumber_t, IEvent *> eventNumberToEventMap;

    for (int i = 0; i < numberOfSamples; i++) {
        eventLog->progress();
        // draw random
        double percentage = lcgRandom.next01();
        IEvent *event = eventLog->getApproximateEventAt(percentage);
        eventNumberToEventMap[event->getEventNumber()] = event;
        // look before origin
        if (timelineCoordinateSystemOriginEventNumber != -1) {
            if (timelineCoordinateSystemOriginEventNumber - i >= 0) {
                event = eventLog->getEventForEventNumber(timelineCoordinateSystemOriginEventNumber - i);
                if (event)
                    eventNumberToEventMap[event->getEventNumber()] = event;
            }

            // look after origin
            event = eventLog->getEventForEventNumber(timelineCoordinateSystemOriginEventNumber + i);
            if (event)
                eventNumberToEventMap[event->getEventNumber()] = event;
        }
    }

    return getMessageDependencyCountAdjacencyMatrix(moduleIdToAxisIndexMap, eventNumberToEventMap, messageSendWeight, messageReuseWeight);
}

} // namespace eventlog
}  // namespace omnetpp

