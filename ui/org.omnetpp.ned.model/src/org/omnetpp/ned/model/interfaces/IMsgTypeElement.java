package org.omnetpp.ned.model.interfaces;

import java.util.Map;

import org.omnetpp.ned.model.pojo.FieldElement;

public interface IMsgTypeElement extends ITypeElement {
    /**
     * Returns the name - field node association. The returned map must not be modified.
     */
    public Map<String, FieldElement> getFields();
}
