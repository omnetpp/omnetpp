package org.omnetpp.experimental.animation.live.model;

import org.omnetpp.experimental.animation.live.IEnvirCallback;
import org.omnetpp.experimental.animation.model.IRuntimeSimulation;

//XXX should be Java wrapper around cSimulation
public class LiveSimulation extends LiveMessage implements IRuntimeSimulation {
	private LiveModule rootModule;
	private IEnvirCallback ev;
	private int eventNumber = -1;
	
	public LiveSimulation(LiveModule rootModule, IEnvirCallback ev) {
		this.rootModule = rootModule;
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
			ev.moduleCreated(new LiveModule(rootModule, 1));
			ev.moduleCreated(new LiveModule(rootModule, 2));
			break;
		case 0:
//			animationPrimitivesToBeRead.add(new SendMessageAnimation(this, 0, 3, new ConnectionId(0, 0)));
//			ev.messageSent(null, null);
			break;
		case 1:
			ev.bubble(rootModule, "Hello");
//			animationPrimitivesToBeRead.add(new SendMessageAnimation(this, 3, 6, new ConnectionId(1, 1)));
//			ev.messageSent(null, null);
			break;
		case 2:
//			animationPrimitivesToBeRead.add(new SendMessageAnimation(this, 6, 9, new ConnectionId(2, 1)));
//			ev.messageSent(null, null);
			break;
		case 3:
			break;
		case 4:
			break;
		}
		eventNumber++;
	}
	
	public double getSimulationTime() {
		return eventNumber / 100.0;
	}
}
