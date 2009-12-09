/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.common.simulation.model.IRuntimeSimulation;
import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.controller.TimerQueue;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.SubmoduleFigure;

/**
 * Base class for animation primitives. The default behaviour is that the animation primitive does not animate and its
 * begin and end animation times are equal.
 */
public abstract class AbstractAnimationPrimitive implements IAnimationPrimitive {
	protected IAnimationEnvironment animationEnvironment;

	protected AnimationPosition animationPosition;

	protected boolean isActive;

	public AbstractAnimationPrimitive(IAnimationEnvironment animationEnvironment, AnimationPosition animationPosition) {
		this.animationEnvironment = animationEnvironment;
		this.animationPosition = animationPosition;
		this.isActive = false;
	}

	public long getEventNumber() {
		return animationPosition.getEventNumber();
	}

	public double getBeginSimulationTime() {
		return animationPosition.getSimulationTime();
	}

	public double getEndSimulationTime() {
		return animationPosition.getSimulationTime();
	}

	public long getAnimationNumber() {
		return animationPosition.getAnimationNumber();
	}

	public double getBeginAnimationTime() {
		return animationEnvironment.getAnimationTimeForAnimationNumber(animationPosition.getAnimationNumber());
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

	public void redo() {
	}

	public void undo() {
	}

	public void animateAt(AnimationPosition animationPosition) {
	}

	protected IRuntimeSimulation getSimulation() {
		return animationEnvironment.getSimulation();
	}

	protected TimerQueue getTimerQueue() {
		return animationEnvironment.getTimerQueue();
	}

	protected IFigure getRootFigure() {
		return animationEnvironment.getRootFigure();
	}

	protected LayoutManager getLayoutManager() {
		return getRootFigure().getLayoutManager();
	}

	protected CompoundModuleFigure getCompoundModuleFigure(IRuntimeModule module) {
		return (CompoundModuleFigure)animationEnvironment.getFigure(module);
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

	protected int getAncestorUnderParentModule(int enclosedModuleId)
	{
		IRuntimeModule parentModule = getSimulation().getRootModule();
		IRuntimeModule module = getSimulation().getModuleByID(enclosedModuleId);

		while (module != null && module.getParentModule() != parentModule) {
			module = module.getParentModule();
		}

		if (module != null)
			return module.getId();
		else
			return -1;
	}

	protected Point getSubmoduleFigureCenter(int moduleId) {
		return getSubmoduleFigure(moduleId).getBounds().getCenter();
	}

	protected Point getConvexCombination(Point p1, Point p2, double alpha) {
		return new Point(Math.round((1 - alpha) * p1.x + alpha * p2.x), Math.round((1 - alpha) * p1.y + alpha * p2.y));
	}
}