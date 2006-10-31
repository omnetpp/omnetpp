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

#include "filereader.h"
#include "event.h"
#include "eventlog.h"
#include "eventlogentry.h"

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

IEventLog *Event::getEventLog()
{
    return eventLog;
}

long Event::parse(FileReader *reader, long offset)
{
    EASSERT(offset >= 0);
    EASSERT(!eventEntry);

    beginOffset = offset;
    reader->seekTo(offset);

    if (PRINT_DEBUG_MESSAGES) printf("Parsing event at offset: %ld\n", offset);

    while (true)
    {
        char *line = reader->readNextLine();

        if (!line) {
            EASSERT(eventEntry);
            endOffset = reader->getFileSize();
            return endOffset;
        }

        EventLogEntry *eventLogEntry = EventLogEntry::parseEntry(this, line, reader->getLastLineLength());
        EventEntry *readEventEntry = dynamic_cast<EventEntry *>(eventLogEntry);

        // first line must be an event entry
        if (!eventEntry) {
            EASSERT(readEventEntry);
            eventEntry = readEventEntry;
        }
        else if (readEventEntry)
            break; // stop at the start of the next event

        EASSERT(eventEntry);

        if (eventLogEntry)
            eventLogEntries.push_back(eventLogEntry);

        EventLogMessage *eventLogMessage = dynamic_cast<EventLogMessage *>(eventLogEntry);
        if (eventLogMessage)
            eventLogMessages.push_back(eventLogMessage);
    }

    return endOffset = reader->getLastLineStartOffset();
}

void Event::print(FILE *file)
{
    for (EventLogEntryList::iterator it = eventLogEntries.begin(); it != eventLogEntries.end(); it++)
    {
        EventLogEntry *eventLogEntry = *it;
        eventLogEntry->print(file);
    }
}

Event *Event::getPreviousEvent()
{
    if (!previousEvent && eventLog->getFirstEvent() != this)
    {
        previousEvent = eventLog->getEventForEndOffset(beginOffset);

        if (previousEvent)
            IEvent::linkEvents(previousEvent, this);
    }

    return (Event *)previousEvent;
}

Event *Event::getNextEvent()
{
    if (!nextEvent && eventLog->getLastEvent() != this)
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

            if (beginSendEntry && 
                beginSendEntry->previousEventNumber != -1 &&
                beginSendEntry->previousEventNumber != getEventNumber())
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
