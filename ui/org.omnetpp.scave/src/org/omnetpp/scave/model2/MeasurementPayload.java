package org.omnetpp.scave.model2;

public class MeasurementPayload implements Comparable<MeasurementPayload> {
    public String name;

    public MeasurementPayload(String name) {
        this.name = name;
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((name == null) ? 0 : name.hashCode());
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        MeasurementPayload other = (MeasurementPayload) obj;
        if (name == null) {
            if (other.name != null)
                return false;
        }
        else if (!name.equals(other.name))
            return false;
        return true;
    }

    public int compareTo(MeasurementPayload other) {
        return name.compareTo(other.name);
    }
}
