package org.omnetpp.ned2.model.interfaces;

import java.util.Map;

import org.omnetpp.ned2.model.NEDElement;

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
