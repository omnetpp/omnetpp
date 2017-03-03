//=========================================================================
//  EVENTLOGFACADE.H - part of
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

#ifndef __OMNETPP_EVENTLOG_EVENTLOGFACADE_H
#define __OMNETPP_EVENTLOG_EVENTLOGFACADE_H

#include "ievent.h"
#include "ieventlog.h"

namespace omnetpp {
namespace eventlog {

#define PTR(ptr) if (ptr == 0) throw opp_runtime_error("nullptr ptr exception");
#define IEVENT_PTR(ptr) PTR(ptr) Assert(dynamic_cast<IEvent *>((IEvent *)ptr));
#define EVENT_LOG_ENTRY_PTR(ptr) PTR(ptr) Assert(dynamic_cast<EventLogEntry *>((EventLogEntry *)ptr));
#define MESSAGE_ENTRY_PTR(ptr) PTR(ptr) Assert(dynamic_cast<BeginSendEntry *>((BeginSendEntry *)ptr));
#define BEGIN_SEND_ENTRY_PTR(ptr) PTR(ptr) Assert(dynamic_cast<BeginSendEntry *>((BeginSendEntry *)ptr));
#define END_SEND_ENTRY_PTR(ptr) PTR(ptr) Assert(dynamic_cast<EndSendEntry *>((EndSendEntry *)ptr));
#define MODULE_CREATED_ENTRY_PTR(ptr) PTR(ptr) Assert(dynamic_cast<ModuleCreatedEntry *>((ModuleCreatedEntry *)ptr));
#define MODULE_METHOD_BEGIN_ENTRY_PTR(ptr) PTR(ptr) Assert(dynamic_cast<ModuleMethodBeginEntry *>((ModuleMethodBeginEntry *)ptr));
#define IMESSAGE_DEPENDENCY_PTR(ptr) PTR(ptr) Assert(dynamic_cast<IMessageDependency *>((IMessageDependency *)ptr));
#define MESSAGE_SEND_DEPENDENCY_PTR(ptr) PTR(ptr) Assert(dynamic_cast<MessageSendDependency *>((MessageSendDependency *)ptr));
#define MESSAGE_REUSE_DEPENDENCY_PTR(ptr) PTR(ptr) Assert(dynamic_cast<MessageReuesDependency *>((MessageReuseDependency *)ptr));
#define FILTERED_MESSAGE_DEPENDENCY_PTR(ptr) PTR(ptr) Assert(dynamic_cast<FilteredMessageDependency *>((FilteredMessageDependency *)ptr));

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
        virtual void synchronize(FileReader::FileChangedState change);

        IEvent* IEvent_getEvent(ptr_t ptr);
        ptr_t IEvent_getNonFilteredEvent(ptr_t ptr);
        ptr_t IEvent_getEventForEventNumber(eventnumber_t eventNumber);
        ptr_t IEvent_getNonFilteredEventForEventNumber(eventnumber_t eventNumber);
        ptr_t IEvent_getPreviousEvent(ptr_t ptr);
        ptr_t IEvent_getNextEvent(ptr_t ptr);
        eventnumber_t IEvent_getEventNumber(ptr_t ptr);
        simtime_t IEvent_getSimulationTime(ptr_t ptr);
        double IEvent_getSimulationTimeAsDouble(ptr_t ptr);
        int IEvent_getModuleId(ptr_t ptr);
        int IEvent_getNumCauses(ptr_t ptr);
        int IEvent_getNumConsequences(ptr_t ptr);
        ptr_t IEvent_getCause(ptr_t ptr, int index);
        ptr_t IEvent_getConsequence(ptr_t ptr, int index);
        bool IEvent_isSelfMessageProcessingEvent(ptr_t ptr);

        EventLogEntry *EventLogEntry_getEventLogEntry(ptr_t ptr);
        ptr_t EventLogEntry_getEvent(ptr_t ptr);
        int EventLogEntry_getContextModuleId(ptr_t ptr);
        int EventLogEntry_getEntryIndex(ptr_t ptr);

        bool MessageEntry_isBeginSendEntry(ptr_t ptr);
        int MessageEntry_getMessageId(ptr_t ptr);

        bool BeginSendEntry_isSelfMessage(ptr_t ptr);
        ptr_t BeginSendEntry_getEndSendEntry(ptr_t ptr);
        simtime_t BeginSendEntry_getTransmissionDelay(ptr_t ptr);
        bool EndSendEntry_isReceptionStart(ptr_t ptr);

        const char *ModuleCreatedEntry_getModuleFullPath(ptr_t ptr);

        int ModuleMethodBeginEntry_getFromModuleId(ptr_t ptr);
        int ModuleMethodBeginEntry_getToModuleId(ptr_t ptr);
        const char *ModuleMethodBeginEntry_getMethod(ptr_t ptr);

        IMessageDependency *IMessageDependency_getMessageDependency(ptr_t ptr);
        const char *IMessageDependency_getMessageName(ptr_t ptr);
        bool IMessageDependency_isReuse(ptr_t ptr);
        bool IMessageDependency_isFilteredMessageDependency(ptr_t ptr);
        ptr_t IMessageDependency_getCauseEvent(ptr_t ptr);
        ptr_t IMessageDependency_getConsequenceEvent(ptr_t ptr);
        ptr_t IMessageDependency_getMessageEntry(ptr_t ptr);
        simtime_t IMessageDependency_getCauseSimulationTime(ptr_t ptr);
        simtime_t IMessageDependency_getConsequenceSimulationTime(ptr_t ptr);

        const char *FilteredMessageDependency_getBeginMessageName(ptr_t ptr);
        const char *FilteredMessageDependency_getEndMessageName(ptr_t ptr);
        FilteredMessageDependency::Kind FilteredMessageDependency_getKind(ptr_t ptr);
};

} // namespace eventlog
}  // namespace omnetpp


#endif
