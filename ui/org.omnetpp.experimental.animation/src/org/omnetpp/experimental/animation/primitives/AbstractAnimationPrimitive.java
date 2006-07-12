package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.experimental.animation.controller.TimerQueue;

public abstract class AbstractAnimationPrimitive implements IAnimationPrimitive {
	protected IAnimationEnvironment animationEnvironment;
	
	protected long eventNumber;
	
	protected double beginSimulationTime;

	protected long animationNumber;
	
	protected boolean shown;
	
	public AbstractAnimationPrimitive(IAnimationEnvironment animationEnvironment, long eventNumber, double beginSimulationTime, long animationNumber) {
		this.animationEnvironment = animationEnvironment;
		this.eventNumber = eventNumber;
		this.beginSimulationTime = beginSimulationTime;
		this.animationNumber = animationNumber;
	}
	
	public long getEventNumber() {
		return eventNumber;
	}
	
	public double getSimulationTime() {
		return getBeginSimulationTime();
	}

	public double getBeginSimulationTime() {
		return beginSimulationTime;
	}

	public double getEndSimulationTime() {
		return beginSimulationTime;
	}
	
	public long getAnimationNumber() {
		return animationNumber;
	}
	
	public double getBeginAnimationTime() {
		switch (animationEnvironment.getAnimationMode()) {
			case LINEAR:
				return getBeginSimulationTime();
			case EVENT:
				return animationEnvironment.getAnimationTimeForAnimationNumber(animationNumber);
			case NON_LINEAR:
				throw new RuntimeException();
		}

		throw new RuntimeException("Unreachable code reached");
	}

	public double getEndAnimationTime() {
		switch (animationEnvironment.getAnimationMode()) {
			case LINEAR:
				return getEndSimulationTime();
			case EVENT:
				return animationEnvironment.getAnimationTimeForAnimationNumber(animationNumber + 1);
			case NON_LINEAR:
				throw new RuntimeException();
		}

		throw new RuntimeException("Unreachable code reached");
	}

	public abstract void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime);
	
	protected TimerQueue getTimerQueue() {
		return animationEnvironment.getTimerQueue();
	}

	protected IFigure getRootFigure() {
		return animationEnvironment.getRootFigure();
	}

	protected LayoutManager getLayoutManager() {
		return getRootFigure().getLayoutManager();
	}

	protected void addFigure(IFigure figure) {
		getRootFigure().add(figure);
	}

	protected void removeFigure(IFigure figure) {
		getRootFigure().remove(figure);
	}

	protected void showFigure(IFigure figure) {
		figure.setVisible(true);

		if (figure.getParent() == null)
			addFigure(figure);
	}

	protected void hideFigure(IFigure figure) {
		//if (figure.getParent() != null)
		//	removeFigure(figure);
		
		figure.setVisible(false);
	}
	
	protected void show() {
		shown = true;
	}
	
	protected void hide() {
		shown = false;
	}

	protected void setConstraint(IFigure figure, Rectangle constraint) {
		getLayoutManager().setConstraint(figure, constraint);
	}
}