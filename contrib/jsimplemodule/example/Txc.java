import org.omnetpp.simkernel.*;

class Tic extends JSimpleModule {

    protected void initialize() {
        System.out.println("initialize of "+getFullPath());
        cMessage msg = new cMessage("TESTMSG-"+getFullName());
        scheduleAt(simTime() + .9, msg);
    }

    protected void handleMessage(cMessage msg) {
        System.out.println(msg.getName()+" arrived");
        send(msg, "out");
    }

    protected void finish() {
        System.out.println("finish of "+getFullPath());
    }

};




