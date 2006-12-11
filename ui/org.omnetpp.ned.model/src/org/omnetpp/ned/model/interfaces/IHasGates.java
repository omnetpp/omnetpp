package org.omnetpp.ned.model.interfaces;

import java.util.Map;

import org.omnetpp.ned.model.NEDElement;

/**
 * @author rhornig
 * Elements that may contain gates
 */
public interface IHasGates {

    /**
     * @return All Gate elements
     */
    Map<String, NEDElement> getGates();

    /**
     * @return All Gate elements wheter size is specified
     */
    Map<String, NEDElement> getGateSizes();

}
