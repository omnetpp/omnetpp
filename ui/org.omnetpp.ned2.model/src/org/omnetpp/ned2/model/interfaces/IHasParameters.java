package org.omnetpp.ned2.model.interfaces;

import java.util.Map;

import org.omnetpp.ned2.model.NEDElement;

/**
 * @author rhornig
 * Interface for aquiring parameter lists for the object
 */
public interface IHasParameters {
    
    /**
     * @return The name / parameter node association. You SHOULD NOT modify the returned
     * map.
     */
    public Map<String, NEDElement> getParams();
    
    /**
     * @return The name / parameter node association where the last parameter - value
     * assignment was done. You SHOULD NOT modify the returned map.
     */
    public Map<String, NEDElement> getParamValues();

}
