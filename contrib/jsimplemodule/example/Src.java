import org.omnetpp.simkernel.*;

public class Src extends JSimpleModule {

    public Src(long cptr) {
        super(cptr);
    }

    protected void initialize() {
        ev().println("initialize of "+getFullPath());
        cMessage msg = new cMessage("timeout-"+getFullName());
        scheduleAt(simTime() + 1, msg);
    }

    protected void handleMessage(cMessage msg) {
        ev().println("sending...");
        send(new cMessage("Hello"), "out");
        scheduleAt(simTime() + Simkernel.exponential(1), msg);
    }

    protected void finish() {
        ev().println("finish of "+getFullPath());
    }

};




