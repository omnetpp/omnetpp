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

USING_NAMESPACE

Event::Event(EventLog *eventLog)
{
    this->eventLog = eventLog;
    
    clearInternalState();
}

Event::~Event()
{
    deleteAllocatedObjects();
}

void Event::deleteAllocatedObjects()
{
    for (EventLogEntryList::iterator it = eventLogEntries.begin(); it != eventLogEntries.end(); it++)
        delete *it;

    if (!cause)
        delete cause;

    if (causes)
    {
        for (IMessageDependencyList::iterator it = causes->begin(); it != causes->end(); it++)
            delete *it;
        delete causes;
    }

    if (consequences)
    {
        for (IMessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++)
            delete *it;
        delete consequences;
    }

    clearInternalState();
}

void Event::clearInternalState()
{
    beginOffset = -1;
    endOffset = -1;
    numEventLogMessages = -1;
    numBeginSendEntries = -1;

    eventEntry = NULL;
    cause = NULL;
    causes = NULL;
    consequences = NULL;

    eventLogEntries.clear();
}

void Event::synchronize()
{
    if (consequences)
    {
        for (IMessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++)
            delete *it;
        delete consequences;
        consequences = NULL;
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
    eventLog->progress();

    deleteAllocatedObjects();
    numEventLogMessages = 0;
    numBeginSendEntries = 0;

    Assert(offset >= 0);
    beginOffset = offset;
    reader->seekTo(offset);

    if (PRINT_DEBUG_MESSAGES) printf("Parsing event at offset: %lld\n", offset);

    std::deque<int> contextModuleIds;

    while (true)
    {
        char *line = reader->getNextLineBufferPointer();

        if (!line) {
            Assert(eventEntry);
            endOffset = reader->getFileSize();
            return endOffset;
        }

        EventLogEntry *eventLogEntry = EventLogEntry::parseEntry(this, line, reader->getCurrentLineLength());

        // skip empty lines
        if (!eventLogEntry)
            continue;

        // first line must be an event entry
        EventEntry *readEventEntry = dynamic_cast<EventEntry *>(eventLogEntry);
        if (!eventEntry) {
            Assert(readEventEntry);
            eventEntry = readEventEntry;
            contextModuleIds.push_back(eventEntry->moduleId);
        }
        else if (readEventEntry) {
            // stop at the start of the next event
            delete eventLogEntry;
            break;
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
        BeginSendEntry *beginSendEntry = dynamic_cast<BeginSendEntry *>(eventLogEntry);
        if (beginSendEntry) {
            eventLog->putMessageName(beginSendEntry->messageFullName);
            eventLog->putMessageClassName(beginSendEntry->messageClassName);
            numBeginSendEntries++;
        }
    }

    return endOffset = reader->getCurrentLineStartOffset();
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

bool Event::isSelfMessageProcessingEvent()
{
    BeginSendEntry *beginSendEntry = getCauseBeginSendEntry();
    return beginSendEntry && dynamic_cast<EndSendEntry *>(getCauseEvent()->getEventLogEntry(beginSendEntry->getIndex() + 1));
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

MessageDependency *Event::getCause()
{
    if (!cause)
    {
        Event *event = getCauseEvent();

        if (event)
        {
            int beginSendEntryNumber = event->findBeginSendEntryIndex(getMessageId());

            if (beginSendEntryNumber != -1)
                cause = new MessageDependency(eventLog, false, getCauseEventNumber(), beginSendEntryNumber);
        }
    }

    return cause;
}

IMessageDependencyList *Event::getCauses()
{
    if (!causes)
    {
        causes = new IMessageDependencyList();

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
                causes->push_back(new MessageDependency(eventLog, true, getEventNumber(), beginSendEntryNumber));
                break;
            }
        }
    }

    return causes;
}

IMessageDependencyList *Event::getConsequences()
{
    if (!consequences)
    {
        consequences = new IMessageDependencyList();

        for (int beginSendEntryNumber = 0; beginSendEntryNumber < (int)eventLogEntries.size(); beginSendEntryNumber++)
        {
            EventLogEntry *eventLogEntry = eventLogEntries[beginSendEntryNumber];

            if (eventLogEntry->isMessageSend())
                // using "t" from "ES" lines
                consequences->push_back(new MessageDependency(eventLog, false, getEventNumber(), beginSendEntryNumber));
        }

        int beginSendEntryNumber;
        Event *reuserEvent = getReuserEvent(beginSendEntryNumber);

        if (reuserEvent != NULL && reuserEvent != this)
            consequences->push_back(new MessageDependency(eventLog, true, reuserEvent->getEventNumber(), beginSendEntryNumber));
    }

    return consequences;
}

Event *Event::getReuserEvent(int &beginSendEntryNumber)
{
    eventLog->progress();
    Event *current = this;

    // TODO: the result of this calculation should be put into an index file lazily
    // TODO: and first we should look it up there, so that the expesive computation is not repeated
    // TODO: the limit on this loop should be configurable
    int maxLookAhead = 1000;
    while (current && maxLookAhead--) {
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
