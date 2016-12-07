package org.omnetpp.scave.writers.example;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.PriorityQueue;

import org.omnetpp.scave.writers.IOutputScalarManager;
import org.omnetpp.scave.writers.IOutputVectorManager;
import org.omnetpp.scave.writers.ISimulationTimeProvider;
import org.omnetpp.scave.writers.impl.FileOutputScalarManager;
import org.omnetpp.scave.writers.impl.FileOutputVectorManager;

public class SimulationManager {
    double now = 0.0;
    PriorityQueue<Event> fes = new PriorityQueue<Event>();
    List<Component> components = new ArrayList<Component>();

    IOutputScalarManager scalarManager;
    IOutputVectorManager vectorManager;

    public SimulationManager(String runID, Map<String,String> runAttributes, String resultFilenameBase) {
        scalarManager = new FileOutputScalarManager(resultFilenameBase+".sca");
        vectorManager = new FileOutputVectorManager(resultFilenameBase+".vec");
        vectorManager.setSimtimeProvider(new ISimulationTimeProvider() {
            public long getEventNumber() { return 0; /*not counted*/ }
            public Number getSimulationTime() { return now; }
        });
        scalarManager.open(runID, runAttributes);
        vectorManager.open(runID, runAttributes);
    }

    void simulate(double timeLimit) {
        while (!fes.isEmpty() && fes.peek().time < timeLimit) {
            Event event = fes.remove();
            now = event.time;
            event.execute();
        }

        for (Component c : components)
            c.recordSummaryResults();

        scalarManager.close();
        vectorManager.close();
    }

}
