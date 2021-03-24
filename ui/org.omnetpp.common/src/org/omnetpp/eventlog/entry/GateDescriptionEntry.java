package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
public class GateDescriptionEntry extends GateReferenceEntry
{
    public String name;
    public int index;
    public boolean isOutput;

    public GateDescriptionEntry() {
        name = null;
        index = -1;
        isOutput = false;
    }

    public GateDescriptionEntry(IChunk chunk, int entryIndex) {
        super(chunk, entryIndex);
        name = null;
        index = -1;
        isOutput = false;
    }

    public String getName() { return name; }
    public int getIndex() { return index; }
    public boolean getIsOutput() { return isOutput; }

    public void parse(String[] tokens, int numTokens) {
        super.parse(tokens, numTokens);
        name = getStringToken(tokens, numTokens, "n", true, name);
        index = getIntToken(tokens, numTokens, "i", false, index);
        isOutput = getBoolToken(tokens, numTokens, "o", true, isOutput);
    }

    public void print(OutputStream stream) {
        try {
            super.print(stream);
            stream.write((" n " + qoute(name)).getBytes());
            if (index != -1)
                stream.write((" i " + String.valueOf(index)).getBytes());
            stream.write((" o " + (isOutput ? "1" : "0")).getBytes());
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
        names.add("n");
        names.add("i");
        names.add("o");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("n"))
            return name;
        else if (attribute.equals("i"))
            return String.valueOf(index);
        else if (attribute.equals("o"))
            return String.valueOf(isOutput);
        else
            return super.getAsString(attribute);

    }

    public String getClassName() { return "GateDescriptionEntry"; }
}

