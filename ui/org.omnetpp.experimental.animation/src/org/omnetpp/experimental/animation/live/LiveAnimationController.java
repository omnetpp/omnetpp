package org.omnetpp.experimental.animation.live;

import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.common.simulation.model.ConnectionId;
import org.omnetpp.common.simulation.model.GateId;
import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.common.simulation.model.IRuntimeSimulation;
import org.omnetpp.experimental.animation.primitives.BubbleAnimation;
import org.omnetpp.experimental.animation.primitives.CreateConnectionAnimation;
import org.omnetpp.experimental.animation.primitives.CreateModuleAnimation;
import org.omnetpp.experimental.animation.primitives.HandleMessageAnimation;
import org.omnetpp.experimental.animation.primitives.IAnimationPrimitive;
import org.omnetpp.experimental.animation.primitives.SendBroadcastAnimation;
import org.omnetpp.experimental.animation.primitives.SendMessageAnimation;
import org.omnetpp.experimental.animation.primitives.SetModuleDisplayStringAnimation;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;
import org.omnetpp.experimental.simkernel.IEnvirCallback;
import org.omnetpp.experimental.simkernel.swig.Javaenv;
import org.omnetpp.experimental.simkernel.swig.Simkernel;
import org.omnetpp.experimental.simkernel.swig.cGate;
import org.omnetpp.experimental.simkernel.swig.cMessage;
import org.omnetpp.experimental.simkernel.swig.cModule;
import org.omnetpp.experimental.simkernel.swig.cObject;
import org.omnetpp.experimental.simkernel.swig.cSimulation;

public class LiveAnimationController extends ReplayAnimationController implements IEnvirCallback {
	public LiveAnimationController(AnimationCanvas canvas) {
		super(canvas, null);
		Javaenv jenv = Simkernel.getJavaenv();
		jenv.setJCallback(null, this);
		Simkernel.getJavaenv().doOneStep();
		initializeSimulation(Simkernel.getSimulation().getRootModule());
	}

	public double getLiveSimulationTime() {
		return getLiveSimulation().simTime();
	}

	public long getLiveEventNumber() {
		return getLiveSimulation().eventNumber();
	}

	public long getEventNumber() {
		// TODO: temporarily
		return getLiveEventNumber();
	}
	
	public void breakpointHit(String lbl, cModule mod) {
	}

	// TODO: add to interface
	public void bubble(cModule mod, String text) {
		animationPrimitives.add(new BubbleAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), 0, text, new Point(0, 0)));
	}

	public void connectionCreated(cGate srcgate) {
		animationPrimitives.add(new CreateConnectionAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), 0, new GateId(srcgate.getOwnerModule().getId(), srcgate.getId()), null));
	}

	public void connectionRemoved(cGate srcgate) {
	}

	public void displayStringChanged(cGate gate) {
	}

	public void displayStringChanged(cModule module) {
		animationPrimitives.add(new SetModuleDisplayStringAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), 0, module, module.getDisplayString().toString()));
	}

	public void messageDelivered(cMessage msg) {
		animationPrimitives.add(new HandleMessageAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), 0, simulation.getModuleByID(msg.getArrivalModuleId()), msg));
	}

	// TODO: this callback was split (remove it)
	public void messageSent(cMessage msg, cGate directToGate) {
		// TODO: remove this test stuff
		animationPrimitives.add(new SendBroadcastAnimation(this,
			getLiveEventNumber(),
			msg.getSendingTime(),
			0,
			msg.getArrivalTime(),
			new Point(0, 0)));
		animationPrimitives.add(new SendMessageAnimation(this,
			getLiveEventNumber(),
			msg.getSendingTime(),
			0,
			msg.getArrivalTime(),
			0,
			new ConnectionId(directToGate.getOwnerModule().getId(), directToGate.getId())));
	}

	public void moduleCreated(cModule module) {
		animationPrimitives.add(new CreateModuleAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), 0, module));
	}

	public void moduleDeleted(cModule module) {
	}

	public void moduleMethodCalled(cModule from, cModule to, String method) {
	}

	public void moduleReparented(cModule module, cModule oldparent) {
	}

	public void backgroundDisplayStringChanged(cModule parentmodule) {
	}

	public void beginSend(cMessage msg) {
	}

	public void messageCancelled(cMessage msg) {
	}

	public void messageScheduled(cMessage msg) {
	}

	public void messageSendDirect(cMessage msg, cGate toGate, double propagationDelay) {
	}

	public void messageSendHop(cMessage msg, cGate srcGate, double propagationDelay) {
	}

	public void messageSendHop(cMessage msg, cGate srcGate, double propagationDelay, double transmissionDelay, double transmissionStartTime) {
	}

	public void objectDeleted(cObject object) {
	}

	@Override
	protected IRuntimeSimulation createSimulation(IRuntimeModule rootModule) {
		return Simkernel.getSimulation();
	}

	@Override
	protected void initializeSimulation(IRuntimeModule rootModule) {
		// TODO: rootModule and callback interface
		super.initializeSimulation(rootModule);
		Javaenv jenv = Simkernel.getJavaenv();
		jenv.newRun(1);
	}

	protected cSimulation getLiveSimulation() {
		return (cSimulation)simulation;
	}

	@Override
	protected long loadAnimationPrimitivesForPosition() {
		int count = animationPrimitives.size();

		// TODO: why Simkernel
		while (Simkernel.getJavaenv().getSimulationState() != Javaenv.eState.SIM_TERMINATED.swigValue()) {
			Simkernel.getJavaenv().doOneStep();
			positionChanged();
			
			IAnimationPrimitive lastAnimationPrimitive = animationPrimitives.get(animationPrimitives.size() - 1);
			
			if (count != animationPrimitives.size() &&
				lastAnimationPrimitive.getEventNumber() > eventNumber &&
				lastAnimationPrimitive.getBeginSimulationTime() > simulationTime)
				break;
		}
		
		// TODO:
		return -1;
	}
}
