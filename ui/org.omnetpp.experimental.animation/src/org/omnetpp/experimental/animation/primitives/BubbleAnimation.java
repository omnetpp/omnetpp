package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.RoundedRectangle;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.controller.Timer;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.ModuleFigure;

public class BubbleAnimation extends AbstractAnimationPrimitive {

	private int moduleId;
	
	private RoundedRectangle background;

	private Label label;
	
	private Timer bubbleTimer;

	private String text;
	
	public BubbleAnimation(ReplayAnimationController animationController,
						   long eventNumber,
						   double simulationTime,
						   long animationNumber,
						   String text,
						   int moduleId) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.background = new RoundedRectangle();
		this.text = text;
		this.label = new Label(text);
		this.moduleId = moduleId;
		this.bubbleTimer = new Timer(3000, false, false) {
			public void run() {
				removeFigure(background);
				removeFigure(label);
				getTimerQueue().removeTimer(bubbleTimer);
			}
		};
	}

	@Override
	public double getEndAnimationTime() {
		return super.getBeginAnimationTime();
	}
	
	public void redo() {
		IRuntimeModule module = getModule();
		IRuntimeModule parentModule = module.getParentModule();
		
		if (parentModule == animationEnvironment.getSimulation().getRootModule()) {
			addFigure(background);
			addFigure(label);
		}

		ModuleFigure moduleFigure = (ModuleFigure)animationEnvironment.getFigure(module);
		Rectangle r = new Rectangle(moduleFigure.getLocation(), new Dimension(10 * text.length(), 20));
		setConstraint(background, r);
		setConstraint(label, r);
	}

	public void undo() {
		IRuntimeModule module = getModule();
		IRuntimeModule parentModule = module.getParentModule();
		
		if (parentModule == animationEnvironment.getSimulation().getRootModule()) {
			bubbleTimer.reset();
			getTimerQueue().addTimer(bubbleTimer);
		}
	}
	
	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
	}

	protected IRuntimeModule getModule() {
		return animationEnvironment.getSimulation().getModuleByID(moduleId);
	}
}
