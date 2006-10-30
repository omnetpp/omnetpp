package org.omnetpp.ned2.model;

import java.util.Map;

/**
 * @author rhornig
 * Interface for aquiring parameter lists for the object
 */
public interface IParametrized {
    
    /**
     * @return The name / parameter node association. You SHUOLD NOT modify the returned
     * map.
     */
    public Map<String, NEDElement> getParams();
    
    /**
     * @return The name / parameter node association where the last parameter - value
     * assignment was done. You SHUOLD NOT modify the returned map.
     */
    public Map<String, NEDElement> getParamValues();

}
