package org.omnetpp.experimental.simkernel;

import org.omnetpp.experimental.simkernel.swig.*;

public interface IEnvirCallback {
    void objectDeleted(cObject object);
    void messageSent(cMessage msg, cGate directToGate);
    void messageDelivered(cMessage msg);
    void breakpointHit(String label, cSimpleModule mod);
    void moduleMethodCalled(cModule from, cModule to, String method);
    void moduleCreated(cModule newmodule);
    void moduleDeleted(cModule module);
    void moduleReparented(cModule module, cModule oldparent);
    void connectionCreated(cGate srcgate);
    void connectionRemoved(cGate srcgate);
    void displayStringChanged(cGate gate);
    void displayStringChanged(cModule submodule);
    void backgroundDisplayStringChanged(cModule parentmodule);
}
