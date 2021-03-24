package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.eventlog.EventLogTokenBasedEntry;
public class SimulationBeginEntry extends EventLogTokenBasedEntry
{
    public int omnetppVersion;
    public int eventlogVersion;
    public String runId;

    public SimulationBeginEntry() {
        this.chunk = null;
        omnetppVersion = -1;
        eventlogVersion = -1;
        runId = null;
    }

    public SimulationBeginEntry(IChunk chunk, int entryIndex) {
        this.chunk = chunk;
        this.entryIndex = entryIndex;
        omnetppVersion = -1;
        eventlogVersion = -1;
        runId = null;
    }

    public int getOmnetppVersion() { return omnetppVersion; }
    public int getEventlogVersion() { return eventlogVersion; }
    public String getRunId() { return runId; }

    public void parse(String[] tokens, int numTokens) {
        omnetppVersion = getIntToken(tokens, numTokens, "ov", true, omnetppVersion);
        eventlogVersion = getIntToken(tokens, numTokens, "ev", true, eventlogVersion);
        runId = getStringToken(tokens, numTokens, "rid", true, runId);
    }

    public void print(OutputStream stream) {
        try {
            stream.write(("SB").getBytes());
            stream.write((" ov " + String.valueOf(omnetppVersion)).getBytes());
            stream.write((" ev " + String.valueOf(eventlogVersion)).getBytes());
            stream.write((" rid " + qoute(runId)).getBytes());
            stream.write(("\n").getBytes());
            stream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return 60; }

    public String getAsString() { return "SB"; }

    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        names.add("ov");
        names.add("ev");
        names.add("rid");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("ov"))
            return String.valueOf(omnetppVersion);
        else if (attribute.equals("ev"))
            return String.valueOf(eventlogVersion);
        else if (attribute.equals("rid"))
            return runId;
        else
            return null;

    }

    public String getClassName() { return "SimulationBeginEntry"; }
}

