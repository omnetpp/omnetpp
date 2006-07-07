package org.omnetpp.experimental.animation.live.model;

import org.omnetpp.experimental.animation.live.IEnvirCallback;
import org.omnetpp.experimental.animation.model.IRuntimeSimulation;

//XXX should be Java wrapper around cSimulation
public class LiveSimulation implements IRuntimeSimulation {
	private LiveModule rootModule;
	private IEnvirCallback ev;
	private int eventNumber = -1;
	private boolean finished;
	
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
	
	public boolean isFinished() {
		return finished;
	}
	
	// TODO:
	LiveModule module1 = new LiveModule(rootModule, 1);
	LiveModule module2 = new LiveModule(rootModule, 2);
	LiveGate gate1 = new LiveGate();
	LiveGate gate2 = new LiveGate();

	public void doOneEvent() {
		switch (eventNumber) {
		case -1: // initialize
			module1.addGate(gate1);
			module2.addGate(gate2);
			ev.moduleCreated(module1);
			ev.moduleCreated(module2);
			ev.connectionCreated(gate1);
			break;
		case 0:
//			animationPrimitivesToBeRead.add(new SendMessageAnimation(this, 0, 3, new ConnectionId(0, 0)));
			break;
		case 1:
			ev.bubble(rootModule, "Hello");
//			animationPrimitivesToBeRead.add(new SendMessageAnimation(this, 3, 6, new ConnectionId(1, 1)));
//			ev.messageSent(null, null);
			break;
		case 2:
			LiveMessage msg = new LiveMessage();
			msg.setSendingTime(getSimulationTime());
			msg.setArrivalTime(getSimulationTime() * 2);
			ev.messageSent(msg, gate1);
//			animationPrimitivesToBeRead.add(new SendMessageAnimation(this, 6, 9, new ConnectionId(2, 1)));
//			ev.messageSent(null, null);
			break;
		case 3:
			break;
		case 4:
			break;
		case 1000:
			finished = true;
			break;
		}
		
		if (!finished)
			eventNumber++;
	}
	
	public double getSimulationTime() {
		return eventNumber / 10.0;
	}
}
