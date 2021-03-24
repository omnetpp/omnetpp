package org.omnetpp.eventlog;

import java.io.OutputStream;
import java.util.ArrayList;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.entry.IndexEntry;
import org.omnetpp.eventlog.entry.ReferenceAddedEntry;
import org.omnetpp.eventlog.entry.ReferenceEntry;
import org.omnetpp.eventlog.entry.ReferenceFoundEntry;
import org.omnetpp.eventlog.entry.ReferenceRemovedEntry;

/**
 * Manages all eventlog entries for a single index. (all lines belonging to an "I" line)
 * The content of the index is actually loaded lazily when accessed.
 */
public class Index implements IChunk
{
    protected EventLog eventLog; // the corresponding eventlog
    protected Event event; // the corresponding event
    protected IndexEntry indexEntry; // the index entry that corresponds to the actual index ("I" line)
    protected long beginOffset = -1; // file offset where the index starts
    protected long endOffset = -1; // file offset where the index ends (including the following empty line, equals to the begin of the next thunk)

    protected Snapshot previousSnapshot;
    protected Snapshot nextSnapshot;
    protected Index previousIndex;
    protected Index nextIndex;

    protected ArrayList<ReferenceEntry> referenceAddedEntries = new ArrayList<ReferenceEntry >(); // unresolved references (pure numbers)
    protected ArrayList<ReferenceEntry> referenceRemovedEntries = new ArrayList<ReferenceEntry >(); // unresolved references (pure numbers)
    protected ArrayList<ReferenceEntry> referenceFoundEntries; // unresolved references (pure numbers)

    protected ArrayList<EventLogEntry> addedEventLogEntries; // resolved references (lazy)
    protected ArrayList<EventLogEntry> removedEventLogEntries; // resolved references (lazy)
    protected EventLogEntryCache foundEventLogEntries; // after applying resolved added and removed references (lazy)

    public Index(EventLog eventLog, long beginOffset, Snapshot previousSnapshot) {
        Assert.isTrue(beginOffset >= 0);
        this.eventLog = eventLog;
        this.beginOffset = beginOffset;
        this.previousSnapshot = previousSnapshot;
        event = null;
        indexEntry = null;
        nextSnapshot = null;
        previousIndex = null;
        nextIndex = null;
        referenceFoundEntries = null;
        addedEventLogEntries = null;
        removedEventLogEntries = null;
        foundEventLogEntries = null;
    }

    /**
     * Parse the index from a file.
     */
    public final long parse(FileReader reader) {
        reader.seekTo(beginOffset);
        int index = 0;
        while (true) {
            String line = reader.getNextLine();
            if (line == null) {
                endOffset = reader.getFileSize();
                break;
            }
            EventLogEntry eventLogEntry = EventLogEntry.parseEntry(eventLog, null, index, reader.getCurrentLineStartOffset(), line, (int)reader.getCurrentLineLength());
            // stop at first empty line
            if (eventLogEntry == null) {
                endOffset = reader.getCurrentLineEndOffset();
                break;
            }
            else
                index++;
            // first line must be an index entry
            IndexEntry readIndexEntry = eventLogEntry instanceof IndexEntry ? (IndexEntry)eventLogEntry : null;
            Assert.isTrue((indexEntry == null && readIndexEntry != null) || (indexEntry != null && readIndexEntry == null));
            if (indexEntry == null)
                indexEntry = readIndexEntry;
            ReferenceAddedEntry referenceAddedEntry = eventLogEntry instanceof ReferenceAddedEntry ? (ReferenceAddedEntry)eventLogEntry : null;
            if (referenceAddedEntry != null)
                referenceAddedEntries.add(referenceAddedEntry);
            ReferenceRemovedEntry referenceRemovedEntry = eventLogEntry instanceof ReferenceRemovedEntry ? (ReferenceRemovedEntry)eventLogEntry : null;
            if (referenceRemovedEntry != null)
                referenceRemovedEntries.add(referenceRemovedEntry);
        }
        return endOffset;
    }

    /**
     * Print the index into a file.
     */
    public final void print(OutputStream stream) {
        ensureParsed();
        indexEntry.print(stream);
        for (ReferenceEntry it : referenceRemovedEntries) {
            ReferenceEntry referenceEntry = it;
            referenceEntry.print(stream);
        }
        for (ReferenceEntry it : referenceAddedEntries) {
            ReferenceEntry referenceEntry = it;
            referenceEntry.print(stream);
        }
    }

    @Override
    public String getAsString() {
        return null;
    }

    @Override
    public String getAsString(String name) {
        return null;
    }

    @Override
    public final long getBeginOffset() {
        return beginOffset;
    }

    @Override
    public final long getEndOffset() {
        ensureParsed();
        return endOffset;
    }

    public final IndexEntry getIndexEntry() {
        ensureParsed();
        return indexEntry;
    }

    @Override
    public final long getEventNumber() {
        ensureParsed();
        return indexEntry.eventNumber;
    }

    @Override
    public final BigDecimal getSimulationTime() {
        ensureParsed();
        return indexEntry.simulationTime;
    }

    public final Event getEvent() {
        if (event == null)
            event = (Event)eventLog.getEventForEventNumber(getIndexEntry().eventNumber);
        return event;
    }

    public final Event getPreviousEvent() {
        return getEvent().getPreviousEvent();
    }

    public final Event getNextEvent() {
        return getEvent();
    }

    public final Index getPreviousIndex() {
        return previousIndex;
    }

    public final Index getNextIndex() {
        return nextIndex;
    }

    public final Snapshot getPreviousSnapshot() {
        return previousSnapshot;
    }

    public final Snapshot getNextSnapshot() {
        if (nextSnapshot == null)
            nextSnapshot = eventLog.getSnapshot(getIndexEntry().eventNumber, MatchKind.LAST_OR_NEXT);
        return nextSnapshot;
    }

    public final boolean containsReferenceAddedEntry(long eventNumber, int entryIndex) {
        ensureParsed();
        return containsReferenceEntry(referenceAddedEntries, eventNumber, entryIndex);
    }

    public final boolean containsReferenceRemovedEntry(long eventNumber, int entryIndex) {
        ensureParsed();
        return containsReferenceEntry(referenceRemovedEntries, eventNumber, entryIndex);
    }

    public final boolean containsReferenceFoundEntry(long eventNumber, int entryIndex) {
        ensureParsed();
        return containsReferenceEntry(getReferenceFoundEntries(), eventNumber, entryIndex);
    }

    public final ArrayList<ReferenceEntry> getReferenceAddedEntries() {
        ensureParsed();
        return new ArrayList<ReferenceEntry >(referenceAddedEntries);
    }

    public final ArrayList<ReferenceEntry> getReferenceRemovedEntries() {
        ensureParsed();
        return new ArrayList<ReferenceEntry >(referenceRemovedEntries);
    }

    public final ArrayList<ReferenceEntry> getReferenceFoundEntries() {
        if (referenceFoundEntries == null) {
            ensureParsed();
            referenceFoundEntries = new ArrayList<ReferenceEntry>();
            ArrayList<ReferenceEntry> previousReferenceFoundEntries = null;
            if (previousSnapshot != null && (previousIndex == null || previousIndex.getEventNumber() <= previousSnapshot.getEventNumber()))
                previousReferenceFoundEntries = previousSnapshot.getReferenceFoundEntries();
            else if (previousIndex != null)
                previousReferenceFoundEntries = previousIndex.getReferenceFoundEntries();
            if (previousReferenceFoundEntries != null) {
                for (ReferenceEntry previousReferenceFoundEntry : previousReferenceFoundEntries) {
                    ReferenceFoundEntry referenceFoundEntry = new ReferenceFoundEntry(this, -1);
                    referenceFoundEntry.eventNumber = previousReferenceFoundEntry.eventNumber;
                    referenceFoundEntry.beginEntryIndex = previousReferenceFoundEntry.beginEntryIndex;
                    referenceFoundEntry.endEntryIndex = previousReferenceFoundEntry.endEntryIndex;
                    referenceFoundEntries.add(referenceFoundEntry);
                }
            }
            for (ReferenceEntry referenceRemovedEntry : referenceRemovedEntries) {
                for (int index = 0; index < (int)referenceFoundEntries.size();) {
                    ReferenceEntry referenceFoundEntry = referenceFoundEntries.get(index);
                    if (referenceFoundEntry.eventNumber == referenceRemovedEntry.eventNumber) {
                        // unrelated remove
                        if (referenceRemovedEntry.endEntryIndex < referenceFoundEntry.beginEntryIndex || referenceRemovedEntry.beginEntryIndex> referenceFoundEntry.endEntryIndex)
                            index++;
                        // complete remove
                        else if (referenceRemovedEntry.beginEntryIndex <= referenceFoundEntry.beginEntryIndex && referenceRemovedEntry.endEntryIndex >= referenceFoundEntry.endEntryIndex) {
                            referenceFoundEntries.remove(index);
                            referenceFoundEntry = null;
                        }
                        // remove lower part
                        else if (referenceRemovedEntry.beginEntryIndex <= referenceFoundEntry.beginEntryIndex && referenceRemovedEntry.endEntryIndex < referenceFoundEntry.endEntryIndex) {
                            referenceFoundEntry.beginEntryIndex = referenceRemovedEntry.endEntryIndex - 1;
                            index++;
                        }
                        // remove upper part
                        else if (referenceRemovedEntry.beginEntryIndex> referenceFoundEntry.beginEntryIndex && referenceRemovedEntry.endEntryIndex >= referenceFoundEntry.endEntryIndex) {
                            referenceFoundEntry.endEntryIndex = referenceRemovedEntry.beginEntryIndex - 1;
                            index++;
                        }
                        // split remove
                        else {
                            referenceFoundEntries.remove(index);
                            ReferenceFoundEntry lowerReferenceFoundEntry = new ReferenceFoundEntry(this, -1);
                            lowerReferenceFoundEntry.eventNumber = referenceFoundEntry.eventNumber;
                            lowerReferenceFoundEntry.beginEntryIndex = referenceFoundEntry.beginEntryIndex;
                            lowerReferenceFoundEntry.endEntryIndex = referenceRemovedEntry.beginEntryIndex - 1;
                            referenceFoundEntries.add(lowerReferenceFoundEntry);
                            ReferenceFoundEntry upperReferenceFoundEntry = new ReferenceFoundEntry(this, -1);
                            upperReferenceFoundEntry.eventNumber = referenceFoundEntry.eventNumber;
                            upperReferenceFoundEntry.beginEntryIndex = referenceRemovedEntry.endEntryIndex + 1;
                            upperReferenceFoundEntry.endEntryIndex = referenceFoundEntry.endEntryIndex;
                            referenceFoundEntries.add(upperReferenceFoundEntry);
                            referenceFoundEntry = null;
                        }
                    }
                    else
                        index++;
                }
            }
            // added entries are disjunct
            for (ReferenceEntry ait : referenceAddedEntries) {
                ReferenceEntry referenceAddedEntry = ait;
                ReferenceFoundEntry referenceFoundEntry = new ReferenceFoundEntry(this, -1);
                referenceFoundEntry.eventNumber = referenceAddedEntry.eventNumber;
                referenceFoundEntry.beginEntryIndex = referenceAddedEntry.beginEntryIndex;
                referenceFoundEntry.endEntryIndex = referenceAddedEntry.endEntryIndex;
                referenceFoundEntries.add(referenceFoundEntry);
            }
        }
        return referenceFoundEntries;
    }

    public final ArrayList<EventLogEntry> getAddedEventLogEntries() {
        if (addedEventLogEntries == null)
            addedEventLogEntries = resolveReferenceEntries(referenceAddedEntries);
        return addedEventLogEntries;
    }

    public final ArrayList<EventLogEntry> getRemovedEventLogEntries() {
        if (removedEventLogEntries == null)
            removedEventLogEntries = resolveReferenceEntries(referenceRemovedEntries);
        return removedEventLogEntries;
    }

    /**
     * Returns the eventlog entries merged backwards until the previous snapshot.
     */
    public final EventLogEntryCache getFoundEventLogEntries() {
        if (foundEventLogEntries == null) {
            ensureParsed();
            foundEventLogEntries = new EventLogEntryCache();
            ArrayList<EventLogEntry> previousFoundEventLogEntries = null;
            if (previousSnapshot != null && (previousIndex == null || previousIndex.getEventNumber() <= previousSnapshot.getEventNumber()))
                previousFoundEventLogEntries = previousSnapshot.getFoundEventLogEntries().getEventLogEntries();
            else if (previousIndex != null)
                previousFoundEventLogEntries = previousIndex.getFoundEventLogEntries().getEventLogEntries();
            if (previousFoundEventLogEntries != null)
                for (EventLogEntry eventLogEntry : previousFoundEventLogEntries)
                    foundEventLogEntries.getEventLogEntries().add(eventLogEntry);
            ArrayList<EventLogEntry> removedEventLogEntries = getRemovedEventLogEntries();
            for (EventLogEntry eventLogEntry : removedEventLogEntries)
                foundEventLogEntries.getEventLogEntries().remove(eventLogEntry);
            for (EventLogEntry eventLogEntry : getAddedEventLogEntries())
                foundEventLogEntries.getEventLogEntries().add(eventLogEntry);
        }
        return foundEventLogEntries;
    }

    /**
     * Used to maintain the double linked list.
     */
    public static void linkIndices(Index previousIndex, Index nextIndex) {
        previousIndex.nextIndex = nextIndex;
        nextIndex.previousIndex = previousIndex;
    }

    public static void unlinkIndices(Index previousIndex, Index nextIndex) {
        previousIndex.nextIndex = null;
        nextIndex.previousIndex = null;
    }

    public static void unlinkNeighbourIndices(Index index) {
        if (index.previousIndex != null)
            unlinkIndices(index.previousIndex, index);
        if (index.nextIndex != null)
            unlinkIndices(index, index.nextIndex);
    }

    protected final void ensureParsed() {
        if (indexEntry == null)
            parse(eventLog.getFileReader());
    }

    protected final ArrayList<EventLogEntry> resolveReferenceEntries(ArrayList<ReferenceEntry> referenceEntries) {
        ensureParsed();
        ArrayList<EventLogEntry> entries = new ArrayList<EventLogEntry>();
        for (ReferenceEntry it : referenceEntries) {
            ReferenceEntry referenceEntry = it;
            IEvent event = eventLog.getEventForEventNumber(referenceEntry.eventNumber);
            if (event != null) {
                for (int i = referenceEntry.beginEntryIndex; i <= referenceEntry.endEntryIndex; i++)
                    entries.add(event.getEventLogEntry(i));
            }
        }
        return entries;
    }

    boolean containsReferenceEntry(ArrayList<ReferenceEntry> referenceEntries, long eventNumber, int entryIndex) {
        for (ReferenceEntry referenceEntry : referenceEntries)
            if (referenceEntry.eventNumber == eventNumber && referenceEntry.entryIndex == entryIndex)
                return true;
        return false;
    }
}