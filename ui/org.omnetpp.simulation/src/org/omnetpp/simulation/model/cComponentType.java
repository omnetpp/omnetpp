package org.omnetpp.simulation.model;

import java.util.Map;

import org.omnetpp.simulation.controller.SimulationController;

/**
 * TODO
 * @author Andras
 */
public class cComponentType extends cObject {
    // note: C++ class has no useful fields to reproduce here
    
    public cComponentType(SimulationController controller, long id) {
        super(controller, id);
    }

    @Override
    protected void doFillFromJSON(Map jsonObject) {
        super.doFillFromJSON(jsonObject);
    }
}
