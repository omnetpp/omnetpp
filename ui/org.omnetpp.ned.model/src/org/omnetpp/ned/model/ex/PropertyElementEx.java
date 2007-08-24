package org.omnetpp.ned.model.ex;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.pojo.PropertyElement;

/**
 * Extended property element.
 * @author rhornig
 */
public class PropertyElementEx extends PropertyElement implements IHasName {

    public PropertyElementEx() {
        super();
    }

    public PropertyElementEx(INEDElement parent) {
        super(parent);
    }
}

