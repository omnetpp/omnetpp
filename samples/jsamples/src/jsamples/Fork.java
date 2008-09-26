package jsamples;

import org.omnetpp.simkernel.*;

/**
 * Duplicated messages on all outputs.
 */
public class Fork extends JSimpleModule {

    protected long counter = 0;

    public void handleMessage(cMessage msg) {
        ev.println("Received: " + msg.getName());
        counter++;

        int n = gateSize("out");
        for (int i=0; i<n; i++)
            send(cMessage.cast(msg.dup()), "out", i);
        msg.delete();
    }

    protected void finish() {
        recordScalar("msgs processed", counter);
    }
}
