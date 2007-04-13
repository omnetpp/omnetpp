//=========================================================================
//  SEQUENCECHARTFACADE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __SEQUENCECHARTFACADE_H_
#define __SEQUENCECHARTFACADE_H_

#include <vector>
#include <map>
#include "ievent.h"
#include "ieventlog.h"
#include "eventlogfacade.h"

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
        long timelineCoordinateSystemVersion;
        long timelineCoordinateOriginEventNumber;
        double timelineCoordinateOriginSimulationTime;
        long timelineCoordinateRangeStartEventNumber;
        long timelineCoordinateRangeEndEventNumber;
        TimelineMode timelineMode;

    public:
        SequenceChartFacade(IEventLog *eventLog);
        virtual ~SequenceChartFacade() {}

        TimelineMode getTimelineMode() { return timelineMode; }
        void setTimelineMode(TimelineMode timelineMode);

        IEvent *getTimelineCoordinateSystemOriginEvent() { return eventLog->getEventForEventNumber(timelineCoordinateOriginEventNumber); }
        double getNonLinearTimelineCoordinateDelta(double simulationTimeDelta);
        double getTimelineCoordinate(IEvent *event);
        double Event_getTimelineCoordinate(int64 ptr);
        void relocateTimelineCoordinateSystem(IEvent *event);

        IEvent *getEventForNonLinearTimelineCoordinate(double timelineCoordinate, bool &forward);
        IEvent *getLastEventNotAfterTimelineCoordinate(double timelineCoordinate);
        IEvent *getFirstEventNotBeforeTimelineCoordinate(double timelineCoordinate);

        /**
         * Timeline coordinate can be given in the range (-infinity, +infinity).
         * Simulation time will be in the range [0, lastEventSimulationTime].
         */
        double getSimulationTimeForTimelineCoordinate(double timelineCoordinate, bool upperBound = false);
        /**
         * Simulation time must be in the range [0, lastEventSimulationTime].
         * Timeline coordinate will be in the range [0, lastEventTimelineCoordinate] if the
         * timeline origin is at the first event.
         */
        double getTimelineCoordinateForSimulationTime(double simulationTime, bool upperBound = false);

        std::vector<int64> *getIntersectingMessageDependencies(int64 startEventPtr, int64 endEventPtr, int lookAroundCount);
        std::vector<int> getApproximateMessageDependencyCountAdjacencyMatrix(std::map<int, int> *moduleIdToAxisIdMap, int numberOfSamples);

    protected:

        void extractSimulationTimesAndTimelineCoordinates(
            IEvent *event, IEvent *&nextEvent,
            double &eventSimulationTime, double &eventTimelineCoordinate,
            double &nextEventSimulationTime, double &nextEventTimelineCoordinate,
            double &simulationTimeDelta, double &timelineCoordinateDelta);
};

#endif
