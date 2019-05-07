package org.omnetpp.scave.model;

public abstract class AnalysisItem extends AnalysisObject {
    // TODO: add ID
    protected String name;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
        notifyListeners();
    }

    @Override
    protected AnalysisItem clone() throws CloneNotSupportedException {
        return (AnalysisItem)super.clone();
    }
}
