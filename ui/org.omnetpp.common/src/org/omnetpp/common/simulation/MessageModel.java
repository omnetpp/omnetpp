package org.omnetpp.common.simulation;

import org.omnetpp.common.engine.BigDecimal;

public class MessageModel {
    private String name;
    private String className;

    private short kind;
    private short priority;
    private long bitLength;
    private boolean bitError;

    private int id;
    private int treeId;
    private int encapsulationId;
    private int encapsulationTreeId;

    private long senderEventNumber;
    private long arrivalEventNumber;

    private ModuleModel senderModule;
    private GateModel senderGate;
    private ModuleModel arrivalModule;
    private GateModel arrivalGate;
    private BigDecimal sendingTime, arrivalTime;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public boolean hasBitError() {
        return bitError;
    }

    public void setBitError(boolean error) {
        this.bitError = error;
    }

    public long getBitLength() {
        return bitLength;
    }

    public void setBitLength(long length) {
        this.bitLength = length;
    }

    public short getKind() {
        return kind;
    }

    public void setKind(short kind) {
        this.kind = kind;
    }

    public short getSchedulingPriority() {
        return priority;
    }

    public void setSchedulingPriority(short priority) {
        this.priority = priority;
    }

    public boolean isSelfMessage() {
        return arrivalGate == null;
    }

    public void setSenderEventNumber(long senderEventNumber) {
        this.senderEventNumber = senderEventNumber;
    }

    public long getSenderEventNumber() {
        return senderEventNumber;
    }

    public void setArrivalEventNumber(long arrivalEventNumber) {
        this.arrivalEventNumber = arrivalEventNumber;
    }

    public long getArrivalEventNumber() {
        return arrivalEventNumber;
    }

    public ModuleModel getSenderModule() {
        return senderModule;
    }

    public GateModel getSenderGate() {
        return senderGate;
    }

    public ModuleModel getArrivalModule() {
        return arrivalModule;
    }

    public GateModel getArrivalGate() {
        return arrivalGate;
    }

    public BigDecimal getSendingTime()  {
        return sendingTime;
    }

    public BigDecimal getArrivalTime() {
        return arrivalTime;
    }

    public void setArrivalGate(GateModel arrivalGate) {
        this.arrivalGate = arrivalGate;
    }

    public void setArrivalModule(ModuleModel arrivalModule) {
        this.arrivalModule = arrivalModule;
    }

    public void setArrivalTime(BigDecimal arrivalTime) {
        this.arrivalTime = arrivalTime;
    }

    public void setSenderGate(GateModel senderGate) {
        this.senderGate = senderGate;
    }

    public void setSenderModule(ModuleModel senderModule) {
        this.senderModule = senderModule;
    }

    public void setSendingTime(BigDecimal sendingTime) {
        this.sendingTime = sendingTime;
    }

    public int getEncapsulationId() {
        return encapsulationId;
    }

    public void setEncapsulationId(int encapsulationId) {
        this.encapsulationId = encapsulationId;
    }

    public int getEncapsulationTreeId() {
        return encapsulationTreeId;
    }

    public void setEncapsulationTreeId(int encapsulationTreeId) {
        this.encapsulationTreeId = encapsulationTreeId;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getTreeId() {
        return treeId;
    }

    public void setTreeId(int treeId) {
        this.treeId = treeId;
    }

    public String getClassName() {
        return className;
    }

    public void setClassName(String className) {
        this.className = className;
    }
}
