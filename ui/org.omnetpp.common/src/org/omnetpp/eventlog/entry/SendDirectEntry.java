package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.eventlog.EventLogTokenBasedEntry;
import org.omnetpp.common.util.BigDecimal;

public class SendDirectEntry extends EventLogTokenBasedEntry
{
    public int senderModuleId;
    public int destModuleId;
    public int destGateId;
    public BigDecimal propagationDelay;
    public BigDecimal transmissionDelay;
    public BigDecimal remainingDuration;

    public SendDirectEntry() {
        this.chunk = null;
        senderModuleId = -1;
        destModuleId = -1;
        destGateId = -1;
        propagationDelay = new BigDecimal(0);
        transmissionDelay = new BigDecimal(0);
        remainingDuration = new BigDecimal(0);
    }

    public SendDirectEntry(IChunk chunk, int entryIndex) {
        this.chunk = chunk;
        this.entryIndex = entryIndex;
        senderModuleId = -1;
        destModuleId = -1;
        destGateId = -1;
        propagationDelay = new BigDecimal(0);
        transmissionDelay = new BigDecimal(0);
        remainingDuration = new BigDecimal(0);
    }

    public int getSenderModuleId() { return senderModuleId; }
    public int getDestModuleId() { return destModuleId; }
    public int getDestGateId() { return destGateId; }
    public BigDecimal getPropagationDelay() { return propagationDelay; }
    public BigDecimal getTransmissionDelay() { return transmissionDelay; }
    public BigDecimal getRemainingDuration() { return remainingDuration; }

    public void parse(String[] tokens, int numTokens) {
        senderModuleId = getIntToken(tokens, numTokens, "sm", true, senderModuleId);
        destModuleId = getIntToken(tokens, numTokens, "dm", true, destModuleId);
        destGateId = getIntToken(tokens, numTokens, "dg", true, destGateId);
        propagationDelay = getSimtimeToken(tokens, numTokens, "pd", false, propagationDelay);
        transmissionDelay = getSimtimeToken(tokens, numTokens, "td", false, transmissionDelay);
        remainingDuration = getSimtimeToken(tokens, numTokens, "rd", false, remainingDuration);
    }

    public void print(OutputStream stream) {
        try {
            stream.write(("SD").getBytes());
            stream.write((" sm " + String.valueOf(senderModuleId)).getBytes());
            stream.write((" dm " + String.valueOf(destModuleId)).getBytes());
            stream.write((" dg " + String.valueOf(destGateId)).getBytes());
            if (!propagationDelay.equals(new BigDecimal(0)))
                stream.write((" pd " + propagationDelay.toString()).getBytes());
            if (!transmissionDelay.equals(new BigDecimal(0)))
                stream.write((" td " + transmissionDelay.toString()).getBytes());
            if (!remainingDuration.equals(new BigDecimal(0)))
                stream.write((" rd " + remainingDuration.toString()).getBytes());
            stream.write(("\n").getBytes());
            stream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return 60; }

    public String getAsString() { return "SD"; }

    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        names.add("sm");
        names.add("dm");
        names.add("dg");
        names.add("pd");
        names.add("td");
        names.add("rd");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("sm"))
            return String.valueOf(senderModuleId);
        else if (attribute.equals("dm"))
            return String.valueOf(destModuleId);
        else if (attribute.equals("dg"))
            return String.valueOf(destGateId);
        else if (attribute.equals("pd"))
            return propagationDelay.toString();
        else if (attribute.equals("td"))
            return transmissionDelay.toString();
        else if (attribute.equals("rd"))
            return remainingDuration.toString();
        else
            return null;

    }

    public String getClassName() { return "SendDirectEntry"; }
}

