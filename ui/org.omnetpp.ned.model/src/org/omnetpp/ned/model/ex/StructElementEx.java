package org.omnetpp.ned.model.ex;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IMsgTypeElement;
import org.omnetpp.ned.model.interfaces.ITypeElement;
import org.omnetpp.ned.model.pojo.StructElement;

public class StructElementEx extends StructElement implements IMsgTypeElement {
    protected StructElementEx() {
        super();
    }

    protected StructElementEx(INEDElement parent) {
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
