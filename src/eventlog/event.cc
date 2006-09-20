//=========================================================================
//  EVENT.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "linetokenizer.h"
#include "filereader.h"
#include "event.h"
#include "eventlog.h"
#include "eventlogentry.h"

MessageDependency::MessageDependency(EventLog *eventLog, long causeEventNumber, long consequenceEventNumber, int messageSendEntryNumber)
{
    this->eventLog = eventLog;
    this->causeEventNumber = causeEventNumber;
    this->consequenceEventNumber = consequenceEventNumber;
    this->messageSendEntryNumber = messageSendEntryNumber;
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
        return eventLog->getEvent(causeEventNumber);
}

EventLogEntry *MessageDependency::getMessageSendEntry()
{
    Event *event = getCauseEvent();
    return event->getEventLogEntry(messageSendEntryNumber);
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
        return eventLog->getEvent(consequenceEventNumber);
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

MessageReuse::MessageReuse(EventLog *eventLog, long senderEventNumber, int messageSendEntryNumber)
    : MessageDependency(eventLog, -2, senderEventNumber, messageSendEntryNumber)
{
}

MessageSend::MessageSend(EventLog *eventLog, long senderEventNumber, int messageSendEntryNumber)
    : MessageDependency(eventLog, senderEventNumber, -2, messageSendEntryNumber)
{
}

/**************************************************/

long Event::numParsedEvent = 0;

Event::Event(EventLog *eventLog)
{
    this->eventLog = eventLog;
    beginOffset = -1;
    endOffset = -1;
    eventEntry = NULL;
    cause = NULL;
    causes = NULL;
    consequences = NULL;
}

Event::~Event()
{
    for (EventLogEntryList::iterator it = eventLogEntries.begin(); it != eventLogEntries.end(); it++)
        delete *it;
}

Event *Event::getCauseEvent()
{
    if (getCauseEventNumber() != -1)
        return eventLog->getEvent(getCauseEventNumber());
    else
        return NULL;
}

MessageSend *Event::getCause()
{
    if (cause == NULL)
    {
        Event *event = getCauseEvent();

        if (event != NULL)
        {
            for (int messageEntryNumber = 0; messageEntryNumber < event->eventLogEntries.size(); messageEntryNumber++)
            {
                EventLogEntry *eventLogEntry = event->eventLogEntries[messageEntryNumber];

                if (eventLogEntry->isMessageSend() &&
                    eventLogEntry->getMessageId() == getMessageId())
                {
                    cause = new MessageSend(eventLog, getCauseEventNumber(), messageEntryNumber);
                    break;
                }
            }
        }
    }

    return cause;
}

Event::MessageSendList *Event::getCauses()
{
    if (causes == NULL)
    {
        causes = new MessageSendList();

        if (getCause() != NULL)
            causes->push_back(getCause());

        for (int messageEntryNumber = 0; messageEntryNumber < eventLogEntries.size(); messageEntryNumber++)
        {
            EventLogEntry *eventLogEntry = eventLogEntries[messageEntryNumber];

            if (eventLogEntry->isMessageSend() && eventLogEntry->getPreviousEventNumber() != getEventNumber())
            {
                causes->push_back(new MessageReuse(eventLog, getEventNumber(), messageEntryNumber));
                break;
            }
        }
    }

    return causes;
}

Event::MessageSendList *Event::getConsequences()
{
    if (consequences == NULL)
    {
        consequences = new MessageSendList();

        for (int messageEntryNumber = 0; messageEntryNumber < eventLogEntries.size(); messageEntryNumber++)
        {
            EventLogEntry *eventLogEntry = eventLogEntries[messageEntryNumber];

            if (eventLogEntry->isMessageSend())
                consequences->push_back(new MessageSend(eventLog, getEventNumber(), messageEntryNumber));
        }
    }

    return consequences;
}

long Event::parse(FileReader *reader, long offset)
{
    if (eventEntry != NULL)
        throw new Exception("Reusing event objects are not supported");

    beginOffset = offset;
    numParsedEvent++;
    reader->seekTo(offset);

    while (true)
    {
        char *line = reader->readLine();

        if (!line)
            return reader->fileSize();

        EventLogEntry *eventLogEntry = EventLogEntry::parseEntry(line);
        EventEntry *eventEntry = dynamic_cast<EventEntry *>(eventLogEntry);

        if (eventEntry)
        {
            if (this->eventEntry)
                break; // stop at the start of next event
            else
                this->eventEntry = eventEntry;
        }

        if (eventLogEntry)
            eventLogEntries.push_back(eventLogEntry);
    }

    printf("*** Parsed event: %ld\n", getEventNumber());

    return endOffset = reader->lineStartOffset();
}

void Event::print(FILE *file)
{
    for (EventLogEntryList::iterator it = eventLogEntries.begin(); it != eventLogEntries.end(); it++)
    {
        EventLogEntry *eventLogEntry = *it;
        eventLogEntry->print(file);
    }
}
