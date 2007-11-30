package org.omnetpp.ned.model.ex;

import java.util.Map;
import java.util.Set;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IMsgTypeElement;
import org.omnetpp.ned.model.interfaces.IMsgTypeInfo;
import org.omnetpp.ned.model.interfaces.ITypeElement;
import org.omnetpp.ned.model.pojo.EnumElement;

public class EnumElementEx extends EnumElement implements IMsgTypeElement {
    private IMsgTypeInfo typeInfo;

    protected EnumElementEx() {
        super();
    }

    protected EnumElementEx(INEDElement parent) {
        super(parent);
    }

    public IMsgTypeInfo getMsgTypeInfo() {
        if (typeInfo == null)
            typeInfo = getDefaultMsgTypeResolver().createTypeInfoFor(this);
        
        return typeInfo;
    }

    public String getFirstExtends() {
        String name = getExtendsName();
        
        if (name != null && !name.equals(""))
            return name;
        else
            return null;
    }

    public ITypeElement getFirstExtendsRef() {
        return getMsgTypeInfo().getFirstExtendsRef();
    }

    public Set<IMsgTypeElement> getLocalUsedTypes() {
        return getMsgTypeInfo().getLocalUsedTypes();
    }

    public Map<String, PropertyElementEx> getProperties() {
        return getMsgTypeInfo().getProperties();
    }
}
