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
        return eventLog->getEventForEventNumber(getCauseEventNumber());
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
