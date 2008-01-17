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

NAMESPACE_BEGIN

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

        void setEventLog(IEventLog *eventLog) { Assert(eventLog); this->eventLog = eventLog; }
        virtual void synchronize();

        IEvent* Event_getEvent(ptr_t ptr);
        ptr_t Event_getNonFilteredEvent(ptr_t ptr);
        ptr_t Event_getEventForEventNumber(long eventNumber);
        ptr_t Event_getNonFilteredEventForEventNumber(long eventNumber);
        ptr_t Event_getPreviousEvent(ptr_t ptr);
        ptr_t Event_getNextEvent(ptr_t ptr);
        long Event_getEventNumber(ptr_t ptr);
        simtime_t& Event_getSimulationTime(ptr_t ptr);
        double Event_getSimulationTimeAsDouble(ptr_t ptr);
        int Event_getModuleId(ptr_t ptr);
        int Event_getNumCauses(ptr_t ptr);
        int Event_getNumConsequences(ptr_t ptr);
        ptr_t Event_getCause(ptr_t ptr, int index);
        ptr_t Event_getConsequence(ptr_t ptr, int index);
        bool Event_isSelfMessageProcessingEvent(ptr_t ptr);

        IMessageDependency *MessageDependency_getMessageDependency(ptr_t ptr);
        const char *MessageDependency_getMessageName(ptr_t ptr);
        const char *FilteredMessageDependency_getBeginMessageName(ptr_t ptr);
        const char *FilteredMessageDependency_getEndMessageName(ptr_t ptr);
        bool MessageDependency_getIsReuse(ptr_t ptr);
        bool MessageDependency_isFilteredMessageDependency(ptr_t ptr);
        ptr_t MessageDependency_getCauseEvent(ptr_t ptr);
        ptr_t MessageDependency_getConsequenceEvent(ptr_t ptr);
        simtime_t& MessageDependency_getCauseSimulationTime(ptr_t ptr);
        simtime_t& MessageDependency_getConsequenceSimulationTime(ptr_t ptr);
};

NAMESPACE_END


#endif
