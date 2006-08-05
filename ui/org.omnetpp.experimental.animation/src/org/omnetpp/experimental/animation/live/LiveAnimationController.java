package org.omnetpp.experimental.animation.live;

import org.eclipse.jface.dialogs.MessageDialog;
import org.omnetpp.common.simulation.model.ConnectionId;
import org.omnetpp.common.simulation.model.GateId;
import org.omnetpp.experimental.animation.primitives.BubbleAnimation;
import org.omnetpp.experimental.animation.primitives.CreateConnectionAnimation;
import org.omnetpp.experimental.animation.primitives.CreateModuleAnimation;
import org.omnetpp.experimental.animation.primitives.DeleteModuleAnimation;
import org.omnetpp.experimental.animation.primitives.HandleMessageAnimation;
import org.omnetpp.experimental.animation.primitives.IAnimationPrimitive;
import org.omnetpp.experimental.animation.primitives.SendBroadcastAnimation;
import org.omnetpp.experimental.animation.primitives.SendDirectAnimation;
import org.omnetpp.experimental.animation.primitives.SendMessageAnimation;
import org.omnetpp.experimental.animation.primitives.SetConnectionDisplayStringAnimation;
import org.omnetpp.experimental.animation.primitives.SetModuleDisplayStringAnimation;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.experimental.animation.replay.model.ReplayMessage;
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
	
	protected cSimulation liveSimulation;

	protected long lastStopLiveEventNumber;
	
	protected double lastStopLiveSimulationTime;
	
	protected long lastStopLiveAnimationNumber;
	
	protected double lastStopLiveAnimationTime;
	
	protected long liveAnimationNumber;

	private boolean isRunningLive;

	public LiveAnimationController(AnimationCanvas canvas) {
		super(canvas, null);
		liveSimulation = Simkernel.getSimulation();
		jenv = Simkernel.getJavaenv();
		jenv.setJCallback(null, this);
	}
	
	@Override
	public void restart() {
		super.restart();
		lastStopLiveEventNumber = -1;
		lastStopLiveSimulationTime = -1;
		lastStopLiveAnimationNumber = -1;
		lastStopLiveAnimationTime = -1;
		liveAnimationNumber = 0;
		isRunningLive = false;

		jenv.newRun(1);
		addNextHandleMessageAnimation();
		gotoAnimationBegin();
		setLastStopLivePositionAtCurrentPosition();
		setEndPositionAtLastStopLivePosition();
		positionChanged();
	}

	@Override
	public void shutdown() {
		super.shutdown();
		jenv.setJCallback(null, null);
	}
	
	// simulate state

	public cSimulation getLiveSimulation() {
		return liveSimulation;
	}

	public long getLiveEventNumber() {
		return liveSimulation.eventNumber();
	}

	public double getLiveSimulationTime() {
		return liveSimulation.simTime();
   }
	
	public double guessNextEventLiveSimulationTime() {
		return liveSimulation.guessNextSimtime();
	}
	
	public long getLiveAnimationNumber() {
		return liveAnimationNumber;
	}
	
	public long getLiveAnimationTime() {
		throw new RuntimeException();
	}

	public void gotoLivePosition() {
		if (!isRunningLive) {
			eventNumber = lastStopLiveEventNumber;
			simulationTime = lastStopLiveSimulationTime;
			animationNumber = lastStopLiveAnimationNumber;
			animationTime = lastStopLiveAnimationTime;
		}
	}

	public boolean isAtLivePosition() {		
		return eventNumber == lastStopLiveEventNumber &&
			simulationTime == lastStopLiveSimulationTime &&
			animationNumber == lastStopLiveAnimationNumber &&
			animationTime == lastStopLiveAnimationTime;
	}
	
	// public animation API

	@Override
	public void gotoAnimationBegin() {
		setRunningLive(false);
		super.gotoAnimationBegin();
	}
	
	@Override
	public void runAnimationBack() {
		setRunningLive(false);
		super.runAnimationBack();
	}
	
	@Override
	public void stepAnimationBack() {
		setRunningLive(false);
		super.stepAnimationBack();
	}
	
	@Override
	public void stopAnimation() {
		setRunningLive(false);
		super.stopAnimation();
	}
	
	@Override
	public void runAnimation() {
		setRunningLive(false);
		super.runAnimation();
	}
	
	@Override
	public void stepAnimation() {
		setRunningLive(false);
		super.stepAnimation();
	}

	@Override
	public void gotoAnimationEnd() {
		setRunningLive(false);
		super.gotoAnimationEnd();
	}
	
	// public simulation API
	
	public void restartSimulation() {
		restart();
		animateAtCurrentPosition();
	}

	public void stepSimulation() {
		gotoLivePosition();
		setRunningLive(true);
		resetEndPosition();
		super.stepAnimation();
	}

	public void runSimulation() {
		gotoLivePosition();
		setRunningLive(true);
		resetEndPosition();
		super.runAnimation();
	}

	public void runSimulationFast() {
		gotoLivePosition();
		setRunningLive(true);
		resetEndPosition();
		super.animateStart(true, FAST_REAL_TIME_TO_ANIMATION_TIME_SCALE, -1, -1, -1);
	}

	public void runSimulationExpress() {
		setRunningLive(true);
		// TODO: show a modal dialog with a big stop button		
	}

	public void stopSimulation() {
		super.stopAnimation();
	}

	public void finishSimulation() {
		setRunningLive(false);

		if (!isSimulationFinished()) {
			gotoLivePosition();
			setLastStopLivePositionAtCurrentPosition();
			setEndPositionAtLastStopLivePosition();
			jenv.finishSimulation();
			
			controllerStateChanged();
		}
	}

	public boolean isSimulationFinished() {
		return jenv.getSimulationState() == Javaenv.eState.SIM_TERMINATED.swigValue() ||
			jenv.getSimulationState() == Javaenv.eState.SIM_FINISHCALLED.swigValue();
	}
	
	public boolean isRunningLive() {
		return isRunningLive;
	}
	
	public void setRunningLive(boolean isRunningLive) {
		this.isRunningLive = isRunningLive;
		
		controllerStateChanged();
	}
	
	@Override
	protected void animateStop() {
		if (isRunningLive) {
			setRunningLive(false);
			setLastStopLivePositionAtCurrentPosition();
			setEndPositionAtLastStopLivePosition();			
		}

		super.animateStop();
	}

	@Override
	protected long loadAnimationPrimitivesForPosition() {
		int count = beginOrderedAnimationPrimitives.size();

		if (isRunningLive)
			while (!isSimulationFinished()) {
				IAnimationPrimitive lastAnimationPrimitive = beginOrderedAnimationPrimitives.get(beginOrderedAnimationPrimitives.size() - 1);
	
				if (lastAnimationPrimitive.getEventNumber() > eventNumber &&
					lastAnimationPrimitive.getBeginSimulationTime() > simulationTime &&
					lastAnimationPrimitive.getAnimationNumber() > animationNumber &&
					lastAnimationPrimitive.getBeginAnimationTime() > animationTime)
						break;
	
				jenv.doOneStep();
				addNextHandleMessageAnimation();
				//jenv.runSimulation(mode, until_time, until_event);
			}
		
		return beginOrderedAnimationPrimitives.size() - count;
	}

	protected void addNextHandleMessageAnimation() {
		// FIXME: maybe we should be able to go to the very beginning of the simulation without this workaround
		liveAnimationNumber++;
		cMessage message = liveSimulation.guessNextEvent();
		cModule module = liveSimulation.guessNextModule();
		// FIXME: liveEventNumber is set to -1 before first doOneEvent, this is not consistent when later it is set to the next event number
		long liveEventNumber = getLiveEventNumber() == -1 ? 0 : getLiveEventNumber();
		HandleMessageAnimation handleMessageAnimation = new HandleMessageAnimation(this, liveEventNumber, guessNextEventLiveSimulationTime(), getLiveAnimationNumber(), module.getId(), message);
		addAnimationPrimitive(handleMessageAnimation);

		if (liveEventNumber == 0) {
			beginEventNumber = liveEventNumber;
			beginSimulationTime = guessNextEventLiveSimulationTime();
			beginAnimationNumber = liveAnimationNumber;
			beginAnimationTime = handleMessageAnimation.getBeginAnimationTime();
		}
	}

	protected void setEndPositionAtLastStopLivePosition() {
		endEventNumber = lastStopLiveEventNumber;
		endSimulationTime = lastStopLiveSimulationTime;
		endAnimationNumber = lastStopLiveAnimationNumber;
		endAnimationTime = lastStopLiveAnimationTime;
	}
	
/*	protected void setLivePositionAtCurrentPosition() {
		lastStopLiveEventNumber = getLiveEventNumber() - 1;
		lastStopLiveSimulationTime = getSimulationTimeForEventNumber(endEventNumber);
		lastStopLiveAnimationNumber = getAnimationNumberForEventNumber(endEventNumber);
		lastStopLiveAnimationTime = getAnimationTimeForAnimationNumber(endAnimationNumber);		
	}
*/	
	protected void setLastStopLivePositionAtCurrentPosition() {
		lastStopLiveEventNumber = eventNumber;
		lastStopLiveSimulationTime = simulationTime;
		lastStopLiveAnimationNumber = animationNumber;
		lastStopLiveAnimationTime = animationTime;		
	}

	protected void resetEndPosition() {
		endEventNumber = -1;
		endSimulationTime = -1;
		endAnimationNumber = -1;
		endAnimationTime = -1;
	}

	protected ReplayModule toReplayModule(cModule module) {
		ReplayModule replayModule = new ReplayModule();
		replayModule.setId(module.getId());
		replayModule.setName(module.getName());
		replayModule.setIndex(module.getIndex());
		replayModule.setSize(!module.isVector() ? -1 : module.getSize());
		
		return replayModule;
	}
	
	protected ReplayMessage toReplayMessage(cMessage msg) {
		ReplayMessage replayMessage = new ReplayMessage();
		replayMessage.setName(msg.getName());
		replayMessage.setClassName(msg.getClassName());
		replayMessage.setKind(msg.getKind());
		replayMessage.setLength(msg.getLength());
		replayMessage.setId(msg.getId());
		replayMessage.setTreeId(msg.getTreeId());
		replayMessage.setEncapsulationId(msg.getEncapsulationId());
		replayMessage.setEncapsulationTreeId(msg.getEncapsulationTreeId());
		return replayMessage;
	}

	// simulation callbacks
	
	public void breakpointHit(String lbl, cModule mod) {
	}

	public void bubble(cModule module, String text) {
		addAnimationPrimitive(new BubbleAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), getLiveAnimationNumber(), text, module.getId()));
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
		HandleMessageAnimation handleMessageAnimation = handleMessageAnimationPrimitives.get(handleMessageAnimationPrimitives.size() - 1);
		
		if (handleMessageAnimation.getEventNumber() != getLiveEventNumber() ||
			handleMessageAnimation.getBeginSimulationTime() != getLiveSimulationTime() ||
			handleMessageAnimation.getAnimationNumber() != getLiveAnimationNumber())
		{
			MessageDialog.openError(null, "Error", "Handle message animation differs!");
		}
	}

	public void moduleCreated(cModule module) {
		ReplayModule replayModule = toReplayModule(module);

		if (simulation == null)
			initializeSimulation(replayModule);

		int parentModuleId = module.getParentModule() != null ? module.getParentModule().getId() : -1;
		addAnimationPrimitive(new CreateModuleAnimation(this, getLiveEventNumber(), getLiveSimulationTime(), getLiveAnimationNumber(), replayModule, parentModuleId));
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

	public void messageSendDirect(cMessage msg, cGate toGate, double propagationDelay, double transmissionDelay) {
		addAnimationPrimitive(new SendBroadcastAnimation(this, 
				getLiveEventNumber(),
				msg.getSendingTime(),
				getLiveAnimationNumber(),
				propagationDelay,
				transmissionDelay,
				msg.getSenderModuleId(), 
				toGate.getOwnerModule().getId(),
				toReplayMessage(msg)));
		addAnimationPrimitive(new SendDirectAnimation(this, 
				getLiveEventNumber(),
				msg.getSendingTime(),
				getLiveAnimationNumber(),
				propagationDelay,
				transmissionDelay,
				msg.getSenderModuleId(), 
				toGate.getOwnerModule().getId(),
				toReplayMessage(msg)));
	}

	public void messageSendHop(cMessage msg, cGate gate, double propagationTime) {
		addAnimationPrimitive(new SendMessageAnimation(this,
				getLiveEventNumber(),
				msg.getSendingTime(),
				getLiveAnimationNumber(),
				propagationTime,
				0,
				new ConnectionId(gate.getOwnerModule().getId(), gate.getId()),
				toReplayMessage(msg)));
	}

	public void messageSendHop(cMessage msg, cGate gate, double propagationTime, double transmissionTime, double transmissionStartTime) {
		addAnimationPrimitive(new SendMessageAnimation(this,
				getLiveEventNumber(),
				msg.getSendingTime(), //transmissionStartTime,
				getLiveAnimationNumber(),
				propagationTime,
				transmissionTime,
				new ConnectionId(gate.getOwnerModule().getId(), gate.getId()),
				toReplayMessage(msg)));
	}

	public void objectDeleted(cObject object) {
	}

	public boolean idle() {
		return false;
	}
}
