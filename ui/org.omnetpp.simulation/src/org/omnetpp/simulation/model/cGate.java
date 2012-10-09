package org.omnetpp.simulation.model;

import java.util.Map;

import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.simulation.controller.Simulation;

public class cGate extends cObject {
    public enum Type {INPUT, OUTPUT, INOUT};
    private int gateId;
    private int index;
    private int vectorSize;  // -1 if not vector
    private Type type;
    private cModule ownerModule;
    private IDisplayString displayString;
    private cGate previousGate;
    private cGate nextGate;
    private cChannel channel;

    public cGate(Simulation simulation, long id) {
        super(simulation, id);
    }

    public int getId() {
        checkState();
        return gateId;
    }

    public int getIndex() {
        checkState();
        return index;
    }

    public boolean isVector() {
        checkState();
        return vectorSize != -1;
    }

    public int getVectorSize() {
        checkState();
        return vectorSize;
    }

    public Type getType() {
        checkState();
        return type;
    }

    public cModule getOwnerModule() {
        checkState();
        return ownerModule;
    }

    public IDisplayString getDisplayString() {
        checkState();
        return displayString;
    }

    public cGate getPreviousGate() {
        checkState();
        return previousGate;
    }

    public cGate getNextGate() {
        checkState();
        return nextGate;
    }

    public cChannel getChannel() {
        checkState();
        return channel;
    }

    @Override
    @SuppressWarnings("rawtypes")
    protected void doFillFromJSON(Map jsonObject) {
        super.doFillFromJSON(jsonObject);

        gateId = ((Number)jsonObject.get("gateId")).intValue();
        index = ((Number)jsonObject.get("index")).intValue();
        vectorSize = ((Number)jsonObject.get("vectorSize")).intValue();
        type = Type.valueOf(((String)jsonObject.get("type")).toUpperCase());

        ownerModule = (cModule) getSimulation().getObjectByJSONRef((String) jsonObject.get("ownerModule"));
        displayString = new DisplayString((String) jsonObject.get("displayString"));
        previousGate = (cGate) getSimulation().getObjectByJSONRef((String) jsonObject.get("previousGate"));
        nextGate = (cGate) getSimulation().getObjectByJSONRef((String) jsonObject.get("nextGate"));
        channel = (cChannel) getSimulation().getObjectByJSONRef((String) jsonObject.get("channel"));
    }

    protected void clearReferences() {
        super.clearReferences();
        ownerModule = null;
        previousGate = null;
        nextGate = null;
        channel = null;
    }


}
