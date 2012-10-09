package org.omnetpp.simulation.model;

import java.util.Map;

import org.omnetpp.simulation.controller.Simulation;

public class cQueue extends cObject {
    private int size;
    private int count;

    public cQueue(Simulation simulation, long id) {
        super(simulation, id);
    }

    public int getSize() {
        return size;
    }

    public int getCount() {
        return count;
    }

    @Override
    @SuppressWarnings("rawtypes")
    protected void doFillFromJSON(Map jsonObject) {
        super.doFillFromJSON(jsonObject);
        //TODO
    }
}
