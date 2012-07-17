package org.omnetpp.simulation.controller;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.common.engine.BigDecimal;

/**
 * TODO
 *
 * @author Andras
 */
//FIXME making it possible to change module name and parent after creation is a REALLY BAD IDEA -- makes our job much more difficult in the IDE!!!
public class LogBuffer {
    public static final Object[] EMPTY_ARRAY = new Object[0];
    
    public static class EventEntry {
        public long eventNumber;
        public BigDecimal simulationTime;
        public int moduleId;  
        public String moduleNedType;  // fully qualified NED type name
        public String moduleFullPath; //XXX we assume that the module's fullPath doesn't change during its lifetime
        public String messageClassName;
        public String messageName;
        public Object[] logItems = EMPTY_ARRAY; //TODO store method calls too which affect the context
    }
    
    private List<EventEntry> eventEntries = new ArrayList<EventEntry>();

    public LogBuffer() {
    }
    
    public void addEventEntry(EventEntry e) {
        eventEntries.add(e);
    }

    public int getNumEntries() {
        return eventEntries.size();
    }
    
    public EventEntry getEventEntry(int index) {
        return eventEntries.get(index);
    }
    
    public EventEntry getLastEventEntry() {
        return eventEntries.isEmpty() ? null : eventEntries.get(eventEntries.size()-1);
    }
}
