/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.runtime.nativelibs;

import org.omnetpp.runtime.nativelibs.simkernel.*;

public interface IEnvirCallback {
    void simulationEvent(cMessage msg);
    void messageScheduled(cMessage msg);
    void messageCancelled(cMessage msg);
    void beginSend(cMessage msg);
    void messageSendDirect(cMessage msg, cGate toGate, double propagationDelay, double transmissionDelay);
    void messageSendHop(cMessage msg, cGate srcGate);
    void messageSendHop(cMessage msg, cGate srcGate, double propagationDelay, double transmissionDelay);
    void endSend(cMessage msg);
    void messageDeleted(cMessage msg);
    void moduleReparented(cModule module, cModule oldparent);
    void componentMethodBegin(cComponent from, cComponent to, String method);
    void componentMethodEnd();
    void moduleCreated(cModule newmodule);
    void moduleDeleted(cModule module);
    void gateCreated(cGate srcgate);
    void gateDeleted(cGate srcgate);
    void connectionCreated(cGate srcgate);
    void connectionDeleted(cGate srcgate);
    void displayStringChanged(cComponent component);
    void undisposedObject(cObject obj);
    void bubble(cModule mod, String text);
    String gets(String prompt, String defaultreply);
    boolean idle();
}
