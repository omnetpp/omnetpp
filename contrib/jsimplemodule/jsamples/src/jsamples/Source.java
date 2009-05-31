package jsamples;

import org.omnetpp.simkernel.*;

/**
 * Sends a message periodically.
 */
public class Source extends JSimpleModule {
    private int counter = 0;

    protected void initialize() {
        ev.println("initialize of "+getFullPath());
        cMessage msg = new cMessage("timeout-"+getFullName());
        scheduleAt(simTime().add(new SimTime(1)), msg);
    }

    protected void handleMessage(cMessage msg) {
        ev.println("send #" + ++counter);

        HelloMessage hello = new HelloMessage("msg-" + counter);
        hello.setText("I am #" + counter);
        send(hello, "out");

        ev.println("scheduling next send");
        scheduleAt(simTime().add(new SimTime(Simkernel.exponential(1))), msg);
    }

    protected void finish() {
        ev.println("finish of "+getFullPath());
    }
}
