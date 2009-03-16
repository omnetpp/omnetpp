/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.live;

import org.eclipse.jface.dialogs.MessageDialog;
import org.omnetpp.common.simulation.model.ConnectionId;
import org.omnetpp.common.simulation.model.GateId;
import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.primitives.BubbleAnimation;
import org.omnetpp.experimental.animation.primitives.CreateConnectionAnimation;
import org.omnetpp.experimental.animation.primitives.CreateModuleAnimation;
import org.omnetpp.experimental.animation.primitives.DeleteModuleAnimation;
import org.omnetpp.experimental.animation.primitives.HandleMessageAnimation;
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

	protected AnimationPosition lastStopLiveAnimationPosition;
	
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
		lastStopLiveAnimationPosition = new AnimationPosition();
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
	
	public double getLiveAnimationTime() {
		return getAnimationTimeForAnimationNumber(getLiveAnimationNumber());
	}

	public void gotoLivePosition() {
		if (!isRunningLive)
			setAnimationPosition(lastStopLiveAnimationPosition);
	}

	public boolean isAtLivePosition() {		
		return currentAnimationPosition.equals(lastStopLiveAnimationPosition);
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
		endAnimationPosition = new AnimationPosition();
		super.stepAnimation();
	}

	public void runSimulation() {
		gotoLivePosition();
		setRunningLive(true);
		endAnimationPosition = new AnimationPosition();
		super.runAnimation();
	}

	public void runSimulationFast() {
		gotoLivePosition();
		setRunningLive(true);
		endAnimationPosition = new AnimationPosition();
		super.animateStart(true, FAST_REAL_TIME_TO_ANIMATION_TIME_SCALE, new AnimationPosition());
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
	protected long loadAnimationPrimitivesForPosition(AnimationPosition animationPosition) {
		int count = beginOrderedAnimationPrimitives.size();

		if (isRunningLive)
			while (!isSimulationFinished()) {
				if (animationPosition.compareTo(getLiveSimulationPosition()) < 0)
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
		AnimationPosition animationPosition = new AnimationPosition(liveEventNumber, guessNextEventLiveSimulationTime(), getLiveAnimationNumber(), getLiveAnimationTime());
		HandleMessageAnimation handleMessageAnimation = new HandleMessageAnimation(this, animationPosition, module.getId(), message);
		addAnimationPrimitive(handleMessageAnimation);

		if (liveEventNumber == 0)
			beginAnimationPosition = animationPosition;
	}

	protected void setEndPositionAtLastStopLivePosition() {
		endAnimationPosition = lastStopLiveAnimationPosition;
	}

	protected void setLastStopLivePositionAtCurrentPosition() {
		lastStopLiveAnimationPosition = currentAnimationPosition;
	}

	private AnimationPosition getLiveSimulationPosition() {
		return new AnimationPosition(getLiveEventNumber(), getLiveSimulationTime(), getLiveAnimationNumber(), getLiveAnimationTime());
	}

	protected ReplayModule toReplayModule(cModule module) {
		ReplayModule replayModule = new ReplayModule();
		replayModule.setId(module.getId());
		replayModule.setName(module.getName());
		replayModule.setIndex(module.getIndex());
		replayModule.setSize(!module.isVector() ? -1 : module.getVectorSize());
		
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
		addAnimationPrimitive(new BubbleAnimation(this, getLiveSimulationPosition(), text, module.getId()));
	}

	public void connectionCreated(cGate gate) {
		cGate targetGate = gate.destinationGate();
		addAnimationPrimitive(new CreateConnectionAnimation(this, getLiveSimulationPosition(), new GateId(gate.getOwnerModule().getId(), gate.getId()), new GateId(targetGate.getOwnerModule().getId(), targetGate.getId())));
	}

	public void connectionRemoved(cGate gate) {
	}

	public void displayStringChanged(cGate gate) {
		addAnimationPrimitive(new SetConnectionDisplayStringAnimation(this, getLiveSimulationPosition(), new ConnectionId(gate.getOwnerModule().getId(), gate.getId()), gate.getDisplayString()));
	}

	public void displayStringChanged(cModule module) {
		addAnimationPrimitive(new SetModuleDisplayStringAnimation(this, getLiveSimulationPosition(), module.getId(), module.getDisplayString()));
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
		addAnimationPrimitive(new CreateModuleAnimation(this, getLiveSimulationPosition(), replayModule, parentModuleId));
	}

	public void moduleDeleted(cModule module) {
		addAnimationPrimitive(new DeleteModuleAnimation(this, getLiveSimulationPosition(), module.getId()));
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
		// FIXME:
		if (msg.getSendingTime() != getLiveSimulationTime())
			throw new RuntimeException("Future sendings are not handled");

		addAnimationPrimitive(new SendBroadcastAnimation(this, 
				getLiveSimulationPosition(),
				propagationDelay,
				transmissionDelay,
				msg.getSenderModuleId(), 
				toGate.getOwnerModule().getId(),
				toReplayMessage(msg)));
		addAnimationPrimitive(new SendDirectAnimation(this, 
				getLiveSimulationPosition(),
				propagationDelay,
				transmissionDelay,
				msg.getSenderModuleId(), 
				toGate.getOwnerModule().getId(),
				toReplayMessage(msg)));
	}

	public void messageSendHop(cMessage msg, cGate gate, double propagationTime) {
		// FIXME:
		if (msg.getSendingTime() != getLiveSimulationTime())
			throw new RuntimeException("Future sendings are not handled");

		addAnimationPrimitive(new SendMessageAnimation(this,
				getLiveSimulationPosition(),
				propagationTime,
				0,
				new ConnectionId(gate.getOwnerModule().getId(), gate.getId()),
				toReplayMessage(msg)));
	}

	public void messageSendHop(cMessage msg, cGate gate, double propagationTime, double transmissionTime, double transmissionStartTime) {
		// FIXME:
		if (msg.getSendingTime() != getLiveSimulationTime())
			throw new RuntimeException("Future sendings are not handled");

		addAnimationPrimitive(new SendMessageAnimation(this,
				getLiveSimulationPosition(),
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
