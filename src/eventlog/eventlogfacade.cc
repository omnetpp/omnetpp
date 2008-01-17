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

USING_NAMESPACE

EventLogFacade::EventLogFacade(IEventLog *eventLog)
{
    Assert(eventLog);
    this->eventLog = eventLog;
}

void EventLogFacade::synchronize()
{
    return eventLog->synchronize();
}

IEvent* EventLogFacade::Event_getEvent(ptr_t ptr)
{
    EVENT_PTR(ptr);
    return (IEvent*)ptr;
}

ptr_t EventLogFacade::Event_getNonFilteredEvent(ptr_t ptr)
{
    EVENT_PTR(ptr);
    IEvent *ievent = (IEvent*)ptr;

    Event *event = dynamic_cast<Event *>(ievent);
    if (event)
        return ptr;
    else
        return Event_getNonFilteredEvent((ptr_t)((FilteredEvent *)ievent)->getEvent());
}

ptr_t EventLogFacade::Event_getEventForEventNumber(long eventNumber)
{
    return (ptr_t)eventLog->getEventForEventNumber(eventNumber);
}

ptr_t EventLogFacade::Event_getNonFilteredEventForEventNumber(long eventNumber)
{
    EventLog *nonFilteredEventLog = dynamic_cast<EventLog *>(eventLog);

    if (nonFilteredEventLog)
        return (ptr_t)nonFilteredEventLog->getEventForEventNumber(eventNumber);
    else
        return (ptr_t)((FilteredEventLog *)eventLog)->getEventLog()->getEventForEventNumber(eventNumber);
}

ptr_t EventLogFacade::Event_getPreviousEvent(ptr_t ptr)
{
    EVENT_PTR(ptr);
    return (ptr_t)((IEvent*)ptr)->getPreviousEvent();
}

ptr_t EventLogFacade::Event_getNextEvent(ptr_t ptr)
{
    EVENT_PTR(ptr);
    return (ptr_t)((IEvent*)ptr)->getNextEvent();
}

long EventLogFacade::Event_getEventNumber(ptr_t ptr)
{
    EVENT_PTR(ptr);
    return ((IEvent*)ptr)->getEventNumber();
}

simtime_t& EventLogFacade::Event_getSimulationTime(ptr_t ptr)
{
    EVENT_PTR(ptr);
    return ((IEvent*)ptr)->getSimulationTime();
}

double EventLogFacade::Event_getSimulationTimeAsDouble(ptr_t ptr)
{
    EVENT_PTR(ptr);
    return ((IEvent*)ptr)->getSimulationTime().dbl();
}

int EventLogFacade::Event_getModuleId(ptr_t ptr)
{
    EVENT_PTR(ptr);
    return ((IEvent*)ptr)->getModuleId();
}

int EventLogFacade::Event_getNumCauses(ptr_t ptr)
{
    EVENT_PTR(ptr);
    return ((IEvent*)ptr)->getCauses()->size();
}

int EventLogFacade::Event_getNumConsequences(ptr_t ptr)
{
    EVENT_PTR(ptr);
    return ((IEvent*)ptr)->getConsequences()->size();
}

ptr_t EventLogFacade::Event_getCause(ptr_t ptr, int index)
{
    EVENT_PTR(ptr);
    return (ptr_t)((IEvent*)ptr)->getCauses()->at(index);
}

ptr_t EventLogFacade::Event_getConsequence(ptr_t ptr, int index)
{
    EVENT_PTR(ptr);
    return (ptr_t)((IEvent*)ptr)->getConsequences()->at(index);
}

bool EventLogFacade::Event_isSelfMessageProcessingEvent(ptr_t ptr)
{
    EVENT_PTR(ptr);
    return ((IEvent*)ptr)->isSelfMessageProcessingEvent();
}

IMessageDependency *EventLogFacade::MessageDependency_getMessageDependency(ptr_t ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return (IMessageDependency*)ptr;
}

const char *EventLogFacade::MessageDependency_getMessageName(ptr_t ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return ((IMessageDependency*)ptr)->getBeginSendEntry()->messageFullName;
}

const char *EventLogFacade::FilteredMessageDependency_getBeginMessageName(ptr_t ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return ((FilteredMessageDependency*)ptr)->getBeginMessageDependency()->getBeginSendEntry()->messageFullName;
}

const char *EventLogFacade::FilteredMessageDependency_getEndMessageName(ptr_t ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return ((FilteredMessageDependency*)ptr)->getEndMessageDependency()->getBeginSendEntry()->messageFullName;
}

bool EventLogFacade::MessageDependency_getIsReuse(ptr_t ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return ((IMessageDependency*)ptr)->getIsReuse();
}

bool EventLogFacade::MessageDependency_isFilteredMessageDependency(ptr_t ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return dynamic_cast<FilteredMessageDependency*>((IMessageDependency*)ptr);
}

ptr_t EventLogFacade::MessageDependency_getCauseEvent(ptr_t ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return (ptr_t)((IMessageDependency*)ptr)->getCauseEvent();
}

ptr_t EventLogFacade::MessageDependency_getConsequenceEvent(ptr_t ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return (ptr_t)((IMessageDependency*)ptr)->getConsequenceEvent();
}

simtime_t& EventLogFacade::MessageDependency_getCauseSimulationTime(ptr_t ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return ((IMessageDependency*)ptr)->getCauseSimulationTime();
}

simtime_t& EventLogFacade::MessageDependency_getConsequenceSimulationTime(ptr_t ptr)
{
    MESSAGE_DEPENDENCY_PTR(ptr);
    return ((IMessageDependency*)ptr)->getConsequenceSimulationTime();
}
