package org.omnetpp.ned2.model;

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
        return null;
    }

}
