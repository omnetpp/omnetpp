package org.omnetpp.scave.model;

public class Property extends ModelObject {
    protected String name;
    protected String value;

    public Property(String name, String value) {
        this.name = name;
        this.value = value;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
        notifyListeners();
    }

    public String getValue() {
        return value;
    }

    public void setValue(String value) {
        this.value = value;
        notifyListeners();
    }

    @Override
    protected Property clone() throws CloneNotSupportedException {
        return (Property)super.clone();
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (!(obj instanceof Property))
            return false;

        Property other = (Property)obj;

        if (value == null && other.value != null)
            return false;

        return name.equals(other.name) && value.equals(other.value);
    }
}
