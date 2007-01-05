import org.omnetpp.simkernel.*;

class TicMessage extends JMessage {

    protected int counter = 0;

    int getTimesRead() {
        return counter++;
    }

};




