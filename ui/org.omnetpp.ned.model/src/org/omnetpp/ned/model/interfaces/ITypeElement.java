package org.omnetpp.ned.model.interfaces;

import java.util.Set;

/**
 * Common base class for INedTypeElement and IMsgTypeElement.
 * This is chiefly needed so that we can handle NED and MSG stuff
 * during documentation generation (neddoc) in a uniform way.
 * 
 * @author levy
 */
public interface ITypeElement extends IHasName, IHasProperties {
    public String getFirstExtends();

    public ITypeElement getFirstExtendsRef();

    public Set<? extends ITypeElement> getLocalUsedTypes();
}
