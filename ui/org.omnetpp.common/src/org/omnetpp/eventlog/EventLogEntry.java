package org.omnetpp.eventlog;

import java.io.OutputStream;
import java.util.ArrayList;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.engine.LineTokenizer;
import org.omnetpp.common.engine.PStringVector;
import org.omnetpp.common.engineext.IMatchableObject;

/**
 * Base class for all kind of eventlog entries.
 * An entry is represented by a single line in the log file.
 */
public abstract class EventLogEntry implements IMatchableObject
{
    public int contextModuleId = -1;
    public int level = -1; // indent level (method call depth)

    protected long offset = -1;
    protected IChunk chunk = null; // back reference to event/snapshot/index
    protected int entryIndex = -1; // index within the event
    protected static String buffer = new String(new char[128]); // non-thread safe buffer temporarily used to parse eventlog entries
    protected static String currentLine;
    protected static int currentLineLength;

    public abstract ArrayList<String> getAttributeNames();

    public abstract String getAsString();
    public abstract String getAsString(String attribute);

    public abstract void print(OutputStream stream);

    public abstract int getClassIndex();
    public abstract String getClassName();

    public int getContextModuleId() {
        return contextModuleId;
    }

    public int getLevel() {
        return level;
    }

    public final long getOffset() {
        return offset;
    }

    public final IChunk getChunk() {
        return chunk;
    }

    public final Event getEvent() {
        return chunk instanceof Event ? (Event)chunk : null;
    }

    public long getEventNumber() {
        return chunk.getEventNumber();
    }

    public BigDecimal getSimulationTime() {
        return chunk.getSimulationTime();
    }

    public final int getEntryIndex() {
        return entryIndex;
    }

    public final EventLogEntry getPreviousEventLogEntry() {
        Event event = getEvent();
        if (entryIndex == 0) {
            IEvent previousEvent = event.getPreviousEvent();
            return previousEvent != null ? previousEvent.getEventLogEntry(previousEvent.getNumEventLogEntries() - 1) : null;
        }
        else
            return event.getEventLogEntry(entryIndex - 1);
    }

    public final EventLogEntry getNextEventLogEntry() {
        Event event = getEvent();
        if (entryIndex == event.getNumEventLogEntries() - 1) {
            IEvent nextEvent = event.getNextEvent();
            return nextEvent != null ? nextEvent.getEventLogEntry(0) : null;
        }
        else
            return event.getEventLogEntry(entryIndex + 1);
    }

    public static EventLogEntry parseEntry(EventLog eventLog, IChunk chunk, int entryIndex, long offset, String line, int length)
    {
        try {
            currentLine = line;
            currentLineLength = length;

            if (line.length() == 0)
                return null;
            else if (line.charAt(0) == '-') {
                EventLogMessageEntry eventLogMessage = new EventLogMessageEntry(chunk, entryIndex);
                eventLogMessage.parse(line, length);
                eventLogMessage.offset = offset;
                return eventLogMessage;
            }
            else {
                Assert.isTrue(entryIndex >= 0);
                LineTokenizer tokenizer = new LineTokenizer();
                tokenizer.tokenize(line, length);
                PStringVector tokensVector = tokenizer.tokensVector();
                String[] tokens = new String[tokenizer.numTokens()];
                for (int i = 0; i < tokenizer.numTokens(); i++)
                    tokens[i] = tokensVector.get(i);
                EventLogEntry eventLogEntry = EventLogEntryFactory.parseEntry(chunk, entryIndex, tokens, tokenizer.numTokens());
                if (eventLogEntry != null)
                    eventLogEntry.offset = offset;
                return eventLogEntry;
            }
        }
        catch (RuntimeException e) {
            String fileName = eventLog.getFileReader().getFileName();
            if (chunk != null)
                throw new RuntimeException("Error parsing elog file " + fileName + " at line " + entryIndex + " of event #" + chunk.getEventNumber() + " near file offset " + offset, e);
            else
                throw new RuntimeException("Error parsing elog file " + fileName + " near file offset " + offset, e);
        }
    }

    public static long parseEventNumber(String str) {
        return Long.valueOf(str);
    }

    public static BigDecimal parseSimulationTime(String str) {
        return BigDecimal.parse(str);
    }
}
