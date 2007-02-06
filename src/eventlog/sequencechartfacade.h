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
class SequenceChartFacade : public EventLogFacade
{
    protected:
        long timelineCoordinateSystemVersion;
        TimelineMode timelineMode;

    public:
        SequenceChartFacade(IEventLog *eventLog);
        virtual ~SequenceChartFacade() {}

        TimelineMode getTimelineMode() { return timelineMode; }
        void setTimelineMode(TimelineMode timelineMode) { this->timelineMode = timelineMode; invalidateTimelineCoordinateSystem(); }
        double getTimelineCoordinate(IEvent *event);
        void invalidateTimelineCoordinateSystem() { timelineCoordinateSystemVersion++; }
        double Event_getTimelineCoordinate(int64 ptr);

        IEvent *getLastEventNotAfterTimelineCoordinate(double timelineCoordinate);
        IEvent *getFirstEventNotBeforeTimelineCoordinate(double timelineCoordinate);

        double getSimulationTimeForTimelineCoordinate(double timelineCoordinate);
        double getTimelineCoordinateForSimulationTime(double simulationTime);

        std::vector<int64> *getIntersectingMessageDependencies(int64 startEventPtr, int64 endEventPtr);
};

#endif
