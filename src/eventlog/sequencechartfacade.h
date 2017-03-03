//=========================================================================
//  SEQUENCECHARTFACADE.H - part of
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

#ifndef __OMNETPP_SEQUENCECHARTFACADE_H
#define __OMNETPP_SEQUENCECHARTFACADE_H

#include <cfloat>
#include <vector>
#include <map>
#include "ievent.h"
#include "ieventlog.h"
#include "eventlogfacade.h"

namespace omnetpp {
namespace eventlog {

/**
 * A class that makes it possible to extract info about events, without
 * returning objects. (Wherever a C++ method returns an object pointer,
 * SWIG-generated wrapper creates a corresponding Java object with the
 * pointer value inside. This has disastrous effect on performance
 * when dealing with huge amounts of data).
 */
class EVENTLOG_API SequenceChartFacade : public EventLogFacade
{
    protected:
        long timelineCoordinateSystemVersion; // a counter incremented each time the timeline coordinate system is relocated
        eventnumber_t timelineCoordinateOriginEventNumber; // -1 means undefined, otherwise the event number of the timeline coordinate system origin
        simtime_t timelineCoordinateOriginSimulationTime; // simtime_nil means undefined
        eventnumber_t timelineCoordinateRangeStartEventNumber; // -1 means undefined, the beginning of the continuous event range which has timeline coordinates assigned
        eventnumber_t timelineCoordinateRangeEndEventNumber; // -1 means undefined, the end of the continuous event range which has timeline coordinates assigned
        TimelineMode timelineMode;
        double nonLinearFocus; // a useful constant for the nonlinear transformation between simulation time and timeline coordinate
        double nonLinearMinimumTimelineCoordinateDelta; // minimum timeline coordinate difference between two events

    public:
        SequenceChartFacade(IEventLog *eventLog);
        virtual ~SequenceChartFacade() {}

        double calculateNonLinearFocus();
        virtual void synchronize(FileReader::FileChangedState change) override;

        TimelineMode getTimelineMode() { return timelineMode; }
        void setTimelineMode(TimelineMode timelineMode);

        double getNonLinearMinimumTimelineCoordinateDelta() { return nonLinearMinimumTimelineCoordinateDelta; }
        void setNonLinearMinimumTimelineCoordinateDelta(double value);
        double getNonLinearFocus();
        void setNonLinearFocus(double nonLinearFocus);

        IEvent *getTimelineCoordinateSystemOriginEvent() { return eventLog->getEventForEventNumber(timelineCoordinateOriginEventNumber); }
        eventnumber_t getTimelineCoordinateSystemOriginEventNumber() { return timelineCoordinateOriginEventNumber; }
        double getTimelineCoordinateDelta(double simulationTimeDelta);
        /**
         * Calculates the timeline coordinate for the given event. Returns value from the cache if already there or calculates it
         * while taking care about the calculation limits.
         */
        double getTimelineCoordinate(ptr_t ptr, double lowerTimelineCoordinateCalculationLimit = -DBL_MAX, double upperTimelineCoordinateCalculationLimit = DBL_MAX);
        double getTimelineCoordinate(IEvent *event, double lowerTimelineCoordinateCalculationLimit = -DBL_MAX, double upperTimelineCoordinateCalculationLimit = DBL_MAX);
        double getCachedTimelineCoordinate(IEvent *event);
        double IEvent_getTimelineCoordinate(ptr_t ptr);
        void undefineTimelineCoordinateSystem();
        void relocateTimelineCoordinateSystem(IEvent *event);

        IEvent *getEventForNonLinearTimelineCoordinate(double timelineCoordinate, bool &forward);
        IEvent *getLastEventNotAfterTimelineCoordinate(double timelineCoordinate);
        IEvent *getFirstEventNotBeforeTimelineCoordinate(double timelineCoordinate);

        /**
         * Timeline coordinate can be given in the range (-infinity, +infinity).
         * Simulation time will be in the range [0, lastEventSimulationTime].
         */
        simtime_t getSimulationTimeForTimelineCoordinate(double timelineCoordinate, bool upperLimit = false);
        /**
         * Simulation time must be in the range [0, lastEventSimulationTime].
         * Timeline coordinate will be in the range [0, lastEventTimelineCoordinate] if the
         * timeline origin is at the first event.
         */
        double getTimelineCoordinateForSimulationTime(simtime_t simulationTime, bool upperLimit = false);
        double getTimelineCoordinateForSimulationTimeAndEventInModule(simtime_t simulationTime, int moduleId);

        std::vector<ptr_t> *getModuleMethodBeginEntries(ptr_t startEventPtr, ptr_t endEventPtr);

        std::vector<ptr_t> *getIntersectingMessageDependencies(ptr_t startEventPtr, ptr_t endEventPtr);
        std::vector<int> getApproximateMessageDependencyCountAdjacencyMatrix(std::map<int, int> *moduleIdToAxisIdMap, int numberOfSamples, int messageSendWeight = 1, int messageReuseWeight = 1);

    protected:
        void extractSimulationTimesAndTimelineCoordinates(
            IEvent *event, IEvent *&nextEvent,
            simtime_t &eventSimulationTime, double &eventTimelineCoordinate,
            simtime_t &nextEventSimulationTime, double &nextEventTimelineCoordinate,
            simtime_t &simulationTimeDelta, double &timelineCoordinateDelta);
};

} // namespace eventlog
}  // namespace omnetpp

#endif
