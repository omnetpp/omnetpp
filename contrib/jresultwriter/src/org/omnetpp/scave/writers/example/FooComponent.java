package org.omnetpp.scave.writers.example;

public class FooComponent extends Component {
    private Event mySelfEvent;

    // statistics
    private int numEvents = 0;
    private double delaySqrSum = 0;

    public FooComponent(String name, SimulationManager sim, Component parent) {
        super(name, sim, parent);

        mySelfEvent = new Event() {
            void execute() {
                handleEvent(this);
            }
        };
        schedule(0, mySelfEvent);
    }

    protected void handleEvent(Event event) {
        System.out.println("t=" + now() +" at " + getFullPath());

        double delay = Math.random();
        schedule(now() + delay, mySelfEvent);

        numEvents++;
        delaySqrSum += delay * delay;
    }
    
    @Override
    protected void recordSummaryResults() {
        recordScalar("numEvents", numEvents);
        recordScalar("avgDelay", now()/numEvents, "s");
        recordScalar("sqrSumDelay", delaySqrSum, "s2");
    }
}
