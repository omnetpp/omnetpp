//=========================================================================
//  SNAPSHOT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __SNAPSHOT_H_
#define __SNAPSHOT_H_

#include "eventlog.h"

namespace omnetpp {
namespace eventlog {

/**
 * Manages all eventlog entries for a single snapshot. (all lines belonging to an "S" line)
 * The content of the snapshot is actually loaded lazily when accessed.
 */
class EVENTLOG_API Snapshot : public IChunk
{
    protected:
        EventLog *eventLog; // the corresponding eventlog
        Event *event; // the corresponding event
        SnapshotEntry *snapshotEntry; // the snapshot entry that corresponds to the actual snapshot ("S" line)
        file_offset_t beginOffset; // file offset where the snapshot starts
        file_offset_t endOffset; // file offset where the snapshot ends (including the following empty line, equals to the begin of the next thunk)
        EventLogEntryList eventLogEntries; // all entries parsed from the file (lines below "S" line)

        Snapshot *previousSnapshot;
        Snapshot *nextSnapshot;
        Index *previousIndex;
        Index *nextIndex;

        std::vector<ReferenceEntry *> referenceFoundEntries;
        EventLogEntryCache *foundEventLogEntries;

    public:
        Snapshot(EventLog *eventLog, file_offset_t beginOffset);
        virtual ~Snapshot();

        /**
         * Parse the snapshot from a file.
         */
        file_offset_t parse(FileReader *reader);

        /**
         * Print the snapshot into a file.
         */
        void print(FILE *file);

        file_offset_t getBeginOffset() { return beginOffset; }
        file_offset_t getEndOffset() { ensureParsed(); return endOffset; }
        SnapshotEntry *getSnapshotEntry() { ensureParsed(); return snapshotEntry; }
        eventnumber_t getEventNumber() { ensureParsed(); return snapshotEntry->eventNumber; }
        simtime_t getSimulationTime() { ensureParsed(); return snapshotEntry->simulationTime; }
        Event *getEvent();

        Event *getPreviousEvent() { return getEvent()->getPreviousEvent(); }
        Event *getNextEvent() { return getEvent(); }
        Index *getPreviousIndex();
        Index *getNextIndex();
        Snapshot *getPreviousSnapshot();
        Snapshot *getNextSnapshot();

        std::vector<ReferenceEntry *> *getReferenceFoundEntries() { ensureParsed(); return &referenceFoundEntries; }
        EventLogEntryCache *getFoundEventLogEntries();

        bool containsReferenceFoundEntry(eventnumber_t eventNumber, int entryIndex);

        int getNumEventLogEntries() { ensureParsed(); return eventLogEntries.size(); }
        EventLogEntry *getEventLogEntry(int index) { ensureParsed(); return eventLogEntries[index]; }

    protected:
        void ensureParsed();
};

}  // namespace eventlog
}  // namespace omnetpp

#endif
