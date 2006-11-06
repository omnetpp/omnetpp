package org.omnetpp.ned2.model.ex;

import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.interfaces.IIndexable;
import org.omnetpp.ned2.model.interfaces.INamed;
import org.omnetpp.ned2.model.interfaces.IParentable;
import org.omnetpp.ned2.model.interfaces.ITopLevelElement;
import org.omnetpp.ned2.model.pojo.GateNode;

/**
 * @author rhornig
 * Extended GateNode
 */
public class GateNodeEx extends GateNode implements IIndexable, INamed, IParentable {

    GateNodeEx() {
        super();
    }

    GateNodeEx(NEDElement parent) {
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
    public NEDElement getContainingTopLevelElement() {
        NEDElement parent = getParent(); 
        while (parent != null && !(parent instanceof ITopLevelElement)) 
            parent = parent.getParent();
        return parent;
    }
}
