package org.omnetpp.simulation.model;

import java.util.Map;

import org.omnetpp.simulation.controller.Simulation;

public class cChannel extends cComponent {
    private boolean isTransmissionChannel;

    public cChannel(Simulation simulation, long id) {
        super(simulation, id);
    }

    public boolean isTransmissionChannel() {
        checkState();
        return isTransmissionChannel;
    }

    @Override
    @SuppressWarnings("rawtypes")
    protected void doFillFromJSON(Map jsonObject) {
        super.doFillFromJSON(jsonObject);
        isTransmissionChannel = (Boolean) jsonObject.get("isTransmissionChannel");
    }

}
