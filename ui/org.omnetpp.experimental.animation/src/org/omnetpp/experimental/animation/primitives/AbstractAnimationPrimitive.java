package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.experimental.animation.controller.TimerQueue;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public abstract class AbstractAnimationPrimitive implements IAnimationPrimitive {
	protected ReplayAnimationController animationController;
	
	protected double beginSimulationTime;
	
	public AbstractAnimationPrimitive(ReplayAnimationController controller, double beginSimulationTime) {
		this.animationController = controller;
		this.beginSimulationTime = beginSimulationTime;
	}

	public double getBeginSimulationTime() {
		return beginSimulationTime;
	}

	public double getEndSimulationTime() {
		return beginSimulationTime;
	}

	public void gotoSimulationTime(double t) {
	}
	
	protected TimerQueue getTimerQueue() {
		return animationController.getTimerQueue();
	}

	protected IFigure getRootFigure() {
		return animationController.getCanvas().getRootFigure();
	}

	protected LayoutManager getLayoutManager() {
		return getRootFigure().getLayoutManager();
	}

	protected void removeFigure(IFigure figure) {
		getRootFigure().remove(figure);
	}

	protected void addFigure(IFigure figure) {
		getRootFigure().add(figure);
	}

	protected void showFigure(IFigure figure) {
		if (figure.getParent() == null)
			addFigure(figure);
	}

	protected void hideFigure(IFigure figure) {
		if (figure.getParent() != null)
			removeFigure(figure);
	}

	protected void setConstraint(IFigure figure, Rectangle constraint) {
		getLayoutManager().setConstraint(figure, constraint);
	}
}