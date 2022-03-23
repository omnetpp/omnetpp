package org.omnetpp.eventlog;

import java.io.OutputStream;
import java.util.ArrayList;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.util.BigDecimal;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.entry.ReferenceEntry;
import org.omnetpp.eventlog.entry.ReferenceFoundEntry;
import org.omnetpp.eventlog.entry.SnapshotEntry;

/**
 * Manages all eventlog entries for a single snapshot. (all lines belonging to an "S" line)
 * The content of the snapshot is actually loaded lazily when accessed.
 */
public class Snapshot implements IChunk
{
    protected EventLog eventLog; // the corresponding eventlog
    protected Event event; // the corresponding event
    protected SnapshotEntry snapshotEntry; // the snapshot entry that corresponds to the actual snapshot ("S" line)
    protected long beginOffset = -1; // file offset where the snapshot starts
    protected long endOffset = -1; // file offset where the snapshot ends (including the following empty line, equals to the begin of the next thunk)
    protected ArrayList<EventLogEntry> eventLogEntries = new ArrayList<EventLogEntry>(); // all entries parsed from the file (lines below "S" line)

    protected Snapshot previousSnapshot;
    protected Snapshot nextSnapshot;
    protected Index previousIndex;
    protected Index nextIndex;

    protected ArrayList<ReferenceEntry > referenceFoundEntries = new ArrayList<ReferenceEntry >();
    protected EventLogEntryCache foundEventLogEntryCache;

    public Snapshot(EventLog eventLog, long beginOffset) {
        Assert.isTrue(beginOffset >= 0);
        this.eventLog = eventLog;
        this.beginOffset = beginOffset;
        event = null;
        snapshotEntry = null;
        previousSnapshot = null;
        nextSnapshot = null;
        previousIndex = null;
        nextIndex = null;
        foundEventLogEntryCache = null;
    }

    /**
     * Parse the snapshot from a file.
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
            // first line must be an snapshot entry
            SnapshotEntry readSnapshotEntry = eventLogEntry instanceof SnapshotEntry ? (SnapshotEntry)eventLogEntry : null;
            Assert.isTrue((snapshotEntry == null && readSnapshotEntry != null) || (snapshotEntry != null && readSnapshotEntry == null));
            if (snapshotEntry == null)
                snapshotEntry = readSnapshotEntry;
            eventLogEntries.add(eventLogEntry);
            eventLog.cacheEventLogEntry(eventLogEntry);
            // collect module found entries
            ReferenceFoundEntry referenceFoundEntry = eventLogEntry instanceof ReferenceFoundEntry ? (ReferenceFoundEntry)eventLogEntry : null;
            if (referenceFoundEntry != null)
                referenceFoundEntries.add(referenceFoundEntry);
        }
        Assert.isTrue(snapshotEntry != null);
        return endOffset;
    }

    /**
     * Print the snapshot into a file.
     */
    public final void print(OutputStream stream) {
        ensureParsed();
        for (EventLogEntry it : eventLogEntries) {
            EventLogEntry eventLogEntry = it;
            eventLogEntry.print(stream);
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

    public final SnapshotEntry getSnapshotEntry() {
        ensureParsed();
        return snapshotEntry;
    }

    @Override
    public final long getEventNumber() {
        ensureParsed();
        return snapshotEntry.eventNumber;
    }

    @Override
    public final BigDecimal getSimulationTime() {
        ensureParsed();
        return snapshotEntry.simulationTime;
    }

    public final Event getEvent() {
        if (event == null)
            event = (Event)eventLog.getEventForEventNumber(getSnapshotEntry().eventNumber);
        return event;
    }

    public final Event getPreviousEvent() {
        return getEvent().getPreviousEvent();
    }

    public final Event getNextEvent() {
        return getEvent();
    }

    public final Index getPreviousIndex() {
        if (previousIndex == null)
            previousIndex = eventLog.getIndex(getSnapshotEntry().eventNumber, MatchKind.FIRST_OR_PREVIOUS);
        return previousIndex;
    }

    public final Index getNextIndex() {
        if (nextIndex == null)
            nextIndex = eventLog.getIndex(getSnapshotEntry().eventNumber, MatchKind.LAST_OR_NEXT);
        return nextIndex;
    }

    public final Snapshot getPreviousSnapshot() {
        if (previousSnapshot == null)
            previousSnapshot = eventLog.getSnapshot(getSnapshotEntry().eventNumber - 1, MatchKind.FIRST_OR_PREVIOUS);
        return previousSnapshot;
    }

    public final Snapshot getNextSnapshot() {
        if (nextSnapshot == null)
            nextSnapshot = eventLog.getSnapshot(getSnapshotEntry().eventNumber + 1, MatchKind.LAST_OR_NEXT);
        return nextSnapshot;
    }

    public final ArrayList<ReferenceEntry > getReferenceFoundEntries() {
        ensureParsed();
        return new ArrayList<ReferenceEntry >(referenceFoundEntries);
    }

    public final EventLogEntryCache getFoundEventLogEntries() {
        if (foundEventLogEntryCache == null) {
            ensureParsed();
            foundEventLogEntryCache = new EventLogEntryCache();
            for (ReferenceEntry it : referenceFoundEntries) {
                ReferenceEntry referenceFoundEntry = it;
                Event event = (Event)eventLog.getEventForEventNumber(referenceFoundEntry.eventNumber);
                if (event != null)
                    for (int i = referenceFoundEntry.beginEntryIndex; i <= referenceFoundEntry.endEntryIndex; i++)
                        foundEventLogEntryCache.getEventLogEntries().add(event.getEventLogEntry(i));
            }
        }
        return foundEventLogEntryCache;
    }

    public final int getNumEventLogEntries() {
        ensureParsed();
        return eventLogEntries.size();
    }

    public final EventLogEntry getEventLogEntry(int index) {
        ensureParsed();
        return eventLogEntries.get(index);
    }

    protected final void ensureParsed() {
        if (snapshotEntry == null)
            parse(eventLog.getFileReader());
    }
}