package org.omnetpp.experimental.animation.live;

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
import org.omnetpp.experimental.animation.replay.model.ReplayModule;
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
	
	protected cSimulation liveSimulation;

	public LiveAnimationController(AnimationCanvas canvas) {
		super(canvas, null);
		liveSimulation = Simkernel.getSimulation();
		jenv = Simkernel.getJavaenv();
		jenv.setJCallback(null, this);
		jenv.newRun(1);
		initializeSimulation(toReplayModule(liveSimulation.getRootModule()));
	}
	
	public void shutdown() {
		super.shutdown();
		jenv.setJCallback(null, null);
	}

	public cSimulation getLiveSimulation() {
		return liveSimulation;
	}

	public long getLiveEventNumber() {
		return liveSimulation.eventNumber();
	}

	public double getLiveSimulationTime() {
		return liveSimulation.simTime();
   }
	
	public long getLiveAnimationNumber() {
		return liveAnimationNumber;
	}
	
	public long getLiveAnimationTime() {
		throw new RuntimeException();
	}

	public void gotoLivePosition() {
		// TODO: not precise
		gotoSimulationTime(getLiveSimulationTime());
	}

	public boolean isAtLivePosition() {
		// TODO: not precise
		return simulationTime == getLiveSimulationTime();
	}

	public void restartSimulation() {
		jenv.newRun(1);
	}

	public void simulateStep() {
		gotoSimulationTime(getLiveSimulationTime());
		animateStep();
	}

	public void simulateRun() {
		gotoSimulationTime(getLiveSimulationTime());
		animatePlay();
	}

	public void simulateFast() {	
		animateStart(true, FAST_REAL_TIME_TO_ANIMATION_TIME_SCALE, -1, -1, -1);
	}

	public void simulateExpress() {
		// TODO: show a modal dialog with a big stop button		
	}

	public void simulateStop() {
		gotoSimulationTime(getLiveSimulationTime());
		animateStop();
	}

	public void finishSimulation() {
		gotoSimulationTime(getLiveSimulationTime());
		jenv.finishSimulation();
	}

	@Override
	protected long loadAnimationPrimitivesForPosition() {
		int count = animationPrimitives.size();

		while (jenv.getSimulationState() != Javaenv.eState.SIM_TERMINATED.swigValue()) {
			IAnimationPrimitive lastAnimationPrimitive = animationPrimitives.get(animationPrimitives.size() - 1);

			if (lastAnimationPrimitive.getEventNumber() > eventNumber &&
				lastAnimationPrimitive.getBeginSimulationTime() > simulationTime &&
				lastAnimationPrimitive.getAnimationNumber() > animationNumber &&
				lastAnimationPrimitive.getBeginAnimationTime() > animationTime + 10) // FIXME: WTF?
					break;

			jenv.doOneStep();
			//jenv.runSimulation(mode, until_time, until_event);
		}
		
		return animationPrimitives.size() - count;
	}
	
	protected ReplayModule toReplayModule(cModule module) {
		ReplayModule replayModule = new ReplayModule();
		replayModule.setId(module.getId());
		replayModule.setName(module.getName());
		
		return replayModule;
	}
	
	// Simulation callbacks
	
	public void breakpointHit(String lbl, cModule mod) {
	}

	public void bubble(cModule module, String text) {
		addAnimationPrimitive(new BubbleAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), getLiveAnimationNumber(), text, module));
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
		addAnimationPrimitive(new SetModuleDisplayStringAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), getLiveAnimationNumber(), module.getId(), module.getDisplayString()));
	}

	public void messageDelivered(cMessage msg) {
		liveAnimationNumber++;
		addAnimationPrimitive(new HandleMessageAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), getLiveAnimationNumber(), msg.getArrivalModuleId(), msg));
	}

	public void moduleCreated(cModule module) {
		String parentModulePath = module.getParentModule() != null ? module.getParentModule().getFullPath() : null;
		addAnimationPrimitive(new CreateModuleAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), getLiveAnimationNumber(), toReplayModule(module), parentModulePath));
	}

	public void moduleDeleted(cModule module) {
		addAnimationPrimitive(new DeleteModuleAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), getLiveAnimationNumber(), module.getId()));
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
}
