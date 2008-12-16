/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.experimental.animation.controller.AnimationPosition;

/**
 * IAnimationPrimitive is an atomic animation element managed by the IAnimationController. An animation primitive
 * may or may not be active depending on the current animation position managed by its controller. Being active means
 * having effect on the state of the model or the state of the animation.
 * 
 * The controller is responsible for notifying the animation primitives when they become active or inactive based
 * on the animation position. The animation primitive life cycle is the following: (activate, animateAt*, deactivate)*
 */
public interface IAnimationPrimitive {
	/**
	 * Returns the event number when this animation primitive begins.
	 */
	public long getEventNumber();

	/**
	 * Returns the simulation time when this animation primitive begins and shall become active.
	 * BeginSimulationTime and EndSimulationTime may be equal.
	 */
	public double getBeginSimulationTime();

	/**
	 * Returns the simulation time when this animation primitive ends and shall become inactive.
	 * BeginSimulationTime and EndSimulationTime may be equal.
	 */
	public double getEndSimulationTime();

	/**
	 * Returns the animation number when this animation primitive begins.
	 */
	public long getAnimationNumber();
	
	/**
	 * Returns the animation time when this animation primitive begins and shall become active.
	 * BeginAnimationTime and EndAnimationTime may be equal.
	 */
	public double getBeginAnimationTime();

	/**
	 * Returns the animation time when this animation primitive ends and shall become inactive.
	 * BeginAnimationTime and EndAnimationTime may be equal.
	 */
	public double getEndAnimationTime();

	/**
	 * Applies changes to the model. This method is called when the model is to be
	 * updated to an animation time where this animation primitve is not active while
	 * it was active at the current animation time.
	 */
	public void activate();

	/**
	 * Undos changes to the model. This method is called when the model is to be
	 * updated to an animation time where this animation primitve is active while
	 * it was not active at the current animation time.
	 */
	public void deactivate();

	/**
	 * True if the animation primitive is active. This should be remembered by the activate and deactivate calls.
	 */
	public boolean isActive();
	
	/**
	 * Updates figures according to the given animation position. This method will be called only
	 * when the animation primitive is active.
	 */
	public void animateAt(AnimationPosition animationPosition);
}
