import org.omnetpp.simkernel.*;

class TicMessage extends JMessage implements Cloneable {

    protected int counter = 0;
    protected String text = "Hello";

    public TicMessage(String name) {
        super(name);
    }

    public int getTimesRead() {
        return counter++;
    }

    public String getText() {
        return text;
    }

    public void setText(String value) {
        text = value;
    }

};




