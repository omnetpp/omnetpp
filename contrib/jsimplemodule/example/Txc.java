import org.omnetpp.simkernel.*;

class Txc extends JSimpleModule {

    protected void initialize() {
        ev().println("initialize of "+getFullPath());
        if (getFullName().equals("tic")) {
            cMessage msg = new cMessage("msg");
            scheduleAt(simTime() + .9, msg);
        }
    }

    protected void handleMessage(cMessage msg) {
        ev().println(msg.getName()+" arrived");
        send(msg, "out");
    }

    protected void finish() {
        ev().println("finish of "+getFullPath());
    }

};




