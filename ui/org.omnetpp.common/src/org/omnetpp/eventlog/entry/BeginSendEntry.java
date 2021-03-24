package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.common.engine.BigDecimal;

public class BeginSendEntry extends MessageDescriptionEntry
{
    public BigDecimal sendDelay;
    public boolean isUpdate;
    public long transmissionId;

    public BeginSendEntry() {
        sendDelay = new BigDecimal(0);
        isUpdate = false;
        transmissionId = -1;
    }

    public BeginSendEntry(IChunk chunk, int entryIndex) {
        super(chunk, entryIndex);
        sendDelay = new BigDecimal(0);
        isUpdate = false;
        transmissionId = -1;
    }

    public BigDecimal getSendDelay() { return sendDelay; }
    public boolean getIsUpdate() { return isUpdate; }
    public long getTransmissionId() { return transmissionId; }

    public void parse(String[] tokens, int numTokens) {
        super.parse(tokens, numTokens);
        sendDelay = getSimtimeToken(tokens, numTokens, "sd", false, sendDelay);
        isUpdate = getBoolToken(tokens, numTokens, "up", false, isUpdate);
        transmissionId = getLongToken(tokens, numTokens, "tx", false, transmissionId);
    }

    public void print(OutputStream stream) {
        try {
            stream.write(("BS").getBytes());
            super.print(stream);
            if (!sendDelay.equals(new BigDecimal(0)))
                stream.write((" sd " + sendDelay.toString()).getBytes());
            if (isUpdate != false)
                stream.write((" up " + (isUpdate ? "1" : "0")).getBytes());
            if (transmissionId != -1)
                stream.write((" tx " + String.valueOf(transmissionId)).getBytes());
            stream.write(("\n").getBytes());
            stream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return 60; }

    public String getAsString() { return "BS"; }

    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        names.addAll(super.getAttributeNames());
        names.add("sd");
        names.add("up");
        names.add("tx");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("sd"))
            return sendDelay.toString();
        else if (attribute.equals("up"))
            return String.valueOf(isUpdate);
        else if (attribute.equals("tx"))
            return String.valueOf(transmissionId);
        else
            return super.getAsString(attribute);

    }

    public String getClassName() { return "BeginSendEntry"; }
}

