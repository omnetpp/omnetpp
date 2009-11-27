package org.omnetpp.scave.model2;


public class ExperimentPayload implements Comparable<ExperimentPayload> {
    public String name;

    public ExperimentPayload(String name) {
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
        ExperimentPayload other = (ExperimentPayload) obj;
        if (name == null) {
            if (other.name != null)
                return false;
        }
        else if (!name.equals(other.name))
            return false;
        return true;
    }

    public int compareTo(ExperimentPayload other) {
        return name.compareTo(other.name);
    }
}
