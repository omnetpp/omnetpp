//package org.omnetpp.simulation.model;
//
//import org.omnetpp.common.displaymodel.IDisplayString;
//
//public class ConnectionModel {
//    private cModule sourceModule;
//    private cGate sourceGate;
//    private cModule destinationModule;
//    private cGate destinationGate;
//    private IDisplayString displayString;
//
//    public ConnectionModel(cModule sourceModule, cGate sourceGate, cModule destinationModule, cGate destinationGate) {
//        this.sourceModule = sourceModule;
//        this.sourceGate = sourceGate;
//        this.destinationModule = destinationModule;
//        this.destinationGate = destinationGate;
//        sourceGate.setOutgoingConnection(this);
//        destinationGate.setIncomingConnection(this);
//    }
//
//    public cModule getSourceModule() {
//        return sourceModule;
//    }
//
//    public cGate getSourceGate() {
//        return sourceGate;
//    }
//
//    public cModule getDestinationModule() {
//        return destinationModule;
//    }
//
//    public cGate getDestinationGate() {
//        return destinationGate;
//    }
//
//    public IDisplayString getDisplayString() {
//        return displayString;
//    }
//}
