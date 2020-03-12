package org.omnetpp.scave.engineext;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.engine.ResultFileManager;

public class ResultFileManagerChangeEvent {

    public enum ChangeType {
        LOAD,
        UNLOAD,
    }

    private ResultFileManager manager;
    private ChangeType change;
    private String filename;

    public ResultFileManagerChangeEvent(ResultFileManager manager, ChangeType change, String filename)
    {
        Assert.isLegal(manager != null);
        this.manager = manager;
        this.change = change;
        this.filename = filename;
    }

    public ResultFileManager getResultFileManager() {
        return manager;
    }

    public ChangeType getChangeType() {
        return change;
    }

    public String getFilename() {
        return filename;
    }
}
