package org.omnetpp.ned.model.ex;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IMsgTypeElement;
import org.omnetpp.ned.model.interfaces.ITypeElement;
import org.omnetpp.ned.model.pojo.MessageElement;

public class MessageElementEx extends MessageElement implements IMsgTypeElement {
    protected MessageElementEx() {
        super();
    }

    protected MessageElementEx(INEDElement parent) {
        super(parent);
    }

    public String getFirstExtends() {
        return NEDElementUtilEx.getFirstExtends(this);
    }

    public ITypeElement getFirstExtendsRef() {
        // TODO:
        return null;
    }
}
