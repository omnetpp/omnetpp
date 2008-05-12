package org.omnetpp.ned.model.ex;

import java.util.HashMap;
import java.util.Map;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.pojo.ParamElement;

/**
 * Extended parameter node
 *
 * @author rhornig
 */
public class ParamElementEx extends ParamElement implements IHasName {

    protected ParamElementEx() {
        super();
    }

    protected ParamElementEx(INEDElement parent) {
        super(parent);
    }
    
    /**
     * Returns local properties of this element. The map is indexed with the
     * property name, or with name + ":" + index for properties that have 
     * a non-empty index.
     * 
     * Note about efficiency: the result is currently NOT cached, this method
     * will create a new map on each call.
     */
    public Map<String,PropertyElementEx> getLocalProperties() {
        Map<String,PropertyElementEx> result = new HashMap<String, PropertyElementEx>();
        for (INEDElement child : this) {
            if (child instanceof PropertyElementEx) {
                PropertyElementEx property = (PropertyElementEx)child;
                if (StringUtils.isEmpty(property.getIndex()))
                    result.put(property.getName(), property);
                else
                    result.put(property.getName() + ":" + property.getIndex(), property);
            }
        }
        return result;
    }
}
