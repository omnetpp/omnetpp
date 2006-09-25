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

MessageDependency::MessageDependency(EventLog *eventLog,
    long causeEventNumber, int causeMessageSendEntryNumber,
    long consequenceEventNumber, int consequenceMessageSendEntryNumber)
{
    this->eventLog = eventLog;
    this->causeEventNumber = causeEventNumber;
    this->consequenceEventNumber = consequenceEventNumber;
    this->causeMessageSendEntryNumber = causeMessageSendEntryNumber;
    this->consequenceMessageSendEntryNumber = consequenceMessageSendEntryNumber;

    EASSERT(causeEventNumber >= 0 || consequenceEventNumber >= 0);
}

EventLogEntry *MessageDependency::getMessageSendEntry()
{
    if (causeMessageSendEntryNumber != -1 && consequenceMessageSendEntryNumber != -1)
        return NULL;
    else if (causeMessageSendEntryNumber != -1)
        return getCauseMessageSendEntry();
    else
        return getConsequenceMessageSendEntry();
}

EventLogEntry *MessageDependency::getCauseMessageSendEntry()
{
    return getCauseEvent()->getEventLogEntry(causeMessageSendEntryNumber);
}

long MessageDependency::getCauseEventNumber()
{
    if (causeEventNumber == -2)
    {
        // only consequence is present, calculate cause from it
        Event *consequenceEvent = getConsequenceEvent();
        EASSERT(consequenceEvent);
        causeEventNumber = consequenceEvent->getEventLogEntry(consequenceMessageSendEntryNumber)->getPreviousEventNumber();
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
    return getCauseEvent()->getSimulationTime();
}

EventLogEntry *MessageDependency::getConsequenceMessageSendEntry()
{
    return getConsequenceEvent()->getEventLogEntry(consequenceMessageSendEntryNumber);
}

long MessageDependency::getConsequenceEventNumber()
{
    if (consequenceEventNumber == -2)
    {
        // only cause is present, calculate consequence from it.
        //
        // when a message is scheduled/sent, we don't know the arrival event number
        // yet, only the simulation time is recorded in the event log file.
        // So here we have to look through all events at the arrival time,
        // and find the one "caused by" our message.
        simtime_t consequenceTime = getConsequenceTime();
        long offset = eventLog->getOffsetForSimulationTime(consequenceTime, EventLogIndex::MatchKind::FIRST);

        while (offset != -1)
        {
            Event *event = eventLog->getEventForBeginOffset(offset);

            if (event == NULL)
                // end of file
                return -1;

            if (event->getCauseEventNumber() == getCauseEventNumber())
            {
                consequenceEventNumber = event->getEventNumber();
                break;
            }

            if (event->getSimulationTime() > consequenceTime)
            {
                // no more event at that simulation time, and consequence event
                // still not found. It must have been cancelled (self message),
                // or it is not in the file (filtered out by the user, etc).
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
    if (consequenceEventNumber >= 0)
        return getConsequenceEvent()->getSimulationTime();
    else
    {
        // find the arrival time of the message
        Event *event = getCauseEvent();
        EventLogEntry *eventLogEntry = event->getEventLogEntry(causeMessageSendEntryNumber);

        // 1. BeginSendEntry
        BeginSendEntry *beginSendEntry = dynamic_cast<BeginSendEntry *>(eventLogEntry);

        if (beginSendEntry != NULL)
        {
            // arrival time is in the EndSendEntry ("ES" line), find it
            int i = causeMessageSendEntryNumber + 1;

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
}

void MessageDependency::printCause(FILE *file)
{
    if (getCauseEventNumber() != -1)
        getCauseEvent()->getEventEntry()->print(file);

    if (getCauseMessageSendEntryNumber() != -1)
        getCauseMessageSendEntry()->print(file);
}

void MessageDependency::printConsequence(FILE *file)
{
    if (getConsequenceEventNumber() != -1)
       getConsequenceEvent()->getEventEntry()->print(file);

    if (getConsequenceMessageSendEntryNumber() != -1)
       getConsequenceMessageSendEntry()->print(file);
}

/**************************************************/

MessageReuse::MessageReuse(EventLog *eventLog, long senderEventNumber, int messageSendEntryNumber)
    : MessageDependency(eventLog, -2, -1, senderEventNumber, messageSendEntryNumber)
{
}

/**************************************************/

MessageSend::MessageSend(EventLog *eventLog, long senderEventNumber, int messageSendEntryNumber)
    : MessageDependency(eventLog, senderEventNumber, messageSendEntryNumber, -2, -1)
{
}

/**************************************************/
FilteredMessageDependency::FilteredMessageDependency(EventLog *eventLog,
    long causeEventNumber, int causeMessageSendEntryNumber,
    long middleEventNumber, int middleMessageSendEntryNumber,
    long consequenceEventNumber, int consequenceMessageSendEntryNumber)
    : MessageDependency(eventLog, causeEventNumber, causeMessageSendEntryNumber, consequenceEventNumber, consequenceMessageSendEntryNumber)
{
    this->middleEventNumber = middleEventNumber;
    this->middleMessageSendEntryNumber = middleMessageSendEntryNumber;
}

Event *FilteredMessageDependency::getMiddleEvent()
{
    return eventLog->getEventForEventNumber(middleEventNumber);
}

simtime_t FilteredMessageDependency::getMiddleTime()
{
    return getMiddleEvent()->getSimulationTime();
}

EventLogEntry *FilteredMessageDependency::getMiddleMessageSendEntry()
{
    return getMiddleEvent()->getEventLogEntry(middleMessageSendEntryNumber);
}

void FilteredMessageDependency::printMiddle(FILE *file)
{
    if (getMiddleEventNumber() != -1)
       getMiddleEvent()->getEventEntry()->print(file);

    if (getMiddleMessageSendEntryNumber() != -1)
       getMiddleMessageSendEntry()->print(file);
}

void FilteredMessageDependency::print(FILE *file)
{
    printCause(file);
    printMiddle(file);
    printConsequence(file);
}