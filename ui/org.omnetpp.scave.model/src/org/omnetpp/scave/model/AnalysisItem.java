package org.omnetpp.scave.model;

public abstract class AnalysisItem extends ModelObject {
    protected static int nextId = 1;

    protected int id = (nextId++);
    protected String name;

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
        if (id >= nextId)
            nextId = id + 1;
        notifyListeners();
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
        notifyListeners();
    }

    @Override
    protected AnalysisItem clone() throws CloneNotSupportedException {
        AnalysisItem clone = (AnalysisItem)super.clone();
        clone.setId(nextId++);
        return clone;
    }
}
