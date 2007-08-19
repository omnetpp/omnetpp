package org.omnetpp.ned.model.ex;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.INedTypeNode;
import org.omnetpp.ned.model.pojo.ParamNode;

/**
 * Extended parameter node
 *
 * @author rhornig
 */
public class ParamNodeEx extends ParamNode implements IHasName {

    protected ParamNodeEx() {
        super();
    }

    protected ParamNodeEx(INEDElement parent) {
        super(parent);
    }

    /**
     * Returns the module, channel or any named element that contains this parameter definition
     */
    public INedTypeNode getContainingNedTypeNode() {
        INEDElement parent = getParent();
        while (parent != null && !(parent instanceof INedTypeNode))
            parent = parent.getParent();
        return (INedTypeNode)parent;
    }

}
