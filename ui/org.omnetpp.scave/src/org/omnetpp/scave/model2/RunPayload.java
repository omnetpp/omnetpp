package org.omnetpp.scave.model2;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.engine.Run;

public class RunPayload {
    private String runName;
    private int runNumber;

    public RunPayload(Run run) {
        Assert.isNotNull(run.getRunName());
        this.runName = run.getRunName();
        this.runNumber = run.getRunNumber();
    }

    public String getRunName() {
        return runName;
    }

    public int getRunNumber() {
        return runNumber;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null || getClass() != obj.getClass())
            return false;

        RunPayload other = (RunPayload) obj;
        return runName.equals(other.runName) && runNumber == other.runNumber;
    }

    @Override
    public int hashCode() {
        return 31 * runName.hashCode() + runNumber;
    }
}
