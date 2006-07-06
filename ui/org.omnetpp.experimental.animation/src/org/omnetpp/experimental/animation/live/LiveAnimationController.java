package org.omnetpp.experimental.animation.live;

import org.omnetpp.experimental.animation.live.model.LiveMessage;
import org.omnetpp.experimental.animation.live.model.LiveSimulation;
import org.omnetpp.experimental.animation.model.IRuntimeGate;
import org.omnetpp.experimental.animation.model.IRuntimeMessage;
import org.omnetpp.experimental.animation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.model.IRuntimeSimulation;
import org.omnetpp.experimental.animation.primitives.IAnimationPrimitive;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;

public class LiveAnimationController extends ReplayAnimationController implements IEnvirCallback {
	public LiveAnimationController(AnimationCanvas canvas) {
		super(canvas);
	}

	protected IRuntimeSimulation createSimulation() {
		return new LiveSimulation(this);
	}

	protected LiveSimulation getLiveSimulation() {
		return (LiveSimulation)simulation;
	}

	protected IAnimationPrimitive loadNextAnimationPrimitive() {
		getLiveSimulation().doOneEvent();
		
		return null;
	}

	public void breakpointHit(String lbl, IRuntimeModule mod) {
	}

	public void bubble(IRuntimeModule mod, String text) {
	}

	public void connectionCreated(IRuntimeGate srcgate) {
	}

	public void connectionRemoved(IRuntimeGate srcgate) {
	}

	public void displayStringChanged(IRuntimeGate gate) {
	}

	public void displayStringChanged(IRuntimeModule module) {
	}

	public void messageDelivered(IRuntimeMessage msg) {
	}

	public void messageSent(IRuntimeMessage msg, IRuntimeGate directToGate) {
	}

	public void moduleCreated(IRuntimeModule newmodule) {
	}

	public void moduleDeleted(IRuntimeModule module) {
	}

	public void moduleMethodCalled(IRuntimeModule from, IRuntimeModule to, String method) {
	}

	public void moduleReparented(IRuntimeModule module, IRuntimeModule oldparent) {
	}

	public void objectDeleted(LiveMessage object) {
	}
}
