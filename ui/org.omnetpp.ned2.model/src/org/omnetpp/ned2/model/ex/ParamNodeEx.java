package org.omnetpp.ned2.model.ex;

import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.interfaces.INamed;
import org.omnetpp.ned2.model.interfaces.IParentable;
import org.omnetpp.ned2.model.interfaces.ITopLevelElement;
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
