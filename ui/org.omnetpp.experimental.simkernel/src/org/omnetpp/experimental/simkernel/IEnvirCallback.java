/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.simkernel;

import org.omnetpp.experimental.simkernel.swig.*;

public interface IEnvirCallback {
	boolean idle();
	void bubble(cModule mod, String text);
    void objectDeleted(cObject object);
    void messageDelivered(cMessage msg);
    void messageScheduled(cMessage msg);
    void messageCancelled(cMessage msg);
    void beginSend(cMessage msg);
    void messageSendDirect(cMessage msg, cGate toGate, double propagationDelay, double transmissionDelay);
    void messageSendHop(cMessage msg, cGate srcGate, double propagationDelay);
    void messageSendHop(cMessage msg, cGate srcGate, double propagationDelay, double transmissionDelay, double transmissionStartTime);
    void breakpointHit(String label, cModule mod);
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
