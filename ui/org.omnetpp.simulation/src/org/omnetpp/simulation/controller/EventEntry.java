package org.omnetpp.simulation.controller;

import org.omnetpp.common.engine.BigDecimal;

/**
 * TODO
 * @author Andras
 */
public class EventEntry {
    private static final Object[] EMPTY_ARRAY = new Object[0];
    public long eventNumber;
    public BigDecimal simulationTime;
    public int moduleId;  // >=1 if this is a simulation event (and not loglines from the simulator)
    public String moduleNedType;  // fully qualified NED type name
    public String moduleFullPath; //XXX we assume that the module's fullPath doesn't change during its lifetime
    public String messageClassName;
    public String messageName;
    public Object[] logItems = EMPTY_ARRAY; //TODO store method calls too which affect the context
    public boolean isEvent() { return moduleId >= 1; }

    @Override
    public String toString() {
        return "#" + eventNumber + ",t=" + simulationTime + "," + moduleFullPath + "," + messageName + "(" + messageClassName + ")";
    }
}