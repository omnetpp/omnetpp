package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.experimental.animation.controller.TimerQueue;
import org.omnetpp.figures.CompoundModuleFigure;

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
				// TODO: is it really +1? or is it just a default?
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

	protected CompoundModuleFigure getCompoundModuleFigure() { //FIXME must accept IModule whose figure to return
		return (CompoundModuleFigure)animationEnvironment.getRootFigure();  //FIXME
	}
	
	protected LayoutManager getLayoutManager() {
		return getRootFigure().getLayoutManager();
	}

	//FIXME to be replaced by specific calls
	protected IFigure addFigure(IFigure figure) {
		getRootFigure().add(figure);
		
		return figure;
	}

	//FIXME to be replaced by specific calls
	protected IFigure removeFigure(IFigure figure) {
		getRootFigure().remove(figure);
		
		return figure;
	}

	protected void setConstraint(IFigure figure, Rectangle constraint) {
		getLayoutManager().setConstraint(figure, constraint);
	}
}