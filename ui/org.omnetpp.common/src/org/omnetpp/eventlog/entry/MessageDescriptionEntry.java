package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.common.engine.BigDecimal;

public class MessageDescriptionEntry extends MessageReferenceEntry
{
    public long messageTreeId;
    public long messageEncapsulationId;
    public long messageEncapsulationTreeId;
    public String messageClassName;
    public String messageName;
    public short messageKind;
    public short messagePriority;
    public long messageLength;
    public boolean hasBitError;
    public int ownerModuleId;
    public int senderModuleId;
    public int senderGateId;
    public BigDecimal sendingTime;
    public int arrivalModuleId;
    public int arrivalGateId;
    public BigDecimal arrivalTime;
    public String detail;
    public long previousEventNumber;

    public MessageDescriptionEntry() {
        messageTreeId = -1;
        messageEncapsulationId = -1;
        messageEncapsulationTreeId = -1;
        messageClassName = null;
        messageName = null;
        messageKind = 0;
        messagePriority = 0;
        messageLength = 0;
        hasBitError = false;
        ownerModuleId = -1;
        senderModuleId = -1;
        senderGateId = -1;
        sendingTime = new BigDecimal(-1);
        arrivalModuleId = -1;
        arrivalGateId = -1;
        arrivalTime = new BigDecimal(-1);
        detail = null;
        previousEventNumber = -1;
    }

    public MessageDescriptionEntry(IChunk chunk, int entryIndex) {
        super(chunk, entryIndex);
        messageTreeId = -1;
        messageEncapsulationId = -1;
        messageEncapsulationTreeId = -1;
        messageClassName = null;
        messageName = null;
        messageKind = 0;
        messagePriority = 0;
        messageLength = 0;
        hasBitError = false;
        ownerModuleId = -1;
        senderModuleId = -1;
        senderGateId = -1;
        sendingTime = new BigDecimal(-1);
        arrivalModuleId = -1;
        arrivalGateId = -1;
        arrivalTime = new BigDecimal(-1);
        detail = null;
        previousEventNumber = -1;
    }

    public long getMessageTreeId() { return messageTreeId; }
    public long getMessageEncapsulationId() { return messageEncapsulationId; }
    public long getMessageEncapsulationTreeId() { return messageEncapsulationTreeId; }
    public String getMessageClassName() { return messageClassName; }
    public String getMessageName() { return messageName; }
    public short getMessageKind() { return messageKind; }
    public short getMessagePriority() { return messagePriority; }
    public long getMessageLength() { return messageLength; }
    public boolean getHasBitError() { return hasBitError; }
    public int getOwnerModuleId() { return ownerModuleId; }
    public int getSenderModuleId() { return senderModuleId; }
    public int getSenderGateId() { return senderGateId; }
    public BigDecimal getSendingTime() { return sendingTime; }
    public int getArrivalModuleId() { return arrivalModuleId; }
    public int getArrivalGateId() { return arrivalGateId; }
    public BigDecimal getArrivalTime() { return arrivalTime; }
    public String getDetail() { return detail; }
    public long getPreviousEventNumber() { return previousEventNumber; }

    public void parse(String[] tokens, int numTokens) {
        super.parse(tokens, numTokens);
        messageTreeId = getLongToken(tokens, numTokens, "tid", true, messageTreeId);
        messageEncapsulationId = getLongToken(tokens, numTokens, "eid", true, messageEncapsulationId);
        messageEncapsulationTreeId = getLongToken(tokens, numTokens, "etid", true, messageEncapsulationTreeId);
        messageClassName = getStringToken(tokens, numTokens, "c", true, messageClassName);
        messageName = getStringToken(tokens, numTokens, "n", true, messageName);
        messageKind = getShortToken(tokens, numTokens, "k", false, messageKind);
        messagePriority = getShortToken(tokens, numTokens, "p", false, messagePriority);
        messageLength = getInt64Token(tokens, numTokens, "l", false, messageLength);
        hasBitError = getBoolToken(tokens, numTokens, "er", false, hasBitError);
        ownerModuleId = getIntToken(tokens, numTokens, "m", false, ownerModuleId);
        senderModuleId = getIntToken(tokens, numTokens, "sm", false, senderModuleId);
        senderGateId = getIntToken(tokens, numTokens, "sg", false, senderGateId);
        sendingTime = getSimtimeToken(tokens, numTokens, "st", false, sendingTime);
        arrivalModuleId = getIntToken(tokens, numTokens, "am", false, arrivalModuleId);
        arrivalGateId = getIntToken(tokens, numTokens, "ag", false, arrivalGateId);
        arrivalTime = getSimtimeToken(tokens, numTokens, "at", false, arrivalTime);
        detail = getStringToken(tokens, numTokens, "d", false, detail);
        previousEventNumber = getEventNumberToken(tokens, numTokens, "pe", true, previousEventNumber);
    }

    public void print(OutputStream stream) {
        try {
            super.print(stream);
            stream.write((" tid " + String.valueOf(messageTreeId)).getBytes());
            stream.write((" eid " + String.valueOf(messageEncapsulationId)).getBytes());
            stream.write((" etid " + String.valueOf(messageEncapsulationTreeId)).getBytes());
            stream.write((" c " + qoute(messageClassName)).getBytes());
            stream.write((" n " + qoute(messageName)).getBytes());
            if (messageKind != 0)
                stream.write((" k " + String.valueOf(messageKind)).getBytes());
            if (messagePriority != 0)
                stream.write((" p " + String.valueOf(messagePriority)).getBytes());
            if (messageLength != 0)
                stream.write((" l " + String.valueOf(messageLength)).getBytes());
            if (hasBitError != false)
                stream.write((" er " + (hasBitError ? "1" : "0")).getBytes());
            if (ownerModuleId != -1)
                stream.write((" m " + String.valueOf(ownerModuleId)).getBytes());
            if (senderModuleId != -1)
                stream.write((" sm " + String.valueOf(senderModuleId)).getBytes());
            if (senderGateId != -1)
                stream.write((" sg " + String.valueOf(senderGateId)).getBytes());
            if (!sendingTime.equals(new BigDecimal(-1)))
                stream.write((" st " + sendingTime.toString()).getBytes());
            if (arrivalModuleId != -1)
                stream.write((" am " + String.valueOf(arrivalModuleId)).getBytes());
            if (arrivalGateId != -1)
                stream.write((" ag " + String.valueOf(arrivalGateId)).getBytes());
            if (!arrivalTime.equals(new BigDecimal(-1)))
                stream.write((" at " + arrivalTime.toString()).getBytes());
            if (detail != null)
                stream.write((" d " + qoute(detail)).getBytes());
            stream.write((" pe " + String.valueOf(previousEventNumber)).getBytes());
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
        names.addAll(super.getAttributeNames());
        names.add("tid");
        names.add("eid");
        names.add("etid");
        names.add("c");
        names.add("n");
        names.add("k");
        names.add("p");
        names.add("l");
        names.add("er");
        names.add("m");
        names.add("sm");
        names.add("sg");
        names.add("st");
        names.add("am");
        names.add("ag");
        names.add("at");
        names.add("d");
        names.add("pe");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("tid"))
            return String.valueOf(messageTreeId);
        else if (attribute.equals("eid"))
            return String.valueOf(messageEncapsulationId);
        else if (attribute.equals("etid"))
            return String.valueOf(messageEncapsulationTreeId);
        else if (attribute.equals("c"))
            return messageClassName;
        else if (attribute.equals("n"))
            return messageName;
        else if (attribute.equals("k"))
            return String.valueOf(messageKind);
        else if (attribute.equals("p"))
            return String.valueOf(messagePriority);
        else if (attribute.equals("l"))
            return String.valueOf(messageLength);
        else if (attribute.equals("er"))
            return String.valueOf(hasBitError);
        else if (attribute.equals("m"))
            return String.valueOf(ownerModuleId);
        else if (attribute.equals("sm"))
            return String.valueOf(senderModuleId);
        else if (attribute.equals("sg"))
            return String.valueOf(senderGateId);
        else if (attribute.equals("st"))
            return sendingTime.toString();
        else if (attribute.equals("am"))
            return String.valueOf(arrivalModuleId);
        else if (attribute.equals("ag"))
            return String.valueOf(arrivalGateId);
        else if (attribute.equals("at"))
            return arrivalTime.toString();
        else if (attribute.equals("d"))
            return detail;
        else if (attribute.equals("pe"))
            return String.valueOf(previousEventNumber);
        else
            return super.getAsString(attribute);

    }

    public String getClassName() { return "MessageDescriptionEntry"; }
}

