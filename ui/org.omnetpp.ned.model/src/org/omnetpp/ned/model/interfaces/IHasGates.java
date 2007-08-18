package org.omnetpp.ned.model.interfaces;

import java.util.Map;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.pojo.GateNode;

/**
 * Elements that may contain gates
 *
 * @author rhornig
 */
public interface IHasGates extends INEDElement {

    /**
     * Returns the list of all gate elements
     */
    Map<String, GateNode> getGates();

    /**
     * Returns the list of all gate elements where size is specified
     */
    Map<String, GateNode> getGateSizes();

}
