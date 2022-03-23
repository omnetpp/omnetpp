package org.omnetpp.eventlog;

import java.util.ArrayList;
import java.util.TreeMap;
import java.util.TreeSet;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.common.util.BigDecimal;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.entry.IndexEntry;
import org.omnetpp.eventlog.entry.MessageDescriptionEntry;
import org.omnetpp.eventlog.entry.SimulationBeginEntry;
import org.omnetpp.eventlog.entry.SimulationEndEntry;
import org.omnetpp.eventlog.entry.SnapshotEntry;

/**
 * Manages an eventlog file in memory and caches some events.
 */
public class EventLog extends EventLogIndex implements IEventLog
{
    protected long numParsedEvents = 0;
    protected long approximateNumberOfEvents = -1;

    protected long progressCallInterval;
    protected long lastProgressCall;
    protected IProgressMonitor progressMonitor;

    protected Event firstEvent;
    protected Event lastEvent;

    protected SimulationBeginEntry simulationBeginEntry;
    protected SimulationEndEntry simulationEndEntry;

    protected EventLogEntryCache eventLogEntryCache = new EventLogEntryCache(); // access all eventlog entries efficiently

    // TODO: move to eventlog entry cache
    protected final TreeSet<String> messageClassNames = new TreeSet<String>(); // message class names seen so far (see Event::parse)
    protected final TreeSet<String> messageNames = new TreeSet<String>(); // message names seen so far (see Event::parse)

    protected TreeMap<Long, Event> eventNumberToEventMap = new TreeMap<Long, Event>(); // all parsed events so far

    protected TreeMap<Long, Event> beginOffsetToEventMap = new TreeMap<Long, Event>(); // all parsed events so far
    protected TreeMap<Long, Event> endOffsetToEventMap = new TreeMap<Long, Event>(); // all parsed events so far

    protected TreeMap<Long, Index> eventNumberToIndexMap = new TreeMap<Long, Index>(); // all indices are parsed at once

    protected TreeMap<Long, Snapshot> eventNumberToSnapshotMap = new TreeMap<Long, Snapshot>(); // snapshots are parsed lazily

    public EventLog(FileReader reader) {
        super(reader);
        reader.setFileLocking(true);
        parseIndicesAndSnapshots();
        if (reader.getFileSize() < 10E+6)
            parseAll();
        else {
            parseBegin((long)1E+6);
            parseEnd((long)1E+6);
        }
    }

    public EventLogEntryCache getEventLogEntryCache() {
        return eventLogEntryCache;
    }

    @Override
    public void setProgressMonitor(IProgressMonitor progressMonitor)
    {
        this.progressMonitor = progressMonitor;
    }

    @Override
    public void setProgressCallInterval(double seconds) {
        progressCallInterval = (long)(seconds * 1000);
        lastProgressCall = System.currentTimeMillis();
    }

    @Override
    public void progress() {
        if (progressMonitor != null && lastProgressCall + progressCallInterval < System.currentTimeMillis()) {
            progressMonitor.worked(0);
            lastProgressCall = System.currentTimeMillis();
        }
    }

    /**
     * Returns the event exactly starting at the given offset or null if there is no such event.
     */
    public final Event getEventForBeginOffset(long beginOffset) {
        Assert.isTrue(beginOffset >= 0);
        var it = beginOffsetToEventMap.get(beginOffset);
        if (it != null)
            return it;
        else if (reader.getFileSize() != beginOffset) {
            Event event = new Event(this);
            parseEvent(event, beginOffset);
            cacheEvent(event);
            return event;
        }
        else {
            beginOffsetToEventMap.put(beginOffset, null);
            return null;
        }
    }

    /**
     * Returns the event exactly ending at the given offset or null if there is no such event.
     */
    public final Event getEventForEndOffset(long endOffset) {
        Assert.isTrue(endOffset >= 0);
        var it = endOffsetToEventMap.get(endOffset);
        if (it != null)
            return it;
        else {
            long beginOffset = getBeginOffsetForEndOffset(endOffset);
            if (beginOffset == -1) {
                endOffsetToEventMap.put(endOffset, null);
                return null;
            }
            else
                return getEventForBeginOffset(beginOffset);
        }
    }

    // IEventLog interface
    @Override
    public void synchronize(int change) {
        if (change != FileReader.FileChange.UNCHANGED) {
            super.synchronize(change);
            super.synchronize(change);
            switch (change) {
                case FileReader.FileChange.OVERWRITTEN:
                    clearInternalState();
                    parseIndicesAndSnapshots();
                    break;
                case FileReader.FileChange.APPENDED:
                    approximateNumberOfEvents = -1;
                    if (lastEvent != null) {
                        lastEvent.parseLines(reader, lastEvent.getEndOffset());
                        eventNumberToCacheEntryMap.remove(lastEvent.getEventNumber());
                        endOffsetToEventMap.remove(lastEvent.getEndOffset());
                        lastEventNumber = EventNumberKind.EVENT_NOT_YET_CALCULATED;
                        lastSimulationTime = BigDecimal.MINUS_ONE;
                        lastEventOffset = -1;
                        lastEvent = null;
                    }
                    for (Event it : eventNumberToEventMap.values())
                        it.synchronize(change);
                    parseIndicesAndSnapshots();
                    break;
                default:
                    throw new RuntimeException("Unknown file change");
            }
        }
    }

    @Override
    public FileReader getFileReader() {
        return reader;
    }

    @Override
    public long getNumParsedEvents() {
        return numParsedEvents;
    }

    @Override
    public TreeSet<String> getMessageNames() {
        return new TreeSet<String>(messageNames);
    }

    @Override
    public TreeSet<String> getMessageClassNames() {
        return new TreeSet<String>(messageClassNames);
    }

    @Override
    public SimulationBeginEntry getSimulationBeginEntry() {
        if (simulationBeginEntry == null) {
            reader.seekTo(0);
            String line = reader.getNextLine();
            if (line != null) {
                EventLogEntry eventLogEntry = (EventLogEntry)EventLogEntry.parseEntry(this, null, 0, reader.getCurrentLineStartOffset(), line, (int)reader.getCurrentLineLength());
                SimulationBeginEntry simulationBeginEntry = eventLogEntry instanceof SimulationBeginEntry ? (SimulationBeginEntry)eventLogEntry : null;
                if (simulationBeginEntry != null)
                    this.simulationBeginEntry = simulationBeginEntry;
            }
        }
        return simulationBeginEntry;
    }

    @Override
    public SimulationEndEntry getSimulationEndEntry() {
        if (simulationEndEntry == null) {
            reader.seekTo(reader.getFileSize());
            String line = reader.getPreviousLine();
            if (line != null) {
                EventLogEntry eventLogEntry = (EventLogEntry)EventLogEntry.parseEntry(this, null, 0, reader.getCurrentLineStartOffset(), line, (int)reader.getCurrentLineLength());
                SimulationEndEntry simulationEndEntry = eventLogEntry instanceof SimulationEndEntry ? (SimulationEndEntry)eventLogEntry : null;
                if (simulationEndEntry != null)
                    this.simulationEndEntry = simulationEndEntry;
            }
        }
        return simulationEndEntry;
    }

    @Override
    public Event getFirstEvent() {
        if (firstEvent == null) {
            long offset = getFirstEventOffset();
            if (offset != -1)
                firstEvent = getEventForBeginOffset(offset);
        }
        return firstEvent;
    }

    @Override
    public Event getLastEvent() {
        if (lastEvent == null) {
            long offset = getLastEventOffset();
            if (offset != -1)
                lastEvent = getEventForBeginOffset(offset);
        }
        return lastEvent;
    }

    @Override
    public Event getEventForEventNumber(long eventNumber, MatchKind matchKind, boolean useCacheOnly) {
        Assert.isTrue(eventNumber >= 0);
        if (matchKind == MatchKind.EXACT) {
            var it = eventNumberToEventMap.get(eventNumber);
            if (it != null)
                return it;
            else if (useCacheOnly)
                return null;
            else {
                // the following two are still faster than binary searching
                // but this may access the disk
                it = eventNumberToEventMap.get(eventNumber - 1);
                if (it != null) {
                    Event event = it.getNextEvent();
                    // the file might be filtered
                    return event != null && event.getEventNumber() == eventNumber ? event : null;
                }
                it = eventNumberToEventMap.get(eventNumber + 1);
                if (it != null) {
                    Event event = it.getPreviousEvent();
                    // the file might be filtered
                    return event != null && event.getEventNumber() == eventNumber ? event : null;
                }
            }
        }
        if (useCacheOnly)
            return null;
        else {
            long offset = getOffsetForEventNumber(eventNumber, matchKind);
            if (offset == -1)
                return null;
            else
                return getEventForBeginOffset(offset);
        }
    }

    @Override
    public Event getEventForSimulationTime(BigDecimal simulationTime, MatchKind matchKind, boolean useCacheOnly) {
        if (useCacheOnly)
            return null;
        else {
            Assert.isTrue(simulationTime.greaterOrEqual(BigDecimal.ZERO));
            long offset = getOffsetForSimulationTime(simulationTime, matchKind);
            if (offset == -1)
                return null;
            else
                return getEventForBeginOffset(offset);
        }
    }

    @Override
    public EventLogEntry findEventLogEntry(EventLogEntry start, String search, boolean forward, boolean caseSensitive) {
        if (search.isEmpty())
            return forward ? start.getNextEventLogEntry() : start.getPreviousEventLogEntry();
        else {
            String line;
            long eventBeginOffset = -1;
            long matchOffset = -1;
            Event matchEvent = null;
            reader.seekTo(start.getOffset());
            if (forward) {
                reader.getNextLine();
                while ((line = reader.getNextLine()) != null) {
                    if (line.charAt(0) == 'E' && line.charAt(1) == ' ')
                        eventBeginOffset = reader.getCurrentLineStartOffset();
                    if (caseSensitive ? StringUtils.contains(line, search) : StringUtils.containsIgnoreCase(line, search)) {
                        long currentLineStartOffset = reader.getCurrentLineStartOffset();
                        Event event = eventBeginOffset == -1 ? start.getEvent() : getEventForBeginOffset(eventBeginOffset);
                        if (event != null && event.getBeginOffset() <= currentLineStartOffset && currentLineStartOffset <= event.getEndOffset()) {
                            matchEvent = event;
                            matchOffset = currentLineStartOffset;
                            break;
                        }
                    }
                    progress();
                }
            }
            else {
                while ((line = reader.getPreviousLine()) != null) {
                    if (caseSensitive ? StringUtils.contains(line, search) : StringUtils.containsIgnoreCase(line, search)) {
                        long currentLineStartOffset = reader.getCurrentLineStartOffset();
                        Event event = eventBeginOffset == -1 ? (start.getEntryIndex() == 0 ? start.getEvent().getPreviousEvent() : start.getEvent()) : getEventForBeginOffset(eventBeginOffset).getPreviousEvent();
                        if (event != null && event.getBeginOffset() <= currentLineStartOffset && currentLineStartOffset <= event.getEndOffset()) {
                            matchEvent = event;
                            matchOffset = currentLineStartOffset;
                            break;
                        }
                    }
                    if (line.charAt(0) == 'E' && line.charAt(1) == ' ')
                        eventBeginOffset = reader.getCurrentLineStartOffset();
                    progress();
                }
            }
            if (matchEvent != null && matchOffset != -1) {
                for (int i = 0; i < matchEvent.getNumEventLogEntries(); i++) {
                    EventLogEntry eventLogEntry = matchEvent.getEventLogEntry(i);
                    if (eventLogEntry.getOffset() == matchOffset)
                        return eventLogEntry;
                }
            }
            return null;
        }
    }

    @Override
    public Index getFirstIndex() {
        return eventNumberToIndexMap.isEmpty() ? null : eventNumberToIndexMap.firstEntry().getValue();
    }

    @Override
    public Index getLastIndex() {
        return eventNumberToIndexMap.isEmpty() ? null : eventNumberToIndexMap.lastEntry().getValue();
    }

    @Override
    public Index getIndex(long eventNumber, MatchKind matchKind) {
        switch (matchKind) {
            case EXACT:
                return eventNumberToIndexMap.get(eventNumber);
            case FIRST_OR_PREVIOUS:
            case LAST_OR_PREVIOUS: {
                var entry = eventNumberToIndexMap.ceilingEntry(eventNumber);
                if (entry == null || entry == eventNumberToIndexMap.firstEntry())
                    return null;
                else if (!eventNumberToIndexMap.isEmpty())
                    return eventNumberToIndexMap.lowerEntry(eventNumber).getValue();
            }
            case FIRST_OR_NEXT:
            case LAST_OR_NEXT: {
                var entry = eventNumberToIndexMap.floorEntry(eventNumber);
                if (entry != null && entry.getKey() == eventNumber)
                    return eventNumberToIndexMap.higherEntry(eventNumber).getValue();
                else
                    return null;
            }
            default:
                throw new RuntimeException("Unknown match kind");
        }
    }

    @Override
    public Snapshot getFirstSnapshot() {
        return eventNumberToSnapshotMap.isEmpty() ? null : eventNumberToSnapshotMap.firstEntry().getValue();
    }

    @Override
    public Snapshot getLastSnapshot() {
        return eventNumberToSnapshotMap.isEmpty() ? null : eventNumberToSnapshotMap.lastEntry().getValue();
    }

    @Override
    public Snapshot getSnapshot(long eventNumber, MatchKind matchKind) {
        switch (matchKind) {
            case EXACT:
                return eventNumberToSnapshotMap.get(eventNumber);
            case FIRST_OR_PREVIOUS:
            case LAST_OR_PREVIOUS: {
                var entry = eventNumberToSnapshotMap.lowerEntry(eventNumber);
                if (entry == eventNumberToSnapshotMap.firstEntry())
                    return null;
                else if (!eventNumberToSnapshotMap.isEmpty())
                    return eventNumberToSnapshotMap.lowerEntry(eventNumber).getValue();
            }
            case FIRST_OR_NEXT:
            case LAST_OR_NEXT: {
                var entry = eventNumberToSnapshotMap.floorEntry(eventNumber);
                if (entry != null && entry.getKey() == eventNumber)
                    return eventNumberToSnapshotMap.higherEntry(eventNumber).getValue();
                else
                    return null;
            }
            default:
                throw new RuntimeException("Unknown match kind");
        }
    }

    @Override
    public long getApproximateNumberOfEvents() {
        if (approximateNumberOfEvents == -1) {
            Event firstEvent = getFirstEvent();
            Event lastEvent = getLastEvent();

            if (firstEvent == null)
                approximateNumberOfEvents = 0;
            else {
                long beginOffset = firstEvent.getBeginOffset();
                long endOffset = lastEvent.getEndOffset();
                int sum = 0;
                int count = 0;
                int eventCount = 100;
                for (int i = 0; i < eventCount; i++) {
                    if (firstEvent != null) {
                        sum += firstEvent.getEndOffset() - firstEvent.getBeginOffset();
                        count++;
                        firstEvent = firstEvent.getNextEvent();
                    }
                    if (lastEvent != null) {
                        sum += lastEvent.getEndOffset() - lastEvent.getBeginOffset();
                        count++;
                        lastEvent = lastEvent.getPreviousEvent();
                    }
                }
                double average = (double)sum / count;
                approximateNumberOfEvents = (int)((endOffset - beginOffset) / average);
            }
        }
        return approximateNumberOfEvents;
    }

    @Override
    public Event getApproximateEventAt(double percentage) {
        Event firstEvent = getFirstEvent();
        Event lastEvent = getLastEvent();
        if (firstEvent == null)
            return null;
        else {
            long beginOffset = firstEvent.getBeginOffset();
            long endOffset = lastEvent.getEndOffset();
            long offset = beginOffset + (long)((endOffset - beginOffset) * percentage);
            ReadToEventLineResult result = new ReadToEventLineResult();
            Event event = null;
            if (readToEventLine(false, offset, result))
                event = getEventForBeginOffset(result.lineBeginOffset);
            else
                event = getFirstEvent();
            Assert.isTrue(event != null);
            return event;
        }
    }

    protected final void parseEvent(Event event, long beginOffset) {
        event.parse(reader, beginOffset);
        cacheEntry(event.getEventNumber(), event.getSimulationTime(), event.getBeginOffset(), event.getEndOffset());
        cacheEventLogEntries(event);
        numParsedEvents++;
        Assert.isTrue(event.getEventEntry() != null);
    }

    protected final void cacheEvent(Event event) {
        long eventNumber = event.getEventNumber();
        Assert.isTrue(firstEvent == null || eventNumber >= firstEvent.getEventNumber());
        Assert.isTrue(lastEvent == null || eventNumber <= lastEvent.getEventNumber());
        Assert.isTrue(!eventNumberToEventMap.containsKey(eventNumber));
        eventNumberToEventMap.put(eventNumber, event);
        beginOffsetToEventMap.put(event.getBeginOffset(), event);
        endOffsetToEventMap.put(event.getEndOffset(), event);
    }

    protected final void cacheEventLogEntries(Event event) {
        for (int i = 0; i < event.getNumEventLogEntries(); i++)
            cacheEventLogEntry(event.getEventLogEntry(i));
    }

    protected final void cacheEventLogEntry(EventLogEntry eventLogEntry) {
        eventLogEntryCache.cacheEventLogEntry(eventLogEntry);
        // collect message description entries
        MessageDescriptionEntry messageDescriptionEntry = eventLogEntry instanceof MessageDescriptionEntry ? (MessageDescriptionEntry)eventLogEntry : null;
        if (messageDescriptionEntry != null) {
            messageNames.add(messageDescriptionEntry.messageName);
            messageClassNames.add(messageDescriptionEntry.messageClassName);
        }
    }

    protected final void clearInternalState() {
        super.clearInternalState();
        numParsedEvents = 0;
        approximateNumberOfEvents = -1;
        progressCallInterval = 100;
        lastProgressCall = -1;
        firstEvent = null;
        lastEvent = null;
        messageNames.clear();
        messageClassNames.clear();
        eventLogEntryCache.clearCache();
        simulationBeginEntry = null;
        simulationEndEntry = null;
        eventNumberToEventMap.clear();
        eventNumberToIndexMap.clear();
        eventNumberToSnapshotMap.clear();
        beginOffsetToEventMap.clear();
        endOffsetToEventMap.clear();
    }

    protected final void parseIndicesAndSnapshots() {
        // this function is optimized for performance
        // the idea is to read indices backwards starting from the end of file
        // file offsets must be shifted to be able to read truncated files
        TreeMap<Long, Snapshot> snapshotFileOffsetsToSnapshotMap = new TreeMap<Long, Snapshot>();
        reader.seekTo(reader.getFileSize());
        String line = reader.getPreviousLine();
        ArrayList<Index> indices = new ArrayList<Index>();
        while (line != null) {
            EventLogEntry eventLogEntry = (EventLogEntry)EventLogEntry.parseEntry(this, null, 0, reader.getCurrentLineStartOffset(), line, (int)reader.getCurrentLineLength());
            SnapshotEntry snapshotEntry = eventLogEntry instanceof SnapshotEntry ? (SnapshotEntry)eventLogEntry : null;
            IndexEntry indexEntry = eventLogEntry instanceof IndexEntry ? (IndexEntry)eventLogEntry : null;
            if (indexEntry != null) {
//                System.out.println("Index: " + indexEntry.eventNumber);
                // check if we already have this index
                var it = eventNumberToIndexMap.get(indexEntry.eventNumber);
                if (it != null) {
                    indices.add(it);
                    break;
                }
                // jump to previous snapshot entry
                Snapshot snapshot = null;
                var jt = snapshotFileOffsetsToSnapshotMap.get(indexEntry.previousSnapshotFileOffset);
                if (indexEntry.previousSnapshotFileOffset != -1 && jt == null) {
                    long realFileOffset = indexEntry.getOffset() - indexEntry.fileOffset + indexEntry.previousSnapshotFileOffset;
                    if (realFileOffset >= 0) {
                        reader.seekTo(indexEntry.getOffset() - indexEntry.fileOffset + indexEntry.previousSnapshotFileOffset);
                        line = reader.getNextLine();
                        snapshotEntry = (SnapshotEntry)EventLogEntry.parseEntry(this, null, 0, reader.getCurrentLineStartOffset(), line, (int)reader.getCurrentLineLength());
                        Assert.isTrue(snapshotEntry != null);
                        var kt = eventNumberToSnapshotMap.get(snapshotEntry.getEventNumber());
                        if (kt == null) {
                            snapshot = new Snapshot(this, reader.getCurrentLineStartOffset());
                            eventNumberToSnapshotMap.put(snapshotEntry.getEventNumber(), snapshot);
                        }
                        else
                            snapshot = kt;
                        snapshotFileOffsetsToSnapshotMap.put(indexEntry.previousSnapshotFileOffset, snapshot);
                        snapshotEntry = null;
                    }
                }
                else if (jt != null)
                    snapshot = jt;
                // create index
                Index index = new Index(this, indexEntry.getOffset(), snapshot);
                indices.add(index);
                eventNumberToIndexMap.put(indexEntry.eventNumber, index);
                // jump to previous index entry
                long realFileOffset = index.getBeginOffset() - indexEntry.fileOffset + indexEntry.previousIndexFileOffset;
                if (realFileOffset >= 0) {
                    reader.seekTo(realFileOffset);
                    line = reader.getNextLine();
                }
                else
                    line = null;
            }
            else if (snapshotEntry != null) {
                Snapshot snapshot = new Snapshot(this, reader.getCurrentLineStartOffset());
                eventNumberToSnapshotMap.put(snapshotEntry.getEventNumber(), snapshot);
                line = reader.getPreviousLine();
            }
            else
                line = reader.getPreviousLine();
            eventLogEntry = null;
            progress();
        }
        for (int i = 1; i < (int)indices.size(); i++) {
            // indices is in reverse order
            Index.linkIndices(indices.get(i), indices.get(i - 1));
        }
    }

    protected final void parseBegin(long limit) {
        long initialReadBytes = reader.getNumReadBytes();
        Snapshot snapshot = getFirstSnapshot();
        if (snapshot != null)
            snapshot.ensureParsed();
        Index index = getFirstIndex();
        if (index != null)
            index.ensureParsed();
        IEvent event = getFirstEvent();
        while (event != null) {
            event = event.getNextEvent();
            if (reader.getNumReadBytes() - initialReadBytes > limit)
                break;
        }
    }

    protected final void parseEnd(long limit) {
        long initialReadBytes = reader.getNumReadBytes();
        Snapshot snapshot = getLastSnapshot();
        if (snapshot != null)
            snapshot.ensureParsed();
        Index index = getLastIndex();
        if (index != null)
            index.ensureParsed();
        IEvent event = getLastEvent();
        while (event != null) {
            event = event.getPreviousEvent();
            if (reader.getNumReadBytes() - initialReadBytes > limit)
                break;
        }
    }

    protected final void parseAll() {
        IEvent event = getFirstEvent();
        while (event != null)
            event = event.getNextEvent();
        for (Snapshot snapshot : eventNumberToSnapshotMap.values())
            snapshot.ensureParsed();
        for (Index index : eventNumberToIndexMap.values())
            index.ensureParsed();
    }
}
