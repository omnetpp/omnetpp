package org.omnetpp.experimental.simkernel;

import org.omnetpp.experimental.simkernel.swig.cGate;
import org.omnetpp.experimental.simkernel.swig.cMessage;
import org.omnetpp.experimental.simkernel.swig.cModule;
import org.omnetpp.experimental.simkernel.swig.cObject;
import org.omnetpp.experimental.simkernel.swig.cSimpleModule;

public class DummyEnvirCallback implements IEnvirCallback {

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

	public void messageSent(cMessage msg, cGate directToGate) {
		System.out.println("messageSent()");
	}

	public void moduleCreated(cModule newmodule) {
		System.out.println("moduleCreated()");
	}

	public void moduleDeleted(cModule module) {
		System.out.println("moduleDeleted()");
	}

	public void moduleMethodCalled(cModule from, cModule to, String method) {
		System.out.println("moduleDeleted()");
	}

	public void moduleReparented(cModule module, cModule oldparent) {
		System.out.println("moduleDeleted()");
	}

	public void objectDeleted(cObject object) {
		System.out.println("moduleDeleted()");
	}

}
