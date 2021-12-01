//=========================================================================
//  MESSAGEDEPENDENCY.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include "eventlogdefs.h"
#include "ievent.h"
#include "ieventlog.h"
#include "event.h"
#include "eventlogentry.h"
#include "messagedependency.h"

namespace omnetpp {
namespace eventlog {

/**************************************************/

IMessageDependency::IMessageDependency(IEventLog *eventLog) : eventLog(eventLog)
{
}

bool IMessageDependency::corresponds(IMessageDependency *dependency1, IMessageDependency *dependency2)
{
    if (!dependency1 || !dependency2)
        return false;
    else {
        MessageDescriptionEntry *entry1 = dependency1->getBeginMessageDescriptionEntry();
        MessageDescriptionEntry *entry2 = dependency2->getBeginMessageDescriptionEntry();
        if (!entry1 || !entry2)
            return false;
        else
            return (entry1->messageId != -1 && entry1->messageId == entry2->messageId) ||
                   (entry1->messageTreeId != -1 && entry1->messageTreeId == entry2->messageTreeId) ||
                   (entry1->messageEncapsulationId != -1 && entry1->messageEncapsulationId == entry2->messageEncapsulationId) ||
                   (entry1->messageEncapsulationTreeId != -1 && entry1->messageEncapsulationTreeId == entry2->messageEncapsulationTreeId);
    }
}

bool IMessageDependency::equals(IMessageDependency *other)
{
    Assert(eventLog == other->eventLog);
    return true;
}

/**************************************************/

MessageSendDependency::MessageSendDependency(IEventLog *eventLog, eventnumber_t eventNumber, int eventLogEntryIndex)
    : IMessageDependency(eventLog)
{
    Assert(eventNumber >= 0 && eventLogEntryIndex >= 0);
    this->causeEventNumber = eventNumber;
    this->eventLogEntryIndex = eventLogEntryIndex;
    this->consequenceEventNumber = EVENT_NOT_YET_CALCULATED;
}

IEvent *MessageSendDependency::getCauseEvent()
{
    return eventLog->getEventForEventNumber(causeEventNumber);
}

simtime_t MessageSendDependency::getCauseSimulationTime()
{
    return getCauseEvent()->getSimulationTime();
}

eventnumber_t MessageSendDependency::getConsequenceEventNumber()
{
    if (consequenceEventNumber == EVENT_NOT_YET_CALCULATED || consequenceEventNumber == EVENT_NOT_YET_REACHED) {
        // only cause is present, calculate consequence from it.
        //
        // when a message is scheduled/sent, we don't know the arrival event number
        // yet, only the simulation time is recorded in the eventlog file.
        // So here we have to look through all events at the arrival time,
        // and find the one "caused by" our message.
        simtime_t consequenceTime = getConsequenceSimulationTime();

        if (consequenceTime == simtime_nil)
            consequenceEventNumber = NO_SUCH_EVENT;
        else if (consequenceTime > eventLog->getLastEvent()->getSimulationTime())
            consequenceEventNumber = EVENT_NOT_YET_REACHED;
        else {
            IEvent *event = eventLog->getEventForSimulationTime(consequenceTime, FIRST_OR_PREVIOUS);
            MessageDescriptionEntry *messageDescriptionEntry = getBeginMessageDescriptionEntry();

            // LONG RUNNING OPERATION
            while (event)
            {
                eventLog->progress();

                if (event->getCauseEventNumber() == getCauseEventNumber() &&
                    event->getMessageId() == messageDescriptionEntry->messageId)
                {
                    consequenceEventNumber = event->getEventNumber();
                    break;
                }

                if (event->getSimulationTime() > consequenceTime) {
                    // no more event at that simulation time, and consequence event
                    // still not found. It must have been cancelled (self message),
                    // or it is not in the file (filtered out by the user, etc).
                    consequenceEventNumber = NO_SUCH_EVENT;
                    break;
                }

                event = event->getNextEvent();
            }

            // end of file
            if (!event)
                consequenceEventNumber = EVENT_NOT_YET_REACHED;
        }
    }

    return consequenceEventNumber;
}

IEvent *MessageSendDependency::getConsequenceEvent()
{
    eventnumber_t consequenceEventNumber = getConsequenceEventNumber();

    if (consequenceEventNumber < 0)
        return nullptr;
    else
        return eventLog->getEventForEventNumber(consequenceEventNumber);
}

simtime_t MessageSendDependency::getConsequenceSimulationTime()
{
    if (consequenceEventNumber >= 0)
        return getConsequenceEvent()->getSimulationTime();
    else {
        // find the arrival time of the message
        IEvent *event = getCauseEvent();
        BeginSendEntry *beginSendEntry = (BeginSendEntry *)(event->getEventLogEntry(eventLogEntryIndex));
        EndSendEntry *endSendEntry = event->getEndSendEntry(beginSendEntry);

        if (endSendEntry)
            return endSendEntry->arrivalTime;
        else
            return simtime_nil;
    }
}

MessageDescriptionEntry *MessageSendDependency::getBeginMessageDescriptionEntry()
{
    Assert(eventLogEntryIndex != -1);
    IEvent *event = getCauseEvent();
    Assert(event);
    return (MessageDescriptionEntry *)event->getEventLogEntry(eventLogEntryIndex);
}

MessageDescriptionEntry *MessageSendDependency::getEndMessageDescriptionEntry()
{
    IEvent *event = getCauseEvent();
    BeginSendEntry *beginSendEntry = (BeginSendEntry *)(event->getEventLogEntry(eventLogEntryIndex));
    EndSendEntry *endSendEntry = event->getEndSendEntry(beginSendEntry);
    return endSendEntry;
}

MessageSendDependency *MessageSendDependency::duplicate(IEventLog *eventLog)
{
    MessageSendDependency *messageSendDependency = new MessageSendDependency(*this);
    messageSendDependency->eventLog = eventLog;
    return messageSendDependency;
}

bool MessageSendDependency::equals(IMessageDependency *other)
{
    MessageSendDependency *otherMessageSendDependency = dynamic_cast<MessageSendDependency *>(other);
    return IMessageDependency::equals(other) && otherMessageSendDependency &&
           causeEventNumber == otherMessageSendDependency->causeEventNumber &&
           consequenceEventNumber == otherMessageSendDependency->consequenceEventNumber &&
           eventLogEntryIndex == otherMessageSendDependency->eventLogEntryIndex;
}

void MessageSendDependency::print(FILE *file)
{
    getCauseEvent()->getEventEntry()->print(file);
    getBeginMessageDescriptionEntry()->print(file);
}

/**************************************************/

MessageReuseDependency::MessageReuseDependency(IEventLog *eventLog, eventnumber_t eventNumber, int eventLogEntryIndex)
    : IMessageDependency(eventLog)
{
    Assert(eventNumber >= 0 && eventLogEntryIndex >= 0);
    this->causeEventNumber = EVENT_NOT_YET_CALCULATED;
    this->consequenceEventNumber = eventNumber;
    this->eventLogEntryIndex = eventLogEntryIndex;
}

eventnumber_t MessageReuseDependency::getCauseEventNumber()
{
    if (causeEventNumber == EVENT_NOT_YET_CALCULATED) {
        // only consequence is present, calculate cause from it
        IEvent *consequenceEvent = getConsequenceEvent();
        Assert(consequenceEvent);
        MessageDescriptionEntry *messageDescriptionEntry = (MessageDescriptionEntry *)consequenceEvent->getEventLogEntry(eventLogEntryIndex);
        causeEventNumber = messageDescriptionEntry->previousEventNumber;
    }

    return causeEventNumber;
}

IEvent *MessageReuseDependency::getCauseEvent()
{
    eventnumber_t causeEventNumber = getCauseEventNumber();

    if (causeEventNumber < 0)
        return nullptr;
    else
        return eventLog->getEventForEventNumber(causeEventNumber);
}

simtime_t MessageReuseDependency::getCauseSimulationTime()
{
    if (causeEventNumber >= 0)
        return getCauseEvent()->getSimulationTime();
    else
        return simtime_nil;
}

IEvent *MessageReuseDependency::getConsequenceEvent()
{
    return eventLog->getEventForEventNumber(consequenceEventNumber);
}

simtime_t MessageReuseDependency::getConsequenceSimulationTime()
{
    return getConsequenceEvent()->getSimulationTime();
}

MessageDescriptionEntry *MessageReuseDependency::getBeginMessageDescriptionEntry()
{
    Assert(eventLogEntryIndex != -1);
    IEvent *event = getConsequenceEvent();
    Assert(event);
    return (MessageDescriptionEntry *)event->getEventLogEntry(eventLogEntryIndex);
}

MessageDescriptionEntry *MessageReuseDependency::getEndMessageDescriptionEntry()
{
    return getBeginMessageDescriptionEntry();
}

MessageReuseDependency *MessageReuseDependency::duplicate(IEventLog *eventLog)
{
    MessageReuseDependency *messageReuseDependency = new MessageReuseDependency(*this);
    messageReuseDependency->eventLog = eventLog;
    return messageReuseDependency;
}

bool MessageReuseDependency::equals(IMessageDependency *other)
{
    MessageReuseDependency *otherMessageReuseDependency = dynamic_cast<MessageReuseDependency *>(other);
    return IMessageDependency::equals(other) && otherMessageReuseDependency &&
           causeEventNumber == otherMessageReuseDependency->causeEventNumber &&
           consequenceEventNumber == otherMessageReuseDependency->consequenceEventNumber &&
           eventLogEntryIndex == otherMessageReuseDependency->eventLogEntryIndex;
}

void MessageReuseDependency::print(FILE *file)
{
    getConsequenceEvent()->getEventEntry()->print(file);
    getBeginMessageDescriptionEntry()->print(file);
}

/**************************************************/

FilteredMessageDependency::FilteredMessageDependency(IEventLog *eventLog, Kind kind, IMessageDependency *beginMessageDependency, IMessageDependency *endMessageDependency)
    : IMessageDependency(eventLog), kind(kind), beginMessageDependency(beginMessageDependency), endMessageDependency(endMessageDependency)
{
}

FilteredMessageDependency::~FilteredMessageDependency()
{
    delete beginMessageDependency;
    delete endMessageDependency;
}

FilteredMessageDependency *FilteredMessageDependency::duplicate(IEventLog *eventLog)
{
    return new FilteredMessageDependency(eventLog, kind, beginMessageDependency->duplicate(eventLog), endMessageDependency->duplicate(eventLog));
}

IEvent *FilteredMessageDependency::getCauseEvent()
{
    eventnumber_t causeEventNumber = beginMessageDependency->getCauseEventNumber();

    if (causeEventNumber < 0)
        return nullptr;
    else
        return eventLog->getEventForEventNumber(causeEventNumber);
}

IEvent *FilteredMessageDependency::getConsequenceEvent()
{
    eventnumber_t consequenceEventNumber = endMessageDependency->getConsequenceEventNumber();

    if (consequenceEventNumber < 0)
        return nullptr;
    else
        return eventLog->getEventForEventNumber(consequenceEventNumber);
}

bool FilteredMessageDependency::equals(IMessageDependency *other)
{
    FilteredMessageDependency *otherFiltered = dynamic_cast<FilteredMessageDependency *>(other);
    return IMessageDependency::equals(other) && otherFiltered &&
           beginMessageDependency->equals(otherFiltered->beginMessageDependency) &&
           endMessageDependency->equals(otherFiltered->endMessageDependency);
}

void FilteredMessageDependency::print(FILE *file)
{
    beginMessageDependency->print(file);
    endMessageDependency->print(file);
}

}  // namespace eventlog
}  // namespace omnetpp

