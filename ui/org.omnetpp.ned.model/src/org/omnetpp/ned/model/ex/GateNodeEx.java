package org.omnetpp.ned.model.ex;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasIndex;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.INedTypeNode;
import org.omnetpp.ned.model.pojo.GateNode;

/**
 * Extended GateNode
 *
 * @author rhornig
 */
public class GateNodeEx extends GateNode implements IHasIndex, IHasName {

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
     * Returns the module that contains this gate definition
     */
    public INedTypeNode getContainingNedTypeNode() {
        INEDElement parent = getParent();
        while (parent != null && !(parent instanceof INedTypeNode))
            parent = parent.getParent();
        return (INedTypeNode)parent;
    }
}
