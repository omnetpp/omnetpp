package org.omnetpp.ned.model.interfaces;

import java.util.Map;

import org.omnetpp.ned.model.ex.PropertyElementEx;

public interface IHasProperties {
    /**
     * Returns the name - property node association. The returned map must not be modified.
     */
    public Map<String, PropertyElementEx> getProperties();
}
