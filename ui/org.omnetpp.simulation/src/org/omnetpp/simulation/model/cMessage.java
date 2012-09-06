package org.omnetpp.simulation.model;

import java.util.Map;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.simulation.controller.Simulation;

public class cMessage extends cObject {
    private short kind;
    private short priority;

    private long id;
    private long treeId;

    private cModule senderModule;
    private cGate senderGate;
    private cModule arrivalModule;
    private cGate arrivalGate;

    private BigDecimal sendingTime;
    private BigDecimal arrivalTime;

    public cMessage(Simulation simulation, long id) {
        super(simulation, id);
    }

    public short getKind() {
        checkState();
        return kind;
    }

    public short getSchedulingPriority() {
        checkState();
        return priority;
    }

    public long getId() {
        checkState();
        return id;
    }

    public long getTreeId() {
        checkState();
        return treeId;
    }

    public boolean isSelfMessage() {
        checkState();
        return arrivalGate == null;
    }

    public cModule getSenderModule() {
        checkState();
        return senderModule;
    }

    public cGate getSenderGate() {
        checkState();
        return senderGate;
    }

    public cModule getArrivalModule() {
        checkState();
        return arrivalModule;
    }

    public cGate getArrivalGate() {
        checkState();
        return arrivalGate;
    }

    public BigDecimal getSendingTime()  {
        checkState();
        return sendingTime;
    }

    public BigDecimal getArrivalTime() {
        checkState();
        return arrivalTime;
    }

    @Override
    @SuppressWarnings("rawtypes")
    protected void doFillFromJSON(Map jsonObject) {
        super.doFillFromJSON(jsonObject);

        kind = ((Number)jsonObject.get("kind")).shortValue();
        priority = ((Number)jsonObject.get("priority")).shortValue();

        id = ((Number)jsonObject.get("id")).longValue();
        treeId = ((Number)jsonObject.get("treeId")).longValue();

        senderModule = (cModule) getSimulation().getObjectByJSONRef((String) jsonObject.get("senderModule"));
        senderGate = (cGate) getSimulation().getObjectByJSONRef((String) jsonObject.get("senderGate"));
        arrivalModule = (cModule) getSimulation().getObjectByJSONRef((String) jsonObject.get("arrivalModule"));
        arrivalGate = (cGate) getSimulation().getObjectByJSONRef((String) jsonObject.get("arrivalGate"));

        sendingTime = BigDecimal.parse((String) jsonObject.get("sendingTime"));
        arrivalTime = BigDecimal.parse((String) jsonObject.get("arrivalTime"));
    }

    protected void clearReferences() {
        super.clearReferences();
        senderModule = arrivalModule = null;
        senderGate = arrivalGate = null;
    }

}