//=========================================================================
//  EVENTLOGFACADE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "ievent.h"
#include "ieventlog.h"
#include "event.h"
#include "eventlog.h"
#include "filteredevent.h"
#include "filteredeventlog.h"
#include "eventlogfacade.h"

EventLogFacade::EventLogFacade(IEventLog *eventLog)
{
    Assert(eventLog);
    this->eventLog = eventLog;
}

void EventLogFacade::synchronize()
{
    return eventLog->synchronize();
}

IEvent* EventLogFacade::Event_getEvent(int64 ptr)
{
    EVENT_PTR(ptr);
    return (IEvent*)ptr;
}

int64 EventLogFacade::Event_getNonFilteredEvent(int64 ptr)
{
    EVENT_PTR(ptr);
    IEvent *ievent = (IEvent*)ptr;

    Event *event = dynamic_cast<Event *>(ievent);
    if (event)
        return ptr;
    else
        return Event_getNonFilteredEvent((int64)((FilteredEvent *)ievent)->getEvent());
}

int64 EventLogFacade::Event_getEventForEventNumber(long eventNumber)
{
    return (int64)eventLog->getEventForEventNumber(eventNumber);
}

int64 EventLogFacade::Event_getNonFilteredEventForEventNumber(long eventNumber)
{
    EventLog *nonFilteredEventLog = dynamic_cast<EventLog *>(eventLog);

    if (nonFilteredEventLog)
        return (int64)nonFilteredEventLog->getEventForEventNumber(eventNumber);
    else
        return (int64)((FilteredEventLog *)eventLog)->getEventLog()->getEventForEventNumber(eventNumber);
}

int64 EventLogFacade::Event_getPreviousEvent(int64 ptr)
{
    EVENT_PTR(ptr);
    return (int64)((IEvent*)ptr)->getPreviousEvent();
}

int64 EventLogFacade::Event_getNextEvent(int64 ptr)
{
    EVENT_PTR(ptr);
    return (int64)((IEvent*)ptr)->getNextEvent();
}

long EventLogFacade::Event_getEventNumber(int64 ptr)
{
    EVENT_PTR(ptr);
    return ((IEvent*)ptr)->getEventNumber();
}

simtime_t EventLogFacade::Event_getSimulationTime(int64 ptr)
{
    EVENT_PTR(ptr);
    return ((IEvent*)ptr)->getSimulationTime();
}

double EventLogFacade::Event_getSimulationTimeAsDouble(int64 ptr)
{
    EVENT_PTR(ptr);
    return ((IEvent*)ptr)->getSimulationTime().dbl();
}

int EventLogFacade::Event_getModuleId(int64 ptr)
{
    EVENT_PTR(ptr);
    return ((IEvent*)ptr)->getModuleId();
}

int EventLogFacade::Event_getNumCauses(int64 ptr)
{
    EVENT_PTR(ptr);
    return ((IEvent*)ptr)->getCauses()->size();
}

int EventLogFacade::Event_getNumConsequences(int64 ptr)
{
    EVENT_PTR(ptr);
    return ((IEvent*)ptr)->getConsequences()->size();
}

int64 EventLogFacade::Event_getCause(int64 ptr, int index)
{
    EVENT_PTR(ptr);
    return (int64)((IEvent*)ptr)->getCauses()->at(index);
}

int64 EventLogFacade::Event_getConsequence(int64 ptr, int index)
{
    EVENT_PTR(ptr);
    return (int64)((IEvent*)ptr)->getConsequences()->at(index);
}

bool EventLogFacade::Event_isSelfMessageProcessingEvent(int64 ptr)
{
    EVENT_PTR(ptr);
    return ((IEvent*)ptr)->isSelfMessageProcessingEvent();
}

IMessageDependency *EventLogFacade::MessageDependency_getMessageDependency(int64 ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return (IMessageDependency*)ptr;
}

const char *EventLogFacade::MessageDependency_getMessageName(int64 ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return ((IMessageDependency*)ptr)->getBeginSendEntry()->messageFullName;
}

const char *EventLogFacade::FilteredMessageDependency_getBeginMessageName(int64 ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return ((FilteredMessageDependency*)ptr)->getBeginMessageDependency()->getBeginSendEntry()->messageFullName;
}

const char *EventLogFacade::FilteredMessageDependency_getEndMessageName(int64 ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return ((FilteredMessageDependency*)ptr)->getEndMessageDependency()->getBeginSendEntry()->messageFullName;
}

bool EventLogFacade::MessageDependency_getIsReuse(int64 ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return ((IMessageDependency*)ptr)->getIsReuse();
}

bool EventLogFacade::MessageDependency_isFilteredMessageDependency(int64 ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return dynamic_cast<FilteredMessageDependency*>((IMessageDependency*)ptr);
}

int64 EventLogFacade::MessageDependency_getCauseEvent(int64 ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return (int64)((IMessageDependency*)ptr)->getCauseEvent();
}

int64 EventLogFacade::MessageDependency_getConsequenceEvent(int64 ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return (int64)((IMessageDependency*)ptr)->getConsequenceEvent();
}

simtime_t EventLogFacade::MessageDependency_getCauseSimulationTime(int64 ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return ((IMessageDependency*)ptr)->getCauseSimulationTime();
}

simtime_t EventLogFacade::MessageDependency_getConsequenceSimulationTime(int64 ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return ((IMessageDependency*)ptr)->getConsequenceSimulationTime();
}
