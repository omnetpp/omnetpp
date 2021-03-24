package org.omnetpp.eventlog;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

/**
 * One line log message entry.
 */
public class EventLogMessageEntry extends EventLogEntry
{
    public String text = null;

    public EventLogMessageEntry(IChunk chunk, int entryIndex) {
        this.chunk = chunk;
        this.entryIndex = entryIndex;
    }

    public String getText() {
        return text;
    }

    public void parse(String line, int length) {
        text = line.substring(2);
    }

    @Override
    public void print(OutputStream stream) {
        try {
            stream.write(("- " + text + "\n").getBytes());
        }
        catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public int getClassIndex() {
        return 0;
    }

    @Override
    public String getClassName() {
        return "EventLogMessageEntry";
    }

    @Override
    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        names.add("-");
        return names;
    }

    @Override
    public String getAsString() {
        return "-";
    }

    @Override
    public String getAsString(String attribute) {
        if (attribute.equals("type"))
            return "-";
        else if (attribute.equals("-"))
            return text;
        else
            return null;
    }
}
