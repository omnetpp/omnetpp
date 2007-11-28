package org.omnetpp.ned.model.interfaces;


public interface ITypeElement extends IHasName {
    public String getFirstExtends();

    public ITypeElement getFirstExtendsRef();
}
