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

long Event::numParsedEvent = 0;

Event::Event(EventLog *eventLog)
{
    this->eventLog = eventLog;

    beginOffset = -1;
    endOffset = -1;
    numEventLogMessages = 0;
    eventEntry = NULL;
    cause = NULL;
    causes = NULL;
    consequences = NULL;
}

Event::~Event()
{
    for (EventLogEntryList::iterator it = eventLogEntries.begin(); it != eventLogEntries.end(); it++)
        delete *it;

    if (!causes)
        delete cause;

    if (causes)
    {
        for (MessageDependencyList::iterator it = causes->begin(); it != causes->end(); it++)
            delete *it;
        delete causes;
    }

    if (consequences)
    {
        for (MessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++)
            delete *it;
        delete consequences;
    }
}

long Event::parse(FileReader *reader, long offset)
{
    if (eventEntry)
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
        EventEntry *readEventEntry = dynamic_cast<EventEntry *>(eventLogEntry);

        // first line is an event entry
        if (!eventEntry) {
            EASSERT(readEventEntry);
            eventEntry = readEventEntry;
        }
        else if (readEventEntry)
            break; // stop at the start of next event

        if (eventLogEntry)
            eventLogEntries.push_back(eventLogEntry);

        if (dynamic_cast<EventLogMessage *>(eventLogEntry))
            numEventLogMessages++;
    }

    //printf("*** Parsed event: %ld\n", getEventNumber());

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

EventLogMessage *Event::getEventLogMessage(int index)
{
    int i = 0;

    for (EventLogEntryList::iterator it = eventLogEntries.begin(); it != eventLogEntries.end(); it++)
    {
        EventLogMessage *eventLogMessage = dynamic_cast<EventLogMessage *>(*it);

        if (eventLogMessage)
        {
            if (i == index)
                return eventLogMessage;

            i++;
        }
    }
}

Event *Event::getPreviousEvent()
{
    if (!previousEvent)
    {
        previousEvent = eventLog->getEventForEndOffset(beginOffset);

        if (previousEvent)
            IEvent::linkEvents(previousEvent, this);
    }

    return (Event *)previousEvent;
}

Event *Event::getNextEvent()
{
    if (!nextEvent)
    {
        nextEvent = eventLog->getEventForBeginOffset(endOffset);

        if (nextEvent)
            Event::linkEvents(this, nextEvent);
    }

    return (Event *)nextEvent;
}

Event *Event::getCauseEvent()
{
    if (getCauseEventNumber() != -1)
        return eventLog->getEventForEventNumber(getCauseEventNumber());
    else
        return NULL;
}

MessageSend *Event::getCause()
{
    if (!cause)
    {
        Event *event = getCauseEvent();

        if (event)
        {
            // find the "BS" or "SA" line in the cause event
            for (int messageEntryNumber = 0; messageEntryNumber < event->eventLogEntries.size(); messageEntryNumber++)
            {
                BeginSendEntry *beginSendEntry = dynamic_cast<BeginSendEntry *>(event->eventLogEntries[messageEntryNumber]);

                if (beginSendEntry && beginSendEntry->messageId == getMessageId())
                {
                    cause = new MessageSend(eventLog, getCauseEventNumber(), messageEntryNumber);
                    break;
                }
            }
        }
    }

    return cause;
}

MessageDependencyList *Event::getCauses()
{
    if (!causes)
    {
        causes = new MessageDependencyList();

        if (getCause())
            causes->push_back(getCause());

        // add message reuses
        for (int messageEntryNumber = 0; messageEntryNumber < eventLogEntries.size(); messageEntryNumber++)
        {
            BeginSendEntry *beginSendEntry = dynamic_cast<BeginSendEntry *>(eventLogEntries[messageEntryNumber]);

            if (beginSendEntry && beginSendEntry->previousEventNumber != getEventNumber())
            {
                // store "pe" key from "BS" or "SA" lines
                causes->push_back(new MessageReuse(eventLog, getEventNumber(), messageEntryNumber));
                break;
            }
        }
    }

    return causes;
}

MessageDependencyList *Event::getConsequences()
{
    if (!consequences)
    {
        consequences = new MessageDependencyList();

        for (int messageEntryNumber = 0; messageEntryNumber < eventLogEntries.size(); messageEntryNumber++)
        {
            EventLogEntry *eventLogEntry = eventLogEntries[messageEntryNumber];

            if (eventLogEntry->isMessageSend())
                consequences->push_back(new MessageSend(eventLog, getEventNumber(), messageEntryNumber));
        }
    }

    return consequences;
}
