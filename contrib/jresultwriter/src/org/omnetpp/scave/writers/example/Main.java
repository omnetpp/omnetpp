package org.omnetpp.scave.writers.example;

import java.util.HashMap;
import java.util.Map;

import org.omnetpp.scave.writers.IResultManager;
import org.omnetpp.scave.writers.impl.FileOutputScalarManager;

public class Main {
    public static void main(String[] args) {
        String runID = FileOutputScalarManager.generateRunID("bubu");
        Map<String,String> runAttributes = new HashMap<String, String>();
        runAttributes.put(IResultManager.ATTR_NETWORK, "BubuNetwork");

        SimulationManager sim = new SimulationManager("./bubu", runID, runAttributes);

        //TODO multiple simulations! experiment-measurement-replication
        Component top = new Component("top", sim, null);
        new FooComponent("alice", sim, top);
        new FooComponent("bob", sim, top);
        new FooComponent("cecil", sim, top);

        sim.simulate(5000.0);
    }
}
