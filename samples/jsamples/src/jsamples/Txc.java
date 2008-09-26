package jsamples;

import org.omnetpp.simkernel.*;

/**
 * Tic-Toc component.
 */
public class Txc extends JSimpleModule {

    protected void initialize() {
        ev.println("initialize of "+getFullPath());
        if (getFullName().equals("tic")) {
            cMessage msg = new HelloMessage("msg");
            send(msg, "out");
        }
    }

    protected void handleMessage(cMessage msg) {
        ev.println(msg.getName()+" arrived");

        HelloMessage helloMsg = HelloMessage.cast(msg);
        ev.println("counter read " + helloMsg.getTimesRead() + " times");

        send(msg, "out");
    }

    protected void finish() {
        ev.println("finish of "+getFullPath());
    }
}
