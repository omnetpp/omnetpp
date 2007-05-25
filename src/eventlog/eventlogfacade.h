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

#define PTR(ptr) if (ptr == 0) throw opp_runtime_error("NULL ptr exception");
#define EVENT_PTR(ptr) PTR(ptr) Assert(dynamic_cast<IEvent *>((IEvent *)ptr));
#define MESSAGE_DEPENDENCY_PTR(ptr) PTR(ptr) Assert(dynamic_cast<IMessageDependency *>((IMessageDependency *)ptr));

/**
 * A class that makes it possible to extract info about events, without
 * returning objects. (Wherever a C++ method returns an object pointer,
 * SWIG-generated wrapper creates a corresponding Java object with the
 * pointer value inside. This has disastrous effect on performance
 * when dealing with huge amounts of data).
 */
class EVENTLOG_API EventLogFacade
{
    protected:
        IEventLog *eventLog;

    public:
        EventLogFacade(IEventLog *eventLog);
        virtual ~EventLogFacade() {}

        void setEventLog(IEventLog *eventLog) { this->eventLog = eventLog; }
        virtual void synchronize();

        IEvent* Event_getEvent(int64 ptr);
        int64 Event_getPreviousEvent(int64 ptr);
        int64 Event_getNextEvent(int64 ptr);
        long Event_getEventNumber(int64 ptr);
        simtime_t Event_getSimulationTime(int64 ptr);
        int Event_getModuleId(int64 ptr);
        int Event_getNumCauses(int64 ptr);
        int Event_getNumConsequences(int64 ptr);
        int64 Event_getCause(int64 ptr, int index);
        int64 Event_getConsequence(int64 ptr, int index);
        bool Event_isSelfEvent(int64 ptr);

        IMessageDependency *MessageDependency_getMessageDependency(int64 ptr);
        const char *MessageDependency_getMessageName(int64 ptr);
        const char *FilteredMessageDependency_getBeginMessageName(int64 ptr);
        const char *FilteredMessageDependency_getEndMessageName(int64 ptr);
        bool MessageDependency_getIsReuse(int64 ptr);
        bool MessageDependency_isFilteredMessageDependency(int64 ptr);
        int64 MessageDependency_getCauseEvent(int64 ptr);
        int64 MessageDependency_getConsequenceEvent(int64 ptr);
        simtime_t MessageDependency_getCauseSimulationTime(int64 ptr);
        simtime_t MessageDependency_getConsequenceSimulationTime(int64 ptr);
};

#endif
