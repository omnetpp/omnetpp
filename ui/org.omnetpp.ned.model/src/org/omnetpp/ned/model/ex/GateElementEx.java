package org.omnetpp.ned.model.ex;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasIndex;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.pojo.GateElement;

/**
 * Extended GateElement
 *
 * @author rhornig
 */
public class GateElementEx extends GateElement implements IHasIndex, IHasName {

    protected GateElementEx() {
        super();
    }

    protected GateElementEx(INEDElement parent) {
        super(parent);
    }

    public String getNameWithIndex() {
        String result = getName();
        if (getIsVector())
            result += "["+ ((getVectorSize()==null) ? "" : getVectorSize())+"]";
        return result;
    }
}
