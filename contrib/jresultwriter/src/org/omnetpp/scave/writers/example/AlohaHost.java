package org.omnetpp.scave.writers.example;

import java.util.Random;

import org.omnetpp.scave.writers.IOutputVector;
import org.omnetpp.scave.writers.IResultManager;


public class AlohaHost extends Component {
    Event startTxEvent;
    Event endTxEvent;
    double interarrivalTime;
    double packetDuration;
    AlohaServer alohaServer;
    Random rng;

    int numPkSent = 0;
    IOutputVector radioStateVector;

    private double exponential(double mean) {
        return -mean * Math.log(rng.nextDouble());
    }

    public AlohaHost(String name, SimulationManager sim, Component parent,
            double interarrivalTime, double packetDuration,
            AlohaServer alohaServer, Random rng) {
        super(name, sim, parent);
        this.interarrivalTime = interarrivalTime;
        this.packetDuration = packetDuration;
        this.alohaServer = alohaServer;
        this.rng = rng;

        radioStateVector = createOutputVector("radioState", null, null, IResultManager.IM_SAMPLE_HOLD);
        radioStateVector.record(0); //"idle"

        startTxEvent = new Event() {
            void execute() { startTransmission(); }
        };
        endTxEvent = new Event() {
            void execute() { endTransmission(); }
        };

        schedule(exponential(interarrivalTime), startTxEvent);
    }

    protected void startTransmission() {
        //System.out.println("t=" + now() +" " + getFullPath() + ": begin tx");
        alohaServer.packetReceptionStart();
        schedule(now() + packetDuration, endTxEvent);
        radioStateVector.record(1); //"tx"
    }

    protected void endTransmission() {
        //System.out.println("t=" + now() +" " + getFullPath() + ": end tx");
        alohaServer.packetReceptionEnd();
        numPkSent++;
        schedule(now() + exponential(interarrivalTime), startTxEvent);
        radioStateVector.record(0); //"idle"
    }

    @Override
    protected void recordSummaryResults() {
        recordScalar("pkSent", numPkSent);
    }
}
