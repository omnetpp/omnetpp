package org.omnetpp.ned.model.ex;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasIndex;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IHasParent;
import org.omnetpp.ned.model.interfaces.ITopLevelElement;
import org.omnetpp.ned.model.pojo.GateNode;

/**
 * @author rhornig
 * Extended GateNode
 */
public final class GateNodeEx extends GateNode implements IHasIndex, IHasName, IHasParent {

    protected GateNodeEx() {
        super();
    }

    protected GateNodeEx(INEDElement parent) {
        super(parent);
    }

    public String getNameWithIndex() {
        String result = getName();
        if (getIsVector())
            result += "["+ ((getVectorSize()==null) ? "" : getVectorSize())+"]";
        return result;
    }

    /**
     * @return Which module  contains this gate definition
     */
    public INEDElement getContainingTopLevelElement() {
        INEDElement parent = getParent();
        while (parent != null && !(parent instanceof ITopLevelElement))
            parent = parent.getParent();
        return parent;
    }
}
