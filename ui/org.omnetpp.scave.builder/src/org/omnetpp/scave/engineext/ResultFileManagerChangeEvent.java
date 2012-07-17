package org.omnetpp.scave.engineext;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.engine.ResultFileManager;

public class ResultFileManagerChangeEvent {

    public enum ChangeType {
        LOAD,
        UNLOAD,
        COMPUTED_ITEM_ADDED,
        COMPUTED_SCALARS_CLEARED,
    }

    private ResultFileManager manager;
    private ChangeType change;
    private String filename;
    private long computedItemID;

    public ResultFileManagerChangeEvent(ResultFileManager manager, ChangeType change, String filename)
    {
        Assert.isLegal(manager != null);
        Assert.isLegal(change == ChangeType.LOAD || change == ChangeType.UNLOAD);

        this.manager = manager;
        this.change = change;
        this.filename = filename;
    }

    public ResultFileManagerChangeEvent(ResultFileManager manager, ChangeType change, long computedItemID)
    {
        Assert.isLegal(manager != null);
        Assert.isLegal(change == ChangeType.COMPUTED_ITEM_ADDED);
        this.manager = manager;
        this.change = change;
        this.computedItemID = computedItemID;
    }

    public ResultFileManagerChangeEvent(ResultFileManager manager, ChangeType change)
    {
        Assert.isLegal(manager != null);
        Assert.isLegal(change == ChangeType.COMPUTED_SCALARS_CLEARED);
        this.manager = manager;
        this.change = change;
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

    public long getComputedItemID() {
        return computedItemID;
    }
}
