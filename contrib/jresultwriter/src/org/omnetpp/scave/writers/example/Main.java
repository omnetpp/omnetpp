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
    static final long[] GOOD_SEEDS = new long[]{634,3424,744,2525,852}; // completely made up
    
    public static void main(String[] args) {
        int runNumber = 0;
        for (int numHosts : new int[]{5, 10, 20})
            for (double interarrivalTime = 0.2; interarrivalTime < 2; interarrivalTime += 0.2)
                for (int trial=0; trial<3; trial++)
                    simulateAloha(runNumber++, numHosts, interarrivalTime, trial);
    }

    public static void simulateAloha(int runNumber, int numHosts, double interarrivalTime, int trial) {
        System.out.println("Run #" + runNumber+ ": numHosts="+numHosts + ", interarrivalTime="+interarrivalTime+", trial "+trial);
        double packetDuration = 1.0;

        String runID = FileOutputScalarManager.generateRunID("aloha-"+runNumber);
        Map<String,String> runAttributes = new HashMap<String, String>();
        runAttributes.put(IResultManager.ATTR_NETWORK, "Aloha");
        runAttributes.put(IResultManager.ATTR_RUNNUMBER, ""+runNumber);
        runAttributes.put(IResultManager.ATTR_EXPERIMENT, "");
        runAttributes.put(IResultManager.ATTR_MEASUREMENT, "");
        runAttributes.put(IResultManager.ATTR_REPLICATION, ""+trial);        runAttributes.put(IResultManager.ATTR_DATETIME, new SimpleDateFormat("yyyyMMdd-HH:mm:ss").format(new Date()));
        runAttributes.put(IResultManager.ATTR_PROCESSID, ManagementFactory.getRuntimeMXBean().getName());
        runAttributes.put(IResultManager.ATTR_REPETITION, ""+trial);
        runAttributes.put(IResultManager.ATTR_SEEDSET, ""+trial);
        runAttributes.put(IResultManager.ATTR_ITERATIONVARS, "");
        runAttributes.put(IResultManager.ATTR_ITERATIONVARS2, "");
        
        SimulationManager sim = new SimulationManager(runID, runAttributes, "./aloha-"+runNumber);

        Component top = new Component("top", sim, null);
        AlohaServer alohaServer = new AlohaServer("server", sim, top);
        Random random = new Random(GOOD_SEEDS[trial]);
        for (int i=0; i<numHosts; i++)
            new AlohaHost("host"+i, sim, top, interarrivalTime, packetDuration, alohaServer, random);

        sim.simulate(5000.0);
    }

}
