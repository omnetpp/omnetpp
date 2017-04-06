//=========================================================================
//  EVENT.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include "common/filereader.h"
#include "deque"
#include "event.h"
#include "eventlog.h"
#include "eventlogentry.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace eventlog {

Event::Event(EventLog *eventLog)
{
    this->eventLog = eventLog;
    clearInternalState();
}

Event::~Event()
{
    deleteAllocatedObjects();
}

void Event::clearInternalState()
{
    beginOffset = -1;
    endOffset = -1;
    numEventLogMessages = -1;
    numBeginSendEntries = -1;
    eventEntry = nullptr;
    moduleCreatedEntry = nullptr;
    cause = nullptr;
    causes = nullptr;
    consequences = nullptr;
    eventLogEntries.clear();
}

void Event::deleteConsequences()
{
    if (consequences) {
        for (auto & consequence : *consequences)
            delete consequence;
        delete consequences;
        consequences = nullptr;
    }
}

void Event::deleteAllocatedObjects()
{
    if (cause && !causes) {
        delete cause;
        cause = nullptr;
    }
    if (causes) {
        for (auto & cause : *causes)
            delete cause;
        delete causes;
        causes = nullptr;
    }
    deleteConsequences();
    for (auto & eventLogEntry : eventLogEntries)
        delete eventLogEntry;
    eventLogEntries.clear();
}

void Event::synchronize(FileReader::FileChangedState change)
{
    if (change != FileReader::UNCHANGED) {
        IEvent::synchronize(change);
        switch (change) {
            case FileReader::OVERWRITTEN:
                deleteAllocatedObjects();
                clearInternalState();
                break;
            case FileReader::APPENDED:
                deleteConsequences();
                break;
            case FileReader::UNCHANGED:   // just to avoid unused enumeration value warnings
                break;
        }
    }
}

IEventLog *Event::getEventLog()
{
    return eventLog;
}

ModuleCreatedEntry *Event::getModuleCreatedEntry()
{
    if (!moduleCreatedEntry)
        moduleCreatedEntry = eventLog->getModuleCreatedEntry(getModuleId());

    return moduleCreatedEntry;
}

file_offset_t Event::parse(FileReader *reader, file_offset_t offset)
{
    eventLog->progress();

    deleteAllocatedObjects();
    clearInternalState();
    numEventLogMessages = 0;
    numBeginSendEntries = 0;

    Assert(offset >= 0);
    beginOffset = offset;
    reader->seekTo(offset);

    if (PRINT_DEBUG_MESSAGES)
        printf("Parsing event at offset: %" PRId64 "\n", offset);

    int index = 0;
    std::deque<int> contextModuleIds;

    while (true) {
        char *line = reader->getNextLineBufferPointer();

        if (!line) {
            Assert(eventEntry);
            endOffset = reader->getFileSize();
            return endOffset;
        }

        EventLogEntry *eventLogEntry = EventLogEntry::parseEntry(eventLog, this, index, reader->getCurrentLineStartOffset(), line, reader->getCurrentLineLength());

        // skip empty lines
        if (!eventLogEntry)
            continue;
        else
            index++;

        // first line must be an event entry
        EventEntry *readEventEntry = dynamic_cast<EventEntry *>(eventLogEntry);
        if (!eventEntry) {
            Assert(readEventEntry);
            eventEntry = readEventEntry;
            contextModuleIds.push_front(eventEntry->moduleId);
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
        eventLogEntry->level = contextModuleIds.size() - 1;
        eventLogEntry->contextModuleId = contextModuleIds.front();
        eventLogEntries.push_back(eventLogEntry);

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

    Assert(eventEntry);
    return endOffset = reader->getCurrentLineStartOffset();
}

void Event::print(FILE *file, bool outputEventLogMessages)
{
    for (auto eventLogEntry : eventLogEntries) {
        if (outputEventLogMessages || !dynamic_cast<EventLogMessageEntry *>(eventLogEntry))
            eventLogEntry->print(file);
    }
}

EventLogMessageEntry *Event::getEventLogMessage(int index)
{
    Assert(index >= 0);

    for (auto & eventLogEntry : eventLogEntries) {
        EventLogMessageEntry *eventLogMessage = dynamic_cast<EventLogMessageEntry *>(eventLogEntry);

        if (eventLogMessage) {
            if (index == 0)
                return eventLogMessage;
            else
                index--;
        }
    }

    throw opp_runtime_error("Index out of range");
}

bool Event::isSelfMessage(BeginSendEntry *beginSendEntry)
{
    Assert(beginSendEntry && this == beginSendEntry->getEvent());
    int index = beginSendEntry->getEntryIndex();

    if (index + 1 < (int)eventLogEntries.size())
        return dynamic_cast<EndSendEntry *>(eventLogEntries[index + 1]);
    else
        return false;
}

bool Event::isSelfMessageProcessingEvent()
{
    BeginSendEntry *beginSendEntry = getCauseBeginSendEntry();
    return beginSendEntry && beginSendEntry->getEvent()->isSelfMessage(beginSendEntry);
}

EndSendEntry *Event::getEndSendEntry(BeginSendEntry *beginSendEntry)
{
    Assert(beginSendEntry && this == beginSendEntry->getEvent());
    for (int i = beginSendEntry->getEntryIndex(); i < (int)eventLogEntries.size(); i++) {
        EventLogEntry *eventLogEntry = eventLogEntries[i];

        EndSendEntry *endSendEntry = dynamic_cast<EndSendEntry *>(eventLogEntry);
        if (endSendEntry)
            return endSendEntry;

        // message deleted on the channel (skip all deletes until the matching one is found)
        DeleteMessageEntry *deleteMessageEntry = dynamic_cast<DeleteMessageEntry *>(eventLogEntry);
        if (deleteMessageEntry && deleteMessageEntry->messageId == beginSendEntry->messageId)
            return nullptr;
    }

    throw opp_runtime_error("Neither EndSendEntry nor DeleteMessageEntry found");
}

simtime_t Event::getTransmissionDelay(BeginSendEntry *beginSendEntry)
{
    Assert(beginSendEntry && this == beginSendEntry->getEvent());
    EndSendEntry *endSendEntry = getEndSendEntry(beginSendEntry);
    int index = beginSendEntry->getEntryIndex() + 1;
    simtime_t transmissionDelay = BigDecimal::Zero;

    // there is at most one entry which specifies a transmission delay
    while (index < (int)eventLogEntries.size()) {
        EventLogEntry *eventLogEntry = eventLogEntries[index];
        if (SendDirectEntry *sendDirectEntry = dynamic_cast<SendDirectEntry *>(eventLogEntry))
            transmissionDelay = sendDirectEntry->transmissionDelay;
        else if (SendHopEntry *sendHopEntry = dynamic_cast<SendHopEntry *>(eventLogEntry))
            transmissionDelay = sendHopEntry->transmissionDelay;
        else if (eventLogEntry == endSendEntry || (!dynamic_cast<EventLogMessageEntry *>(eventLogEntry) && !dynamic_cast<MessageEntry *>(eventLogEntry)))
            break;
        if (transmissionDelay != 0)
            break;
        index++;
    }

    return transmissionDelay;
}

Event *Event::getPreviousEvent()
{
    if (!previousEvent && eventLog->getFirstEvent() != this) {
        previousEvent = eventLog->getEventForEndOffset(beginOffset);

        if (previousEvent)
            IEvent::linkEvents(previousEvent, this);
    }

    return (Event *)previousEvent;
}

Event *Event::getNextEvent()
{
    if (!nextEvent && eventLog->getLastEvent() != this) {
        nextEvent = eventLog->getEventForBeginOffset(endOffset);

        if (nextEvent)
            Event::linkEvents(this, nextEvent);
    }

    return (Event *)nextEvent;
}

Event *Event::getCauseEvent()
{
    int causeEventNumber = getCauseEventNumber();
    if (causeEventNumber != -1)
        return eventLog->getEventForEventNumber(causeEventNumber);
    else
        return nullptr;
}

BeginSendEntry *Event::getCauseBeginSendEntry()
{
    MessageSendDependency *cause = getCause();
    if (cause)
        return (BeginSendEntry *)cause->getMessageEntry();
    else
        return nullptr;
}

MessageSendDependency *Event::getCause()
{
    if (!cause) {
        Event *event = getCauseEvent();
        if (event) {
            int beginSendEntryNumber = event->findBeginSendEntryIndex(getMessageId());
            if (beginSendEntryNumber != -1)
                cause = new MessageSendDependency(eventLog, getCauseEventNumber(), beginSendEntryNumber);
        }
    }
    return cause;
}

IMessageDependencyList *Event::getCauses()
{
    if (!causes) {
        causes = new IMessageDependencyList();

        if (getCause())
            // using "ce" from "E" line
            causes->push_back(getCause());

        // add message reuses
        eventnumber_t eventNumber = getEventNumber();
        for (int i = 0; i < (int)eventLogEntries.size(); i++) {
            EventLogEntry *eventLogEntry = eventLogEntries[i];
            MessageEntry *messageEntry = dynamic_cast<MessageEntry *>(eventLogEntry);
            if (messageEntry && messageEntry->previousEventNumber != -1 && messageEntry->previousEventNumber != eventNumber)
                causes->push_back(new MessageReuseDependency(eventLog, eventNumber, i));
        }
    }

    return causes;
}

IMessageDependencyList *Event::getConsequences()
{
    if (!consequences) {
        consequences = new IMessageDependencyList();
        for (int beginSendEntryNumber = 0; beginSendEntryNumber < (int)eventLogEntries.size(); beginSendEntryNumber++) {
            EventLogEntry *eventLogEntry = eventLogEntries[beginSendEntryNumber];
            if (dynamic_cast<BeginSendEntry *>(eventLogEntry))
                // using "t" from "ES" lines
                consequences->push_back(new MessageSendDependency(eventLog, getEventNumber(), beginSendEntryNumber));
        }
        std::vector<MessageEntry *> messageEntries = eventLog->getMessageEntriesWithPreviousEventNumber(getEventNumber());
        for (auto messageEntry : messageEntries) {
            Assert(messageEntry->previousEventNumber == getEventNumber());
            consequences->push_back(new MessageReuseDependency(eventLog, messageEntry->getEvent()->getEventNumber(), messageEntry->getEntryIndex()));
        }
    }
    return consequences;
}

} // namespace eventlog
}  // namespace omnetpp

