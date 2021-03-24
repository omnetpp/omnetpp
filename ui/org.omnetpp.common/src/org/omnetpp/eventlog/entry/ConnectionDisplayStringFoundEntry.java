package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
public class ConnectionDisplayStringFoundEntry extends ConnectionDisplayStringEntry
{
    public long lastSeenEventNumber;
    public int lastSeenEntryIndex;

    public ConnectionDisplayStringFoundEntry() {
        lastSeenEventNumber = -1;
        lastSeenEntryIndex = -1;
    }

    public ConnectionDisplayStringFoundEntry(IChunk chunk, int entryIndex) {
        super(chunk, entryIndex);
        lastSeenEventNumber = -1;
        lastSeenEntryIndex = -1;
    }

    public long getLastSeenEventNumber() { return lastSeenEventNumber; }
    public int getLastSeenEntryIndex() { return lastSeenEntryIndex; }

    public void parse(String[] tokens, int numTokens) {
        super.parse(tokens, numTokens);
        lastSeenEventNumber = getEventNumberToken(tokens, numTokens, "#", true, lastSeenEventNumber);
        lastSeenEntryIndex = getIntToken(tokens, numTokens, "ei", true, lastSeenEntryIndex);
    }

    public void print(OutputStream stream) {
        try {
            stream.write(("CDF").getBytes());
            super.print(stream);
            stream.write((" # " + String.valueOf(lastSeenEventNumber)).getBytes());
            stream.write((" ei " + String.valueOf(lastSeenEntryIndex)).getBytes());
            stream.write(("\n").getBytes());
            stream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return 60; }

    public String getAsString() { return "CDF"; }

    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        names.addAll(super.getAttributeNames());
        names.add("#");
        names.add("ei");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("#"))
            return String.valueOf(lastSeenEventNumber);
        else if (attribute.equals("ei"))
            return String.valueOf(lastSeenEntryIndex);
        else
            return super.getAsString(attribute);

    }

    public String getClassName() { return "ConnectionDisplayStringFoundEntry"; }
}

