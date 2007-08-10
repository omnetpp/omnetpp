package org.omnetpp.ned.model.ex;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IHasParent;
import org.omnetpp.ned.model.interfaces.ITopLevelElement;
import org.omnetpp.ned.model.pojo.ParamNode;

/**
 * Extended parameter node
 * @author rhornig
 */
public final class ParamNodeEx extends ParamNode implements IHasName, IHasParent {

    protected ParamNodeEx() {
        super();
    }

    protected ParamNodeEx(INEDElement parent) {
        super(parent);
    }

    /**
     * Returns the module, channel or any named element that contains this parameter definition
     */
    public INEDElement getContainingTopLevelElement() {
        INEDElement parent = getParent();
        while (parent != null && !(parent instanceof ITopLevelElement))
            parent = parent.getParent();
        return parent;
    }

}
