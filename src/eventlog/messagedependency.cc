//=========================================================================
//  MESSAGEDEPENDENCY.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "event.h"
#include "eventlog.h"
#include "eventlogentry.h"
#include "messagedependency.h"

MessageDependency::MessageDependency(EventLog *eventLog, long causeEventNumber, long consequenceEventNumber, int messageSendEntryNumber)
{
    this->eventLog = eventLog;
    this->causeEventNumber = causeEventNumber;
    this->consequenceEventNumber = consequenceEventNumber;
    this->messageSendEntryNumber = messageSendEntryNumber;
}

EventLogEntry *MessageDependency::getMessageSendEntry()
{
    Event *event = getCauseEvent();
    return event->getEventLogEntry(messageSendEntryNumber);
}

long MessageDependency::getCauseEventNumber()
{
    if (causeEventNumber == -2)
    {
        Event *consequenceEvent = getConsequenceEvent();
        causeEventNumber = consequenceEvent->getEventLogEntry(messageSendEntryNumber)->getPreviousEventNumber();
    }

    return causeEventNumber;
}

Event *MessageDependency::getCauseEvent()
{
    long causeEventNumber = getCauseEventNumber();

    if (causeEventNumber == -1)
        return NULL;
    else
        return eventLog->getEventForEventNumber(causeEventNumber);
}

simtime_t MessageDependency::getCauseTime()
{
    throw new Exception("Not yet implemented");
}

long MessageDependency::getConsequenceEventNumber() 
{
    if (consequenceEventNumber == -2)
    {
        simtime_t consequenceTime = getConsequenceTime();
        long offset = eventLog->getOffsetForSimulationTime(consequenceTime, EventLogIndex::MatchKind::FIRST);

        while (offset != -1)
        {
            Event *event = eventLog->getEventForOffset(offset);

            if (event == NULL)
                // end of file
                return -1;

            if (event->getMessageId() == getMessageId())
            {
                consequenceEventNumber = event->getEventNumber();
                break;
            }

            if (event->getSimulationTime() > consequenceTime)
            {
                // self message has been cancelled
                consequenceEventNumber = -1;
                break;
            }

            offset = event->getEndOffset();
        }

        if (offset == -1)
            // end of file
            return -1;
    }

    return consequenceEventNumber;
}

Event *MessageDependency::getConsequenceEvent() 
{
    long consequenceEventNumber = getConsequenceEventNumber();

    if (consequenceEventNumber == -1)
        return NULL;
    else
        return eventLog->getEventForEventNumber(consequenceEventNumber);
}

simtime_t MessageDependency::getConsequenceTime()
{
    Event *event = getCauseEvent();
    EventLogEntry *eventLogEntry = event->getEventLogEntry(messageSendEntryNumber);

    // 1. BeginSendEntry
    BeginSendEntry *beginSendEntry = dynamic_cast<BeginSendEntry *>(eventLogEntry);

    if (beginSendEntry != NULL)
    {
        int i = messageSendEntryNumber + 1;

        while (i < event->getNumEventLogEntries())
        {
            eventLogEntry = event->getEventLogEntry(i++);
            EndSendEntry *endSendEntry = dynamic_cast<EndSendEntry *>(eventLogEntry);

            if (endSendEntry != NULL)
                return endSendEntry->arrivalTime;
        }

        throw new Exception("Missing end message send entry");
    }

    // 2 . MessageScheduledEntry
    MessageScheduledEntry *messageScheduledEntry = dynamic_cast<MessageScheduledEntry *>(eventLogEntry);

    if (messageScheduledEntry != NULL)
        return messageScheduledEntry->arrivalTime;

    throw new Exception("Unknown message entry");
}

/**************************************************/

MessageReuse::MessageReuse(EventLog *eventLog, long senderEventNumber, int messageSendEntryNumber)
    : MessageDependency(eventLog, -2, senderEventNumber, messageSendEntryNumber)
{
}

MessageSend::MessageSend(EventLog *eventLog, long senderEventNumber, int messageSendEntryNumber)
    : MessageDependency(eventLog, senderEventNumber, -2, messageSendEntryNumber)
{
}

