package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
public class EndSendEntry extends MessageDescriptionEntry
{
    public boolean isDeliveredImmediately;

    public EndSendEntry() {
        isDeliveredImmediately = false;
    }

    public EndSendEntry(IChunk chunk, int entryIndex) {
        super(chunk, entryIndex);
        isDeliveredImmediately = false;
    }

    public boolean getIsDeliveredImmediately() { return isDeliveredImmediately; }

    public void parse(String[] tokens, int numTokens) {
        super.parse(tokens, numTokens);
        isDeliveredImmediately = getBoolToken(tokens, numTokens, "i", false, isDeliveredImmediately);
    }

    public void print(OutputStream stream) {
        try {
            stream.write(("ES").getBytes());
            super.print(stream);
            if (isDeliveredImmediately != false)
                stream.write((" i " + (isDeliveredImmediately ? "1" : "0")).getBytes());
            stream.write(("\n").getBytes());
            stream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return 60; }

    public String getAsString() { return "ES"; }

    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        names.addAll(super.getAttributeNames());
        names.add("i");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("i"))
            return String.valueOf(isDeliveredImmediately);
        else
            return super.getAsString(attribute);

    }

    public String getClassName() { return "EndSendEntry"; }
}

