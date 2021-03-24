package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.eventlog.EventLogTokenBasedEntry;
public class GateReferenceEntry extends EventLogTokenBasedEntry
{
    public int moduleId;
    public int gateId;

    public GateReferenceEntry() {
        this.chunk = null;
        moduleId = -1;
        gateId = -1;
    }

    public GateReferenceEntry(IChunk chunk, int entryIndex) {
        this.chunk = chunk;
        this.entryIndex = entryIndex;
        moduleId = -1;
        gateId = -1;
    }

    public int getModuleId() { return moduleId; }
    public int getGateId() { return gateId; }

    public void parse(String[] tokens, int numTokens) {
        moduleId = getIntToken(tokens, numTokens, "m", true, moduleId);
        gateId = getIntToken(tokens, numTokens, "g", true, gateId);
    }

    public void print(OutputStream stream) {
        try {
            stream.write((" m " + String.valueOf(moduleId)).getBytes());
            stream.write((" g " + String.valueOf(gateId)).getBytes());
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
        names.add("m");
        names.add("g");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("m"))
            return String.valueOf(moduleId);
        else if (attribute.equals("g"))
            return String.valueOf(gateId);
        else
            return null;

    }

    public String getClassName() { return "GateReferenceEntry"; }
}

