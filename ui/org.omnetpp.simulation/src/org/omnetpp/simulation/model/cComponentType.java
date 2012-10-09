package org.omnetpp.simulation.model;

import java.util.Map;

import org.omnetpp.simulation.controller.Simulation;

/**
 * TODO
 * @author Andras
 */
public class cComponentType extends cObject {
    // note: C++ class has no useful fields to reproduce here

    public cComponentType(Simulation simulation, long id) {
        super(simulation, id);
    }

    @Override
    @SuppressWarnings("rawtypes")
    protected void doFillFromJSON(Map jsonObject) {
        super.doFillFromJSON(jsonObject);
    }
}
