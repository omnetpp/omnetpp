package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.eventlog.EventLogTokenBasedEntry;
import org.omnetpp.common.util.BigDecimal;

public class SendHopEntry extends EventLogTokenBasedEntry
{
    public int senderModuleId;
    public int senderGateId;
    public BigDecimal propagationDelay;
    public BigDecimal transmissionDelay;
    public BigDecimal remainingDuration;
    public boolean discard;

    public SendHopEntry() {
        this.chunk = null;
        senderModuleId = -1;
        senderGateId = -1;
        propagationDelay = new BigDecimal(0);
        transmissionDelay = new BigDecimal(0);
        remainingDuration = new BigDecimal(0);
        discard = false;
    }

    public SendHopEntry(IChunk chunk, int entryIndex) {
        this.chunk = chunk;
        this.entryIndex = entryIndex;
        senderModuleId = -1;
        senderGateId = -1;
        propagationDelay = new BigDecimal(0);
        transmissionDelay = new BigDecimal(0);
        remainingDuration = new BigDecimal(0);
        discard = false;
    }

    public int getSenderModuleId() { return senderModuleId; }
    public int getSenderGateId() { return senderGateId; }
    public BigDecimal getPropagationDelay() { return propagationDelay; }
    public BigDecimal getTransmissionDelay() { return transmissionDelay; }
    public BigDecimal getRemainingDuration() { return remainingDuration; }
    public boolean getDiscard() { return discard; }

    public void parse(String[] tokens, int numTokens) {
        senderModuleId = getIntToken(tokens, numTokens, "sm", true, senderModuleId);
        senderGateId = getIntToken(tokens, numTokens, "sg", true, senderGateId);
        propagationDelay = getSimtimeToken(tokens, numTokens, "pd", false, propagationDelay);
        transmissionDelay = getSimtimeToken(tokens, numTokens, "td", false, transmissionDelay);
        remainingDuration = getSimtimeToken(tokens, numTokens, "rd", false, remainingDuration);
        discard = getBoolToken(tokens, numTokens, "d", false, discard);
    }

    public void print(OutputStream stream) {
        try {
            stream.write(("SH").getBytes());
            stream.write((" sm " + String.valueOf(senderModuleId)).getBytes());
            stream.write((" sg " + String.valueOf(senderGateId)).getBytes());
            if (!propagationDelay.equals(new BigDecimal(0)))
                stream.write((" pd " + propagationDelay.toString()).getBytes());
            if (!transmissionDelay.equals(new BigDecimal(0)))
                stream.write((" td " + transmissionDelay.toString()).getBytes());
            if (!remainingDuration.equals(new BigDecimal(0)))
                stream.write((" rd " + remainingDuration.toString()).getBytes());
            if (discard != false)
                stream.write((" d " + (discard ? "1" : "0")).getBytes());
            stream.write(("\n").getBytes());
            stream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return 60; }

    public String getAsString() { return "SH"; }

    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        names.add("sm");
        names.add("sg");
        names.add("pd");
        names.add("td");
        names.add("rd");
        names.add("d");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("sm"))
            return String.valueOf(senderModuleId);
        else if (attribute.equals("sg"))
            return String.valueOf(senderGateId);
        else if (attribute.equals("pd"))
            return propagationDelay.toString();
        else if (attribute.equals("td"))
            return transmissionDelay.toString();
        else if (attribute.equals("rd"))
            return remainingDuration.toString();
        else if (attribute.equals("d"))
            return String.valueOf(discard);
        else
            return null;

    }

    public String getClassName() { return "SendHopEntry"; }
}

