package org.omnetpp.common.eventlog;

import org.omnetpp.common.displaymodel.IDisplayString;

public class EventLogConnection {
    private EventLogModule sourceModule;
    private EventLogGate sourceGate;
    private EventLogModule destinationModule;
    private EventLogGate destinationGate;
    private IDisplayString displayString;

    public EventLogConnection(EventLogModule sourceModule, EventLogGate sourceGate, EventLogModule destinationModule, EventLogGate destinationGate) {
        this.sourceModule = sourceModule;
        this.sourceGate = sourceGate;
        this.destinationModule = destinationModule;
        this.destinationGate = destinationGate;
    }

    public EventLogModule getSourceModule() {
        return sourceModule;
    }

    public EventLogGate getSourceGate() {
        return sourceGate;
    }

    public EventLogModule getDestinationModule() {
        return destinationModule;
    }

    public EventLogGate getDestinationGate() {
        return destinationGate;
    }

    public IDisplayString getDisplayString() {
        return displayString;
    }
}
