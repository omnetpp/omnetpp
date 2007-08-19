package org.omnetpp.ned.model.interfaces;

import java.util.Map;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.pojo.ParamNode;

/**
 * Interface for acquiring parameter lists for the object
 *
 * @author rhornig
 */
public interface IHasParameters extends INEDElement {

	/**
     * Returns the name - parameter node association. The returned map must not be modified.
     * 
     * "Best-Effort": This method never returns null, but the returned list
     * may be incomplete if some NED type is incorrect, missing, or duplicate. 
     */
    public Map<String, ParamNode> getParams();

    /**
     * Returns the name - parameter node association where the last parameter - value
     * assignment was done. The returned map must not be modified.
     * 
     * "Best-Effort": This method never returns null, but the returned list
     * may be incomplete if some NED type is incorrect, missing, or duplicate. 
     */
    public Map<String, ParamNode> getParamValues();

}
