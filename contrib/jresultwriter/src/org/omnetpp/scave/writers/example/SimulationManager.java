package org.omnetpp.scave.writers.example;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.PriorityQueue;

import org.omnetpp.scave.writers.IOutputScalarManager;
import org.omnetpp.scave.writers.IOutputVectorManager;
import org.omnetpp.scave.writers.IResultManager;
import org.omnetpp.scave.writers.impl.FileOutputScalarManager;
import org.omnetpp.scave.writers.impl.FileOutputVectorManager;

public class SimulationManager {
    double now = 0.0;
    PriorityQueue<Event> fes = new PriorityQueue<Event>();
    List<Component> components = new ArrayList<Component>();

    IOutputScalarManager scalarManager = new FileOutputScalarManager("bubu.sca");
    IOutputVectorManager vectorManager = new FileOutputVectorManager("bubu.vec");

    public SimulationManager() {
    }

    void simulate(double timeLimit) {
        String runID = FileOutputScalarManager.generateRunID("bubu");
        Map<String,String> runAttributes = new HashMap<String, String>();
        runAttributes.put(IResultManager.ATTR_NETWORK, "BubuNetwork");

        try {
            scalarManager.open(runID, runAttributes);
            vectorManager.open(runID, runAttributes);
        }
        catch (IOException e) {
            e.printStackTrace();
        }

        while (!fes.isEmpty() && fes.peek().time < timeLimit) {
            Event event = fes.remove();
            now = event.time;
            event.execute();
        }

        for (Component c : components)
            c.recordSummaryResults();

        try {
            scalarManager.close();
            vectorManager.close();
        }
        catch (IOException e) {
            e.printStackTrace();
        }
    }

}
