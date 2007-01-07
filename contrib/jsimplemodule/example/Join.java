import org.omnetpp.simkernel.*;

public class Join extends JSimpleModule {

    public void handleMessage(cMessage msg) {
        send(msg, "out");
    }

};




