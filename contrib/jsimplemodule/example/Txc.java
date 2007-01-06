import org.omnetpp.simkernel.*;

class Txc extends JSimpleModule {

    protected void initialize() {
        ev.println("initialize of "+getFullPath());
        if (getFullName().equals("tic")) {
            cMessage msg = new TicMessage("msg");
            send(msg, "out");
        }
    }

    protected void handleMessage(cMessage msg) {
        ev.println(msg.getName()+" arrived");

        TicMessage ticMsg = (TicMessage)TicMessage.castFrom(msg);
        ev.println("counter read "+ticMsg.getTimesRead()+" times");

        send(msg, "out");
    }

    protected void finish() {
        ev.println("finish of "+getFullPath());
    }

};




