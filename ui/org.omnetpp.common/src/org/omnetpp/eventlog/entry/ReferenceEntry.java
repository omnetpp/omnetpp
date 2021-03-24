package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.eventlog.EventLogTokenBasedEntry;
public class ReferenceEntry extends EventLogTokenBasedEntry
{
    public long eventNumber;
    public int beginEntryIndex;
    public int endEntryIndex;

    public ReferenceEntry() {
        this.chunk = null;
        eventNumber = -1;
        beginEntryIndex = -1;
        endEntryIndex = -1;
    }

    public ReferenceEntry(IChunk chunk, int entryIndex) {
        this.chunk = chunk;
        this.entryIndex = entryIndex;
        eventNumber = -1;
        beginEntryIndex = -1;
        endEntryIndex = -1;
    }

    public long getEventNumber() { return eventNumber; }
    public int getBeginEntryIndex() { return beginEntryIndex; }
    public int getEndEntryIndex() { return endEntryIndex; }

    public void parse(String[] tokens, int numTokens) {
        eventNumber = getEventNumberToken(tokens, numTokens, "#", true, eventNumber);
        beginEntryIndex = getIntToken(tokens, numTokens, "b", true, beginEntryIndex);
        endEntryIndex = getIntToken(tokens, numTokens, "e", true, endEntryIndex);
    }

    public void print(OutputStream stream) {
        try {
            stream.write((" # " + String.valueOf(eventNumber)).getBytes());
            stream.write((" b " + String.valueOf(beginEntryIndex)).getBytes());
            stream.write((" e " + String.valueOf(endEntryIndex)).getBytes());
            stream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return 60; }

    public String getAsString() { return "abstract"; }

    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        names.add("#");
        names.add("b");
        names.add("e");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("#"))
            return String.valueOf(eventNumber);
        else if (attribute.equals("b"))
            return String.valueOf(beginEntryIndex);
        else if (attribute.equals("e"))
            return String.valueOf(endEntryIndex);
        else
            return null;

    }

    public String getClassName() { return "ReferenceEntry"; }
}

