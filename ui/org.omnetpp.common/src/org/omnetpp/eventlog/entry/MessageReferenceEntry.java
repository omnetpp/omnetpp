package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.eventlog.EventLogTokenBasedEntry;
public class MessageReferenceEntry extends EventLogTokenBasedEntry
{
    public long messageId;

    public MessageReferenceEntry() {
        this.chunk = null;
        messageId = -1;
    }

    public MessageReferenceEntry(IChunk chunk, int entryIndex) {
        this.chunk = chunk;
        this.entryIndex = entryIndex;
        messageId = -1;
    }

    public long getMessageId() { return messageId; }

    public void parse(String[] tokens, int numTokens) {
        messageId = getInt64Token(tokens, numTokens, "id", true, messageId);
    }

    public void print(OutputStream stream) {
        try {
            stream.write((" id " + String.valueOf(messageId)).getBytes());
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
        names.add("id");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("id"))
            return String.valueOf(messageId);
        else
            return null;

    }

    public String getClassName() { return "MessageReferenceEntry"; }
}

