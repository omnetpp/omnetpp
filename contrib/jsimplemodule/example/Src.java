import org.omnetpp.simkernel.*;

public class Src extends JSimpleModule {
    private int i = 0;

/*
    public Src(long cptr) {
        super(cptr);
    }
*/

    protected void initialize() {
        ev().println("initialize of "+getFullPath());
        cMessage msg = new cMessage("timeout-"+getFullName());
        scheduleAt(simTime() + 1, msg);
    }

    protected void handleMessage(cMessage msg) {
        String msgname = "msg-" + i++;
        ev().println("sending "+msgname);
        send(new cMessage(msgname), "out");
        ev().println("scheduling next send");
        scheduleAt(simTime() + Simkernel.exponential(1), msg);
    }

    protected void finish() {
        ev().println("finish of "+getFullPath());
    }

};





