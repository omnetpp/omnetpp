package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
public class CustomDescriptionEntry extends CustomReferenceEntry
{
    public String content;

    public CustomDescriptionEntry() {
        content = null;
    }

    public CustomDescriptionEntry(IChunk chunk, int entryIndex) {
        super(chunk, entryIndex);
        content = null;
    }

    public String getContent() { return content; }

    public void parse(String[] tokens, int numTokens) {
        super.parse(tokens, numTokens);
        content = getStringToken(tokens, numTokens, "c", true, content);
    }

    public void print(OutputStream stream) {
        try {
            super.print(stream);
            stream.write((" c " + qoute(content)).getBytes());
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
        names.add("c");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("c"))
            return content;
        else
            return super.getAsString(attribute);

    }

    public String getClassName() { return "CustomDescriptionEntry"; }
}

