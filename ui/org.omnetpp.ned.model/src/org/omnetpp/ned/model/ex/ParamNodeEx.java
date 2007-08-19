package org.omnetpp.ned.model.ex;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasName;
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
}
