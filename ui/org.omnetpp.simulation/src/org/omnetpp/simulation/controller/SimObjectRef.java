package org.omnetpp.simulation.controller;


/**
 * Reference to a cObject in the simulation. Should be used instead of 
 * SimObject pointers in most places.
 * 
 * @author Andras
 */
public class SimObjectRef {
    public SimulationController controller;
    public long id;
    
    public SimObjectRef(long id, SimulationController controller) {
        this.controller = controller;
        this.id = id;
    }

    /**
     * Returns null for invalid and state IDs (IDs that refer to deleted objects)
     */
    public SimObject get() {
        return controller.getCachedObject(id);
    }

    public static SimObjectRef[] wrap(long[] ids, SimulationController controller) {
        SimObjectRef[] result = new SimObjectRef[ids.length];
        for (int i = 0; i < ids.length; i++)
            result[i] = new SimObjectRef(ids[i], controller);
        return result;
    }
    
    @Override
    public int hashCode() {
        return (int) (id ^ (id >>> 32)) + 31*controller.hashCode();
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        SimObjectRef other = (SimObjectRef) obj;
        return controller == other.controller && id == other.id;
    }

    @Override
    public String toString() {
        return "#" + id + " " + get();  // note: get() may return null
    }
}
