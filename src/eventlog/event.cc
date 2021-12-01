//=========================================================================
//  EVENT.CC - part of
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
#include "common/filereader.h"
#include "deque"
#include "event.h"
#include "eventlog.h"
#include "eventlogentry.h"
#include "index.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace eventlog {

Event::Event(EventLog *eventLog): eventLog(eventLog)
{
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
    numCustomEntries = -1;
    eventEntry = nullptr;
    moduleDescriptionEntry = nullptr;
    cause = nullptr;
    causes = nullptr;
    consequences = nullptr;
    eventLogEntries.clear();
}

void Event::deleteCauses()
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
        cause = nullptr;
    }
}

void Event::deleteConsequences()
{
    if (consequences) {
        for (IMessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++)
            delete *it;
        delete consequences;
        consequences = nullptr;
    }
}

void Event::deleteEventLogEntries()
{
    for (EventLogEntryList::iterator it = eventLogEntries.begin(); it != eventLogEntries.end(); it++)
        delete *it;
    eventLogEntries.clear();
}

void Event::deleteAllocatedObjects()
{
    deleteCauses();
    deleteConsequences();
    deleteEventLogEntries();
}

void Event::synchronize(FileReader::FileChange change)
{
    if (change != FileReader::UNCHANGED) {
        IEvent::synchronize(change);
        switch (change) {
            case FileReader::OVERWRITTEN:
                deleteAllocatedObjects();
                clearInternalState();
                break;
            case FileReader::APPENDED:
                deleteCauses();
                deleteConsequences();
                break;
            default:
                throw opp_runtime_error("Unknown file change");
        }
    }
}

IEventLog *Event::getEventLog()
{
    return eventLog;
}

ModuleDescriptionEntry *Event::getModuleDescriptionEntry()
{
    if (!moduleDescriptionEntry)
        moduleDescriptionEntry = eventLog->getEventLogEntryCache()->getModuleDescriptionEntry(getModuleId());
    return moduleDescriptionEntry;
}

file_offset_t Event::parse(FileReader *reader, file_offset_t offset)
{
    eventLog->progress();

    deleteAllocatedObjects();
    clearInternalState();
    numEventLogMessages = 0;
    numBeginSendEntries = 0;
    numCustomEntries = 0;

    Assert(offset >= 0);
    beginOffset = offset;
    if (PRINT_DEBUG_MESSAGES) printf("Parsing event at offset: %" PRId64 "\n", offset);
    return parseLines(reader, offset);
}

file_offset_t Event::parseLines(FileReader *reader, file_offset_t offset)
{
    if (PRINT_DEBUG_MESSAGES) printf("Parsing lines at offset: %" PRId64 "\n", offset);
    reader->seekTo(offset);
    // prepare index based on the already loaded eventlog entries
    int index = eventLogEntries.size();
    // prepare context module ids based on the already loaded eventlog entries
    std::deque<int> contextModuleIds;
    if (eventEntry)
        contextModuleIds.push_front(eventEntry->moduleId);
    for (EventLogEntryList::iterator it = eventLogEntries.begin(); it != eventLogEntries.end(); it++) {
        EventLogEntry *eventLogEntry = *it;
        // handle module method end
        ComponentMethodEndEntry *componentMethodEndEntry = dynamic_cast<ComponentMethodEndEntry *>(eventLogEntry);
        if (componentMethodEndEntry)
            contextModuleIds.pop_front();
        // handle module method begin
        ComponentMethodBeginEntry *componentMethodBeginEntry = dynamic_cast<ComponentMethodBeginEntry *>(eventLogEntry);
        if (componentMethodBeginEntry)
            contextModuleIds.push_front(componentMethodBeginEntry->targetComponentId);
    }
    // parse lines one by one
    while (true) {
        char *line = reader->getNextLineBufferPointer();
        if (!line) {
            endOffset = reader->getFileSize();
            break;
        }
        EventLogEntry *eventLogEntry = EventLogEntry::parseEntry(eventLog, this, index, reader->getCurrentLineStartOffset(), line, reader->getCurrentLineLength());
        // stop at first empty line
        if (!eventLogEntry) {
            endOffset = reader->getCurrentLineEndOffset();
            break;
        }
        else
            index++;
        // first line must be an event entry
        EventEntry *readEventEntry = dynamic_cast<EventEntry *>(eventLogEntry);
        Assert((!eventEntry && readEventEntry) || (eventEntry && !readEventEntry));
        if (!eventEntry) {
            eventEntry = readEventEntry;
            contextModuleIds.push_front(eventEntry->moduleId);
        }
        // handle module method end
        ComponentMethodEndEntry *componentMethodEndEntry = dynamic_cast<ComponentMethodEndEntry *>(eventLogEntry);
        if (componentMethodEndEntry)
            contextModuleIds.pop_front();
        // store log entry
        eventLogEntry->level = contextModuleIds.size() - 1;
        eventLogEntry->contextModuleId = contextModuleIds.front();
        eventLogEntries.push_back(eventLogEntry);
        // handle module method begin
        ComponentMethodBeginEntry *componentMethodBeginEntry = dynamic_cast<ComponentMethodBeginEntry *>(eventLogEntry);
        if (componentMethodBeginEntry)
            contextModuleIds.push_front(componentMethodBeginEntry->targetComponentId);
        // count message entry
        if (dynamic_cast<EventLogMessageEntry *>(eventLogEntry))
            numEventLogMessages++;
        // count begin send entry
        else if (dynamic_cast<BeginSendEntry *>(eventLogEntry))
            numBeginSendEntries++;
        // count begin send entry
        else if (dynamic_cast<CustomEntry *>(eventLogEntry))
            numCustomEntries++;
    }
    Assert(eventEntry);
    return endOffset;
}

void Event::print(FILE *file, bool outputEventLogMessages)
{
    for (auto eventLogEntry : eventLogEntries) {
        if (outputEventLogMessages || !dynamic_cast<EventLogMessageEntry *>(eventLogEntry))
            eventLogEntry->print(file);
    }
    fprintf(file, "\n");
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
    for (int i = beginSendEntry->getEntryIndex() + 1; i < (int)eventLogEntries.size(); i++) {
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

ComponentMethodEndEntry *Event::getComponentMethodEndEntry(ComponentMethodBeginEntry *componentMethodBeginEntry)
{
    Assert(componentMethodBeginEntry && this == componentMethodBeginEntry->getEvent());
    int level = 0;
    for (int i = componentMethodBeginEntry->getEntryIndex() + 1; i < (int)eventLogEntries.size(); i++) {
        EventLogEntry *eventLogEntry = eventLogEntries[i];
        if (dynamic_cast<ComponentMethodBeginEntry *>(eventLogEntry))
            level++;
        ComponentMethodEndEntry *componentMethodEndEntry = dynamic_cast<ComponentMethodEndEntry *>(eventLogEntry);
        if (componentMethodEndEntry) {
            if (!level)
                return componentMethodEndEntry;
            else
                level--;
        }
    }
    // premature end of file
    return nullptr;
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
        else if (eventLogEntry == endSendEntry || (!dynamic_cast<EventLogMessageEntry *>(eventLogEntry) && !dynamic_cast<MessageDescriptionEntry *>(eventLogEntry)))
            break;
        if (transmissionDelay != 0)
            break;
        index++;
    }

    return transmissionDelay;
}

simtime_t Event::getRemainingDuration(BeginSendEntry *beginSendEntry)
{
    Assert(beginSendEntry && this == beginSendEntry->getEvent());
    EndSendEntry *endSendEntry = getEndSendEntry(beginSendEntry);
    int index = beginSendEntry->getEntryIndex() + 1;
    simtime_t remainingDuration = BigDecimal::Zero;

    // there is at most one entry which specifies a transmission delay
    while (index < (int)eventLogEntries.size()) {
        EventLogEntry *eventLogEntry = eventLogEntries[index];
        if (SendDirectEntry *sendDirectEntry = dynamic_cast<SendDirectEntry *>(eventLogEntry))
            remainingDuration = sendDirectEntry->remainingDuration;
        else if (SendHopEntry *sendHopEntry = dynamic_cast<SendHopEntry *>(eventLogEntry))
            remainingDuration = sendHopEntry->remainingDuration;
        else if (eventLogEntry == endSendEntry || (!dynamic_cast<EventLogMessageEntry *>(eventLogEntry) && !dynamic_cast<MessageDescriptionEntry *>(eventLogEntry)))
            break;
        if (remainingDuration != 0)
            break;
        index++;
    }

    return remainingDuration;
}

Event *Event::getPreviousEvent()
{
    if (!previousEvent && eventLog->getFirstEvent() != this) {
        eventnumber_t eventNumber;
        file_offset_t lineBeginOffset = -1, lineEndOffset;
        simtime_t simulationTime;
        eventLog->readToEventLine(false, beginOffset, eventNumber, simulationTime, lineBeginOffset, lineEndOffset);
        previousEvent = eventLog->getEventForBeginOffset(lineBeginOffset);

        if (previousEvent)
            IEvent::linkEvents(previousEvent, this);
    }

    return (Event *)previousEvent;
}

Event *Event::getNextEvent()
{
    if (!nextEvent && eventLog->getLastEvent() != this) {
        eventnumber_t eventNumber;
        file_offset_t lineBeginOffset = -1, lineEndOffset;
        simtime_t simulationTime;
        eventLog->readToEventLine(true, endOffset, eventNumber, simulationTime, lineBeginOffset, lineEndOffset);
        nextEvent = eventLog->getEventForBeginOffset(lineBeginOffset);

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
        return (BeginSendEntry *)cause->getBeginMessageDescriptionEntry();
    else
        return nullptr;
}

MessageSendDependency *Event::getCause()
{
    if (!cause) {
        Event *event = getCauseEvent();
        if (event) {
            int index = event->findBeginSendEntryIndex(getMessageId());
            if (index != -1)
                cause = new MessageSendDependency(eventLog, getCauseEventNumber(), index);
        }
    }
    return cause;
}

IMessageDependencyList *Event::getCauses()
{
    if (!causes) {
        causes = new IMessageDependencyList();
        // using "ce" from "E" line
        MessageSendDependency *cause = getCause();
        if (cause)
            causes->push_back(cause);
        // add message reuses
        eventnumber_t eventNumber = getEventNumber();
        for (int i = 0; i < (int)eventLogEntries.size(); i++) {
            MessageDescriptionEntry *messageDescriptionEntry = dynamic_cast<MessageDescriptionEntry *>(eventLogEntries[i]);
            if (messageDescriptionEntry && messageDescriptionEntry->previousEventNumber != -1 && messageDescriptionEntry->previousEventNumber != eventNumber)
                causes->push_back(new MessageReuseDependency(eventLog, eventNumber, i));
        }
    }

    return causes;
}

IMessageDependencyList *Event::getConsequences()
{
    if (!consequences) {
        consequences = new IMessageDependencyList();
        // collect all message ids that are used in this event (eliminating duplicates)
        std::set<int> messageIds;
        if (getMessageId() != -1)
            messageIds.insert(getMessageId());
        for (int i = 0; i < (int)eventLogEntries.size(); i++) {
            MessageDescriptionEntry *messageDescriptionEntry = dynamic_cast<MessageDescriptionEntry *>(eventLogEntries[i]);
            if (messageDescriptionEntry) {
                DeleteMessageEntry *deleteMessageEntry = dynamic_cast<DeleteMessageEntry *>(eventLogEntries[i]);
                if (deleteMessageEntry) {
                    std::set<int>::iterator it = messageIds.find(deleteMessageEntry->messageId);
                    if (it != messageIds.end())
                        messageIds.erase(it);
                }
                else {
                    messageIds.insert(messageDescriptionEntry->messageId);
                    CloneMessageEntry *cloneMessageEntry = dynamic_cast<CloneMessageEntry *>(eventLogEntries[i]);
                    if (cloneMessageEntry)
                        messageIds.insert(cloneMessageEntry->cloneId);
                }
            }
        }
        // collect reuses
        for (std::set<int>::iterator it = messageIds.begin(); it != messageIds.end(); it++) {
            MessageDescriptionEntry *reuseMessageDescriptionEntry = findReuseMessageDescriptionEntry(*it);
            if (reuseMessageDescriptionEntry) {
                Assert(reuseMessageDescriptionEntry->previousEventNumber == getEventNumber());
                consequences->push_back(new MessageReuseDependency(eventLog, reuseMessageDescriptionEntry->getEvent()->getEventNumber(), reuseMessageDescriptionEntry->getEntryIndex()));
            }
        }
        // handle begin sends separately
        for (int i = 0; i < (int)eventLogEntries.size(); i++) {
            eventLog->progress();
            // using "t" from "ES" lines
            BeginSendEntry *beginSendEntry = dynamic_cast<BeginSendEntry *>(eventLogEntries[i]);
            if (beginSendEntry && messageIds.find(beginSendEntry->messageId) != messageIds.end())
                consequences->push_back(new MessageSendDependency(eventLog, getEventNumber(), i));
        }
    }
    return consequences;
}

MessageDescriptionEntry *Event::findReuseMessageDescriptionEntry(int messageId)
{
    // TODO: optimization: cache first message description entries between this event and the first index
    // 1. linear search among the events starting from the immediately following event until the first index
    Index *index = nullptr;
    Event *event = getNextEvent();
    eventnumber_t previousEventNumber = getEventNumber();
    while (event) {
        if (event->getMessageId() == messageId)
            // we found an EventEntry but that is not a message reuse
            return nullptr;
        else {
            MessageDescriptionEntry *messageDescriptionEntry = event->findLocalReuseMessageDescriptionEntry(previousEventNumber, messageId);
            if (messageDescriptionEntry)
                return messageDescriptionEntry;
        }
        index = eventLog->getIndex(event->getEventNumber());
        if (index)
            break;
        event = event->getNextEvent();
    }
    // 2. linear search among the indices starting from the first index after this event until the last index
    event = nullptr;
    while (index) {
        // check if there's any reference to this event
        if (index->containsReferenceRemovedEntry(previousEventNumber, -1)) {
            std::vector<EventLogEntry *> *removedEntries = index->getRemovedEventLogEntries();
            for (int i = 0; i < (int)removedEntries->size(); i++) {
                EventLogEntry *eventLogEntry = removedEntries->at(i);
                EventEntry *eventEntry = dynamic_cast<EventEntry *>(eventLogEntry);
                MessageDescriptionEntry *removedMessageDescriptionEntry = dynamic_cast<MessageDescriptionEntry *>(eventLogEntry);
                // looking for a removed eventlog entry (from this event) with the given message id
                if ((removedMessageDescriptionEntry && removedMessageDescriptionEntry->messageId == messageId && removedMessageDescriptionEntry->getEvent() == this) ||
                    (eventEntry && eventEntry->messageId == messageId && eventEntry->getEvent() == this))
                {
                    // there can be another entry that removed the one we are looking for
                    std::vector<EventLogEntry *> *addedEntries = index->getAddedEventLogEntries();
                    for (int j = 0; j < (int)addedEntries->size(); j++) {
                        MessageDescriptionEntry *messageDescriptionEntry = dynamic_cast<MessageDescriptionEntry *>(addedEntries->at(j));
                        // we must make sure that the previousEventNumber matches
                        if (messageDescriptionEntry && messageDescriptionEntry->messageId == messageId && messageDescriptionEntry->previousEventNumber == previousEventNumber)
                            return messageDescriptionEntry;
                    }
                    // since there was no such entry the message is deleted between the two indices
                    // do a linear search between the previous and this index to find out what happened between them
                    Index *previousIndex = index->getPreviousIndex();
                    if (previousIndex) {
                        event = eventLog->getEventForEventNumber(previousIndex->getIndexEntry()->eventNumber);
                        if (event)
                            event = event->getNextEvent();
                        while (event) {
                            if (event->getMessageId() == messageId)
                                // we found an EventEntry but that is not a message reuse
                                return nullptr;
                            else {
                                MessageDescriptionEntry *messageDescriptionEntry = event->findLocalReuseMessageDescriptionEntry(previousEventNumber, messageId);
                                if (messageDescriptionEntry)
                                    return messageDescriptionEntry;
                            }
                            if (event->getEventNumber() == index->getIndexEntry()->eventNumber)
                                break;
                            event = event->getNextEvent();
                        }
                    }
                    // we can stop here, because there is no sign of the message
                    return nullptr;
                }
            }
        }
        Index *nextIndex = index->getNextIndex();
        if (!nextIndex)
            event = index->getEvent();
        index = nextIndex;
    }
    // 3. linear search among the remaining events (after the last index)
    while (event) {
        if (event->getMessageId() == messageId)
            // we found an EventEntry but that is not a message reuse
            return nullptr;
        else {
            MessageDescriptionEntry *messageDescriptionEntry = event->findLocalReuseMessageDescriptionEntry(previousEventNumber, messageId);
            if (messageDescriptionEntry)
                return messageDescriptionEntry;
        }
        event = event->getNextEvent();
    }
    // reached end of eventlog
    return nullptr;
}

MessageDescriptionEntry *Event::findLocalReuseMessageDescriptionEntry(eventnumber_t previousEventNumber, int messageId)
{
    for (EventLogEntryList::iterator it = eventLogEntries.begin(); it != eventLogEntries.end(); it++) {
        MessageDescriptionEntry *messageDescriptionEntry = dynamic_cast<MessageDescriptionEntry *>(*it);
        if (messageDescriptionEntry && messageDescriptionEntry->messageId == messageId && messageDescriptionEntry->previousEventNumber == previousEventNumber)
            return messageDescriptionEntry;
    }
    return nullptr;
}

}  // namespace eventlog
}  // namespace omnetpp

