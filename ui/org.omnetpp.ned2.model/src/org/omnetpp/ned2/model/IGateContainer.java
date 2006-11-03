package org.omnetpp.ned2.model;

import java.util.Map;

/**
 * @author rhornig
 * Elements that may contain gates
 */
public interface IGateContainer {

    /**
     * @return All Gate elements
     */
    Map<String, NEDElement> getGates();

    /**
     * @return All Gate elements wheter size is specified
     */
    Map<String, NEDElement> getGateSizes();

}
