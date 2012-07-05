package org.omnetpp.simulation.model;

/**
 * Represents a cObject in the simulation.
 * @author Andras
 */
//TODO wrap members into getters (setters are only important for SimulationController)
public class SimObject {
    public enum KnownBaseClass {
        cPlaceholderModule, cSimpleModule, cCompoundModule, cPacket, cMessage,
        cArray, cQueue, cGate, cPar, cChannel, cOutVector, cDensityEstBase, cStatistic, 
        cMessageHeap, cWatchBase, cOwnedObject, cNamedObject, cObject,
    };
    
    public long id;
    public String className;
    public String name;
    public String fullName;
    public String fullPath;
    public String icon;
    public String info;
    public long ownerId;
    public long[] childObjectIds;
    public long lastAccessSerial; // SimulationController's refreshSerial when this object was last accessed

    public SimObject(long id) {
        this.id = id;
    }
    
    @Override
    public String toString() {
        return "(" + className + ") " + fullName + " - " + info + "  [id=" + id + "]";
    }
}
