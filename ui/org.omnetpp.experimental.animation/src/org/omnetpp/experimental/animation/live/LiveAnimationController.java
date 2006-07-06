package org.omnetpp.experimental.animation.live;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.experimental.animation.live.model.LiveMessage;
import org.omnetpp.experimental.animation.live.model.LiveModule;
import org.omnetpp.experimental.animation.live.model.LiveSimulation;
import org.omnetpp.experimental.animation.model.GateId;
import org.omnetpp.experimental.animation.model.IRuntimeGate;
import org.omnetpp.experimental.animation.model.IRuntimeMessage;
import org.omnetpp.experimental.animation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.model.IRuntimeSimulation;
import org.omnetpp.experimental.animation.primitives.BubbleAnimation;
import org.omnetpp.experimental.animation.primitives.CreateModuleAnimation;
import org.omnetpp.experimental.animation.primitives.IAnimationPrimitive;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.GateAnchor;

public class LiveAnimationController extends ReplayAnimationController implements IEnvirCallback {
	public LiveAnimationController(AnimationCanvas canvas) {
		super(canvas);
	}

	protected IRuntimeSimulation createSimulation() {
		// TODO: get as parameter
		LiveModule rootModule = new LiveModule(null, 0);

		CompoundModuleFigure rootModuleFigure = new CompoundModuleFigure();
		rootModuleFigure.setDisplayString(new DisplayString(null, null, "bgb=400,400;bgi=background/hungary,stretch"));
		setFigure(new GateId(rootModule.getId(), 0), new GateAnchor(rootModuleFigure, "in"));
		setFigure(new GateId(rootModule.getId(), 1), new GateAnchor(rootModuleFigure, "out"));
		canvas.getRootFigure().getLayoutManager().setConstraint(rootModuleFigure, new Rectangle(0, 0, -1, -1));
		canvas.getRootFigure().add(rootModuleFigure);

		return new LiveSimulation(rootModule, this);
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
		animationPrimitives.add(new BubbleAnimation(this, text, getLiveSimulation().getSimulationTime(), new Point(0, 0)));
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
//		animationPrimitives.add(new SendMessageAnimation(this, getLiveSimulation().getSimulationTime(), msg.getArrivalTime(), new ConnectionId()));
	}

	public void moduleCreated(IRuntimeModule module) {
		animationPrimitives.add(new CreateModuleAnimation(this, getLiveSimulation().getSimulationTime(), module, new Point(0, 0)));
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
