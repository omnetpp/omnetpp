package org.omnetpp.experimental.animation.live.model;

import org.omnetpp.experimental.animation.live.IEnvirCallback;
import org.omnetpp.experimental.animation.model.IRuntimeSimulation;
import org.omnetpp.experimental.animation.replay.model.ReplayModule;

//XXX should be Java wrapper around cSimulation
public class LiveSimulation extends LiveObject implements IRuntimeSimulation {
	private LiveModule rootModule;
	private IEnvirCallback ev;
	private int eventNumber = -1;
	
	public LiveSimulation(IEnvirCallback ev) {
	    this.ev = ev;	
	}
	
	public LiveModule getRootModule() {
		return rootModule;
	}

	public LiveModule getModuleByID(String fullPath) {
		return null; //TODO
	}

	public LiveModule getModuleByPath(String fullPath) {
		return null; //TODO
	}

	public int getEventNumber() {
		return eventNumber;
	}
	
	public void doOneEvent() {
		switch (eventNumber) {
		case -1: // initialize
			ev.moduleCreated(null);
			ev.moduleCreated(null);
			ev.moduleCreated(null);
			break;
		case 0:
			ev.messageSent(null, null);
			break;
		case 1:
			ev.messageSent(null, null);
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		}
		eventNumber++;
	}
}
