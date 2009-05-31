package jsamples;

import org.omnetpp.simkernel.*;

/**
 * Forwards messages from several inputs to one output.
 */
public class Join extends JSimpleModule {

    public void handleMessage(cMessage msg) {
        send(msg, "out");
    }
}
