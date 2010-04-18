package jsamples;

import org.omnetpp.simkernel.JSimpleModule;
import org.omnetpp.simkernel.SimTime;
import org.omnetpp.simkernel.cObjectFactory;
import org.omnetpp.simkernel.cMessage;

public class Node extends JSimpleModule {

    protected void initialize() {
        scheduleAt(new SimTime(1), new cMessage("timer"));
    }

    protected void handleMessage(cMessage msg) {
        if (msg.getClassName().equals("IPHeader")) {
            ev.println("source=" + msg.getField("source"));
            ev.println("destination=" + msg.getField("destination"));
            ev.println("extrainfo=" + msg.getField("extrainfo"));
        }
        msg.delete();

        cMessage pk = cMessage.cast(cObjectFactory.createOne("IPHeader"));
        pk.setField("source", "1");
        pk.setField("destination", "2");
        pk.setField("extrainfo", "bla");
        send(pk, "out");
    }
}
