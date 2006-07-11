package org.omnetpp.experimental.animation.live;

import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.experimental.animation.live.model.LiveMessage;
import org.omnetpp.experimental.animation.live.model.LiveModule;
import org.omnetpp.experimental.animation.live.model.LiveSimulation;
import org.omnetpp.experimental.animation.model.ConnectionId;
import org.omnetpp.experimental.animation.model.GateId;
import org.omnetpp.experimental.animation.model.IRuntimeGate;
import org.omnetpp.experimental.animation.model.IRuntimeMessage;
import org.omnetpp.experimental.animation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.model.IRuntimeSimulation;
import org.omnetpp.experimental.animation.primitives.BubbleAnimation;
import org.omnetpp.experimental.animation.primitives.CreateConnectionAnimation;
import org.omnetpp.experimental.animation.primitives.CreateModuleAnimation;
import org.omnetpp.experimental.animation.primitives.HandleMessageAnimation;
import org.omnetpp.experimental.animation.primitives.IAnimationPrimitive;
import org.omnetpp.experimental.animation.primitives.SendBroadcastAnimation;
import org.omnetpp.experimental.animation.primitives.SendMessageAnimation;
import org.omnetpp.experimental.animation.primitives.SetDisplayStringAnimation;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;

public class LiveAnimationController extends ReplayAnimationController implements IEnvirCallback {
	public LiveAnimationController(AnimationCanvas canvas) {
		super(canvas, null);
		initializeSimulation(new LiveModule());
	}

	public double getLiveSimulationTime() {
		return getLiveSimulation().getSimulationTime();
	}

	public long getLiveEventNumber() {
		return getLiveSimulation().getEventNumber();
	}

	public long getEventNumber() {
		// TODO: temporarily
		return getLiveEventNumber();
	}
	
	public void breakpointHit(String lbl, IRuntimeModule mod) {
	}

	public void bubble(IRuntimeModule mod, String text) {
		animationPrimitives.add(new BubbleAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), text, new Point(0, 0)));
	}

	public void connectionCreated(IRuntimeGate srcgate) {
		animationPrimitives.add(new CreateConnectionAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), new GateId(srcgate.getOwnerModule().getId(), srcgate.getId()), null));
	}

	public void connectionRemoved(IRuntimeGate srcgate) {
	}

	public void displayStringChanged(IRuntimeGate gate) {
	}

	public void displayStringChanged(IRuntimeModule module) {
		animationPrimitives.add(new SetDisplayStringAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), module, module.getDisplayString().toString()));
	}

	public void messageDelivered(IRuntimeMessage msg) {
		animationPrimitives.add(new HandleMessageAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), simulation.getModuleByID(msg.getArrivalModuleId()), msg));
	}

	public void messageSent(IRuntimeMessage msg, IRuntimeGate directToGate) {
		// TODO: remove this test stuff
		animationPrimitives.add(new SendBroadcastAnimation(this,
			getLiveEventNumber(),
			msg.getSendingTime(),
			msg.getArrivalTime(),
			new Point(0, 0)));
		animationPrimitives.add(new SendMessageAnimation(this,
			getLiveEventNumber(),
			msg.getSendingTime(),
			msg.getArrivalTime(),
			0,
			new ConnectionId(directToGate.getOwnerModule().getId(), directToGate.getId())));
	}

	public void moduleCreated(IRuntimeModule module) {
		animationPrimitives.add(new CreateModuleAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), module));
	}

	public void moduleDeleted(IRuntimeModule module) {
	}

	public void moduleMethodCalled(IRuntimeModule from, IRuntimeModule to, String method) {
	}

	public void moduleReparented(IRuntimeModule module, IRuntimeModule oldparent) {
	}

	public void objectDeleted(LiveMessage object) {
	}
	
	protected IRuntimeSimulation createSimulation(IRuntimeModule rootModule) {
		return new LiveSimulation((LiveModule)rootModule, this);
	}

	protected LiveSimulation getLiveSimulation() {
		return (LiveSimulation)simulation;
	}

	protected void loadAnimationPrimitivesForCurrentPosition() {
		int count = animationPrimitives.size();

		while (!getLiveSimulation().isFinished()) {
			getLiveSimulation().doOneEvent();
			eventNumberChanged();
			
			IAnimationPrimitive lastAnimationPrimitive = animationPrimitives.get(animationPrimitives.size() - 1);
			
			if (count != animationPrimitives.size() &&
				lastAnimationPrimitive.getEventNumber() > eventNumber &&
				lastAnimationPrimitive.getBeginSimulationTime() > simulationTime)
				break;
		}
	}
}
