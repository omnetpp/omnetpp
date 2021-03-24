package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
public class DecapsulatePacketEntry extends MessageReferenceEntry
{
    public long encapsulatedPacketId;

    public DecapsulatePacketEntry() {
        encapsulatedPacketId = -1;
    }

    public DecapsulatePacketEntry(IChunk chunk, int entryIndex) {
        super(chunk, entryIndex);
        encapsulatedPacketId = -1;
    }

    public long getEncapsulatedPacketId() { return encapsulatedPacketId; }

    public void parse(String[] tokens, int numTokens) {
        super.parse(tokens, numTokens);
        encapsulatedPacketId = getLongToken(tokens, numTokens, "cid", true, encapsulatedPacketId);
    }

    public void print(OutputStream stream) {
        try {
            stream.write(("DE").getBytes());
            super.print(stream);
            stream.write((" cid " + String.valueOf(encapsulatedPacketId)).getBytes());
            stream.write(("\n").getBytes());
            stream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return 60; }

    public String getAsString() { return "DE"; }

    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        names.addAll(super.getAttributeNames());
        names.add("cid");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("cid"))
            return String.valueOf(encapsulatedPacketId);
        else
            return super.getAsString(attribute);

    }

    public String getClassName() { return "DecapsulatePacketEntry"; }
}

