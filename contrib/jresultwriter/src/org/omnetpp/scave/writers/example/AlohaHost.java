package org.omnetpp.scave.writers.example;


public class AlohaHost extends Component {
    Event startTxEvent;
    Event endTxEvent;
    double interarrivalTime;
    double packetDuration;
    AlohaServer alohaServer;

    int numPkSent = 0;
    
    private static double exponential(double mean) {
        return -mean * Math.log(Math.random());
    }

    public AlohaHost(String name, SimulationManager sim, Component parent, double interarrivalTime, double packetDuration, AlohaServer alohaServer) {
        super(name, sim, parent);
        this.interarrivalTime = interarrivalTime;
        this.packetDuration = packetDuration;
        this.alohaServer = alohaServer;

        startTxEvent = new Event() {
            void execute() { startTransmission(); }
        };
        endTxEvent = new Event() {
            void execute() { endTransmission(); }
        };
        
        schedule(exponential(interarrivalTime), startTxEvent);
    }

    protected void startTransmission() {
        System.out.println("t=" + now() +" " + getFullPath() + ": begin tx");
        alohaServer.packetReceptionStart();
        schedule(now() + packetDuration, endTxEvent);
    }

    protected void endTransmission() {
        System.out.println("t=" + now() +" " + getFullPath() + ": end tx");
        alohaServer.packetReceptionEnd();
        numPkSent++;
        schedule(now() + exponential(interarrivalTime), startTxEvent);
    }

    @Override
    protected void recordSummaryResults() {
        recordScalar("pkSent", numPkSent);
    }
}
