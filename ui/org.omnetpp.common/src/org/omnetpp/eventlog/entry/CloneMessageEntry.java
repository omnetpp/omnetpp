package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
public class CloneMessageEntry extends MessageDescriptionEntry
{
    public long cloneId;

    public CloneMessageEntry() {
        cloneId = -1;
    }

    public CloneMessageEntry(IChunk chunk, int entryIndex) {
        super(chunk, entryIndex);
        cloneId = -1;
    }

    public long getCloneId() { return cloneId; }

    public void parse(String[] tokens, int numTokens) {
        super.parse(tokens, numTokens);
        cloneId = getInt64Token(tokens, numTokens, "cid", true, cloneId);
    }

    public void print(OutputStream stream) {
        try {
            stream.write(("CL").getBytes());
            super.print(stream);
            stream.write((" cid " + String.valueOf(cloneId)).getBytes());
            stream.write(("\n").getBytes());
            stream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return 60; }

    public String getAsString() { return "CL"; }

    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        names.addAll(super.getAttributeNames());
        names.add("cid");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("cid"))
            return String.valueOf(cloneId);
        else
            return super.getAsString(attribute);

    }

    public String getClassName() { return "CloneMessageEntry"; }
}

