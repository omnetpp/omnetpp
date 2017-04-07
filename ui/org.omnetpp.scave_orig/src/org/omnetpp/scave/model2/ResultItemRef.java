package org.omnetpp.scave.model2;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;

/**
 * TODO
 *
 * @author tomi
 */
public class ResultItemRef {
    private long id;
    private ResultFileManager manager;

    public ResultItemRef(long id, ResultFileManager manager) {
        Assert.isLegal(id != -1);
        Assert.isNotNull(manager);
        this.id = id;
        this.manager = manager;
    }

    public long getID() {
        return id;
    }

    public ResultFileManager getResultFileManager() {
        return manager;
    }

    public ResultItem resolve() {
        return manager.getItem(id);
    }

    public boolean equals(Object other) {
        if (this == other)
            return true;
        if (other == null || getClass() != other.getClass())
            return false;
        ResultItemRef otherRef = (ResultItemRef)other;
        return id == otherRef.id && manager.equals(otherRef.manager);
    }

    public int hashCode() {
        return new Long(id).hashCode() * 31 + manager.hashCode();
    }
}
