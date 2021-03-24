package org.omnetpp.eventlog;

import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.entry.BeginSendEntry;
import org.omnetpp.eventlog.entry.EndSendEntry;
import org.omnetpp.eventlog.entry.EventEntry;
import org.omnetpp.eventlog.entry.ModuleDescriptionEntry;

public interface IEvent extends IChunk
{
    /**
     * Returns the corresponding eventlog.
     */
    public IEventLog getEventLog();

    /**
     * Returns the 'E' entry (line) corresponding to this event.
     */
    public EventEntry getEventEntry();

    /**
     * Returns the eventlog entry which describes the event's module.
     */
    public ModuleDescriptionEntry getModuleDescriptionEntry();

    public int getNumEventLogEntries();
    public int getNumEventLogMessages();
    public int getNumBeginSendEntries();
    public int getNumCustomEntries();

    /**
     * Returns the nth entry (line) for this event.
     */
    public EventLogEntry getEventLogEntry(int index);

    /**
     * Returns the nth message entry (line) for this event.
     */
    public EventLogMessageEntry getEventLogMessage(int index);

    // some of the data found in the 'E' entry (line), to get additional data query the entries
    public int getModuleId();
    public long getMessageId();
    public long getCauseEventNumber();

    /**
     * Returns the immediately preceding event or null if there is no such event.
     */
    public IEvent getPreviousEvent();

    /**
     * Returns the immediately following event or null if there is no such event.
     */
    public IEvent getNextEvent();

    /**
     * Returns the closest preceding event which caused this event by sending a message.
     */
    public IEvent getCauseEvent();
    public BeginSendEntry getCauseBeginSendEntry();
    public IMessageDependency getCause();
    public ArrayList<IMessageDependency> getCauses();
    public ArrayList<IMessageDependency> getConsequences();

    /**
     * Print all entries of this event.
     */
    public void print();
    public void print(OutputStream stream);
    public void print(OutputStream stream, boolean outputEventLogMessages);

    /**
     * Synchorizes state when the underlying log file changes (new events are appended).
     */
    public void synchronize(int change);

    /**
     * Returns the index of the begin send entry where the given message was sent.
     */
    public int findBeginSendEntryIndex(long messageId);

    /**
     * Returns true if the sent message is a self message.
     */
    public boolean isSelfMessage(BeginSendEntry beginSendEntry);
    /**
     * Returns true if the received message was a self message.
     */
    public boolean isSelfMessageProcessingEvent();
    /**
     * Retuns the corresponding end send or null if the message got deleted.
     */
    public EndSendEntry getEndSendEntry(BeginSendEntry beginSendEntry);

    public double getCachedTimelineCoordinateBegin();
    public void setCachedTimelineCoordinateBegin(double cachedTimelineCoordinateBegin);

    public double getCachedTimelineCoordinateEnd();
    public void setCachedTimelineCoordinateEnd(double cachedTimelineCoordinateEnd);

    public int getCachedTimelineCoordinateSystemVersion();
    public void setCachedTimelineCoordinateSystemVersion(int cachedTimelineCoordinateSystemVersion);
}
