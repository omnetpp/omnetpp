package org.omnetpp.simulation.model;

import java.io.IOException;

import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.SimulationController;


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

    public boolean isCached() {
        return controller.isCachedObject(id);
    }
    
    /**
     * Returns null for invalid and stale IDs (IDs that refer to deleted objects); throws exception
     * if there was an error retrieving the object
     */
    public SimObject get() throws IOException {
        return controller.getCachedObject(id);
    }

    /**
     * Like get(), but exceptions are caught and null is returned instead.
     */
    public SimObject safeGet() {
        try {
            return controller.getCachedObject(id);
        }
        catch (IOException e) {
            SimulationPlugin.logError("Could not retrieve object #" + id + " from simulation process", e);
            return null;
        }
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
        try {
            return "#" + id + " " + get(); // note: get() may return null
        }
        catch (IOException e) {
            return "#" + id + " <ERROR>";
        }  
    }
}
