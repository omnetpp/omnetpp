package org.omnetpp.simulation.model;

import java.util.Map;

import org.omnetpp.simulation.controller.SimulationController;

public class cQueue extends cObject {
    private int size;
    private int count;

    public cQueue(SimulationController controller, long id) {
        super(controller, id);
    }

    public int getSize() {
        return size;
    }

    public int getCount() {
        return count;
    }
    
    @Override
    protected void doFillFromJSON(Map jsonObject) {
        super.doFillFromJSON(jsonObject);
        //TODO
    }
}
