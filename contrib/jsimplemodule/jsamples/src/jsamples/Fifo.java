package jsamples;

import org.omnetpp.simkernel.*;

/**
 * Implements a single-server queue
 */
public class Fifo extends JSimpleModule
{
    protected cMessage msgServiced;
    protected cMessage endServiceMsg;
    protected cQueue queue;

    protected cOutVector qlenVector;

    protected void initialize() {
        queue = new cQueue("queue");
        qlenVector = new cOutVector("queue length");

        // add some jobs to the queue
        int n = getParentModule().par("queueNumInitialJobs").longValue();
        for (int i=0; i<n; i++) {
            cMessage msg = new cMessage("job-"+i);
            msg.setTimestamp();
            scheduleAt(simTime(), msg);
        }

        msgServiced = null;
        endServiceMsg = new cMessage("end-service");
    }

    protected void handleMessage(cMessage msg) {
        if (msg.sameAs(endServiceMsg)) {
            //
            // Departure
            //
            endService(msgServiced);
            if (queue.isEmpty()) {
                // There is no remaining customer
                msgServiced = null;
            }
            else {
                msgServiced = cMessage.cast(queue.pop());
                double serviceTime = serviceRequirement(msgServiced);
                scheduleAt(simTime().add(new SimTime(serviceTime)), endServiceMsg);
            }
        }
        else if (msgServiced==null) {
            //
            // Arrival while server is idle
            //
            arrival(msg);

            // Statistics collection
            qlenVector.record(0);

            msgServiced = msg;
            double serviceTime = serviceRequirement(msgServiced);
            scheduleAt(simTime().add(new SimTime(serviceTime)), endServiceMsg);
        }
        else {
            //
            // Arrival while server is busy
            //
            arrival(msg);

            // Statistics collection
            // There is one customer in service, hence queue.length + 1
            qlenVector.record(queue.getLength()+1);
            queue.insert(msg);
        }
    }

    protected void finish() {
        // nothing for now
    }

    protected void arrival(cMessage msg) {
        // nop
    }

    protected double serviceRequirement(cMessage msg) {
        ev.println("Starting service of "+msg.getName());
        return getParentModule().par("queueServiceTime").doubleValue();
    }

    protected void endService(cMessage msg) {
        ev.println("Completed service of "+msg.getName());
        send(msg, "out");
    }
}
