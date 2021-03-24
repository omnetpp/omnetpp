package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
public class ConnectionDisplayStringEntry extends ConnectionReferenceEntry
{
    public String displayString;

    public ConnectionDisplayStringEntry() {
        displayString = null;
    }

    public ConnectionDisplayStringEntry(IChunk chunk, int entryIndex) {
        super(chunk, entryIndex);
        displayString = null;
    }

    public String getDisplayString() { return displayString; }

    public void parse(String[] tokens, int numTokens) {
        super.parse(tokens, numTokens);
        displayString = getStringToken(tokens, numTokens, "d", true, displayString);
    }

    public void print(OutputStream stream) {
        try {
            super.print(stream);
            stream.write((" d " + qoute(displayString)).getBytes());
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
        names.addAll(super.getAttributeNames());
        names.add("d");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("d"))
            return displayString;
        else
            return super.getAsString(attribute);

    }

    public String getClassName() { return "ConnectionDisplayStringEntry"; }
}

