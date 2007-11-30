package org.omnetpp.ned.model.interfaces;

import java.util.Map;
import java.util.Set;

import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.pojo.FieldElement;

public interface IMsgTypeInfo {
    public ITypeElement getFirstExtendsRef();

    /** Types used locally in this type */
    public Set<IMsgTypeElement> getLocalUsedTypes();

    /** Fields declared locally in this type */
    public Map<String, FieldElement> getLocalFields();

    /** Property nodes, including inherited ones; the most recent one for each property.
     * (Given the special inheritance rules for properties, this may not be what you want;
     * see getPropertyInheritanceChain().
     */
    public Map<String, PropertyElementEx> getProperties();

    /** Fields including inherited ones */
    public Map<String, FieldElement> getFields();
}
