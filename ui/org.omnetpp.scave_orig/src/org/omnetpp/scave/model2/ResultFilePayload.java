package org.omnetpp.scave.model2;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.engine.ResultFile;

public class ResultFilePayload {
    private String filePath;

    public ResultFilePayload(ResultFile resultFile) {
        Assert.isNotNull(resultFile.getFilePath());
        this.filePath = resultFile.getFilePath();
    }

    public String getFilePath() {
        return filePath;
    }

    @Override
    public boolean equals(Object other) {
        if (this == other)
            return true;
        if (other == null || getClass() != other.getClass())
            return false;
        return filePath.equals(((ResultFilePayload)other).filePath);
    }

    @Override
    public int hashCode() {
        return filePath.hashCode();
    }
}
