package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.eventlog.EventLogTokenBasedEntry;
public class BubbleEntry extends EventLogTokenBasedEntry
{
    public int moduleId;
    public String text;

    public BubbleEntry() {
        this.chunk = null;
        moduleId = -1;
        text = null;
    }

    public BubbleEntry(IChunk chunk, int entryIndex) {
        this.chunk = chunk;
        this.entryIndex = entryIndex;
        moduleId = -1;
        text = null;
    }

    public int getModuleId() { return moduleId; }
    public String getText() { return text; }

    public void parse(String[] tokens, int numTokens) {
        moduleId = getIntToken(tokens, numTokens, "id", true, moduleId);
        text = getStringToken(tokens, numTokens, "txt", true, text);
    }

    public void print(OutputStream stream) {
        try {
            stream.write(("BU").getBytes());
            stream.write((" id " + String.valueOf(moduleId)).getBytes());
            stream.write((" txt " + qoute(text)).getBytes());
            stream.write(("\n").getBytes());
            stream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return 60; }

    public String getAsString() { return "BU"; }

    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        names.add("id");
        names.add("txt");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("id"))
            return String.valueOf(moduleId);
        else if (attribute.equals("txt"))
            return text;
        else
            return null;

    }

    public String getClassName() { return "BubbleEntry"; }
}

