package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
public class ConnectionDescriptionEntry extends ConnectionReferenceEntry
{
    public int destModuleId;
    public int destGateId;

    public ConnectionDescriptionEntry() {
        destModuleId = -1;
        destGateId = -1;
    }

    public ConnectionDescriptionEntry(IChunk chunk, int entryIndex) {
        super(chunk, entryIndex);
        destModuleId = -1;
        destGateId = -1;
    }

    public int getDestModuleId() { return destModuleId; }
    public int getDestGateId() { return destGateId; }

    public void parse(String[] tokens, int numTokens) {
        super.parse(tokens, numTokens);
        destModuleId = getIntToken(tokens, numTokens, "dm", true, destModuleId);
        destGateId = getIntToken(tokens, numTokens, "dg", true, destGateId);
    }

    public void print(OutputStream stream) {
        try {
            super.print(stream);
            stream.write((" dm " + String.valueOf(destModuleId)).getBytes());
            stream.write((" dg " + String.valueOf(destGateId)).getBytes());
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
        names.add("dm");
        names.add("dg");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("dm"))
            return String.valueOf(destModuleId);
        else if (attribute.equals("dg"))
            return String.valueOf(destGateId);
        else
            return super.getAsString(attribute);

    }

    public String getClassName() { return "ConnectionDescriptionEntry"; }
}

