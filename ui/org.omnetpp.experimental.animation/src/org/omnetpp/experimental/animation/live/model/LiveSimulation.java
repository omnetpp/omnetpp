package org.omnetpp.experimental.animation.live.model;

import java.util.ArrayList;
import java.util.HashMap;

import org.omnetpp.experimental.animation.live.IEnvirCallback;
import org.omnetpp.experimental.animation.model.IRuntimeSimulation;

//XXX should be Java wrapper around cSimulation
public class LiveSimulation implements IRuntimeSimulation {
	private LiveModule rootModule;
	private IEnvirCallback ev;
	private long eventNumber = -1;
	private boolean finished;
	private ArrayList<LiveModule> modules = new ArrayList<LiveModule>();
	private HashMap<String,LiveModule> pathToModuleMap = new HashMap<String, LiveModule>();
	
	public LiveSimulation(LiveModule rootModule, IEnvirCallback ev) {
		this.rootModule = rootModule;
	    this.ev = ev;	
	}
	
	/* to be called after module creation */
	public void addModule(LiveModule module) {
		module.setId(modules.size());
		modules.add(module);
		pathToModuleMap.put(module.getFullPath(), module);
	}

	public void removeModule(int id) {
		LiveModule module = getModuleByID(id);
		modules.set(id, null); // NOT remove(), because it'd shift elements and thus corrupt id-to-module mapping
		pathToModuleMap.remove(module.getFullPath());
	}

	public LiveModule getRootModule() {
		return rootModule;
	}

	public LiveModule getModuleByID(int id) {
		return modules.get(id);
	}

	public LiveModule getModuleByPath(String fullPath) {
		return pathToModuleMap.get(fullPath);
	}

	public long getEventNumber() {
		return eventNumber;
	}
	
	public boolean isFinished() {
		return finished;
	}
	
	// TODO:
	LiveModule module1;
	LiveModule module2;
	LiveGate gate1;
	LiveGate gate2;

	public void doOneEvent() {
		switch ((int)eventNumber) {
		case -1: // initialize
			rootModule = new LiveModule();
			LiveModule module1 = new LiveModule(rootModule);
			LiveModule module2 = new LiveModule(rootModule);
			addModule(rootModule);
			addModule(module1);
			addModule(module2);
			rootModule.setName("net");
			module1.setName("module1");
			module2.setName("module2");

			ev.moduleCreated(rootModule);
			ev.moduleCreated(module1);
			ev.moduleCreated(module2);

			gate1 = new LiveGate();
			gate2 = new LiveGate();
			module1.addGate(gate1);
			module2.addGate(gate2);
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
			msg.setArrivalTime(getSimulationTime() * 10);
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
