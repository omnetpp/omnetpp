package org.omnetpp.ned.model.interfaces;

import java.util.Map;

import org.omnetpp.ned.model.ex.PropertyElementEx;

/**
 * Interface for NED/MSG model elements that support properties.
 * 
 * @author levy
 */
public interface IHasProperties {
    /**
     * Returns the name - property node association. 
     * The returned map must not be modified.
     */
    public Map<String, PropertyElementEx> getProperties();
}
