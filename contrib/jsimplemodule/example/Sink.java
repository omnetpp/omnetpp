import org.omnetpp.simkernel.*;

public class Sink extends JSimpleModule {

    public Sink(long cptr) {
        super(cptr);
    }

    public void handleMessage(cMessage msg) {
        ev().println(msg.getName()+" arrived at sink");
        msg.delete();
    }

    protected void finish() {
        ev().println("finish of "+getFullPath());
    }

};




