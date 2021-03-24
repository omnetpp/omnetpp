package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.eventlog.EventLogTokenBasedEntry;
public class ConnectionReferenceEntry extends EventLogTokenBasedEntry
{
    public int sourceModuleId;
    public int sourceGateId;

    public ConnectionReferenceEntry() {
        this.chunk = null;
        sourceModuleId = -1;
        sourceGateId = -1;
    }

    public ConnectionReferenceEntry(IChunk chunk, int entryIndex) {
        this.chunk = chunk;
        this.entryIndex = entryIndex;
        sourceModuleId = -1;
        sourceGateId = -1;
    }

    public int getSourceModuleId() { return sourceModuleId; }
    public int getSourceGateId() { return sourceGateId; }

    public void parse(String[] tokens, int numTokens) {
        sourceModuleId = getIntToken(tokens, numTokens, "sm", true, sourceModuleId);
        sourceGateId = getIntToken(tokens, numTokens, "sg", true, sourceGateId);
    }

    public void print(OutputStream stream) {
        try {
            stream.write((" sm " + String.valueOf(sourceModuleId)).getBytes());
            stream.write((" sg " + String.valueOf(sourceGateId)).getBytes());
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
        names.add("sm");
        names.add("sg");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("sm"))
            return String.valueOf(sourceModuleId);
        else if (attribute.equals("sg"))
            return String.valueOf(sourceGateId);
        else
            return null;

    }

    public String getClassName() { return "ConnectionReferenceEntry"; }
}

