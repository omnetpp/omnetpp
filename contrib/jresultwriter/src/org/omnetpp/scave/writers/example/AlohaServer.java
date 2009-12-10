package org.omnetpp.scave.writers.example;

import org.omnetpp.scave.writers.IOutputVector;
import org.omnetpp.scave.writers.IResultManager;

public class AlohaServer extends Component {
    // state variables
    int numCurrentTransmissions = 0;
    boolean collision = false;
    double rxStartTime;

    // statistics
    long totalPackets = 0;
    double totalReceiveTime = 0; // non-collision
    double totalCollisionTime = 0;
    IOutputVector numCurrentTransmissionsVector;

    public AlohaServer(String name, SimulationManager sim, Component parent) {
        super(name, sim, parent);
        numCurrentTransmissionsVector = createOutputVector("numConcurrentTransmissions", null, null, IResultManager.IM_SAMPLE_HOLD);
        numCurrentTransmissionsVector.record(0);
    }

    public void packetReceptionStart() {
        if (numCurrentTransmissions == 0)
            rxStartTime = now();
        else
            collision = true;
        numCurrentTransmissions++;
        numCurrentTransmissionsVector.record(numCurrentTransmissions);

        totalPackets++;
    }

    protected void packetReceptionEnd() {
        numCurrentTransmissions--;
        numCurrentTransmissionsVector.record(numCurrentTransmissions);
        if (numCurrentTransmissions == 0) {
            if (collision)
                totalCollisionTime += now() - rxStartTime;
            else
                totalReceiveTime += now() - rxStartTime;
            collision = false;
        }
    }

    @Override
    protected void recordSummaryResults() {
        recordScalar("totalPackets", totalPackets);
        recordScalar("totalCollisionTime", totalCollisionTime, "s");
        recordScalar("totalReceiveTime", totalReceiveTime, "s");
        recordScalar("channelBusy", 100 * (totalReceiveTime+totalCollisionTime) / now(), "%");
        recordScalar("utilization", 100 * totalReceiveTime / now(), "%");
    }
}
