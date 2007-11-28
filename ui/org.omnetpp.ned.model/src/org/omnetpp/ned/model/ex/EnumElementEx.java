package org.omnetpp.ned.model.ex;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IMsgTypeElement;
import org.omnetpp.ned.model.interfaces.ITypeElement;
import org.omnetpp.ned.model.pojo.EnumElement;

public class EnumElementEx extends EnumElement implements IMsgTypeElement {
    protected EnumElementEx() {
        super();
    }

    protected EnumElementEx(INEDElement parent) {
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
