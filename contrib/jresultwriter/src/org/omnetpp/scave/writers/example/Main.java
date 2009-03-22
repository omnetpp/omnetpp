package org.omnetpp.scave.writers.example;

import java.util.HashMap;
import java.util.Map;

import org.omnetpp.scave.writers.IResultManager;
import org.omnetpp.scave.writers.impl.FileOutputScalarManager;

public class Main {
//    public static void main(String[] args) {
//        String runID = FileOutputScalarManager.generateRunID("bubu");
//        Map<String,String> runAttributes = new HashMap<String, String>();
//        runAttributes.put(IResultManager.ATTR_NETWORK, "BubuNetwork");
//
//        SimulationManager sim = new SimulationManager("./bubu", runID, runAttributes);
//
//        //TODO multiple simulations! experiment-measurement-replication
//        Component top = new Component("top", sim, null);
//        new FooComponent("alice", sim, top);
//        new FooComponent("bob", sim, top);
//        new FooComponent("cecil", sim, top);
//
//        sim.simulate(5000.0);
//    }


    public static void main(String[] args) {
        //TODO multiple simulations! experiment-measurement-replication
        int n = 20;
        double interarrivalTime = 10;
        double packetDuration = 1;

        String runID = FileOutputScalarManager.generateRunID("aloha");
        Map<String,String> runAttributes = new HashMap<String, String>();
        runAttributes.put(IResultManager.ATTR_NETWORK, "AlohaNetwork");
        SimulationManager sim = new SimulationManager("./aloha", runID, runAttributes);

        Component top = new Component("top", sim, null);
        AlohaServer alohaServer = new AlohaServer("server", sim, top);
        for (int i=0; i<n; i++)
            new AlohaHost("host"+i, sim, top, interarrivalTime, packetDuration, alohaServer);

        sim.simulate(5000.0);
    }

}
