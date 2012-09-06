package org.omnetpp.simulation.model;

import java.util.Map;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.simulation.controller.Simulation;

public class cPacket extends cMessage {
    private long bitLength;
    private boolean bitError;
    private BigDecimal duration;
    private boolean isReceptionStart;
    private cPacket encapsulatedPacket;
    private long encapsulationId;
    private long encapsulationTreeId;

    public cPacket(Simulation simulation, long id) {
        super(simulation, id);
    }

    public boolean hasBitError() {
        checkState();
        return bitError;
    }

    public long getBitLength() {
        checkState();
        return bitLength;
    }

    public BigDecimal getDuration() {
        checkState();
        return duration;
    }

    public boolean isReceptionStart() {
        checkState();
        return isReceptionStart;
    }

    public cPacket getEncapsulatedPacket() {
        return encapsulatedPacket;
    }

    public long getEncapsulationId() {
        checkState();
        return encapsulationId;
    }

    public long getEncapsulationTreeId() {
        checkState();
        return encapsulationTreeId;
    }

    @Override
    @SuppressWarnings("rawtypes")
    protected void doFillFromJSON(Map jsonObject) {
        super.doFillFromJSON(jsonObject);

        bitLength = ((Number)jsonObject.get("bitLength")).longValue();
        bitError = (Boolean) jsonObject.get("bitError");
        duration = BigDecimal.parse((String) jsonObject.get("duration"));
        isReceptionStart = (Boolean) jsonObject.get("isReceptionStart");
        encapsulatedPacket = (cPacket) getSimulation().getObjectByJSONRef((String) jsonObject.get("encapsulatedPacket"));
        encapsulationId = ((Number)jsonObject.get("encapsulationId")).longValue();
        encapsulationTreeId = ((Number)jsonObject.get("encapsulationTreeId")).longValue();
    }

    protected void clearReferences() {
        super.clearReferences();
        encapsulatedPacket = null;
    }
}