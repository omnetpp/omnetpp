package org.omnetpp.ned.model.ex;

import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IHasParent;
import org.omnetpp.ned.model.interfaces.ITopLevelElement;
import org.omnetpp.ned.model.pojo.ParamNode;

/**
 * @author rhornig
 * Extended parameter node
 */
public final class ParamNodeEx extends ParamNode implements IHasName, IHasParent {

    protected ParamNodeEx() {
        super();
    }

    protected ParamNodeEx(NEDElement parent) {
        super(parent);
    }

    /**
     * @return Which module or channel or any named alament contains this parameter definition
     */
    public NEDElement getContainingTopLevelElement() {
        NEDElement parent = getParent(); 
        while (parent != null && !(parent instanceof ITopLevelElement)) 
            parent = parent.getParent();
        return parent;
    }

}
