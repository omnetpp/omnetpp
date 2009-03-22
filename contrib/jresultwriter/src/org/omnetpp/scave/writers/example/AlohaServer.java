package org.omnetpp.scave.writers.example;

import org.omnetpp.scave.writers.IOutputVector;

public class AlohaServer extends Component {
    // state variables, event pointers
    int numCurrentTransmissions = 0;

    // statistics
    IOutputVector numCurrentTransmissionsVector;

    
    // statistics
    //XXX unused
    long totalFrames;
    long collidedFrames;
    double totalReceiveTime;
    double totalCollisionTime;
    double currentChannelUtilization;

    IOutputVector collisionMultiplicityVector;
    IOutputVector collisionLengthVector;
    IOutputVector channelUtilizationVector;
    
    public AlohaServer(String name, SimulationManager sim, Component parent) {
        super(name, sim, parent);
        numCurrentTransmissionsVector = createOutputVector("numTransmissions");
        numCurrentTransmissionsVector.record(0);
    }

    public void packetReceptionStart() {
        numCurrentTransmissions++;
        numCurrentTransmissionsVector.record(numCurrentTransmissions);
    }

    protected void packetReceptionEnd() {
        numCurrentTransmissions--;
        numCurrentTransmissionsVector.record(numCurrentTransmissions);
    }

    @Override
    protected void recordSummaryResults() {
        //TODO
    }
}
