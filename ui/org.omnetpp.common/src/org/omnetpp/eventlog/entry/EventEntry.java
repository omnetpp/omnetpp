package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.eventlog.EventLogTokenBasedEntry;
import org.omnetpp.common.util.BigDecimal;

public class EventEntry extends EventLogTokenBasedEntry
{
    public long eventNumber;
    public BigDecimal simulationTime;
    public int moduleId;
    public long causeEventNumber;
    public long messageId;
    public String fingerprints;

    public EventEntry() {
        this.chunk = null;
        eventNumber = -1;
        simulationTime = null;
        moduleId = -1;
        causeEventNumber = -1;
        messageId = -1;
        fingerprints = null;
    }

    public EventEntry(IChunk chunk, int entryIndex) {
        this.chunk = chunk;
        this.entryIndex = entryIndex;
        eventNumber = -1;
        simulationTime = null;
        moduleId = -1;
        causeEventNumber = -1;
        messageId = -1;
        fingerprints = null;
    }

    public long getEventNumber() { return eventNumber; }
    public BigDecimal getSimulationTime() { return simulationTime; }
    public int getModuleId() { return moduleId; }
    public long getCauseEventNumber() { return causeEventNumber; }
    public long getMessageId() { return messageId; }
    public String getFingerprints() { return fingerprints; }

    public void parse(String[] tokens, int numTokens) {
        eventNumber = getEventNumberToken(tokens, numTokens, "#", true, eventNumber);
        simulationTime = getSimtimeToken(tokens, numTokens, "t", true, simulationTime);
        moduleId = getIntToken(tokens, numTokens, "m", true, moduleId);
        causeEventNumber = getEventNumberToken(tokens, numTokens, "ce", true, causeEventNumber);
        messageId = getInt64Token(tokens, numTokens, "msg", true, messageId);
        fingerprints = getStringToken(tokens, numTokens, "f", false, fingerprints);
    }

    public void print(OutputStream stream) {
        try {
            stream.write(("E").getBytes());
            stream.write((" # " + String.valueOf(eventNumber)).getBytes());
            stream.write((" t " + simulationTime.toString()).getBytes());
            stream.write((" m " + String.valueOf(moduleId)).getBytes());
            stream.write((" ce " + String.valueOf(causeEventNumber)).getBytes());
            stream.write((" msg " + String.valueOf(messageId)).getBytes());
            if (fingerprints != null)
                stream.write((" f " + qoute(fingerprints)).getBytes());
            stream.write(("\n").getBytes());
            stream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return 60; }

    public String getAsString() { return "E"; }

    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        names.add("#");
        names.add("t");
        names.add("m");
        names.add("ce");
        names.add("msg");
        names.add("f");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("#"))
            return String.valueOf(eventNumber);
        else if (attribute.equals("t"))
            return simulationTime.toString();
        else if (attribute.equals("m"))
            return String.valueOf(moduleId);
        else if (attribute.equals("ce"))
            return String.valueOf(causeEventNumber);
        else if (attribute.equals("msg"))
            return String.valueOf(messageId);
        else if (attribute.equals("f"))
            return fingerprints;
        else
            return null;

    }

    public String getClassName() { return "EventEntry"; }
}

