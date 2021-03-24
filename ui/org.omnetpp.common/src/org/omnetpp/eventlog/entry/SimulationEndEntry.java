package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.eventlog.EventLogTokenBasedEntry;
public class SimulationEndEntry extends EventLogTokenBasedEntry
{
    public boolean isError;
    public int resultCode;
    public String message;

    public SimulationEndEntry() {
        this.chunk = null;
        isError = false;
        resultCode = -1;
        message = null;
    }

    public SimulationEndEntry(IChunk chunk, int entryIndex) {
        this.chunk = chunk;
        this.entryIndex = entryIndex;
        isError = false;
        resultCode = -1;
        message = null;
    }

    public boolean getIsError() { return isError; }
    public int getResultCode() { return resultCode; }
    public String getMessage() { return message; }

    public void parse(String[] tokens, int numTokens) {
        isError = getBoolToken(tokens, numTokens, "e", true, isError);
        resultCode = getIntToken(tokens, numTokens, "c", true, resultCode);
        message = getStringToken(tokens, numTokens, "m", true, message);
    }

    public void print(OutputStream stream) {
        try {
            stream.write(("SE").getBytes());
            stream.write((" e " + (isError ? "1" : "0")).getBytes());
            stream.write((" c " + String.valueOf(resultCode)).getBytes());
            stream.write((" m " + qoute(message)).getBytes());
            stream.write(("\n").getBytes());
            stream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return 60; }

    public String getAsString() { return "SE"; }

    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        names.add("e");
        names.add("c");
        names.add("m");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("e"))
            return String.valueOf(isError);
        else if (attribute.equals("c"))
            return String.valueOf(resultCode);
        else if (attribute.equals("m"))
            return message;
        else
            return null;

    }

    public String getClassName() { return "SimulationEndEntry"; }
}

