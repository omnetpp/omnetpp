package org.omnetpp.experimental.animation.live;

import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.common.simulation.model.ConnectionId;
import org.omnetpp.common.simulation.model.GateId;
import org.omnetpp.experimental.animation.primitives.BubbleAnimation;
import org.omnetpp.experimental.animation.primitives.CreateConnectionAnimation;
import org.omnetpp.experimental.animation.primitives.CreateModuleAnimation;
import org.omnetpp.experimental.animation.primitives.DeleteModuleAnimation;
import org.omnetpp.experimental.animation.primitives.HandleMessageAnimation;
import org.omnetpp.experimental.animation.primitives.IAnimationPrimitive;
import org.omnetpp.experimental.animation.primitives.SendMessageAnimation;
import org.omnetpp.experimental.animation.primitives.SetConnectionDisplayStringAnimation;
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
	protected Javaenv jenv;

	protected long liveAnimationNumber;

	public LiveAnimationController(AnimationCanvas canvas) {
		super(canvas, null);
		simulation = Simkernel.getSimulation();
		jenv = Simkernel.getJavaenv();
		jenv.setJCallback(null, this);
		jenv.newRun(1);
		initializeSimulation();
	}

	public long getLiveEventNumber() {
		return getLiveSimulation().eventNumber();
	}

	public double getLiveSimulationTime() {
		return getLiveSimulation().simTime();
	}
	
	public long getLiveAnimationNumber() {
		return liveAnimationNumber;
	}
	
	public long getLiveAnimationTime() {
		throw new RuntimeException();
	}

	public void breakpointHit(String lbl, cModule mod) {
	}

	public void bubble(cModule mod, String text) {
		addAnimationPrimitive(new BubbleAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), getLiveAnimationNumber(), text, new Point(0, 0)));
	}

	public void connectionCreated(cGate gate) {
		cGate targetGate = gate.destinationGate();
		addAnimationPrimitive(new CreateConnectionAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), getLiveAnimationNumber(), new GateId(gate.getOwnerModule().getId(), gate.getId()), new GateId(targetGate.getOwnerModule().getId(), targetGate.getId())));
	}

	public void connectionRemoved(cGate gate) {
	}

	public void displayStringChanged(cGate gate) {
		addAnimationPrimitive(new SetConnectionDisplayStringAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), getLiveAnimationNumber(), new ConnectionId(gate.getOwnerModule().getId(), gate.getId()), gate.getDisplayString()));
	}

	public void displayStringChanged(cModule module) {
		addAnimationPrimitive(new SetModuleDisplayStringAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), getLiveAnimationNumber(), module, module.getDisplayString()));
	}

	public void messageDelivered(cMessage msg) {
		liveAnimationNumber++;
		addAnimationPrimitive(new HandleMessageAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), getLiveAnimationNumber(), simulation.getModuleByID(msg.getArrivalModuleId()), msg));
	}

	public void moduleCreated(cModule module) {
		addAnimationPrimitive(new CreateModuleAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), getLiveAnimationNumber(), module));
	}

	public void moduleDeleted(cModule module) {
		addAnimationPrimitive(new DeleteModuleAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), getLiveAnimationNumber(), module));
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

	public void messageSendDirect(cMessage msg, cGate toGate, double propagationTime) {
	}

	public void messageSendHop(cMessage msg, cGate gate, double propagationTime) {
		addAnimationPrimitive(new SendMessageAnimation(this,
				getLiveEventNumber(),
				msg.getSendingTime(),
				getLiveAnimationNumber(),
				propagationTime,
				0,
				new ConnectionId(gate.getOwnerModule().getId(), gate.getId())));
	}

	public void messageSendHop(cMessage msg, cGate gate, double propagationTime, double transmissionTime, double transmissionStartTime) {
		addAnimationPrimitive(new SendMessageAnimation(this,
				getLiveEventNumber(),
				msg.getSendingTime(), //transmissionStartTime,
				getLiveAnimationNumber(),
				propagationTime,
				transmissionTime,
				new ConnectionId(gate.getOwnerModule().getId(), gate.getId())));
	}

	public void objectDeleted(cObject object) {
	}

	@Override
	protected long loadAnimationPrimitivesForPosition() {
		int count = animationPrimitives.size();

		while (jenv.getSimulationState() != Javaenv.eState.SIM_TERMINATED.swigValue()) {
			IAnimationPrimitive lastAnimationPrimitive = animationPrimitives.get(animationPrimitives.size() - 1);

			if (lastAnimationPrimitive.getEventNumber() > eventNumber &&
				lastAnimationPrimitive.getBeginSimulationTime() > simulationTime &&
				lastAnimationPrimitive.getAnimationNumber() > animationNumber &&
				getAnimationTimeForSimulationTime(lastAnimationPrimitive.getBeginSimulationTime()) > animationTime)
					break;

			jenv.doOneStep();
		}
		
		return animationPrimitives.size() - count;
	}

	protected cSimulation getLiveSimulation() {
		return (cSimulation)simulation;
	}
}
