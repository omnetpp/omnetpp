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

#include "deque"
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
    numEventLogMessages = -1;
    numBeginSendEntries = -1;
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

ModuleCreatedEntry *Event::getModuleCreatedEntry()
{
    return eventLog->getModuleCreatedEntry(getModuleId());
}

file_offset_t Event::parse(FileReader *reader, file_offset_t offset)
{
    Assert(offset >= 0);
    Assert(!eventEntry);

    beginOffset = offset;
    reader->seekTo(offset);

    numEventLogMessages = 0;
    numBeginSendEntries = 0;

    if (PRINT_DEBUG_MESSAGES) printf("Parsing event at offset: %lld\n", offset);

    std::deque<int> contextModuleIds;
    int emptyLines = 0;

    while (true)
    {
        char *line = reader->getNextLineBufferPointer();

        if (!line) {
            Assert(eventEntry);
            endOffset = reader->getFileSize();
            return endOffset;
        }

        EventLogEntry *eventLogEntry = EventLogEntry::parseEntry(this, line, reader->getLastLineLength());
        if (!eventLogEntry)
            emptyLines++;

        // first line must be an event entry
        EventEntry *readEventEntry = dynamic_cast<EventEntry *>(eventLogEntry);
        if (!eventEntry) {
            Assert(readEventEntry);
            eventEntry = readEventEntry;
            contextModuleIds.push_back(eventEntry->moduleId);
        }
        else if (readEventEntry) {
            if (emptyLines == 1)
                break; // stop at the start of the next event
            else
                throw opp_runtime_error("Too many empty lines for a single event after offset: %lld", offset);
        }
        Assert(eventEntry);

        // handle module method end
        ModuleMethodEndEntry *moduleMethodEndEntry = dynamic_cast<ModuleMethodEndEntry *>(eventLogEntry);
        if (moduleMethodEndEntry)
            contextModuleIds.pop_front();

        // store log entry
        if (eventLogEntry) {
            eventLogEntry->level = contextModuleIds.size() - 1;
            eventLogEntry->contextModuleId = contextModuleIds.front();
            eventLogEntries.push_back(eventLogEntry);
        }

        // handle module method begin
        ModuleMethodBeginEntry *moduleMethodBeginEntry = dynamic_cast<ModuleMethodBeginEntry *>(eventLogEntry);
        if (moduleMethodBeginEntry)
            contextModuleIds.push_front(moduleMethodBeginEntry->toModuleId);

        // count message entry
        if (dynamic_cast<EventLogMessageEntry *>(eventLogEntry))
            numEventLogMessages++;

        // count begin send entry
        if (dynamic_cast<BeginSendEntry *>(eventLogEntry))
            numBeginSendEntries++;
    }

    return endOffset = reader->getLastLineStartOffset();
}

void Event::print(FILE *file, bool outputEventLogMessages)
{
    for (EventLogEntryList::iterator it = eventLogEntries.begin(); it != eventLogEntries.end(); it++)
    {
        EventLogEntry *eventLogEntry = *it;

        if (outputEventLogMessages || !dynamic_cast<EventLogMessageEntry *>(eventLogEntry))
            eventLogEntry->print(file);
    }
}

EventLogMessageEntry *Event::getEventLogMessage(int index)
{
    Assert(index >= 0);

    for (EventLogEntryList::iterator it = eventLogEntries.begin(); it != eventLogEntries.end(); it++)
    {
        EventLogMessageEntry *eventLogMessage = dynamic_cast<EventLogMessageEntry *>(*it);

        if (eventLogMessage) {
            if (index == 0)
                return eventLogMessage;
            else
                index--;
        }
    }

    throw opp_runtime_error("index out of range");
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

BeginSendEntry *Event::getCauseBeginSendEntry()
{
    if (getCause())
        return getCause()->getCauseBeginSendEntry();
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
            for (int beginSendEntryNumber = 0; beginSendEntryNumber < (int)event->eventLogEntries.size(); beginSendEntryNumber++)
            {
                BeginSendEntry *beginSendEntry = dynamic_cast<BeginSendEntry *>(event->eventLogEntries[beginSendEntryNumber]);

                if (beginSendEntry && beginSendEntry->messageId == getMessageId())
                {
                    cause = new MessageSend(eventLog, getCauseEventNumber(), beginSendEntryNumber);
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
            // using "ce" from "E" line
            causes->push_back(getCause());

        // add message reuses
        for (int beginSendEntryNumber = 0; beginSendEntryNumber < (int)eventLogEntries.size(); beginSendEntryNumber++)
        {
            BeginSendEntry *beginSendEntry = dynamic_cast<BeginSendEntry *>(eventLogEntries[beginSendEntryNumber]);

            if (beginSendEntry &&
                beginSendEntry->previousEventNumber != -1 &&
                beginSendEntry->previousEventNumber != getEventNumber())
            {
                // store "pe" key from "BS" or "SA" lines
                causes->push_back(new MessageReuse(eventLog, getEventNumber(), beginSendEntryNumber));
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

        for (int beginSendEntryNumber = 0; beginSendEntryNumber < (int)eventLogEntries.size(); beginSendEntryNumber++)
        {
            EventLogEntry *eventLogEntry = eventLogEntries[beginSendEntryNumber];

            if (eventLogEntry->isMessageSend())
                // using "t" from "ES" lines
                consequences->push_back(new MessageSend(eventLog, getEventNumber(), beginSendEntryNumber));
        }

        int beginSendEntryNumber;
        Event *reuserEvent = getReuserEvent(beginSendEntryNumber);

        if (reuserEvent != NULL && reuserEvent != this)
            consequences->push_back(new MessageReuse(eventLog, reuserEvent->getEventNumber(), beginSendEntryNumber));
    }

    return consequences;
}

Event *Event::getReuserEvent(int &beginSendEntryNumber)
{
    Event *current = this;

    // TODO: the result of this calculation should be put into an index file lazily
    // TODO: and first we should look it up there, so that the expesive computation is not repeated
    // TODO: there should be some kind of limit on this loop not the end of the file
    while (current) {
        for (beginSendEntryNumber = 0; beginSendEntryNumber < (int)current->eventLogEntries.size(); beginSendEntryNumber++)
        {
            EventLogEntry *eventLogEntry = current->eventLogEntries[beginSendEntryNumber];
            BeginSendEntry *beginSendEntry = dynamic_cast<BeginSendEntry *>(eventLogEntry);

            if (beginSendEntry &&
                beginSendEntry->messageId == getMessageId())
            {
                if (beginSendEntry->previousEventNumber == getEventNumber())
                    return current;
                else
                    // events were filtered in between and this is not the first reuse
                    return NULL;
            }

            DeleteMessageEntry *deleteMessageEntry = dynamic_cast<DeleteMessageEntry *>(eventLogEntry);

            if (deleteMessageEntry && deleteMessageEntry->messageId == getMessageId()) {
                beginSendEntryNumber = -1;
                return NULL;
            }
        }

        current = current->getNextEvent();
    }

    beginSendEntryNumber = -1;
    return NULL;
}
