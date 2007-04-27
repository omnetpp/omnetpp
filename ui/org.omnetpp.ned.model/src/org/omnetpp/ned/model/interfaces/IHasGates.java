package org.omnetpp.ned.model.interfaces;

import java.util.Map;

import org.omnetpp.ned.model.INEDElement;

/**
 * Elements that may contain gates
 * @author rhornig
 */
public interface IHasGates extends INEDElement {

    /**
     * @return All Gate elements
     */
    Map<String, INEDElement> getGates();

    /**
     * @return All Gate elements wheter size is specified
     */
    Map<String, INEDElement> getGateSizes();

}
