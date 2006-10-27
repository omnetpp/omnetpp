package org.omnetpp.ned2.model;

import org.omnetpp.ned2.model.pojo.ParamNode;

/**
 * @author rhornig
 * Extended parameter node
 */
public class ParamNodeEx extends ParamNode implements INamed, IParentable {

    ParamNodeEx() {
        super();
    }

    ParamNodeEx(NEDElement parent) {
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
