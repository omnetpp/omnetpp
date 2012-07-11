package org.omnetpp.simulation.model;

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
}
