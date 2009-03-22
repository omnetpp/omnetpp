package org.omnetpp.scave.writers.example;

import java.lang.management.ManagementFactory;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

import org.omnetpp.scave.writers.IResultManager;
import org.omnetpp.scave.writers.impl.FileOutputScalarManager;

public class Main {
    static final long[] GOOD_SEEDS = new long[]{634,3424,744,2525,852}; // completely made-up numbers
    
    public static void main(String[] args) {
        simulateAloha(0, 10, 5.0, 0);

/*Uncomment the following and try a full experiment:
        int runNumber = 0;
        for (int numHosts : new int[]{5, 10, 20})
            for (double interarrivalTime = 0.2; interarrivalTime < 2; interarrivalTime += 0.2)
                for (int trial=0; trial<3; trial++)
                    simulateAloha(runNumber++, numHosts, interarrivalTime, trial);
*/                 
    }

    public static void simulateAloha(int runNumber, int numHosts, double interarrivalTime, int trial) {
        System.out.println("Run #" + runNumber+ ": numHosts="+numHosts + ", interarrivalTime=" + interarrivalTime + "; trial #"+trial);

        String runID = FileOutputScalarManager.generateRunID("aloha-"+runNumber);
        Map<String, String> runAttributes = makeRunAttributes(runNumber, numHosts, interarrivalTime, trial);
        SimulationManager sim = new SimulationManager(runID, runAttributes, "./aloha-"+runNumber);

        double packetDuration = 1.0;

        Component top = new Component("top", sim, null);
        AlohaServer alohaServer = new AlohaServer("server", sim, top);
        Random random = new Random(GOOD_SEEDS[trial]);
        for (int i=0; i<numHosts; i++)
            new AlohaHost("host"+i, sim, top, interarrivalTime, packetDuration, alohaServer, random);

        sim.simulate(5000.0);
    }

    private static Map<String, String> makeRunAttributes(int runNumber, int numHosts, double interarrivalTime, int trial) {
        String iterVars = "numHosts="+numHosts + ", interarrivalTime=" + interarrivalTime;
        Map<String,String> runAttributes = new HashMap<String, String>();
        runAttributes.put(IResultManager.ATTR_NETWORK, "Aloha");
        runAttributes.put(IResultManager.ATTR_RUNNUMBER, String.valueOf(runNumber));
        runAttributes.put(IResultManager.ATTR_EXPERIMENT, "AlohaChannelUtilization");
        runAttributes.put(IResultManager.ATTR_MEASUREMENT, iterVars);
        runAttributes.put(IResultManager.ATTR_REPLICATION, "#"+trial);
        runAttributes.put(IResultManager.ATTR_DATETIME, new SimpleDateFormat("yyyyMMdd-HH:mm:ss").format(new Date()));
        runAttributes.put(IResultManager.ATTR_PROCESSID, ManagementFactory.getRuntimeMXBean().getName());
        runAttributes.put(IResultManager.ATTR_REPETITION, String.valueOf(trial));
        runAttributes.put(IResultManager.ATTR_SEEDSET, String.valueOf(trial));
        runAttributes.put(IResultManager.ATTR_ITERATIONVARS, iterVars);
        runAttributes.put(IResultManager.ATTR_ITERATIONVARS2, iterVars+"; trial #"+trial);
        runAttributes.put("numHosts", String.valueOf(numHosts));
        runAttributes.put("interarrivalTime", String.valueOf(interarrivalTime));
        return runAttributes;
    }

}
