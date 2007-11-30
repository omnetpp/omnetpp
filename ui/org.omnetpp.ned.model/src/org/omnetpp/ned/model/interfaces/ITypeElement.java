package org.omnetpp.ned.model.interfaces;

import java.util.Set;



public interface ITypeElement extends IHasName, IHasProperties {
    public String getFirstExtends();

    public ITypeElement getFirstExtendsRef();

    public Set<? extends ITypeElement> getLocalUsedTypes();
}
