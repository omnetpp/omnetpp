package org.omnetpp.eventlog;

import java.io.OutputStream;
import java.util.TreeSet;

import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.entry.SimulationBeginEntry;
import org.omnetpp.eventlog.entry.SimulationEndEntry;

public interface IEventLog
{
    /**
     * Returns the file reader used to read in events.
     */
    public FileReader getFileReader();

    /**
     * Returns the eventlog entry cache for the whole eventlog.
     */
    public EventLogEntryCache getEventLogEntryCache();

    /**
     * Returns the number of events parsed so far.
     */
    public long getNumParsedEvents();

    /**
     * Returns the message names parsed so far.
     */
    public TreeSet<String> getMessageNames();

    /**
     * Returns the message class names parsed so far.
     */
    public TreeSet<String> getMessageClassNames();

    /**
     * Returns the eventlog entry describing the beginning of the simulation.
     */
    public SimulationBeginEntry getSimulationBeginEntry();

    /**
     * Returns the eventlog entry describing the end of the simulation.
     */
    public SimulationEndEntry getSimulationEndEntry();

    /**
     * Returns true if the eventlog does not contain any events.
     */
    public boolean isEmpty();

    /**
     * Returns the first event number or -1 if the log is empty.
     */
    public long getFirstEventNumber();

    /**
     * Returns the first simulation time or 0 if the log is empty.
     */
    public BigDecimal getFirstSimulationTime();

    /**
     * Returns the first event or null if the log is empty.
     */
    public IEvent getFirstEvent();

    /**
     * Returns the last simulation time or 0 if the log is empty.
     */
    public BigDecimal getLastSimulationTime();

    /**
     * Returns the last event number or -1 if the log is empty.
     */
    public long getLastEventNumber();

    /**
     * Returns the last event or null if the log is empty.
     */
    public IEvent getLastEvent();

    /**
     * Returns the requested event or null if there is no such event included in the log.
     * The given event number may not be included in the log.
     */
    public IEvent getEventForEventNumber(long eventNumber);
    public IEvent getEventForEventNumber(long eventNumber, MatchKind matchKind);
    public IEvent getEventForEventNumber(long eventNumber, MatchKind matchKind, boolean useCacheOnly);

    /**
     * Returns the requested event or null if there is no such event included in the log.
     * The given simulation time may not be included in the log.
     */
    public IEvent getEventForSimulationTime(BigDecimal simulationTime);
    public IEvent getEventForSimulationTime(BigDecimal simulationTime, MatchKind matchKind);
    public IEvent getEventForSimulationTime(BigDecimal simulationTime, MatchKind matchKind, boolean useCacheOnly);

    /**
     * Finds the closest eventlog entry containing the given text.
     */
    public EventLogEntry findEventLogEntry(EventLogEntry start, String search, boolean forward, boolean caseSensitive);

    /**
     * Returns the first index or null.
     */
    public Index getFirstIndex();

    /**
     * Returns the last index or null.
     */
    public Index getLastIndex();

    /**
     * Returns the index with the provided event or null if none found.
     * Returns the index that doesn't have the provided event number when searching for the next or the previous.
     */
    public Index getIndex(long eventNumber);
    public Index getIndex(long eventNumber, MatchKind matchKind);

    /**
     * Returns the first snapshot or null.
     */
    public Snapshot getFirstSnapshot();

    /**
     * Returns the last snapshot or null.
     */
    public Snapshot getLastSnapshot();

    /**
     * Returns the snapshot with the provided event number or null if none found.
     * Returns the snapshot that doesn't have the provided event number when searching for the next or the previous.
     */
    public Snapshot getSnapshot(long eventNumber);
    public Snapshot getSnapshot(long eventNumber, MatchKind matchKind);

    /**
     * Returns the approximate number of events present in the log.
     * This value may be less, equal or greater than the real number of events if there are many.
     */
    public long getApproximateNumberOfEvents();

    /**
     * Returns an event approximately at the given percentage in terms of eventlog size.
     */
    public IEvent getApproximateEventAt(double percentage);

    public double getApproximatePercentageForEventNumber(long eventNumber);

    /**
     * Prints the contents into specified file.
     * The given event numbers may not be present in the log.
     */
    public void print();
    public void print(OutputStream stream);
    public void print(OutputStream stream, long fromEventNumber);
    public void print(OutputStream stream, long fromEventNumber, long toEventNumber);
    public void print(OutputStream stream, long fromEventNumber, long toEventNumber, boolean outputEventLogMessages);

    /**
     * Returns the event at the given distance. 0 means the parameter event will be returned.
     */
    public IEvent getNeighbourEvent(IEvent event, long distance);

    /**
     * Returns true if the event with the given event number is included in the log.
     */
    public boolean isIncludedInLog(long eventNumber);

    public IEvent getFirstEventNotBeforeEventNumber(long eventNumber);

    public IEvent getLastEventNotAfterEventNumber(long eventNumber);

    public IEvent getFirstEventNotBeforeSimulationTime(BigDecimal simulationTime);

    public IEvent getLastEventNotAfterSimulationTime(BigDecimal simulationTime);

    public void setProgressMonitor(IProgressMonitor progressMonitor);

    /**
     * Set the minimum interval between progress callbacks for long running eventlog operations.
     */
    public void setProgressCallInterval(double seconds);

    /**
     * Progress notification. May be used to cancel long running operations.
     */
    public void progress();

    /**
     * Synchronizes state when the underlying log file has been changed.
     * Either new events were appended or the whole file has been overwritten.
     */
    public void synchronize(int change);
}
