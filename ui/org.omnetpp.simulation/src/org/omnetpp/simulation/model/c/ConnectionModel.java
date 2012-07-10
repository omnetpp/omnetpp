package org.omnetpp.simulation.model.c;

import org.omnetpp.common.displaymodel.IDisplayString;

public class ConnectionModel {
    private ModuleModel sourceModule;
    private GateModel sourceGate;
    private ModuleModel destinationModule;
    private GateModel destinationGate;
    private IDisplayString displayString;

    public ConnectionModel(ModuleModel sourceModule, GateModel sourceGate, ModuleModel destinationModule, GateModel destinationGate) {
        this.sourceModule = sourceModule;
        this.sourceGate = sourceGate;
        this.destinationModule = destinationModule;
        this.destinationGate = destinationGate;
        sourceGate.setOutgoingConnection(this);
        destinationGate.setIncomingConnection(this);
    }

    public ModuleModel getSourceModule() {
        return sourceModule;
    }

    public GateModel getSourceGate() {
        return sourceGate;
    }

    public ModuleModel getDestinationModule() {
        return destinationModule;
    }

    public GateModel getDestinationGate() {
        return destinationGate;
    }

    public IDisplayString getDisplayString() {
        return displayString;
    }
}
