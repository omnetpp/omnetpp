package org.omnetpp.simulation.controller;

/**
 * Represents a cObject in the simulation.
 * @author Andras
 */
public class SimObject {
    public long id;
    public String className;
    public String name;
    public String fullName;
    public String fullPath;
    public String icon;
    public String info;
    public long ownerId;
    public long[] childObjectIds;

    public SimObject(long id) {
        this.id = id;
    }
    
    @Override
    public String toString() {
        return "(" + className + ") " + fullName + " - " + info + "  [id=" + id + "]";
    }
}
