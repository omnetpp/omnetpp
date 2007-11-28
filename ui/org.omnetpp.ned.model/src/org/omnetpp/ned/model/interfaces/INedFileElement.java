package org.omnetpp.ned.model.interfaces;

import java.util.List;

public interface INedFileElement {
    public List<? extends ITypeElement> getTopLevelTypeNodes();
}
