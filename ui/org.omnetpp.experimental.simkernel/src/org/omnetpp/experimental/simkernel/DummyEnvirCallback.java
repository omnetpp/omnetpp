/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.simkernel;

import org.omnetpp.experimental.simkernel.swig.cGate;
import org.omnetpp.experimental.simkernel.swig.cMessage;
import org.omnetpp.experimental.simkernel.swig.cModule;
import org.omnetpp.experimental.simkernel.swig.cObject;

public class DummyEnvirCallback implements IEnvirCallback {
	public boolean idle() {
		System.out.println("idle()");
		return false;
	}

	public void bubble(cModule mod, String text) {
		System.out.println("bubble()");
	}

	public void backgroundDisplayStringChanged(cModule parentmodule) {
		System.out.println("backgroundDisplayStringChanged()");
	}

	public void breakpointHit(String label, cModule mod) {
		System.out.println("breakpointHit()");
	}

	public void connectionCreated(cGate srcgate) {
		System.out.println("connectionCreated()");
	}

	public void connectionRemoved(cGate srcgate) {
		System.out.println("connectionRemoved()");
	}

	public void displayStringChanged(cGate gate) {
		System.out.println("displayStringChanged()");
	}

	public void displayStringChanged(cModule submodule) {
		System.out.println("displayStringChanged()");
	}

	public void messageDelivered(cMessage msg) {
		System.out.println("messageDelivered()");
	}

	public void moduleCreated(cModule newmodule) {
		System.out.println("moduleCreated()");
	}

	public void moduleDeleted(cModule module) {
		System.out.println("moduleDeleted()");
	}

	public void moduleMethodCalled(cModule from, cModule to, String method) {
		System.out.println("moduleMethodCalled()");
	}

	public void moduleReparented(cModule module, cModule oldparent) {
		System.out.println("moduleReparented()");
	}

	public void objectDeleted(cObject object) {
		System.out.println("objectDeleted()");
	}

	public void beginSend(cMessage msg) {
		System.out.println("beginSend()");
	}

	public void messageCancelled(cMessage msg) {
		System.out.println("messageCancelled()");
	}

	public void messageScheduled(cMessage msg) {
		System.out.println("messageScheduled()");
	}

	public void messageSendDirect(cMessage msg, cGate toGate, double propagationDelay, double transmissionDelay) {
		System.out.println("messageSendDirect()");
	}

	public void messageSendHop(cMessage msg, cGate srcGate, double propagationDelay) {
		System.out.println("messageSendHop1()");
	}

	public void messageSendHop(cMessage msg, cGate srcGate, double propagationDelay, double transmissionDelay, double transmissionStartTime) {
		System.out.println("messageSendHop3()");
	}
}
