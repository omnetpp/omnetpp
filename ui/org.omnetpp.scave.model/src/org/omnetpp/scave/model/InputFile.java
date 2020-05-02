package org.omnetpp.scave.model;

public class InputFile extends ModelObject {
    protected String name;

    public InputFile(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
        notifyListeners();
    }

    @Override
    protected InputFile clone() throws CloneNotSupportedException {
        return (InputFile)super.clone();
    }

    @Override
    public String toString() {
        return "Input \"" + name + "\"";
    }
}
