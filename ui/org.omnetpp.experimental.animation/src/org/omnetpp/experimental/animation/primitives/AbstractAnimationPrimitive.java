package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.common.simulation.model.IRuntimeSimulation;
import org.omnetpp.experimental.animation.controller.TimerQueue;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.SubmoduleFigure;

public abstract class AbstractAnimationPrimitive implements IAnimationPrimitive {
	protected IAnimationEnvironment animationEnvironment;
	
	protected long eventNumber;
	
	protected double beginSimulationTime;

	protected long animationNumber;
	
	protected boolean shown;

	protected boolean isActive;
	
	public AbstractAnimationPrimitive(IAnimationEnvironment animationEnvironment, long eventNumber, double beginSimulationTime, long animationNumber) {
		this.animationEnvironment = animationEnvironment;
		this.eventNumber = eventNumber;
		this.beginSimulationTime = beginSimulationTime;
		this.animationNumber = animationNumber;
		this.isActive = false;
	}
	
	public long getEventNumber() {
		return eventNumber;
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
		return animationEnvironment.getAnimationTimeForAnimationNumber(animationNumber);
	}

	public double getEndAnimationTime() {
		return getBeginAnimationTime();
	}
 
	public boolean isActive() {
		return isActive;
	}
	
	public void activate() {
		if (!isActive) {
			redo();
			isActive = true;
		}
		else
			throw new RuntimeException();
	}

	public void deactivate() {
		if (isActive) {
			undo();
			isActive = false;
		}
		else
			throw new RuntimeException();
	}
	
	public abstract void redo();

	public abstract void undo();

	public abstract void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime);
	
	protected TimerQueue getTimerQueue() {
		return animationEnvironment.getTimerQueue();
	}

	protected IFigure getRootFigure() {
		return animationEnvironment.getRootFigure();
	}

	protected CompoundModuleFigure getCompoundModuleFigure(IRuntimeModule module) {
		return (CompoundModuleFigure)animationEnvironment.getFigure(module);
	}
	
	protected LayoutManager getLayoutManager() {
		return getRootFigure().getLayoutManager();
	}

	protected void setConstraint(IFigure figure, Rectangle constraint) {
		getLayoutManager().setConstraint(figure, constraint);
	}

	protected CompoundModuleFigure getEnclosingModuleFigure() {
		IRuntimeModule enclosingModule = getSimulation().getRootModule(); //FIXME
		CompoundModuleFigure enclosingModuleFigure = getCompoundModuleFigure(enclosingModule);
		return enclosingModuleFigure;
	}

	protected SubmoduleFigure getSubmoduleFigure(int moduleId) {
		IRuntimeModule module = getSimulation().getModuleByID(moduleId);
		
		if (module != null)
			return (SubmoduleFigure)animationEnvironment.getFigure(module);
		else
			return null;
	}

	protected Point getSubmoduleFigureCenter(int moduleId) {
		return getSubmoduleFigure(moduleId).getBounds().getCenter();
	}


	protected Point getConvexCombination(Point p1, Point p2, double alpha) {
		return new Point(Math.round((1 - alpha) * p1.x + alpha * p2.x), Math.round((1 - alpha) * p1.y + alpha * p2.y));
	}

	protected IRuntimeSimulation getSimulation() {
		return animationEnvironment.getSimulation();
	}
}