package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.eventlog.EventLogTokenBasedEntry;
public class CustomReferenceEntry extends EventLogTokenBasedEntry
{
    public String type;
    public long key;

    public CustomReferenceEntry() {
        this.chunk = null;
        type = null;
        key = -1;
    }

    public CustomReferenceEntry(IChunk chunk, int entryIndex) {
        this.chunk = chunk;
        this.entryIndex = entryIndex;
        type = null;
        key = -1;
    }

    public String getType() { return type; }
    public long getKey() { return key; }

    public void parse(String[] tokens, int numTokens) {
        type = getStringToken(tokens, numTokens, "t", true, type);
        key = getLongToken(tokens, numTokens, "k", true, key);
    }

    public void print(OutputStream stream) {
        try {
            stream.write((" t " + qoute(type)).getBytes());
            stream.write((" k " + String.valueOf(key)).getBytes());
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
        names.add("t");
        names.add("k");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("t"))
            return type;
        else if (attribute.equals("k"))
            return String.valueOf(key);
        else
            return null;

    }

    public String getClassName() { return "CustomReferenceEntry"; }
}

