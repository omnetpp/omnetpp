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

}
