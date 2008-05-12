package org.omnetpp.ned.model.ex;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.pojo.LiteralElement;
import org.omnetpp.ned.model.pojo.PropertyElement;
import org.omnetpp.ned.model.pojo.PropertyKeyElement;

/**
 * Extended property element.
 * @author rhornig, andras
 */
public class PropertyElementEx extends PropertyElement implements IHasName {

    public PropertyElementEx() {
        super();
    }

    public PropertyElementEx(INEDElement parent) {
        super(parent);
    }
    
    /**
     * Returns the first value from the default key (named "") if exists; 
     * otherwise returns null.
     */
    public String getSimpleValue() {
        for (INEDElement child : this)
            if (child instanceof PropertyKeyElement && StringUtils.isEmpty(((PropertyKeyElement)child).getName()))
                for (INEDElement grandChild : child)
                    if (grandChild instanceof LiteralElement)
                        return ((LiteralElement)grandChild).getValue();
        return null;
    }
}

