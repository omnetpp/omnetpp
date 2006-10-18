//=========================================================================
//  EVENTLOGFACADE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTLOGFACADE_H_
#define __EVENTLOGFACADE_H_

#include "ievent.h"
#include "ieventlog.h"

#define PTR(ptr) if (ptr == 0) throw new Exception("NULL ptr exception");

/**
 * A class that makes it possible to extract info about events, without
 * returning objects. (Wherever a C++ method returns an object pointer,
 * SWIG-generated wrapper creates a corresponding Java object with the
 * pointer value inside. This has disastrous effect on performance
 * when dealing with huge amounts of data).
 *
 * IMPORTANT! Java code MUST NOT ask members of objects that don't exist!
 * Otherwise there will be a crash. For example, if getEvent_i_hasCause(i)
 * returns false, then getEvent_i_cause_messageClassName(i) will CRASH!
 */
class EventLogFacade
{
    protected:
        IEventLog *eventLog;

        // EventLogTable state
        long approximateNumberOfEventLogTableEntries;

        // SequenceChart state
        long timelineCoordinateSystemVersion;
        TimelineMode timelineMode;

    public:
        EventLogFacade(IEventLog *eventLog);

        // EventLogTable interface
		EventLogEntry *getFirstEventLogTableEntry();
		EventLogEntry *getLastEventLogTableEntry();
        EventLogEntry *getPreviousEventLogTableEntry(EventLogEntry *eventLogEntry, int& index);
        EventLogEntry *getNextEventLogTableEntry(EventLogEntry *eventLogEntry, int& index);
        int getEventLogTableEntryIndexInEvent(EventLogEntry *eventLogEntry);
        EventLogEntry *getEventLogTableEntryInEvent(IEvent *event, int index);
        EventLogEntry *getEventLogTableEntryAndDistance(EventLogEntry *sourceEventLogEntry, EventLogEntry *targetEventLogEntry, long distance, long& reachedDistance);
		long getDistanceToEventLogTableEntry(EventLogEntry *sourceEventLogEntry, EventLogEntry *targetEventLogEntry, long limit);
		long getDistanceToFirstEventLogTableEntry(EventLogEntry *eventLogEntry, long limit);
		long getDistanceToLastEventLogTableEntry(EventLogEntry *eventLogEntry, long limit);
		EventLogEntry *getNeighbourEventLogTableEntry(EventLogEntry *eventLogEntry, long distance);
        double getApproximatePercentageForEventLogTableEntry(EventLogEntry *eventLogEntry);
		EventLogEntry *getApproximateEventLogEntryTableAt(double percentage);
		long getApproximateNumberOfEventLogTableEntries();

        // SequenceChart interface
        TimelineMode getTimelineMode() { return timelineMode; }
        void setTimelineMode(TimelineMode timelineMode) { this->timelineMode = timelineMode; invalidateTimelineCoordinateSystem(); }
        double getTimelineCoordinate(IEvent *event);
        void invalidateTimelineCoordinateSystem() { timelineCoordinateSystemVersion++; }

        IEvent *getLastEventBeforeTimelineCoordinate(double timelineCoordinate);
        IEvent *getFirstEventAfterTimelineCoordinate(double timelineCoordinate);

        double getSimulationTimeForTimelineCoordinate(double timelineCoordinate);
        double getTimelineCoordinateForSimulationTime(double simulationTime);

        // class short cuts
        int64 Event_getPreviousEvent(int64 ptr) { PTR(ptr); return (int64)((IEvent*)ptr)->getPreviousEvent(); }
        int64 Event_getNextEvent(int64 ptr) { PTR(ptr); return (int64)((IEvent*)ptr)->getNextEvent(); }
        double Event_getTimelineCoordinate(int64 ptr) { PTR(ptr); return getTimelineCoordinate((IEvent*)ptr); }
        long Event_getEventNumber(int64 ptr) { PTR(ptr); return ((IEvent*)ptr)->getEventNumber(); }
        int Event_getModuleId(int64 ptr) { PTR(ptr); return ((IEvent*)ptr)->getModuleId(); }
        int Event_getNumCauses(int64 ptr) { PTR(ptr); return ((IEvent*)ptr)->getCauses()->size(); }
        int Event_getNumConsequences(int64 ptr) { PTR(ptr); return ((IEvent*)ptr)->getConsequences()->size(); }
        int64 Event_getCause(int64 ptr, int index) { PTR(ptr); return (int64)((IEvent*)ptr)->getCauses()->at(index); }
        int64 Event_getConsequence(int64 ptr, int index) { PTR(ptr); return (int64)((IEvent*)ptr)->getConsequences()->at(index); }

        const char *MessageDependency_getMessageName(int64 ptr) { PTR(ptr); return ((MessageDependency*)ptr)->getBeginSendEntry()->messageFullName; }
        bool MessageDependency_isMessageSend(int64 ptr) { PTR(ptr); return dynamic_cast<MessageSend *>((MessageDependency*)ptr); }
        int64 MessageDependency_getCauseEvent(int64 ptr) { PTR(ptr); return (int64)((MessageDependency*)ptr)->getCauseEvent(); }
        int64 MessageDependency_getConsequenceEvent(int64 ptr) { PTR(ptr); return (int64)((MessageDependency*)ptr)->getConsequenceEvent(); }
};

#endif