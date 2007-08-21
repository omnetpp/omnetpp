package org.omnetpp.ned.model.interfaces;

import java.util.Map;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.pojo.GateElement;

/**
 * Elements that may contain gates
 *
 * @author rhornig
 */
public interface IHasGates extends INEDElement {

    /**
     * Returns the list of all gate elements. 
     * 
     * "Best-Effort": This method never returns null, but the returned list
     * may be incomplete if some NED type is incorrect, missing, or duplicate. 
     */
    Map<String, GateElement> getGateDeclarations();

    /**
     * Returns the list of all gate elements where size is specified.
     * 
     * "Best-Effort": This method never returns null, but the returned list
     * may be incomplete if some NED type is incorrect, missing, or duplicate. 
     */
    Map<String, GateElement> getGateSizes();

}
