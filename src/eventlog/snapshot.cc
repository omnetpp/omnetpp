//=========================================================================
//  SNAPSHOT.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "snapshot.h"

namespace omnetpp {
namespace eventlog {

Snapshot::Snapshot(EventLog *eventLog, file_offset_t beginOffset)
{
    Assert(beginOffset >= 0);
    this->eventLog = eventLog;
    this->beginOffset = beginOffset;
    event = nullptr;
    snapshotEntry = nullptr;
    previousSnapshot = nullptr;
    nextSnapshot = nullptr;
    previousIndex = nullptr;
    nextIndex = nullptr;
    foundEventLogEntries = nullptr;
}

Snapshot::~Snapshot()
{
    for (EventLogEntryList::iterator it = eventLogEntries.begin(); it != eventLogEntries.end(); it++)
        delete *it;
    delete foundEventLogEntries;
}

file_offset_t Snapshot::parse(FileReader *reader)
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
        // first line must be an snapshot entry
        SnapshotEntry *readSnapshotEntry = dynamic_cast<SnapshotEntry *>(eventLogEntry);
        Assert((!snapshotEntry && readSnapshotEntry) || (snapshotEntry && !readSnapshotEntry));
        if (!snapshotEntry)
            snapshotEntry = readSnapshotEntry;
        eventLogEntries.push_back(eventLogEntry);
        eventLog->cacheEventLogEntry(eventLogEntry);
        // collect module found entries
        ReferenceFoundEntry *referenceFoundEntry = dynamic_cast<ReferenceFoundEntry *>(eventLogEntry);
        if (referenceFoundEntry)
            referenceFoundEntries.push_back(referenceFoundEntry);
    }
    Assert(snapshotEntry);
    return endOffset;
}

void Snapshot::print(FILE *file)
{
    ensureParsed();
    for (EventLogEntryList::iterator it = eventLogEntries.begin(); it != eventLogEntries.end(); it++) {
        EventLogEntry *eventLogEntry = *it;
        eventLogEntry->print(file);
    }
    fprintf(file, "\n");
}

void Snapshot::ensureParsed()
{
    if (!snapshotEntry)
        parse(eventLog->getFileReader());
}

Event *Snapshot::getEvent()
{
    if (event == nullptr)
        event = eventLog->getEventForEventNumber(getSnapshotEntry()->eventNumber);
    return event;
}

Index *Snapshot::getPreviousIndex()
{
    if (!previousIndex)
        previousIndex = eventLog->getIndex(getSnapshotEntry()->eventNumber, FIRST_OR_PREVIOUS);
    return previousIndex;
}

Index *Snapshot::getNextIndex()
{
    if (!nextIndex)
        nextIndex = eventLog->getIndex(getSnapshotEntry()->eventNumber, LAST_OR_NEXT);
    return nextIndex;
}

Snapshot *Snapshot::getPreviousSnapshot()
{
    if (!previousSnapshot)
        previousSnapshot = eventLog->getSnapshot(getSnapshotEntry()->eventNumber - 1, FIRST_OR_PREVIOUS);
    return previousSnapshot;
}

Snapshot *Snapshot::getNextSnapshot()
{
    if (!nextSnapshot)
        nextSnapshot = eventLog->getSnapshot(getSnapshotEntry()->eventNumber + 1, LAST_OR_NEXT);
    return nextSnapshot;
}

bool Snapshot::containsReferenceFoundEntry(eventnumber_t eventNumber, int entryIndex)
{
    ensureParsed();
    for (std::vector<ReferenceEntry *>::iterator it = referenceFoundEntries.begin(); it != referenceFoundEntries.end(); it++) {
        ReferenceEntry *referenceFoundEntry = *it;
        if (referenceFoundEntry->eventNumber == eventNumber && (entryIndex == -1 || (referenceFoundEntry->beginEntryIndex <= entryIndex && entryIndex <= referenceFoundEntry->endEntryIndex)))
            return true;
    }
    return false;
}

EventLogEntryCache *Snapshot::getFoundEventLogEntries()
{
    if (!foundEventLogEntries) {
        ensureParsed();
        foundEventLogEntries = new EventLogEntryCache();
        for (std::vector<ReferenceEntry *>::iterator it = referenceFoundEntries.begin(); it != referenceFoundEntries.end(); it++) {
            ReferenceEntry *referenceFoundEntry = *it;
            Event *event = eventLog->getEventForEventNumber(referenceFoundEntry->eventNumber);
            if (event)
                for (int i = referenceFoundEntry->beginEntryIndex; i <= referenceFoundEntry->endEntryIndex; i++)
                    foundEventLogEntries->push_back(event->getEventLogEntry(i));
        }
    }
    return foundEventLogEntries;
}

}  // namespace eventlog
}  // namespace omnetpp
