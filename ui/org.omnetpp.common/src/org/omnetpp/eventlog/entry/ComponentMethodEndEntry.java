package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.eventlog.EventLogTokenBasedEntry;
public class ComponentMethodEndEntry extends EventLogTokenBasedEntry
{

    public ComponentMethodEndEntry() {
        this.chunk = null;
    }

    public ComponentMethodEndEntry(IChunk chunk, int entryIndex) {
        this.chunk = chunk;
        this.entryIndex = entryIndex;
    }


    public void parse(String[] tokens, int numTokens) {
    }

    public void print(OutputStream stream) {
        try {
            stream.write(("CME").getBytes());
            stream.write(("\n").getBytes());
            stream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return 60; }

    public String getAsString() { return "CME"; }

    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else
            return null;

    }

    public String getClassName() { return "ComponentMethodEndEntry"; }
}

