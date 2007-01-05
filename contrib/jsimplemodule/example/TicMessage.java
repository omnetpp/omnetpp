import org.omnetpp.simkernel.*;

class TicMessage extends JMessage {

    protected int counter = 0;

    public TicMessage(String name) {
        super(name);
    }

    public int getTimesRead() {
        return counter++;
    }

};




