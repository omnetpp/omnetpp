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
#include "eventlogfacade.h"

EventLogFacade::EventLogFacade(IEventLog *eventLog)
{
    Assert(eventLog);
    this->eventLog = eventLog;
}

IEvent* EventLogFacade::Event_getEvent(int64 ptr)
{
    PTR(ptr);
    return (IEvent*)ptr;
}

int64 EventLogFacade::Event_getPreviousEvent(int64 ptr)
{
    PTR(ptr);
    return (int64)((IEvent*)ptr)->getPreviousEvent();
}

int64 EventLogFacade::Event_getNextEvent(int64 ptr)
{
    PTR(ptr);
    return (int64)((IEvent*)ptr)->getNextEvent();
}

long EventLogFacade::Event_getEventNumber(int64 ptr)
{
    PTR(ptr);
    return ((IEvent*)ptr)->getEventNumber();
}

int EventLogFacade::Event_getModuleId(int64 ptr)
{
    PTR(ptr);
    return ((IEvent*)ptr)->getModuleId();
}

int EventLogFacade::Event_getNumCauses(int64 ptr)
{
    PTR(ptr);
    return ((IEvent*)ptr)->getCauses()->size();
}

int EventLogFacade::Event_getNumConsequences(int64 ptr)
{
    PTR(ptr);
    return ((IEvent*)ptr)->getConsequences()->size();
}

int64 EventLogFacade::Event_getCause(int64 ptr, int index)
{
    PTR(ptr);
    return (int64)((IEvent*)ptr)->getCauses()->at(index);
}

int64 EventLogFacade::Event_getConsequence(int64 ptr, int index)
{
    PTR(ptr);
    return (int64)((IEvent*)ptr)->getConsequences()->at(index);
}

bool EventLogFacade::Event_isSelfEvent(int64 ptr)
{
    PTR(ptr);
    return ((IEvent*)ptr)->isSelfEvent();
}

MessageDependency* EventLogFacade::MessageDependency_getMessageDependency(int64 ptr)
{
    PTR(ptr);
    return (MessageDependency*)ptr;
}

const char *EventLogFacade::MessageDependency_getCauseMessageName(int64 ptr)
{
    PTR(ptr);
    return ((MessageDependency*)ptr)->getCauseBeginSendEntry()->messageFullName;
}

const char *EventLogFacade::MessageDependency_getConsequenceMessageName(int64 ptr)
{
    PTR(ptr);
    return ((MessageDependency*)ptr)->getConsequenceBeginSendEntry()->messageFullName;
}

MessageDependencyKind EventLogFacade::MessageDependency_getKind(int64 ptr)
{
    PTR(ptr);
    if (MessageDependency_isMessageSend(ptr))
        return SEND;
    else if (MessageDependency_isMessageReuse(ptr))
        return REUSE;
    else return FILTERED;
}

bool EventLogFacade::MessageDependency_isMessageSend(int64 ptr)
{
    PTR(ptr);
    return dynamic_cast<MessageSend*>((MessageDependency*)ptr);
}

bool EventLogFacade::MessageDependency_isMessageReuse(int64 ptr)
{
    PTR(ptr);
    return dynamic_cast<MessageReuse*>((MessageDependency*)ptr);
}

bool EventLogFacade::MessageDependency_isFilteredMessageDependency(int64 ptr)
{
    PTR(ptr);
    return dynamic_cast<FilteredMessageDependency*>((MessageDependency*)ptr);
}

int64 EventLogFacade::MessageDependency_getCauseEvent(int64 ptr)
{
    PTR(ptr);
    return (int64)((MessageDependency*)ptr)->getCauseEvent();
}

int64 EventLogFacade::MessageDependency_getConsequenceEvent(int64 ptr)
{
    PTR(ptr);
    return (int64)((MessageDependency*)ptr)->getConsequenceEvent();
}

const char *EventLogFacade::FilteredMessageDependency_getMiddleMessageName(int64 ptr)
{
    PTR(ptr);
    return ((FilteredMessageDependency*)ptr)->getMiddleBeginSendEntry()->messageFullName;
}
