package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.eventlog.EventLogTokenBasedEntry;
public class ComponentMethodBeginEntry extends EventLogTokenBasedEntry
{
    public int sourceComponentId;
    public int targetComponentId;
    public String methodName;

    public ComponentMethodBeginEntry() {
        this.chunk = null;
        sourceComponentId = -1;
        targetComponentId = -1;
        methodName = null;
    }

    public ComponentMethodBeginEntry(IChunk chunk, int entryIndex) {
        this.chunk = chunk;
        this.entryIndex = entryIndex;
        sourceComponentId = -1;
        targetComponentId = -1;
        methodName = null;
    }

    public int getSourceComponentId() { return sourceComponentId; }
    public int getTargetComponentId() { return targetComponentId; }
    public String getMethodName() { return methodName; }

    public void parse(String[] tokens, int numTokens) {
        sourceComponentId = getIntToken(tokens, numTokens, "sm", true, sourceComponentId);
        targetComponentId = getIntToken(tokens, numTokens, "tm", true, targetComponentId);
        methodName = getStringToken(tokens, numTokens, "m", true, methodName);
    }

    public void print(OutputStream stream) {
        try {
            stream.write(("CMB").getBytes());
            stream.write((" sm " + String.valueOf(sourceComponentId)).getBytes());
            stream.write((" tm " + String.valueOf(targetComponentId)).getBytes());
            stream.write((" m " + qoute(methodName)).getBytes());
            stream.write(("\n").getBytes());
            stream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return 60; }

    public String getAsString() { return "CMB"; }

    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        names.add("sm");
        names.add("tm");
        names.add("m");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("sm"))
            return String.valueOf(sourceComponentId);
        else if (attribute.equals("tm"))
            return String.valueOf(targetComponentId);
        else if (attribute.equals("m"))
            return methodName;
        else
            return null;

    }

    public String getClassName() { return "ComponentMethodBeginEntry"; }
}

