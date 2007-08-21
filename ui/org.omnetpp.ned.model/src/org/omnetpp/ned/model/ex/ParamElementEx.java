package org.omnetpp.ned.model.ex;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.pojo.ParamElement;

/**
 * Extended parameter node
 *
 * @author rhornig
 */
public class ParamElementEx extends ParamElement implements IHasName {

    protected ParamElementEx() {
        super();
    }

    protected ParamElementEx(INEDElement parent) {
        super(parent);
    }
}
