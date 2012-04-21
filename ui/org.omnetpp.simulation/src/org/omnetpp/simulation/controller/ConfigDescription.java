package org.omnetpp.simulation.controller;



/**
 * @author Andras
 */
public class ConfigDescription {
    public String name;
    public String description;
    public int numRuns;
    public String[] extendsList;
    
    @Override
    public int hashCode() {
        return name.hashCode() + 31*description.hashCode() + 31*31*numRuns + 31*31*31*extendsList.hashCode();
    }
    
    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null) return false;
        if (getClass() != obj.getClass()) return false;
        ConfigDescription other = (ConfigDescription) obj;
        return name.equals(other.name) && description.equals(other.description) && numRuns == other.numRuns && extendsList == other.extendsList;
    }
    
    @Override
    public String toString() {
        return "[Config " + name + "]";
    }
}
