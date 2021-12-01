//=========================================================================
//  INDEX.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __INDEX_H_
#define __INDEX_H_

#include "eventlog.h"

namespace omnetpp {
namespace eventlog {

class Snapshot;

/**
 * Manages all eventlog entries for a single index. (all lines belonging to an "I" line)
 * The content of the index is actually loaded lazily when accessed.
 */
class EVENTLOG_API Index : public IChunk
{
    protected:
        EventLog *eventLog; // the corresponding eventlog
        Event *event; // the corresponding event
        IndexEntry *indexEntry; // the index entry that corresponds to the actual index ("I" line)
        file_offset_t beginOffset; // file offset where the index starts
        file_offset_t endOffset; // file offset where the index ends (including the following empty line, equals to the begin of the next thunk)

        Snapshot *previousSnapshot;
        Snapshot *nextSnapshot;
        Index *previousIndex;
        Index *nextIndex;

        std::vector<ReferenceEntry *> referenceAddedEntries; // unresolved references (pure numbers)
        std::vector<ReferenceEntry *> referenceRemovedEntries; // unresolved references (pure numbers)
        std::vector<ReferenceEntry *> *referenceFoundEntries; // unresolved references (pure numbers)

        std::vector<EventLogEntry *> *addedEventLogEntries; // resolved references (lazy)
        std::vector<EventLogEntry *> *removedEventLogEntries; // resolved references (lazy)
        EventLogEntryCache *foundEventLogEntries; // after applying resolved added and removed references (lazy)

    public:
        Index(EventLog *eventLog, file_offset_t beginOffset, Snapshot *previousSnapshot);
        virtual ~Index();

        /**
         * Parse the index from a file.
         */
        file_offset_t parse(FileReader *reader);

        /**
         * Print the index into a file.
         */
        void print(FILE *file);

        file_offset_t getBeginOffset() { return beginOffset; }
        file_offset_t getEndOffset() { ensureParsed(); return endOffset; }

        IndexEntry *getIndexEntry() { ensureParsed(); return indexEntry; }
        eventnumber_t getEventNumber() { ensureParsed(); return indexEntry->eventNumber; }
        simtime_t getSimulationTime() { ensureParsed(); return indexEntry->simulationTime; }
        Event *getEvent();

        Event *getPreviousEvent() { return getEvent()->getPreviousEvent(); }
        Event *getNextEvent() { return getEvent(); }
        Index *getPreviousIndex() { return previousIndex; }
        Index *getNextIndex() { return nextIndex; }
        Snapshot *getPreviousSnapshot() { return previousSnapshot; }
        Snapshot *getNextSnapshot();

        bool containsReferenceAddedEntry(eventnumber_t eventNumber, int entryIndex) { ensureParsed(); return containsReferenceEntry(&referenceAddedEntries, eventNumber, entryIndex); }
        bool containsReferenceRemovedEntry(eventnumber_t eventNumber, int entryIndex)  { ensureParsed(); return containsReferenceEntry(&referenceRemovedEntries, eventNumber, entryIndex); }
        bool containsReferenceFoundEntry(eventnumber_t eventNumber, int entryIndex) { ensureParsed(); return containsReferenceEntry(getReferenceFoundEntries(), eventNumber, entryIndex); }

        std::vector<ReferenceEntry *>& getReferenceAddedEntries() { ensureParsed(); return referenceAddedEntries; }
        std::vector<ReferenceEntry *>& getReferenceRemovedEntries() { ensureParsed(); return referenceRemovedEntries; }
        std::vector<ReferenceEntry *> *getReferenceFoundEntries();

        std::vector<EventLogEntry *> *getAddedEventLogEntries();
        std::vector<EventLogEntry *> *getRemovedEventLogEntries();

        /**
         * Returns the eventlog entries merged backwards until the previous snapshot.
         */
        EventLogEntryCache *getFoundEventLogEntries();

        /**
         * Used to maintain the double linked list.
         */
        static void linkIndices(Index *previousEvent, Index *nextEvent);
        static void unlinkIndices(Index *previousEvent, Index *nextEvent);
        static void unlinkNeighbourIndices(Index *nextEvent);

    protected:
        void ensureParsed();
        std::vector<EventLogEntry *> *resolveReferenceEntries(std::vector<ReferenceEntry *>& referenceEntries);
        bool containsReferenceEntry(std::vector<ReferenceEntry *> *referenceEntries, eventnumber_t eventNumber, int entryIndex);
};

}  // namespace eventlog
}  // namespace omnetpp

#endif
