package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.eventlog.EventLogTokenBasedEntry;
import org.omnetpp.common.engine.BigDecimal;

public class IndexEntry extends EventLogTokenBasedEntry
{
    public long fileOffset;
    public long previousIndexFileOffset;
    public long previousSnapshotFileOffset;
    public long eventNumber;
    public BigDecimal simulationTime;

    public IndexEntry() {
        this.chunk = null;
        fileOffset = -1;
        previousIndexFileOffset = -1;
        previousSnapshotFileOffset = -1;
        eventNumber = -1;
        simulationTime = BigDecimal.getNaN();
    }

    public IndexEntry(IChunk chunk, int entryIndex) {
        this.chunk = chunk;
        this.entryIndex = entryIndex;
        fileOffset = -1;
        previousIndexFileOffset = -1;
        previousSnapshotFileOffset = -1;
        eventNumber = -1;
        simulationTime = BigDecimal.getNaN();
    }

    public long getFileOffset() { return fileOffset; }
    public long getPreviousIndexFileOffset() { return previousIndexFileOffset; }
    public long getPreviousSnapshotFileOffset() { return previousSnapshotFileOffset; }
    public long getEventNumber() { return eventNumber; }
    public BigDecimal getSimulationTime() { return simulationTime; }

    public void parse(String[] tokens, int numTokens) {
        fileOffset = getInt64Token(tokens, numTokens, "f", true, fileOffset);
        previousIndexFileOffset = getInt64Token(tokens, numTokens, "i", true, previousIndexFileOffset);
        previousSnapshotFileOffset = getInt64Token(tokens, numTokens, "s", true, previousSnapshotFileOffset);
        eventNumber = getEventNumberToken(tokens, numTokens, "#", true, eventNumber);
        simulationTime = getSimtimeToken(tokens, numTokens, "t", true, simulationTime);
    }

    public void print(OutputStream stream) {
        try {
            stream.write(("I").getBytes());
            stream.write((" f " + String.valueOf(fileOffset)).getBytes());
            stream.write((" i " + String.valueOf(previousIndexFileOffset)).getBytes());
            stream.write((" s " + String.valueOf(previousSnapshotFileOffset)).getBytes());
            stream.write((" # " + String.valueOf(eventNumber)).getBytes());
            stream.write((" t " + simulationTime.toString()).getBytes());
            stream.write(("\n").getBytes());
            stream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return 60; }

    public String getAsString() { return "I"; }

    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        names.add("f");
        names.add("i");
        names.add("s");
        names.add("#");
        names.add("t");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("f"))
            return String.valueOf(fileOffset);
        else if (attribute.equals("i"))
            return String.valueOf(previousIndexFileOffset);
        else if (attribute.equals("s"))
            return String.valueOf(previousSnapshotFileOffset);
        else if (attribute.equals("#"))
            return String.valueOf(eventNumber);
        else if (attribute.equals("t"))
            return simulationTime.toString();
        else
            return null;

    }

    public String getClassName() { return "IndexEntry"; }
}

