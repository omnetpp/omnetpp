package org.omnetpp.eventlog;

import java.io.OutputStream;

import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.entry.BeginSendEntry;

public abstract class EventBase
{
    protected EventBase previousEvent;
    protected EventBase nextEvent;

    // state for SequenceChart
    public double cachedTimelineCoordinateBegin;
    public double cachedTimelineCoordinateEnd;
    public int cachedTimelineCoordinateSystemVersion;

    public abstract int getNumEventLogEntries();
    public abstract EventLogEntry getEventLogEntry(int index);

    public double getCachedTimelineCoordinateBegin() {
        return cachedTimelineCoordinateBegin;
    }

    public void setCachedTimelineCoordinateBegin(double cachedTimelineCoordinateBegin) {
        this.cachedTimelineCoordinateBegin = cachedTimelineCoordinateBegin;
    }

    public double getCachedTimelineCoordinateEnd() {
        return cachedTimelineCoordinateEnd;
    }

    public void setCachedTimelineCoordinateEnd(double cachedTimelineCoordinateEnd) {
        this.cachedTimelineCoordinateEnd = cachedTimelineCoordinateEnd;
    }

    public int getCachedTimelineCoordinateSystemVersion() {
        return cachedTimelineCoordinateSystemVersion;
    }

    public void setCachedTimelineCoordinateSystemVersion(int cachedTimelineCoordinateSystemVersion) {
        this.cachedTimelineCoordinateSystemVersion = cachedTimelineCoordinateSystemVersion;
    }

    /**
     * Synchorizes state when the underlying log file changes (new events are appended).
     */
    public void synchronize(int change) {
        if (change != FileReader.FileChange.UNCHANGED) {
            switch (change) {
                case FileReader.FileChange.OVERWRITTEN:
                    clearInternalState();
                    break;
                case FileReader.FileChange.APPENDED:
                    break;
                default:
                    throw new RuntimeException("Unknown file change");
            }
        }
    }

    /**
     * Print all entries of this event.
     */
    public void print(OutputStream stream) {
        print(stream, true);
    }

    public void print() {
        print(System.out, true);
    }

    public abstract void print(OutputStream stream, boolean outputEventLogMessages);

    /**
     * Returns the index of the begin send entry where the given message was sent.
     */
    public int findBeginSendEntryIndex(long messageId) {
        // find the "BS" or "SA" line
        for (int beginSendEntryNumber = 0; beginSendEntryNumber < getNumEventLogEntries(); beginSendEntryNumber++) {
            BeginSendEntry beginSendEntry = getEventLogEntry(beginSendEntryNumber) instanceof BeginSendEntry ? (BeginSendEntry)getEventLogEntry(beginSendEntryNumber) : null;
            if (beginSendEntry != null && beginSendEntry.messageId == messageId)
                return beginSendEntryNumber;
        }
        return -1;
    }

    /**
     * Used to maintain the double linked list.
     */
    public static void linkEvents(EventBase previousEvent, EventBase nextEvent) {
        previousEvent.nextEvent = nextEvent;
        nextEvent.previousEvent = previousEvent;
    }

    public static void unlinkEvents(EventBase previousEvent, EventBase nextEvent) {
        previousEvent.nextEvent = null;
        nextEvent.previousEvent = null;
    }

    public static void unlinkNeighbourEvents(EventBase event) {
        if (event.previousEvent != null)
            unlinkEvents(event.previousEvent, event);
        if (event.nextEvent != null)
            unlinkEvents(event, event.nextEvent);
    }

    protected void clearInternalState() {
        nextEvent = null;
        previousEvent = null;
        cachedTimelineCoordinateBegin = -1;
        cachedTimelineCoordinateEnd = -1;
        cachedTimelineCoordinateSystemVersion = -1;
    }
}
