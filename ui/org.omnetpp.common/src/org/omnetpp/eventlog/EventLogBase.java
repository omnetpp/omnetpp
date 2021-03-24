package org.omnetpp.eventlog;

import java.io.IOException;
import java.io.OutputStream;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.entry.SimulationBeginEntry;
import org.omnetpp.eventlog.entry.SimulationEndEntry;

public abstract class EventLogBase
{
    // Remembers the last IEvent returned by getNeighbourEvent so that subsequent calls might return much faster.
    protected long lastNeighbourEventNumber = -1;
    protected IEvent lastNeighbourEvent;

    /**
     * Synchronizes state when the underlying log file has been changed.
     * Either new events were appended or the whole file has been overwritten.
     */
    public void synchronize(int change) {
        if (change != FileReader.FileChange.UNCHANGED) {
            switch (change) {
                case FileReader.FileChange.OVERWRITTEN:
                case FileReader.FileChange.APPENDED:
                    clearInternalState();
                    break;
                default:
                    throw new RuntimeException("Unknown file change");
            }
        }
    }

    public abstract SimulationBeginEntry getSimulationBeginEntry();

    public abstract SimulationEndEntry getSimulationEndEntry();

    /**
     * Returns true if the eventlog does not contain any events.
     */
    public boolean isEmpty() {
        return getFirstEvent() == null;
    }

    /**
     * Returns the first event or null if the log is empty.
     */
    public abstract IEvent getFirstEvent();

    /**
     * Returns the last event or null if the log is empty.
     */
    public abstract IEvent getLastEvent();

    /**
     * Returns the requested event or null if there is no such event included in the log.
     * The given event number may not be included in the log.
     */
    public IEvent getEventForEventNumber(long eventNumber, MatchKind matchKind) {
        return getEventForEventNumber(eventNumber, matchKind, false);
    }

    public IEvent getEventForEventNumber(long eventNumber) {
        return getEventForEventNumber(eventNumber, MatchKind.EXACT, false);
    }

    public abstract IEvent getEventForEventNumber(long eventNumber, MatchKind matchKind, boolean useCacheOnly);

    /**
     * Returns the requested event or null if there is no such event included in the log.
     * The given simulation time may not be included in the log.
     */
    public IEvent getEventForSimulationTime(BigDecimal simulationTime, MatchKind matchKind) {
        return getEventForSimulationTime(simulationTime, matchKind, false);
    }

    public IEvent getEventForSimulationTime(BigDecimal simulationTime) {
        return getEventForSimulationTime(simulationTime, MatchKind.EXACT, false);
    }

    public abstract IEvent getEventForSimulationTime(BigDecimal simulationTime, MatchKind matchKind, boolean useCacheOnly);

    /**
     * Returns the index with the provided event or null if none found.
     * Returns the index that doesn't have the provided event number when searching for the next or the previous.
     */
    public Index getIndex(long eventNumber) {
        return getIndex(eventNumber, MatchKind.EXACT);
    }

    public abstract Index getIndex(long eventNumber, MatchKind matchKind);

    /**
     * Returns the snapshot with the provided event number or null if none found.
     * Returns the snapshot that doesn't have the provided event number when searching for the next or the previous.
     */
    public Snapshot getSnapshot(long eventNumber) {
        return getSnapshot(eventNumber, MatchKind.EXACT);
    }

    public abstract Snapshot getSnapshot(long eventNumber, MatchKind matchKind);

    /**
     * Prints the contents into specified file.
     * The given event numbers may not be present in the log.
     */
    public void print(OutputStream stream, long fromEventNumber, long toEventNumber) {
        print(stream, fromEventNumber, toEventNumber, true);
    }

    public void print(OutputStream stream, long fromEventNumber) {
        print(stream, fromEventNumber, -1, true);
    }

    public void print(OutputStream stream) {
        print(stream, -1, -1, true);
    }

    public void print() {
        print(System.out, -1, -1, true);
    }

    public void print(OutputStream stream, long fromEventNumber, long toEventNumber, boolean outputEventLogMessages) {
        try {
            SimulationBeginEntry simulationBeginEntry = getSimulationBeginEntry();
            if (simulationBeginEntry != null) {
                simulationBeginEntry.print(stream);
                stream.write("\n".getBytes());
            }
            IEvent event = fromEventNumber == -1 ? getFirstEvent() : getFirstEventNotBeforeEventNumber(fromEventNumber);
            while (event != null && (toEventNumber == -1 || event.getEventNumber() <= toEventNumber)) {
                Snapshot snapshot = getSnapshot(event.getEventNumber());
                if (snapshot != null) {
                    snapshot.print(stream);
                    stream.write("\n".getBytes());
                }
                Index index = getIndex(event.getEventNumber());
                if (index != null) {
                    index.print(stream);
                    stream.write("\n".getBytes());
                }
                event.print(stream, outputEventLogMessages);
                event = event.getNextEvent();
                if (event != null)
                    stream.write("\n".getBytes());
            }
            SimulationEndEntry simulationEndEntry = getSimulationEndEntry();
            if (simulationEndEntry != null) {
                stream.write("\n".getBytes());
                simulationEndEntry.print(stream);
            }
        }
        catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    /**
     * Returns the event at the given distance. 0 means the parameter event will be returned.
     */

    public IEvent getNeighbourEvent(IEvent event, long distance)
    {
        Assert.isTrue(event != null);
        long neighbourEventNumber = event.getEventNumber() + distance;
        if (lastNeighbourEvent != null && lastNeighbourEventNumber != -1 && Math.abs(neighbourEventNumber - lastNeighbourEventNumber) < Math.abs(distance))
            return getNeighbourEvent(lastNeighbourEvent, neighbourEventNumber - lastNeighbourEventNumber);
        while (event != null && distance != 0) {
            if (distance > 0) {
                distance--;
                event = event.getNextEvent();
            }
            else {
                distance++;
                event = event.getPreviousEvent();
            }
        }
        lastNeighbourEventNumber = neighbourEventNumber;
        lastNeighbourEvent = (IEvent)event;
        return lastNeighbourEvent;
    }

    /**
     * Returns true if the event with the given event number is included in the log.
     */
    public boolean isIncludedInLog(long eventNumber) {
        return getEventForEventNumber(eventNumber) != null;
    }

    public IEvent getFirstEventNotBeforeEventNumber(long eventNumber) {
        return getEventForEventNumber(eventNumber, MatchKind.LAST_OR_NEXT);
    }

    public IEvent getLastEventNotAfterEventNumber(long eventNumber) {
        return getEventForEventNumber(eventNumber, MatchKind.FIRST_OR_PREVIOUS);
    }

    public IEvent getFirstEventNotBeforeSimulationTime(BigDecimal simulationTime) {
        return getEventForSimulationTime(simulationTime, MatchKind.LAST_OR_NEXT);
    }

    public IEvent getLastEventNotAfterSimulationTime(BigDecimal simulationTime) {
        return getEventForSimulationTime(simulationTime, MatchKind.FIRST_OR_PREVIOUS);
    }

    public double getApproximatePercentageForEventNumber(long eventNumber) {
        IEvent firstEvent = getFirstEvent();
        IEvent lastEvent = getLastEvent();
        IEvent event = getEventForEventNumber(eventNumber);
        if (firstEvent == null || firstEvent == lastEvent)
            return 0.0;
        else if (event == null)
            return 0.5;
        else {
            long beginOffset = firstEvent.getBeginOffset();
            long endOffset = lastEvent.getBeginOffset();
            double percentage = (double)(event.getBeginOffset() - beginOffset) / (endOffset - beginOffset);
            Assert.isTrue(0.0 <= percentage && percentage <= 1.0);
            return percentage;
        }
    }

    protected void clearInternalState() {
        lastNeighbourEventNumber = -1;
        lastNeighbourEvent = null;
    }
}
