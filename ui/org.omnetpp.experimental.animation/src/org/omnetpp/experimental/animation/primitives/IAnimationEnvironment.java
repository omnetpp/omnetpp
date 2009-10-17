/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.IFigure;
import org.omnetpp.common.simulation.model.IRuntimeSimulation;
import org.omnetpp.experimental.animation.controller.TimerQueue;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController.AnimationMode;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;

/**
 * IAnimationPrimitives use the IAnimationEnvironment class to query about the
 * current state of the animation and to communicate with each other.
 */
public interface IAnimationEnvironment {
	/**
	 * Returns the current animation mode.
	 */
	public AnimationMode getAnimationMode();

	/**
	 * Returns the current event number.
	 */
	public long getEventNumber();

	/**
	 * Returns the current simulation time.
	 */
	public double getSimulationTime();

	/**
	 * Returns the current animation number.
	 */
	public long getAnimationNumber();

	/**
	 * Returns the current animation time.
	 */
	public double getAnimationTime();

	/**
	 * Returns the event number of the last event that occured before the given simulation time.
	 * If there are more than one event having the same simulation time then the first of them is returned.
	 */
	public long getFirstEventNumberForSimulationTime(double simulationTime);

	/**
	 * Returns the event number of the last event that occured before the given simulation time.
	 * If there are more than one event having the same simulation time then the last of them is returned.
	 */
	public long getLastEventNumberForSimulationTime(double simulationTime);

	/**
	 * Returns the simulation time when the given event occured.
	 */
	public double getSimulationTimeForEventNumber(long eventNumber);

	/**
	 * Returns the animation time for the given event number.
	 */
	public double getAnimationTimeForEventNumber(long eventNumber);

	/**
	 * Returns the animation time when the given animation starts.
	 */
	public double getAnimationTimeForAnimationNumber(long animationNumber);

	/**
	 * Returns the animation time when the given animation starts.
	 */
	public double getAnimationTimeForSimulationTime(double simulationTime);

	/**
	 * Returns a timer queue which can be used by animation primitives to display animated figures.
	 */
	public TimerQueue getTimerQueue();

	/**
	 * Returns the root figure of the canvas to which animation primitives should add their figures.
	 */
	public IFigure getRootFigure();

	public Object getFigure(Object key);

	public void setFigure(Object key, Object value);

	public IRuntimeSimulation getSimulation();

	public AnimationCanvas getCanvas();
}
