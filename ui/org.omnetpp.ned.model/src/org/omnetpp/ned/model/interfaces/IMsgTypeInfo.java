package org.omnetpp.ned.model.interfaces;

import java.util.Map;
import java.util.Set;

import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.pojo.FieldElement;

public interface IMsgTypeInfo {
    public ITypeElement getFirstExtendsRef();

    public Set<IMsgTypeElement> getLocalUsedTypes();

    public Map<String, PropertyElementEx> getProperties();

    public Map<String, FieldElement> getFields();
}
