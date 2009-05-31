package jsamples;

import org.omnetpp.simkernel.*;

/**
 * Just discards messages.
 */
public class Sink extends JSimpleModule {

    public void handleMessage(cMessage msg) {
        ev.println(msg.getName()+" arrived at sink");
        msg.delete();
    }

    protected void finish() {
        ev.println("finish of "+getFullPath());
    }
}
