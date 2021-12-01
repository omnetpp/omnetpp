//=========================================================================
//  INDEX.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <iterator>
#include "common/linetokenizer.h"
#include "snapshot.h"
#include "index.h"

namespace omnetpp {
namespace eventlog {

Index::Index(EventLog *eventLog, file_offset_t beginOffset, Snapshot *previousSnapshot)
{
    Assert(beginOffset >= 0);
    this->eventLog = eventLog;
    this->beginOffset = beginOffset;
    this->previousSnapshot = previousSnapshot;
    event = nullptr;
    indexEntry = nullptr;
    nextSnapshot = nullptr;
    previousIndex = nullptr;
    nextIndex = nullptr;
    referenceFoundEntries = nullptr;
    addedEventLogEntries = nullptr;
    removedEventLogEntries = nullptr;
    foundEventLogEntries = nullptr;
}

Index::~Index()
{
    delete indexEntry;
    for (std::vector<ReferenceEntry *>::iterator it = referenceAddedEntries.begin(); it != referenceAddedEntries.end(); it++)
        delete *it;
    for (std::vector<ReferenceEntry *>::iterator it = referenceRemovedEntries.begin(); it != referenceRemovedEntries.end(); it++)
        delete *it;
    if (referenceFoundEntries)
        for (std::vector<ReferenceEntry *>::iterator it = referenceFoundEntries->begin(); it != referenceFoundEntries->end(); it++)
            delete *it;
    delete referenceFoundEntries;
    delete addedEventLogEntries;
    delete removedEventLogEntries;
    delete foundEventLogEntries;
}

void Index::ensureParsed()
{
    if (!indexEntry)
        parse(eventLog->getFileReader());
}

file_offset_t Index::parse(FileReader *reader)
{
    reader->seekTo(beginOffset);
    int index = 0;
    while (true) {
        char *line = reader->getNextLineBufferPointer();
        if (!line) {
            endOffset = reader->getFileSize();
            break;
        }
        EventLogEntry *eventLogEntry = EventLogEntry::parseEntry(eventLog, nullptr, index, reader->getCurrentLineStartOffset(), line, reader->getCurrentLineLength());
        // stop at first empty line
        if (!eventLogEntry) {
            endOffset = reader->getCurrentLineEndOffset();
            break;
        }
        else
            index++;
        // first line must be an index entry
        IndexEntry *readIndexEntry = dynamic_cast<IndexEntry *>(eventLogEntry);
        Assert((!indexEntry && readIndexEntry) || (indexEntry && !readIndexEntry));
        if (!indexEntry)
            indexEntry = readIndexEntry;
        ReferenceAddedEntry *referenceAddedEntry = dynamic_cast<ReferenceAddedEntry *>(eventLogEntry);
        if (referenceAddedEntry)
            referenceAddedEntries.push_back(referenceAddedEntry);
        ReferenceRemovedEntry *referenceRemovedEntry = dynamic_cast<ReferenceRemovedEntry *>(eventLogEntry);
        if (referenceRemovedEntry)
            referenceRemovedEntries.push_back(referenceRemovedEntry);
    }
    return endOffset;
}

void Index::print(FILE *file)
{
    ensureParsed();
    indexEntry->print(file);
    for (std::vector<ReferenceEntry *>::iterator it = referenceRemovedEntries.begin(); it != referenceRemovedEntries.end(); it++) {
        ReferenceEntry *referenceEntry = *it;
        referenceEntry->print(file);
    }
    for (std::vector<ReferenceEntry *>::iterator it = referenceAddedEntries.begin(); it != referenceAddedEntries.end(); it++) {
        ReferenceEntry *referenceEntry = *it;
        referenceEntry->print(file);
    }
    fprintf(file, "\n");
}

bool Index::containsReferenceEntry(std::vector<ReferenceEntry *> *referenceEntries, eventnumber_t eventNumber, int entryIndex)
{
    for (std::vector<ReferenceEntry *>::iterator it = referenceEntries->begin(); it != referenceEntries->end(); it++) {
        ReferenceEntry *referenceEntry = *it;
        if (referenceEntry->eventNumber == eventNumber && (entryIndex == -1 || (referenceEntry->beginEntryIndex <= entryIndex && entryIndex <= referenceEntry->endEntryIndex)))
            return true;
    }
    return false;
}

std::vector<EventLogEntry *> *Index::resolveReferenceEntries(std::vector<ReferenceEntry *>& referenceEntries)
{
    ensureParsed();
    std::vector<EventLogEntry *> *entries = new std::vector<EventLogEntry *>();
    for (std::vector<ReferenceEntry *>::iterator it = referenceEntries.begin(); it != referenceEntries.end(); it++) {
        ReferenceEntry *referenceEntry = *it;
        Event *event = eventLog->getEventForEventNumber(referenceEntry->eventNumber);
        if (event)
            for (int i = referenceEntry->beginEntryIndex; i <= referenceEntry->endEntryIndex; i++)
                entries->push_back(event->getEventLogEntry(i));
    }
    return entries;
}

Event *Index::getEvent()
{
    if (event == nullptr)
        event = eventLog->getEventForEventNumber(getIndexEntry()->eventNumber);
    return event;
}

std::vector<EventLogEntry *> *Index::getAddedEventLogEntries()
{
    if (!addedEventLogEntries)
        addedEventLogEntries = resolveReferenceEntries(referenceAddedEntries);
    return addedEventLogEntries;
}

std::vector<EventLogEntry *> *Index::getRemovedEventLogEntries()
{
    if (!removedEventLogEntries)
        removedEventLogEntries = resolveReferenceEntries(referenceRemovedEntries);
    return removedEventLogEntries;
}

std::vector<ReferenceEntry *> *Index::getReferenceFoundEntries()
{
    if (!referenceFoundEntries) {
        ensureParsed();
        referenceFoundEntries = new std::vector<ReferenceEntry *>();
        std::vector<ReferenceEntry *> *previousReferenceFoundEntries = nullptr;
        if (previousSnapshot && (!previousIndex || previousIndex->getEventNumber() <= previousSnapshot->getEventNumber()))
            previousReferenceFoundEntries = previousSnapshot->getReferenceFoundEntries();
        else if (previousIndex)
            previousReferenceFoundEntries = previousIndex->getReferenceFoundEntries();
        if (previousReferenceFoundEntries) {
            for (std::vector<ReferenceEntry *>::iterator it = previousReferenceFoundEntries->begin(); it != previousReferenceFoundEntries->end(); it++) {
                ReferenceEntry *previousReferenceFoundEntry = *it;
                ReferenceFoundEntry *referenceFoundEntry = new ReferenceFoundEntry(this, -1);
                referenceFoundEntry->eventNumber = previousReferenceFoundEntry->eventNumber;
                referenceFoundEntry->beginEntryIndex = previousReferenceFoundEntry->beginEntryIndex;
                referenceFoundEntry->endEntryIndex = previousReferenceFoundEntry->endEntryIndex;
                referenceFoundEntries->push_back(referenceFoundEntry);
            }
        }
        for (std::vector<ReferenceEntry *>::iterator rit = referenceRemovedEntries.begin(); rit != referenceRemovedEntries.end(); rit++) {
            for (int it = 0; it < (int)referenceFoundEntries->size();) {
                ReferenceEntry *referenceFoundEntry = referenceFoundEntries->at(it);
                ReferenceEntry *referenceRemovedEntry = *rit;
                if (referenceFoundEntry->eventNumber == referenceRemovedEntry->eventNumber) {
                    // unrelated remove
                    if (referenceRemovedEntry->endEntryIndex < referenceFoundEntry->beginEntryIndex || referenceRemovedEntry->beginEntryIndex > referenceFoundEntry->endEntryIndex)
                        it++;
                    // complete remove
                    else if (referenceRemovedEntry->beginEntryIndex <= referenceFoundEntry->beginEntryIndex && referenceRemovedEntry->endEntryIndex >= referenceFoundEntry->endEntryIndex) {
                        referenceFoundEntries->erase(referenceFoundEntries->begin() + it);
                        delete referenceFoundEntry;
                    }
                    // remove lower part
                    else if (referenceRemovedEntry->beginEntryIndex <= referenceFoundEntry->beginEntryIndex && referenceRemovedEntry->endEntryIndex < referenceFoundEntry->endEntryIndex) {
                        referenceFoundEntry->beginEntryIndex = referenceRemovedEntry->endEntryIndex - 1;
                        it++;
                    }
                    // remove upper part
                    else if (referenceRemovedEntry->beginEntryIndex > referenceFoundEntry->beginEntryIndex && referenceRemovedEntry->endEntryIndex >= referenceFoundEntry->endEntryIndex) {
                        referenceFoundEntry->endEntryIndex = referenceRemovedEntry->beginEntryIndex - 1;
                        it++;
                    }
                    // split remove
                    else {
                        referenceFoundEntries->erase(referenceFoundEntries->begin() + it);
                        ReferenceFoundEntry *lowerReferenceFoundEntry = new ReferenceFoundEntry(this, -1);
                        lowerReferenceFoundEntry->eventNumber = referenceFoundEntry->eventNumber;
                        lowerReferenceFoundEntry->beginEntryIndex = referenceFoundEntry->beginEntryIndex;
                        lowerReferenceFoundEntry->endEntryIndex = referenceRemovedEntry->beginEntryIndex - 1;
                        referenceFoundEntries->push_back(lowerReferenceFoundEntry);
                        ReferenceFoundEntry *upperReferenceFoundEntry = new ReferenceFoundEntry(this, -1);
                        upperReferenceFoundEntry->eventNumber = referenceFoundEntry->eventNumber;
                        upperReferenceFoundEntry->beginEntryIndex = referenceRemovedEntry->endEntryIndex + 1;
                        upperReferenceFoundEntry->endEntryIndex = referenceFoundEntry->endEntryIndex;
                        referenceFoundEntries->push_back(upperReferenceFoundEntry);
                        delete referenceFoundEntry;
                    }
                }
                else
                    it++;
            }
        }
        // added entries are disjunct
        for (std::vector<ReferenceEntry *>::iterator ait = referenceAddedEntries.begin(); ait != referenceAddedEntries.end(); ait++) {
            ReferenceEntry *referenceAddedEntry = *ait;
            ReferenceFoundEntry *referenceFoundEntry = new ReferenceFoundEntry(this, -1);
            referenceFoundEntry->eventNumber = referenceAddedEntry->eventNumber;
            referenceFoundEntry->beginEntryIndex = referenceAddedEntry->beginEntryIndex;
            referenceFoundEntry->endEntryIndex = referenceAddedEntry->endEntryIndex;
            referenceFoundEntries->push_back(referenceFoundEntry);
        }
    }
    return referenceFoundEntries;
}

EventLogEntryCache *Index::getFoundEventLogEntries()
{
    if (!foundEventLogEntries) {
        ensureParsed();
        foundEventLogEntries = new EventLogEntryCache();
        std::vector<EventLogEntry *> *previousFoundEventLogEntries = nullptr;
        if (previousSnapshot && (!previousIndex || previousIndex->getEventNumber() <= previousSnapshot->getEventNumber()))
            previousFoundEventLogEntries = previousSnapshot->getFoundEventLogEntries();
        else if (previousIndex)
            previousFoundEventLogEntries = previousIndex->getFoundEventLogEntries();
        if (previousFoundEventLogEntries)
            std::copy(previousFoundEventLogEntries->begin(), previousFoundEventLogEntries->end(), back_inserter(*foundEventLogEntries));
        std::vector<EventLogEntry *> *removedEventLogEntries = getRemovedEventLogEntries();
        for (std::vector<EventLogEntry *>::iterator it = removedEventLogEntries->begin(); it != removedEventLogEntries->end(); it++) {
            std::vector<EventLogEntry *>::iterator jt = std::find(foundEventLogEntries->begin(), foundEventLogEntries->end(), *it);
            if (jt != foundEventLogEntries->end())
                foundEventLogEntries->erase(jt);
        }
        std::vector<EventLogEntry *> *addedEventLogEntries = getAddedEventLogEntries();
        foundEventLogEntries->reserve(foundEventLogEntries->size() + addedEventLogEntries->size());
        std::copy(addedEventLogEntries->begin(), addedEventLogEntries->end(), back_inserter(*foundEventLogEntries));
    }
    return foundEventLogEntries;
}

void Index::linkIndices(Index *previousIndex, Index *nextIndex)
{
    previousIndex->nextIndex = nextIndex;
    nextIndex->previousIndex = previousIndex;
}

void Index::unlinkIndices(Index *previousIndex, Index *nextIndex)
{
    previousIndex->nextIndex = nullptr;
    nextIndex->previousIndex = nullptr;
}

void Index::unlinkNeighbourIndices(Index *index)
{
    if (index->previousIndex)
        unlinkIndices(index->previousIndex, index);
    if (index->nextIndex)
        unlinkIndices(index, index->nextIndex);
}

Snapshot *Index::getNextSnapshot()
{
    if (!nextSnapshot)
        nextSnapshot = eventLog->getSnapshot(getIndexEntry()->eventNumber, LAST_OR_NEXT);
    return nextSnapshot;
}

}  // namespace eventlog
}  // namespace omnetpp
